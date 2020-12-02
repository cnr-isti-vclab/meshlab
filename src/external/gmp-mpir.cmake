# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_SYSTEM_GMP "Allow use of system-provided GMP" ON)
option(ALLOW_BUNDLED_MPIR "Allow use of bundled MPIR binaries" ON)

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
	add_library(external-mpir SHARED IMPORTED GLOBAL)
	add_library(external-mpirxx SHARED IMPORTED GLOBAL)
	# TODO why are we copying these? They're static libraries, not needed at runtime.
	if (HAVE_BUNDLED_MPIR STREQUAL "WIN-x64")
		file(
			COPY ${MPIR_DIR}/win32-msvc/mpir.lib ${MPIR_DIR}/win32-msvc/mpirxx.lib
			DESTINATION ${MESHLAB_LIB_OUTPUT_DIR})
		set_property(TARGET external-mpir PROPERTY IMPORTED_IMPLIB "${MESHLAB_LIB_OUTPUT_DIR}/mpir.lib")
		set_property(TARGET external-mpir PROPERTY IMPORTED_LOCATION "${MESHLAB_LIB_OUTPUT_DIR}/mpir.lib")
		set_property(TARGET external-mpirxx PROPERTY IMPORTED_IMPLIB "${MESHLAB_LIB_OUTPUT_DIR}/mpirxx.lib")
		set_property(TARGET external-mpirxx PROPERTY IMPORTED_LOCATION "${MESHLAB_LIB_OUTPUT_DIR}/mpirxx.lib")
		target_include_directories(external-mpir INTERFACE ${EXTERNAL_DIR}/inc/win32-msvc/mpir-2.2.1_x64)
	elseif(HAVE_BUNDLED_MPIR STREQUAL "APPLE-x64")
		file(
			COPY ${MPIR_DIR}/macx64/libmpir.a ${MPIR_DIR}/macx64/libmpirxx.a
			DESTINATION ${MESHLAB_LIB_OUTPUT_DIR})
		set_property(TARGET external-mpir PROPERTY IMPORTED_LOCATION "${MESHLAB_LIB_OUTPUT_DIR}/libmpir.a")
		set_property(TARGET external-mpirxx PROPERTY IMPORTED_LOCATION "${MESHLAB_LIB_OUTPUT_DIR}/libmpirxx.a")
		target_include_directories(external-mpir INTERFACE ${EXTERNAL_DIR}/inc/macx64/mpir-2.4.0)
	endif()
endif()
