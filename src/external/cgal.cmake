# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0
option(ALLOW_BUNDLED_CGAL "Allow use of bundled CGAL source" ON)
option(ALLOW_SYSTEM_CGAL "Allow use of system-provided CGAL" ON)

find_package(Threads REQUIRED)
find_package(CGAL)
set(CGAL_DIR "${CMAKE_CURRENT_LIST_DIR}/CGAL-5.2.1")

if(ALLOW_SYSTEM_CGAL AND TARGET CGAL::CGAL)
	message(STATUS "- CGAL - using system-provided library")
	add_library(external-cgal INTERFACE)
	target_link_libraries(external-cgal INTERFACE CGAL::CGAL Threads::Threads)
elseif(ALLOW_BUNDLED_CGAL AND EXISTS "${CGAL_DIR}/include/CGAL/version.h")
	message(STATUS "- CGAL - using bundled source")
	add_library(external-cgal INTERFACE)
	target_include_directories(external-cgal INTERFACE "${CGAL_DIR}/include/")
	
	if (WIN32)
		add_library(mpfr SHARED IMPORTED GLOBAL)
		set_property(TARGET mpfr PROPERTY IMPORTED_IMPLIB "${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.lib")
		set_property(TARGET mpfr PROPERTY IMPORTED_LOCATION "${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.dll")
		target_include_directories(mpfr INTERFACE "${CGAL_DIR}/auxiliary/gmp/include")

		add_library(gmp SHARED IMPORTED GLOBAL)
		set_property(TARGET gmp PROPERTY IMPORTED_IMPLIB "${CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.lib")
		set_property(TARGET gmp PROPERTY IMPORTED_LOCATION "${CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.dll")
	endif()
	
	target_link_libraries(external-cgal INTERFACE mpfr gmp Threads::Threads)
	
	if (WIN32)
		if (DEFINED MESHLAB_LIB_OUTPUT_DIR)
			file(
				COPY
					${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.lib
					${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.dll
					${CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.lib
					${CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.dll
				DESTINATION
					${MESHLAB_LIB_OUTPUT_DIR})
		endif()
		if (DEFINED MESHLAB_LIB_INSTALL_DIR)
			install(
				FILES
					${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.lib
					${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.dll
					${CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.lib
					${CGAL_DIR}/auxiliary/gmp/lib/libgmp-10.dll
				DESTINATION
					${MESHLAB_LIB_INSTALL_DIR})
		endif()
	endif()
else()
	message(STATUS "- CGAL - skipping CGAL library")
endif()
