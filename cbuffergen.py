#!/usr/bin/python3
import os
import re
import sys
import subprocess
import time
import shlex
import json
import argparse


def MapType(type):
	type_pattern = r'(float|int|uint)(([1-4])(x([1-4]))?)?';
	match = re.match(type_pattern, type)
	type_name = "?"
	is_matrix = False
	is_vector = False
	type_align = 1
	dim_x = 1
	dim_y = 1
	size = 1
	if match: 
		#builtin or array type
		type_name = f'hlsl_{match.group(1)}'
		if match.group(5):
			is_matrix = True
			dim_x = int(match.group(3))
			dim_y = int(match.group(5))
			size = dim_x * dim_y
			type_align = 4
		elif match.group(3):
			is_vector = True
			dim_x = int(match.group(3))
			size = dim_x
			type_align = 4

	else:
		if type == 'PalDescriptorHandle':
			type_name = type
			size = 1
		else:
			print(f"unknown type {type}, exiting\n")
			exit(1)

	return type_name, size, is_vector, is_matrix, dim_x, dim_y, type_align

class Line:
	def __init__(L, type, name, array_ext):
		L.type = type
		L.name = name
		if array_ext:
			L.array_ext = array_ext
			L.is_array = True
		else:
			L.array_ext = ""
			L.is_array = False
		L.hlsl_base_type, L.hlsl_size, L.is_vector, L.is_matrix, L.dim_x, L.dim_y, L.type_align = MapType(type)
		if L.is_matrix:
			L.hlsl_type = f"{L.hlsl_base_type}{L.dim_x}x{L.dim_y}"
			L.hlsl_cbarray_type = f"{L.hlsl_base_type}{L.dim_x}x{L.dim_y}_cbarray"
		elif L.is_vector:
			L.hlsl_type = f"{L.hlsl_base_type}{L.dim_x}"
			L.hlsl_cbarray_type = f"{L.hlsl_base_type}{L.dim_x}_cbarray"
		else:
			L.hlsl_type = f"{L.hlsl_base_type}"
			L.hlsl_cbarray_type = f"{L.hlsl_base_type}_cbarray"

class CBufferGen:
	def __init__(A):
		print(" **** RUNNING CBufferGen ****")
		A.parser = argparse.ArgumentParser()
		A.parser.add_argument("-i", "--input_path", help="input directory", default=".")
		A.parser.add_argument("-c", "--c_path", help="directory for generated header c file", default=".")
	def ParseLines(A, lines):
		line_pattern = r'^[\s](\w+)[\s]*(\w+)((\[[\w]*\])*)';
		matches = re.finditer(line_pattern, lines, re.MULTILINE)
		output_lines = []

		for match in matches:
			l = Line(match.group(1), match.group(2), match.group(3))
			output_lines.append(l)
		return output_lines

	def Pad(A, offset, target, f):
		off = offset
		if (off%target) != 0:
			count = 4 - (off%target)
			pad_type = "hlsl_int"
			if count > 1:
				pad_type = f"{pad_type}{count}" 
			f.write(f"\t{pad_type:<30} _pad{off};\n");
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
					if l.is_array:
						#note that for the size of arrays, we just use the plain size, since for alignment purposes each each element can be ignored(since they are all fully aligned)
						#and additionally its allowed to insert elements after the array. WEIRD SHIT
						offset = A.Pad(offset, 4, f)
						output_type = l.hlsl_cbarray_type 
					else:
						if l.hlsl_size > 1 and (offset % 4) + l.hlsl_size > 4:
							offset = A.Pad(offset, 4, f)
					name = f"{l.name}{l.array_ext};";
					f.write(f"\t{output_type:<30} {name} //{offset:<5}\n")
					offset += l.hlsl_size
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