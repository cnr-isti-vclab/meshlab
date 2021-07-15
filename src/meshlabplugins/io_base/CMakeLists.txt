# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

set(HEADERS
	baseio.h
	load_project.h
	save_project.h
	${VCGDIR}/wrap/io_trimesh/export_obj.h
	${VCGDIR}/wrap/io_trimesh/export_off.h
	${VCGDIR}/wrap/io_trimesh/export_ply.h
	${VCGDIR}/wrap/io_trimesh/import_fbx.h
	${VCGDIR}/wrap/io_trimesh/import_obj.h
	${VCGDIR}/wrap/io_trimesh/import_off.h
	${VCGDIR}/wrap/io_trimesh/import_ply.h
	${VCGDIR}/wrap/io_trimesh/import_ptx.h
	${VCGDIR}/wrap/io_trimesh/import_stl.h
	${VCGDIR}/wrap/io_trimesh/io_material.h
	${VCGDIR}/wrap/ply/plylib.h)

set(SOURCES
	baseio.cpp
	load_project.cpp
	save_project.cpp
	${VCGDIR}/wrap/openfbx/src/miniz.c
	${VCGDIR}/wrap/openfbx/src/ofbx.cpp
	${VCGDIR}/wrap/ply/plylib.cpp)

add_meshlab_plugin(io_base ${SOURCES} ${HEADERS})

#target_include_directories(io_base PRIVATE ${EXTERNAL_DIR}/easyexif/)

target_link_libraries(io_base PRIVATE OpenGL::GLU)
