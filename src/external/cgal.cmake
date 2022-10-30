# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_CGAL "Allow download and use of bundled CGAL source" ON)
option(MESHLAB_ALLOW_SYSTEM_CGAL "Allow use of system-provided CGAL" ON)

find_package(Threads REQUIRED)
find_package(CGAL)

if(MESHLAB_ALLOW_SYSTEM_CGAL AND TARGET CGAL::CGAL)
	message(STATUS "- CGAL - using system-provided library")
	add_library(external-cgal INTERFACE)
	target_link_libraries(external-cgal INTERFACE CGAL::CGAL Threads::Threads)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_CGAL)
	set(CGAL_DIR "${CMAKE_CURRENT_LIST_DIR}/CGAL-5.2.1")

	if (NOT EXISTS "${CGAL_DIR}/include/CGAL/version.h")
		set(CGAL_LINK https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1.zip)
		download_and_unzip(${CGAL_LINK} ${CMAKE_CURRENT_LIST_DIR} "CGAL")

		if (WIN32)
			set(CGAL_AUX_LINK https://github.com/CGAL/cgal/releases/download/v5.2.1/CGAL-5.2.1-win64-auxiliary-libraries-gmp-mpfr.zip)
			download_and_unzip(${CGAL_AUX_LINK} ${CGAL_DIR} "CGAL auxiliary libraries")
		endif()
	endif()

	message(STATUS "- CGAL - using downloaded source")
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
		target_link_libraries(external-cgal INTERFACE gmp mpfr Threads::Threads)
	else()
		find_package(GMP)
		target_include_directories(external-cgal SYSTEM INTERFACE ${GMP_INCLUDE_DIRS})
		target_link_libraries(external-cgal INTERFACE ${GMP_LIBRARIES} mpfr Threads::Threads)
	endif()
	

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
