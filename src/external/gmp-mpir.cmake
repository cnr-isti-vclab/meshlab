# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_SYSTEM_GMP "Allow use of system-provided GMP" ON)
option(ALLOW_BUNDLED_MPIR "Allow use of bundled MPIR binaries" ON)

find_package(GMP)
set(MPIR_DIR ${CMAKE_CURRENT_LIST_DIR}/mpir)

unset(HAVE_BUNDLED_MPIR)
if (APPLE AND EXISTS "${MPIR_DIR}/macx64/libmpir.a" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	# We have mac binaries for x64
	# TODO check target processor architecture, not just pointer size
	set(HAVE_BUNDLED_MPIR APPLE-x64)
elseif(WIN32 AND MSVC AND EXISTS "${MPIR_DIR}/win32-msvc/mpir.lib" AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	# We have windows binaries for x64
	# TODO check target processor architecture, not just pointer size
	set(HAVE_BUNDLED_MPIR WIN-x64)
endif()

# gmp or mpir - optional, for filter_csg
if(ALLOW_SYSTEM_GMP AND GMP_FOUND)
	message(STATUS "- GMP/MPIR - using system-provided GMP library")
	add_library(external-gmp INTERFACE)
	target_include_directories(external-gmp SYSTEM INTERFACE ${GMP_INCLUDE_DIRS})
	target_link_libraries(external-gmp INTERFACE ${GMP_LIBRARIES})

elseif(ALLOW_BUNDLED_MPIR AND HAVE_BUNDLED_MPIR)
	message(STATUS "- GMP/MPIR - using already built MPIR library")
	add_library(mpir STATIC IMPORTED GLOBAL)
	add_library(mpirxx STATIC IMPORTED GLOBAL)
	if (HAVE_BUNDLED_MPIR STREQUAL "WIN-x64")
		set_property(TARGET mpir PROPERTY IMPORTED_LOCATION "${MPIR_DIR}/win32-msvc/mpir.lib")
		set_property(TARGET mpirxx PROPERTY IMPORTED_LOCATION "${MPIR_DIR}/win32-msvc/mpirxx.lib")
		target_include_directories(mpir INTERFACE ${CMAKE_CURRENT_LIST_DIR}/inc/win32-msvc/mpir-2.2.1_x64)
	elseif(HAVE_BUNDLED_MPIR STREQUAL "APPLE-x64")
		set_property(TARGET mpir PROPERTY IMPORTED_LOCATION "${MPIR_DIR}/macx64/libmpir.a")
		set_property(TARGET mpirxx PROPERTY IMPORTED_LOCATION "${MPIR_DIR}/macx64/libmpirxx.a")
		target_include_directories(mpir INTERFACE ${CMAKE_CURRENT_LIST_DIR}/inc/macx64/mpir-2.4.0)
	endif()
	add_library(external-mpir INTERFACE)
	target_link_libraries(external-mpir INTERFACE mpir mpirxx)
endif()
