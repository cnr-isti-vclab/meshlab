Install Instructions:
1) Download the latest ode sources from http://www.ode.org/download.html
2 a) To compile ode under linux or macosx:
	- cd in the ode source directory
	- sh autogen.sh
	- CFLAGS="-arch i386" CXXFLAGS="-arch i386"./configure --with-drawstuff=X11
	- make
	- make install
2 b) To compile ode under window:
3) Update the LIBS directive in filter_physics.pro with the correct path to the ode library, e.g.:
	- LIBS += -L/usr/local/lib -lode