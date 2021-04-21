# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_x3d.cpp vrml/Parser.cpp vrml/Scanner.cpp)

set(HEADERS export_x3d.h import_x3d.h io_x3d.h util_x3d.h vrml/Parser.h
            vrml/Scanner.h)

add_meshlab_plugin(io_x3d ${SOURCES} ${HEADERS})

target_link_libraries(io_x3d PRIVATE OpenGL::GLU)
