A simeple ray tracer in C++

The excutables are pre-compiled for Mac.

Requires: 
=========
* GCC compiler > 4.6.2
* OpenGL 4
* premake4
* make
* CMake

Instructions:
=============
1. build all prerequisites
```bash
$ premake4 gmake
$ make clean
$ make
```
2. build the ray tracer program
```bash
$ cd raytracer
$ premake4 gmake
$ make clean
$ make
```
3. render custome scene
```bash
$ ./raytracer [path-to-lua-script]
```

The program renders a .png image based on the scene descripted in .lua script

Additional .obj model can be added in raytracer/Assets/ folder

Sample lua scenes are provided in raytracer/Assets/

Additional notes and test result can be found in raytracer/README

Note:
=====
- Please run this program under raytracer/ folder
- A progress output is displayed during rendering
- Antialiasing, done by supersampling, where the sampling size is specified in the lua file.
- A heriarchical Bounding Volume of all nodes are impletmented to improve performance
- the under lua files are provided for demonstration
  * scene-simple.lua 
  * scene-4k-AAx1.lua 
  * scene-4k-AAx1-bounding.lua
  * scene-720p-AAx1.lua
  * scene-720p-AAx2.lua
  * scene-720p-AAx4.lua
  * scene-720p-AAx16.lua (scene-720p-AAx16.PNG is not included, cause it takes too long...)
- Most of the resulting PNG files are included

Comparing the runtime of bounding volume
========================================
* Rendering meshes: 
  * real    214m27.894s
  * user    214m43.085s
  * sys     0m1.210s
* Rendering bounding volumes of mesh
  * real    7m50.174s
  * user    7m50.506s
  * sys 0m0.205s
* Comparing scene-720p-AAx1.PNG, scene-720p-AAx2.PNG, scene-720p-AAx4.PNG and scene-720p-AAx16.PNG shows the result of Antialiasing

WARNING: 
========
Running those High Antialiasing lua takes a "ridiculous" amount of time (seriously)even with the heriarchical Bounding Volumes, the fact that we are writting a Ray Tracer as a single threaded CPU rendering program makes it not possible to improve perfomance massively.

That is why I make them in 720p only, and rendered them.

Running the program
=================== 
under raytracer/ folder
    
``` bash
    Low resolution(256x256). 
    No Antialiasing.
    (This is the script made to show the basic functionality of the program in a short runtime) 
        $ ./A4 Assests/scene-simple.lua 

    Ultra High resolution(4096x2160)
    No Antialiasing
    ./A4 Assests/scene-4k-AAx1.lua 

    Ultra High resolution(4096x2160)
    No Antialiasing
    Showing bounding volumes of meshes
    ./A4 Assests/scene-4k-AAx1-bounding.lua

    High resolution(1280x720)
    x1 Antialiasing
    ./A4 Assests/scene-720p-AAx1.lua

    High resolution(1280x720)
    x2 Antialiasing
    ./A4 Assests/scene-720p-AAx2.lua

    High resolution(1280x720)
    x4 Antialiasing
    ./A4 Assests/scene-720p-AAx4.lua

    High resolution(1280x720)
    x16 Antialiasing
    ./A4 Assests/scene-720p-AAx16.lua

```
