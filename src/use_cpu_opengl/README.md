#Use CPU OpenGL 

This is a little qmake project **for windows** (it won't be compiled in other platforms) that generates a binary `UseCPUOpenGL.exe` file with admin privileges. 

This binary is useful when MeshLab cannot start due to some graphic drivers that do not implement recent versions of OpenGL. In these cases, a (slow) CPU implementation of newer OpenGL leds MeshLab at least to start.

This binary must be run inside the MeshLab folder, and: 
- it renames the file `opengl32sw.dll` if present inside the MeshLab folder, or
- it generates a `opengl32.dll` file that will be used by MeshLab instead of the system one.