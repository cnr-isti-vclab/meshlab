include (../../shared.pri)

HEADERS += \
    diamond_sampler.h \
    diam_parametrization.h \
    diam_topology.h \
    dual_coord_optimization.h \
    iso_parametrization.h \
    local_optimization.h \
    local_parametrization.h \
    mesh_operators.h \
    opt_patch.h \
    param_collapse.h \
    param_flip.h \
    param_mesh.h \
    parametrizator.h \
    statistics.h \
    texcoord_optimization.h \
    uv_grid.h \
    defines.h \
    filter_isoparametrization.h\
    stat_remeshing.h

SOURCES += \
    filter_isoparametrization.cpp

TARGET = filter_isoparametrization
INCLUDEPATH  += \
    ./ \
    $${MESHLAB_EXTERNAL_DIRECTORY}/levmar-2.3/
				
win32-msvc:QMAKE_CXXFLAGS += /openmp -D_USE_OMP
win32-g++:QMAKE_LFLAGS += -fopenmp
linux-g++:QMAKE_CXXFLAGS += -fopenmp -D_USE_OMP

win32-msvc:LIBS	+= $${MESHLAB_DISTRIB_DIRECTORY}/lib/win32-msvc/levmar.lib
win32-g++:LIBS += -L$${MESHLAB_DISTRIB_DIRECTORY}/lib/win32-gcc -llevmar
macx:LIBS += $${MESHLAB_DISTRIB_DIRECTORY}/lib/macx64/liblevmar.a
linux-g++:LIBS += -llevmar

# Please never ever uncomment this...
#QMAKE_CXXFLAGS += -fpermissive

CONFIG       += opengl
