# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

# Only build if we have u3d
if(TARGET IDTF)
	# IDTF to U3D converter loads some dll/so files at runtime, thar may be placed in different
	# places, depending on:
	# - build or deploy stage
	# - operative system
	# The default paths are set for deploy stage (OS is detected automatically).
	# If you are building (not deploying) MeshLab and you want to test the U3D exporter, enable this
	# option.
	# Without enabling this option, the U3D exporter WILL NOT WORK ON BUILD STAGE.
	option(MESHLAB_U3D_BUILD_MODE "If you need to test u3d exporter in build mode (not deploy), enable this" OFF)

	set(SOURCES io_u3d.cpp)
	set(HEADERS io_u3d.h)

	add_meshlab_plugin(io_u3d ${SOURCES} ${HEADERS})
	target_link_libraries(io_u3d PUBLIC IDTF)

	if(MESHLAB_U3D_BUILD_MODE)
		target_compile_definitions(io_u3d PRIVATE BUILD_MODE)
	endif()
else()
	message(STATUS "Skipping io_u3d - missing u3d in external directory.")
endif()
