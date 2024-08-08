#!/usr/bin/python3
import os
import re
import sys
import subprocess
import time
import shlex
import json
import argparse
from enum import Enum
#
# TODO
#  typedef only supports size==4b
#  typedef does not support arrays
#  structs does not support arrays
#  structs only support structs from the same file
#  size calcs are in dwords, so no support for double, uint16_t

DELAYED_STRUCT_SIZE = -42
class TypeClass(Enum):
	BUILTIN = 1
	TYPEDEF = 2
	STRUCT = 3

# name/size pairs of all typedefs
Typedefs = {
	"PalDescriptorHandle":1
}

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

		L.cb_align = 1
		if L.is_matrix:
			L.cb_align = 4
			if L.array_size:
				padded_mat_size = L.hlsl_size * 4 * L.dim_y
				last_mat_size = L.hlsl_size * (4 * (L.dim_y-1) + L.dim_x)
				L.cb_size = padded_mat_size * (L.array_size-1) + last_mat_size
			else:
				L.cb_size = L.hlsl_size * (4 * (L.dim_y-1) + L.dim_x)
		elif L.is_vector:
			if L.array_size:
				L.cb_align = 4
				L.cb_size = L.hlsl_size * ((4 * (L.array_size-1)) + L.dim_x)
			else:
				L.cb_size = L.hlsl_size * L.dim_x

		else:
			if L.type_class == TypeClass.BUILTIN:
				if L.array_size > 0:
					print("arrays of non-builtin types not supported")
					exit(1)
				L.cb_size = L.hlsl_size
			elif L.type_class == TypeClass.STRUCT:
				if L.array_size > 0:
					print("arrays of non-builtin types not supported")
					exit(1)
				L.cb_size = L.hlsl_size
				L.cb_align = 4
			else:
				L.cb_size = L.hlsl_size

		if L.is_matrix:
			L.hlsl_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}"
			if array_size:
				L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}_cb_array({L.array_ext_cb})"
			else:
				L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}_cb"
				
		elif L.is_vector:
			L.hlsl_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}"
			if L.array_size:
				L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}_cb_array({L.array_ext_cb})"
			else:
				L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}"

		else:
			L.hlsl_type = f"{L.hlsl_base_type}"
			if L.type_class == TypeClass.STRUCT:
				L.hlsl_cb_type = f"{L.hlsl_base_type}_cb"
			else:
				L.hlsl_cb_type = f"{L.hlsl_base_type}"

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

	def Pad(A, offset, target, f):
		off = offset
		if (off%target) != 0:
			count = 4 - (off%target)
			pad_type = f"hlsl_int{count}" 
			name = f"__pad{off};"
			f.write(f"\t{pad_type:<40} {name:<40}//[{off}-{(off+count-1)}] [{4*off}-{4*(off+count-1)}]\n");
			off += count
		return off
		
	def Pad2(A, offset, target):
		off = offset
		pad_string = ""
		if (off%target) != 0:
			count = 4 - (off%target)
			pad_type = f"hlsl_int{count}" 
			name = f"__pad{off};"
			pad_string = f"\t{pad_type:<40} {name:<40}//[{off}-{(off+count-1)}] [{4*off}-{4*(off+count-1)}]\n";
			off += count
		return off, pad_string

	def Parse2(A, file_content, out_file, out_globals_file):
		
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
				for struct_name in file.struct_order:
					struct = file.structs[struct_name]
					f.write("//File generated by cbuffergen.py. Do not modify\n")
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
						offset = l.cb_offset = offset
						if l.cb_pad_string:
							f.write(l.cb_pad_string)
						f.write(f"\t{l.hlsl_cb_type:<40} {l.name:<40}//[{offset}-{offset+l.cb_size-1}] [{offset*4}-{4*(offset+l.cb_size-1)}]\n")
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
						A.WriteMembersRecurse(f, struct_name, struct)
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
				if l.cb_align == 4:
					x, pad_string = A.Pad2(offset, 4)
					offset = offset + x
				elif l.is_vector:
					if l.cb_size > 1 and (offset % 4) + l.cb_size > 4:
						x, pad_string = A.Pad2(offset, 4)
						offset = offset + x
				else:
					pass
				l.cb_offset = offset
				l.cb_pad_string = pad_string
				if l.cb_size == DELAYED_STRUCT_SIZE:
					decl_struct = A.all_structs[l.type]
					l.cb_size = decl_struct.cb_size
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
		type_pattern = r'(float|int|uint|bool)(([1-4])(x([1-4]))?)?';
		match = re.match(type_pattern, type)
		type_name = "?"
		dim_x = 0
		dim_y = 0
		size = 1
		type_class = TypeClass.BUILTIN
		external = 0
		if match:
			#builtin or array type
			type_name = f'{match.group(1)}'
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

	def Run(A):
		A.args = A.parser.parse_args()
		print("input path %s" % A.args.input_path)
		print("c path %s" % A.args.c_path)
		print("global path %s" % A.args.global_path)

		
		for filename in os.listdir(A.args.input_path):
			if filename.endswith(".h"):
				if not (filename.endswith(".cpp.h") or filename.endswith(".globals.h")):
					A.known_structs = {}
					output_file = f"{A.args.c_path}/{filename[:-2]}.cpp.h"
					output_globals_file = ""
					if A.args.global_path:
						output_globals_file = f"{A.args.global_path}/{filename[:-2]}.globals.hlsl"
					input_file = f"{A.args.input_path}/{filename}"
					print(f"read {input_file}")
					with open(input_file, 'r') as input_file:
						file_string = input_file.read()
						A.Parse2(file_string, output_file, output_globals_file)
		A.CalcSizes()
		A.WriteFiles()

G = CBufferGen();
G.Run()

