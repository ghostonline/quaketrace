#!/usr/bin/python
import os, subprocess, sys, shutil, platform

script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)

asset_dir = os.path.normpath(os.path.join(script_dir, "..", "assets"))

if platform.system() == "Windows":
	qbsp_tool_name = "qbsp.exe"
	light_tool_name = "light.exe"
	vis_tool_name = "vis.exe"
else:
	qbsp_tool_name = "qbsp"
	light_tool_name = "light"
	vis_tool_name = "vis"

qbsp_tool = os.path.join(script_dir, qbsp_tool_name)
light_tool = os.path.join(script_dir, light_tool_name)
vis_tool = os.path.join(script_dir, vis_tool_name)


def compile_map(mapname):
	input_file = mapname + ".map"
	output_file = mapname + ".bsp"

	subprocess.check_call((qbsp_tool, input_file))
	subprocess.check_call((light_tool, input_file))
	subprocess.check_call((vis_tool, input_file))

	src = os.path.join(script_dir, output_file)
	dst = os.path.join(asset_dir, output_file)
	shutil.copy(src, dst)

mapfiles = (f for f in os.listdir(script_dir) if f.endswith(".map"))
for file_ in mapfiles:
	compile_map(file_[:-4])
