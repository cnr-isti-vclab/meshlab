Put here the direct unzip of the ode-0.11.1.zip file 

https://sourceforge.net/projects/opende/files/ODE/0.11.1/ode-0.11.1.zip/download

Compilation info for mac:

sh autogen.sh
./configure --with-drawstuff=X11 CFLAGS="-arch i386" CXXFLAGS="-arch i386"
make
cp ode/src/.libs/lib* ../lib/macx/

