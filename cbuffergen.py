#!/usr/bin/python3
import os
import re
import sys
import subprocess
import time
import shlex
import json
import argparse
import math

from io import StringIO
from enum import Enum
#
# TODO
#  size calcs are in dwords, so no support for double, uint16_t

DELAYED_STRUCT_SIZE = -42
class TypeClass(Enum):
	BUILTIN = 1
	TYPEDEF = 2
	STRUCT = 3

# name/size pairs of all typedefs
Typedefs = {
	"PalDescriptorHandle":4
}

for t in Typedefs:
	assert (Typedefs[t] % 4) == 0

def GetAlignedArrayElementSize(size):
	#each array element should be 16b aligned 
	return 16 * (math.floor((size + 15) / 16))

def GetArraySize(size, array_size):
	total_size = (array_size-1) * GetAlignedArrayElementSize(size) + size
	return int(total_size)

class Line:
	def __init__(L, G, type, name, array_size, array_ext):
		L.G = G
		L.type = type
		L.name = name
		L.array_size = array_size
		if array_size:
			L.array_ext = f"[{array_ext}]"
			L.array_ext_cb = f"{array_ext}";
		else:
			L.array_ext = ""
			L.array_ext_cb = ""

		L.hlsl_base_type, L.hlsl_size, L.dim_x, L.dim_y, L.type_class = G.MapType(type)
		if L.dim_y:
			L.is_matrix = True
			L.is_vector = False
		elif L.type_class == TypeClass.BUILTIN:
			L.is_matrix = False
			L.is_vector = True
		else:
			L.is_matrix = False
			L.is_vector = False

		L.cb_align = L.hlsl_size
		if L.type_class == TypeClass.BUILTIN:
			vector_size = L.dim_x * L.hlsl_size
			if L.is_matrix:
				L.cb_align = 16
				if L.array_size:
					L.cb_size = GetArraySize(vector_size, L.dim_y * L.array_size) #padded_mat_size * (L.array_size-1) + mat_size
				else:
					L.cb_size = GetArraySize(vector_size, L.dim_y)

				L.hlsl_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}"
				if array_size:
					L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}_cb_array({L.array_ext_cb})"
				else:
					L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}_cb"

			elif L.is_vector:
				if L.array_size:
					L.cb_align = 16
					L.cb_size = GetArraySize(vector_size, L.array_size) #L.hlsl_size * L.dim_x + (16 * (L.array_size-1)) #L.hlsl_size * ((4 * (L.array_size-1)) + L.dim_x)
				else:
					L.cb_size = vector_size
				L.hlsl_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}"
				if L.array_size:
					L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}_cb_array({L.array_ext_cb})"
				else:
					L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}"

		elif L.type_class == TypeClass.STRUCT:
			if L.is_matrix or L.is_vector:
				print(f"matrix/vector structs not supported")
				exit(1)
			L.cb_size = L.hlsl_size
			L.cb_align = 16
			L.hlsl_type = f"{L.hlsl_base_type}"
			if L.array_size > 0:
				L.hlsl_cb_type = f"hlsl_any_array_cb<{L.hlsl_base_type}_cb, {L.array_size}>"
			else:
				L.hlsl_cb_type = f"{L.hlsl_base_type}_cb"

		elif L.type_class == TypeClass.TYPEDEF:
			if L.array_size:
				L.cb_size = GetArraySize(L.hlsl_size, L.array_size) # L.hlsl_size + (L.array_size-1) * 4
				L.cb_align = 16
			else:
				L.cb_size = L.hlsl_size

			L.hlsl_type = f"{L.hlsl_base_type}"
			if L.array_size:
				L.hlsl_cb_type = f"hlsl_any_array_cb<{L.hlsl_base_type}, {L.array_size}>"
			else:
				L.hlsl_cb_type = L.hlsl_base_type

		else:
			print(f"unknown typeclass {L.type_class}")
			exit(1)




class CBufferGenStruct:
	def __init__(A):
		A.dependencies = set()
		A.parse_state = 0

class CBufferGenFile:
	def __init__(A):
		A.structs = {}
		A.struct_order = []


class CBufferGen:
	def __init__(A):
		print(" **** RUNNING CBufferGen ****")
		A.parser = argparse.ArgumentParser()
		A.parser.add_argument("-i", "--input_path", help="input directory", default=".")
		A.parser.add_argument("-c", "--c_path", help="directory for generated header c file", default=".")
		A.parser.add_argument("-g", "--global_path", help="directory for generated hlsl files containing hlsl globals", default="")
		A.known_struct_sizes = {}
		A.all_structs = {}
		A.files = []
		A.struct_stack = []

	def ParseLines(A, lines):
		line_pattern = r'^[\s]*(\w+)[\s]*(\w+)((\[([\w]*)\])*)';
		matches = re.finditer(line_pattern, lines, re.MULTILINE)
		output_lines = []

		for match in matches:
			num_groups = len(match.groups());
			array_ext = None
			array_size = 0
			if match.group(4):
				if match.group(3) != match.group(4):
					print(f"multidimensional arrays not supported '{match.group(3)}'")
					exit(1)
				array_size = 1
				array_ext = match.group(5)
				try:
					array_size = int(array_ext)
				except:
					pass
			l = Line(A, match.group(1), match.group(2), array_size, array_ext)
			output_lines.append(l)
		return output_lines

		
	def Pad2(A, offset, target):
		off = offset
		pad_string = ""
		if (off%target) != 0:
			aligned = target * (math.floor((off + target - 1) / target))
			count_bytes = aligned - off #16 - (off%target)
			shorts = count_bytes % 4
			if shorts > 0:
				assert shorts == 2
				count = int(count_bytes / 2)
				pad_type = f"hlsl_uint16_t{count}" 
				name = f"__pad{int(off)};"
				s3 = off
				s4 = (off+count_bytes)
				pad_string = f"\t{pad_type:<50} {name:<40}//[{s3}-{s4}]\n";

			else:
				count = int(count_bytes / 4)
				pad_type = f"hlsl_int{count}" 
				name = f"__pad{int(off)};"
				s3 = off
				s4 = (off+count_bytes)
				pad_string = f"\t{pad_type:<50} {name:<40}//[{s3}-{s4}]\n";
			off += count_bytes
		return off, pad_string

	def Parse(A, file_content, out_file, out_globals_file):
		
		File = CBufferGenFile()
		File.out_file = out_file
		File.out_globals_file = out_globals_file
		struct_pattern = r'struct ([^\s]+)[\s]+{([^{}]*)}[\s]*;'
		pos = 0
		end = len(file_content)

		matches = re.finditer(struct_pattern, file_content, re.MULTILINE)
		for match in matches:
			idx = match.start()
			end = match.end()
			struct_name = match.group(1)
			contents = match.group(2)
			#print stuff before match.
			pre_text = "" 
			if idx != pos:
				pre_text = file_content[pos:idx]
				pos = end
			struct = CBufferGenStruct()
			struct.pre_text = pre_text
			struct.lines = A.ParseLines(contents)
			struct.file = File
			struct.parse_state = 0
			struct.name = struct_name
			struct.cb_size = DELAYED_STRUCT_SIZE
			for l in struct.lines:
				if l.type_class == TypeClass.STRUCT:
					struct.dependencies.add(l.type)
			File.structs[struct_name] = struct
			File.struct_order.append(struct_name)
			if struct_name in A.all_structs:
				print(f"error: duplicate struct {struct_name}")
				exit(1)
			A.all_structs[struct_name] = struct
		if pos != len(file_content):
			File.tail_text = file_content[pos:]
		A.files.append(File)

	def WriteMembersRecurse(A, f, prefix, struct):
		for l in struct.lines:
			if l.type_class == TypeClass.STRUCT:
				#f.write(f"// {l.type} {prefix}.{l.name} \n")
				A.WriteMembersRecurse(f, f"{prefix}.{l.name}", A.all_structs[l.type])
			else:
				f.write(f"#define {l.name:<40} {prefix}.{l.name}\n")

	def MakeDir(A, filename):
		dir_path = os.path.dirname(filename)
		if dir_path:
			os.makedirs(dir_path, exist_ok=True)

	def WriteFiles(A):
		for file in A.files:
			A.MakeDir(file.out_file)
			with open(file.out_file, "w") as f:
				print(f"write {file.out_file}")
				f.write("//File generated by cbuffergen.py. Do not modify\n")
				for struct_name in file.struct_order:
					struct = file.structs[struct_name]
					f.write(struct.pre_text)
					f.write(f"//plain struct\n")
					f.write(f"struct {struct_name}\n{{\n")
					for l in struct.lines:
						f.write(f"\t{l.hlsl_type:<30} {l.name}{l.array_ext};\n")
					f.write(f"}};\n\n")		
					f.write(f"//const buffer struct\n")
					f.write(f"struct {struct_name}_cb\n{{\n")
					offset = 0
					for l in struct.lines:
						offset = l.cb_offset
						if l.cb_pad_string:
							f.write(l.cb_pad_string)
						n = f"{l.name};"
						s3 = offset
						s4 = (offset+l.cb_size)
						f.write(f"\t{l.hlsl_cb_type:<50} {n:<40}//[{s3}-{s4}]\n")
						offset += l.cb_size
					f.write(f"}}; // struct size:{offset}\n")

			if file.out_globals_file:
				A.MakeDir(file.out_globals_file)
				with open(file.out_globals_file, "w") as f:
					print(f"write {file.out_globals_file}")
					f.write(""" //File generated by cbuffergen.py. Do not modify
// This file contains helper defines to let all members look like globals
// This is mainly a workaround to make it easier to port/reuse older code that relies on this.
""")
					for struct_name in file.struct_order:
						struct = file.structs[struct_name]
						f.write(f"\n\n#ifdef {struct_name.upper()}_GLOBALS\n")
						struct = file.structs[struct_name]
						A.WriteMembersRecurse(f, f"{struct_name.upper()}_GLOBALS", struct)
						f.write(f"#endif //{struct_name.upper()}_GLOBALS\n\n")


	def CalcSizes(A):
		for struct_name in A.all_structs:
			A.ParseRecursive(A.all_structs[struct_name])

	def ParsePush(A, struct):
		A.struct_stack.append(struct)

	def ParsePop(A):
		A.struct_stack.pop()

	def ParseDump(A):
		print(f"Recursive struct references")
		for s in A.struct_stack:
			print(f"\t{s.name}")


	def ParseRecursive(A, struct):
		parse_state = struct.parse_state
		if parse_state == 0:
			struct.parse_state = 1
			A.ParsePush(struct)
			for dep_name in struct.dependencies:
				if not dep_name in A.all_structs:
					print(f"unknown struct {dep_name}")
				dep_struct = A.all_structs[dep_name]
				A.ParseRecursive(dep_struct)
			offset = 0
			for l in struct.lines:
				output_type = l.hlsl_type
				offset_before = offset
				pad_string = ""
				if l.cb_align == 16:
					padded_offset, pad_string = A.Pad2(offset, 16)
					offset = padded_offset
				else:
					if (offset % 16) + l.cb_size > 16:
						padded_offset, pad_string = A.Pad2(offset, 16)
						offset = padded_offset
					elif l.cb_align == 2:
						assert (offset % 2) == 0
					elif l.cb_align == 8:
						padded_offset, pad_string = A.Pad2(offset, 8)
						offset = padded_offset

					
				l.cb_offset = offset
				l.cb_pad_string = pad_string
				if l.cb_size == DELAYED_STRUCT_SIZE:
					decl_struct = A.all_structs[l.type]
					l.cb_size = decl_struct.cb_size
					if l.array_size:
						l.cb_size = GetArraySize(l.cb_size, l.array_size)
					if decl_struct.cb_size == DELAYED_STRUCT_SIZE:
						print(f"struct size for {l.type} unresolved")
						exit(1)
				offset += l.cb_size
			struct.cb_size = offset
			A.ParsePop()
			struct.parse_state = 2
		elif parse_state == 1:
			ParseDump()
		elif parse_state == 2:
			pass #already processed


	def MapType(A, type):
		type_pattern = r'(float|int|uint|bool|uint16_t|double)(([1-4])(x([1-4]))?)?';
		match = re.match(type_pattern, type)
		type_name = "?"
		dim_x = 0
		dim_y = 0
		size = 4
		type_class = TypeClass.BUILTIN
		external = 0
		if match:
			#builtin or array type
			type_name = f'{match.group(1)}'
			if "uint16_t" in type_name:
				size = 2
			elif "double" in type_name:
				size = 8
			if match.group(5):
				dim_x = int(match.group(3))
				dim_y = int(match.group(5))
			elif match.group(3):
				dim_x = int(match.group(3))
			else:
				dim_x = 1
		else:
			if type in Typedefs:
				type_name = type
				size = Typedefs[type_name]
				type_class = TypeClass.TYPEDEF
			else:
				type_name = type
				type_class = TypeClass.STRUCT
				size = DELAYED_STRUCT_SIZE


		return type_name, size, dim_x, dim_y, type_class


	def FixupIncludes(A, input_string, filenames):
		include_pattern = r'^\#include[\s]+"([\S]+)"'
		matches = re.finditer(include_pattern, input_string, re.MULTILINE)
		pos = 0
		input_end = len(input_string)
		buffer = StringIO()
		for match in matches:
			idx = match.start()
			end = match.end()
			includename = match.group(1)
			if idx != pos:
				buffer.write(input_string[pos:idx])
			if includename in filenames:
				includename = f"{includename[:-2]}.cpp.h"
			buffer.write(f'#include "{includename}"')
			pos = end
		if pos != input_end:
			buffer.write(input_string[pos:])
		return buffer.getvalue()


	def Run(A):
		A.args = A.parser.parse_args()
		print("input path %s" % A.args.input_path)
		print("c path %s" % A.args.c_path)
		print("global path %s" % A.args.global_path)

		input_files = []
		
		for filename in os.listdir(A.args.input_path):
			if filename.endswith(".h"):
				if not (filename.endswith(".cpp.h") or filename.endswith(".globals.h")):
					input_files.append(filename)

		for filename in input_files:

			input_file = f"{A.args.input_path}/{filename}"
			print(f"read {input_file}")
			with open(input_file, 'r') as input_file:
				file_string = input_file.read()
				file_string = A.FixupIncludes(file_string, input_files)
				A.known_structs = {}
				output_file = f"{A.args.c_path}/{filename[:-2]}.cpp.h"
				output_globals_file = ""
				if A.args.global_path:
					output_globals_file = f"{A.args.global_path}/{filename[:-2]}.globals.hlsl"
					A.Parse(file_string, output_file, output_globals_file)
		A.CalcSizes()
		A.WriteFiles()

G = CBufferGen();
G.Run()

