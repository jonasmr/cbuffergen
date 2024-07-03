#!/usr/bin/python3
import os
import re
import sys
import subprocess
import time
import shlex
import json
import argparse
#
# TODO
#  User defined types are assumed to be 4 bytes, and arrays of them are not supported
#  double support
#  uint16_t support

def MapType(type):
	type_pattern = r'(float|int|uint)(([1-4])(x([1-4]))?)?';
	match = re.match(type_pattern, type)
	type_name = "?"
	type_align = 1
	dim_x = 0
	dim_y = 0
	size = 1
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
		if type == 'PalDescriptorHandle':
			type_name = type
			size = 1
			external = 1
		else:
			print(f"unknown type {type}, exiting\n")
			exit(1)

	return type_name, size, dim_x, dim_y, external

class Line:
	def __init__(L, type, name, array_size, array_ext):
		L.type = type
		L.name = name
		L.array_size = array_size
		if array_size:
			L.array_ext = f"[{array_ext}]"
			L.array_ext_cb = f"{array_ext}";
		else:
			L.array_ext = ""
			L.array_ext_cb = ""

		L.hlsl_base_type, L.hlsl_size, L.dim_x, L.dim_y, L.is_external = MapType(type)
		if L.dim_y:
			L.is_matrix = True
			L.is_vector = False
		elif not L.is_external:
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
			if L.is_array:
				print("arrays of non-builtin types not supported")
			L.cb_size = L.hlsl_size

		if L.is_matrix:
			L.hlsl_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}"
			if array_size:
				L.hlsl_cb_type = f"hlsl_marray_cb<hlsl_{L.hlsl_base_type}, {L.dim_x}, {L.dim_y}, {L.array_ext_cb}>"				
			else:
				L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}x{L.dim_y}"
				
		elif L.is_vector:
			L.hlsl_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}"
			if L.array_size:
				L.hlsl_cb_type = f"hlsl_varray_cb<hlsl_{L.hlsl_base_type}, {L.dim_x}, {L.array_ext_cb}>"
			else:
				L.hlsl_cb_type = f"hlsl_{L.hlsl_base_type}{L.dim_x}"

		else:
			L.hlsl_type = f"{L.hlsl_base_type}"
			L.hlsl_cb_type = f"{L.hlsl_base_type}"

class CBufferGen:
	def __init__(A):
		print(" **** RUNNING CBufferGen ****")
		A.parser = argparse.ArgumentParser()
		A.parser.add_argument("-i", "--input_path", help="input directory", default=".")
		A.parser.add_argument("-c", "--c_path", help="directory for generated header c file", default=".")
	def ParseLines(A, lines):
		line_pattern = r'^[\s](\w+)[\s]*(\w+)((\[([\w]*)\])*)';
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
			l = Line(match.group(1), match.group(2), array_size, array_ext)
			output_lines.append(l)
		return output_lines

	def Pad(A, offset, target, f):
		off = offset
		if (off%target) != 0:
			count = 4 - (off%target)
			pad_type = f"hlsl_int{count}" 
			name = f"__pad{off};"
			f.write(f"\t{pad_type:<40} {name:<40}//[{off}-{off+count-1}]\n");
			off += count
		return off

	def Parse(A, file_content, output_file):
		struct_pattern = r'struct ([^\s]+)[\s]+{([^{}]*)}[\s]*;'
		pos = 0
		end = len(file_content)
		print(f"writing -> {output_file}")
		with open(output_file, "w") as f:
			matches = re.finditer(struct_pattern, file_content, re.MULTILINE)
			for match in matches:
				idx = match.start()
				end = match.end()
				name = match.group(1)
				contents = match.group(2)
				#print stuff before match.
				if idx != pos:
					f.write(file_content[pos:idx])
					pos = end
				lines = A.ParseLines(contents)
				f.write(f"//plain struct\n")
				f.write(f"struct {name}\n{{\n")
				for l in lines:
					f.write(f"\t{l.hlsl_type:<30} {l.name}{l.array_ext};\n")
				f.write(f"}};\n\n")		
				f.write(f"//const buffer struct\n")
				f.write(f"struct {name}CB\n{{\n")
				offset = 0
				for l in lines:
					output_type = l.hlsl_type
					offset_before = offset
					if l.cb_align == 4:
						offset = A.Pad(offset, 4, f)
					elif l.is_vector:
						if l.cb_size > 1 and (offset % 4) + l.hlsl_size > 4:
							offset = A.Pad(offset, 4, f)
					else:
						pass
					name = f"{l.name};"
					f.write(f"\t{l.hlsl_cb_type:<40} {name:<40}//[{offset}-{offset+l.cb_size-1}]\n")
					offset += l.cb_size
				f.write(f"}}")
		print(f"done -> {output_file}")


	def Run(A):
		A.args = A.parser.parse_args()
		print("input path %s" % A.args.input_path)
		print("c path %s" % A.args.c_path)
		
		for filename in os.listdir(A.args.input_path):
			if filename.endswith(".h"):
				if not filename.endswith(".cpp.h"):
					output_file = f"{filename[:-2]}.cpp.h"
					print(f"input file {filename} -> {output_file}")
					with open(filename, 'r') as input_file:
						file_string = input_file.read()
						A.Parse(file_string, output_file)
						exit(1)


G = CBufferGen();
G.Run()