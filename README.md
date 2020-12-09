# opengl-transformations
 Collaborative code to learn basic graphics transformations in opengl
Usage Instructions:
===================
Two makefiles are provided, one for linux (which works on the lab PCs) and one for Windows.
To use the linux makefile (./Makefile) just run 'make' to compile and 'make run' to run.
To use the Windows makefile (./Makefile_win), run 'make -f Makefile_win' to compile, and 'make -f Makefile_win run' to run.
Note that you will need to have Visual Studio installed and be running from its own console ("Developer Command Prompt for VS...") in order for it to work

When running on Windows, you will need to have `SDL2.dll` and `glew32.dll` included in the same directory as your executable.
For linux you simply need the `libsdl2-dev` and `libglew-dev` package installed.

In directory opengl-prac1 do the following:
Compile: make
Compile: make run

TRANSFORMATION MODE - select first:
t -translate
r - rotate
s - scale

AXIS - select axis second for roation and translation:
x - xaxis
y -yaxis
z -zaxis

now use mouse or trackpad to transform about axes:
(make sure your on right tansformation mode and axis as above)
you are free to switch between modes and axis at any point

translate left - click and hold press and drag mouse left
translate right - click and hold press and drag mouse right
translate up - click and hold press and drag mouse up
translate down - click and hold press and drag mouse down

rotate clockwise about xaxis - click and hold press and drag mouse left
rotate anti-clockwise about xaxis - click and hold press and drag mouse right
rotate clockwise about yaxis - click and hold press and drag mouse up
rotate anti-clockwise about yaxis - click and hold press and drag down
rotate clockwise about zaxis - click and hold press and drag mouse down
rotate anti-clockwise about zaxis - click and hold press and drag mouse up

scale up
scale down

Note GLM included but if issue, install on terminal
