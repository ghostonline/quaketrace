#!/usr/bin/python
import os, subprocess, sys, shutil

script_dir = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_dir)

mapname = "testmap"
input_file = mapname + ".map"
output_file = mapname + ".bsp"
asset_dir = os.path.normpath(os.path.join(script_dir, "..", "assets"))

qbsp_tool = os.path.join(script_dir, "qbsp")
light_tool = os.path.join(script_dir, "light")
vis_tool = os.path.join(script_dir, "vis")


subprocess.check_call((qbsp_tool, input_file))
subprocess.check_call((light_tool, input_file))
subprocess.check_call((vis_tool, input_file))

src = os.path.join(script_dir, output_file)
dst = os.path.join(asset_dir, output_file)
shutil.copy(src, dst)
