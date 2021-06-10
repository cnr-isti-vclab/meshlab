# Copyright 2019-2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

# Prefer OpenGL GLVND
if(POLICY CMP0072)
	cmake_policy(SET CMP0072 NEW)
endif()

### Build settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)

### Settings needed for both "external" and internal code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

### Build Settings
if(WIN32)
	add_definitions(-DNOMINMAX)
	if(MSVC)
		add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
	endif()
endif()
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(BUILD_STRICT AND NOT MSVC AND NOT APPLE)
	# Make sure that our shared libraries were appropriately linked
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
endif()

# Start automoc/autouic/autorcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
