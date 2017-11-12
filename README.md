Quake level Raytracer
=====================

Given a compiled Quake 1 level, this project generates a raytraced image from the perspective of one of the intermission cameras defined in it.

Usage
-----

```
quaketrace (--input|-i) <string> (--output|-o) <string>
	[--width|-w <integer>] [--height|-h <integer>] [--detail|-d <integer>]
	[--occlusion <integer>] [--occlusion-strength <integer>]
	[--shadows <integer>] [--ambient <number>] [--threads|-j <integer>]
	[--camera|-c <integer>] [--camera-list|-l] [--gamma <number>] [--help]

--input, -i
	Path to a compiled Quake 1 level file

--output, -o
	Path to the TGA file that will be written

--width, -w (defaults to 320)
	Width of the generated image

--height, -h (defaults to 240)
	Height of the generated image

--detail, -d (defaults to 1)
	Supersampling factor to apply. A value of 2 results in 4 
	samples per pixel, 3 results in 9 samples, 4 in 16 samples 
	etc.

--occlusion (defaults to 32)
	Number of rays to cast for ambient occlusion detection

--occlusion-strength (defaults to 16)
	Occlusion ray length, a higher value will grow ambient 
	occlusion shadows

--shadows (defaults to 10)
	Number of soft shadow rays

--ambient (defaults to 0.0)
	Ambient lighting level

--threads, -j (defaults to 4)
	Number of worker threads to use while raytracing, best set 
	to the number of CPU cores

--camera, -c (defaults to 0)
	Intermission camera index to use as viewpoint

--camera-list, -l
	Print the number of intermission cameras in the level file

--gamma (defaults to 1.0)
	Apply gamma correction to the generated image

--help
	Display program usage information
```

Compiling from source
---------------------
This project uses CMake for generating project files. This project does not have any dependencies by default, but could be compiled using SDL to show a progress dialog while generating the image. Set the SHOW_GUI parameter to true to enable this functionality.

Background
----------
Quake 1 levels have the interesting property of containing almost all data needed for rendering its geometry. A compiled level contains the level geometry, texture mipmaps, lighting information (lightmaps and the original light defintions) and gameplay entity definitions (like monsters, player spawn points, weapons). This allows the raytracer to generate a good representation of the level, albeit without any entity models, since these are stored separately.

After completing a level in Quake, the game shows the image from an intermission camera overlaid with gameplay statistics. These cameras are often pointed at interesting or memorable geometry from the level. Using these camera, the raytracer has a better chance of generating a recognisable image from the level.

For some example renders, see [this webpage](http://bartveldstra.com/personal/quaketrace/).

Limitations
-----------

- The original quake tools appear to strip the ambient light information during map compilation. Use the ```--ambient``` command line parameter with a custom value if the level appears to be too dark when rendered.
- Sky rendering is a rough approximation of how Quake renders it and will sometimes generate a wrong-looking skybox
- FOV is locked to 60
- Light sizes (used for soft shadowing) are set to 16
