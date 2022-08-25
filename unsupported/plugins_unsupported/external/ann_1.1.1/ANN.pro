TEMPLATE = lib
TARGET = ANN
CONFIG += staticlib
ANNDIR = .
INCLUDEPATH  += $$ANNDIR/include
DEPENDPATH += $$ANNDIR/src
DESTDIR = $$ANNDIR/lib

win32-g++:	DEFINES += _ANN_WIN_MINGW
linux-g++:  QMAKE_CXXFLAGS += -include stdlib.h -include string.h

HEADERS       = kd_tree.h \
				kd_split.h \
				kd_util.h \
				kd_search.h \
				kd_pr_search.h \
				kd_fix_rad_search.h \
				pr_queue.h \
				pr_queue_k.h
SOURCES       = ANN.cpp \
				brute.cpp \
				kd_tree.cpp \
				kd_util.cpp \
				kd_split.cpp \
				kd_dump.cpp \
				kd_search.cpp \
				kd_pr_search.cpp \
				kd_fix_rad_search.cpp \
				bd_tree.cpp \
				bd_search.cpp \
				bd_pr_search.cpp \
				bd_fix_rad_search.cpp \
				perf.cpp
