copy here the lib compiled for the system with the name specified in the directory name according to the QT system configuration names


Some compilation notes
------ libgmp ------

For mac osx 10.6 with meshlab 32 bit…
After extracting the archive

./configure CFLAGS="-mmacosx-version-min=10.5 -arch i386" LDFLAGS="-mmacosx-version-min=10.5 -arch i386" ABI=32 --enable-cxx --enable-static --with-pic

make

cp .libs/*.a ../../external/lib/macx/

--- using mpir instead GMP --- 
Exactly the same steps as above… 
For mac osx 10.6 with meshlab 32 bit…
After extracting the archive

./configure CFLAGS="-mmacosx-version-min=10.5 -arch i386" LDFLAGS="-mmacosx-version-min=10.5 -arch i386" ABI=32 --enable-cxx --enable-static --with-pic

make

cp .libs/*.a ../../external/lib/macx/
