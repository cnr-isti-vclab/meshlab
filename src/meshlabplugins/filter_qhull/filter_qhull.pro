include (../../shared.pri)

QT           += opengl

HEADERS       += filter_qhull.h \
		qhull_tools.h 

SOURCES       += filter_qhull.cpp \ 
		qhull_tools.cpp 

HEADERS += 	../../external/qhull-2003.1/src/geom.h \
		../../external/qhull-2003.1/src/io.h \
		../../external/qhull-2003.1/src/mem.h \
		../../external/qhull-2003.1/src/merge.h \
		../../external/qhull-2003.1/src/poly.h \
		../../external/qhull-2003.1/src/qhull.h \
		../../external/qhull-2003.1/src/qset.h \
		../../external/qhull-2003.1/src/stat.h \
		../../external/qhull-2003.1/src/user.h 

SOURCES +=	../../external/qhull-2003.1/src/geom.c \
		../../external/qhull-2003.1/src/geom2.c \
		../../external/qhull-2003.1/src/global.c \
		../../external/qhull-2003.1/src/io.c \
		../../external/qhull-2003.1/src/mem.c \
		../../external/qhull-2003.1/src/merge.c \
		../../external/qhull-2003.1/src/poly.c \
		../../external/qhull-2003.1/src/poly2.c \
		../../external/qhull-2003.1/src/qconvex.c \
		../../external/qhull-2003.1/src/qhull.c \
		../../external/qhull-2003.1/src/qset.c \
		../../external/qhull-2003.1/src/stat.c \
		../../external/qhull-2003.1/src/user.c 

TARGET        = filter_qhull