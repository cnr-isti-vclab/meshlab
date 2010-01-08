This folder contains other not so common GPL libraries that we need link into meshlab. The compiled libraries for the various systems should go in the 'lib' folder in a subdir named according to the QT system configuration  files (win32-g++, macx, etc).

Currently:

---- ann-1.1.1

---- bzip2-1.0.5

---- glew-1.5.1

-----lib3ds-1.3.0
Direct expansion of lib3ds-1.3.0.zip
The latest stable version of lib3ds
Downloaded from lib3ds.sourceforge.net
Modifications:
* added qmake project files in folder lib3ds

---- muparser_v130

---- ode-0.11.1
put there the direct unzip of the ode-0.11.1.zip file 
https://sourceforge.net/projects/opende/files/ODE/0.11.1/ode-0.11.1.zip/download
compilation info in the folder.

---- qhull-2003.1
Direct expansion of qhull-2003.1.tar.gz. 
Downloaded from www.qhull.org.
Modifications:
* added qmake project files

---- tsai-30b3
direct expansion of tsai30b3.zip
The latest stable version of routines for calibrating Roger Tsai's perspective projection camera model. (October 28, 95)
Downloaded from http://www.cs.cmu.edu/~rgw/TsaiCode.html
Modifications:
* added qmake project files
* converted from uppercase to lowercase filenames (internally the files refers to themselves using lowercase names)


