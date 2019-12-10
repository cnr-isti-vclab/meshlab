include (../../shared.pri)

HEADERS       += ./diamond_sampler.h \
        ./diam_parametrization.h \
        ./diam_topology.h \
        ./dual_coord_optimization.h \
        ./iso_parametrization.h \
        ./local_optimization.h \
        ./local_parametrization.h \
        ./mesh_operators.h \
        ./opt_patch.h \
        ./param_collapse.h \
        ./param_flip.h \
        ./param_mesh.h \
        ./parametrizator.h \
        ./statistics.h \
        ./texcoord_optimization.h \
        ./uv_grid.h \
        ./defines.h \
        ./filter_isoparametrization.h\
        ./stat_remeshing.h

SOURCES       += ./filter_isoparametrization.cpp

TARGET        = filter_isoparametrization
INCLUDEPATH  += ./  \
                ../../external/levmar-2.3/
				
win32-msvc:QMAKE_CXXFLAGS   +=  /openmp -D_USE_OMP
#win32-g++:QMAKE_CXXFLAGS   +=  -fopenmp -D_USE_OMP
win32-g++:QMAKE_LFLAGS += -fopenmp
#linux-g++:QMAKE_CXXFLAGS   +=  -fopenmp -D_USE_OMP
#linux-g++-32:QMAKE_CXXFLAGS   +=  -fopenmp -D_USE_OMP
linux-g++-64:QMAKE_CXXFLAGS   +=  -fopenmp -D_USE_OMP


#macx:QMAKE_CXX = g++
macx: {
  contains(QMAKE_CXX,g++-mp-4.8) {
macx:QMAKE_CXXFLAGS += -fopenmp -D_USE_OMP
macx:QMAKE_LFLAGS += -fopenmp
  }
}
win32-msvc:LIBS	+= ../../external/lib/win32-msvc/levmar.lib
win32-g++:LIBS		+= -L../../external/lib/win32-gcc -llevmar
linux-g++:LIBS		+= -llevmar
macx:LIBS += $$MACLIBDIR/liblevmar.a

# Please never ever uncomment this...
#QMAKE_CXXFLAGS += -fpermissive

CONFIG       += opengl
