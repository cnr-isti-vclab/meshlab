# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have u3d
if(TARGET external-IDTFConverter)

	set(SOURCES io_u3d.cpp)

	set(HEADERS io_u3d.h)

	add_meshlab_plugin(io_u3d ${SOURCES} ${HEADERS})

	target_link_libraries(io_u3d PUBLIC external-IDTFConverter IFXCoreStatic ${CMAKE_DL_LIBS})

else()
	message(STATUS "Skipping io_u3d - missing u3d in external directory.")
endif()
