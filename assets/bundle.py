#!/usr/bin/python
import os, textwrap, sys

script_src = os.path.dirname(os.path.abspath(__file__))
if len(sys.argv) < 2:
	output = os.path.join(script_src, "bundled.h")
else:
	output = sys.argv[1]

assets = []
files = sorted(f for f in os.listdir(script_src) if f.endswith(".bmp") or f.endswith(".bsp"))
for file_ in files:
	full_path = os.path.join(script_src, file_)
	varname = "ASSET_%s" % os.path.splitext(file_)[0].upper()
	with open(full_path, "rb") as f:
		data = f.read().encode("hex").upper()
	formatted ="".join(["0x"+x+y+", " for (x,y) in zip(data[0::2], data[1::2])])
	formatted = "\n\t".join(textwrap.wrap(formatted,80))
	array = 'static const unsigned char %s[] = {\n\t%s\n};' % (varname, formatted)
	assets.append(array)

with open(output, "w") as f:
	for asset in assets:
		f.write(asset)
		f.write("\n")
