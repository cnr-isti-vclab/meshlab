# Copyright 2019, 2021, Collabora, Ltd.
# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_CGAL "Allow download and use of CGAL source" ON)
option(MESHLAB_ALLOW_SYSTEM_CGAL "Allow use of system-provided CGAL" ON)

set(CGAL_VER 5.6)

find_package(Threads REQUIRED)
find_package(CGAL ${CGAL_VER} QUIET)

if(MESHLAB_ALLOW_SYSTEM_CGAL AND TARGET CGAL::CGAL)
	message(STATUS "- CGAL - using system-provided library")
	add_library(external-cgal INTERFACE)
	target_link_libraries(external-cgal INTERFACE CGAL::CGAL Threads::Threads)
elseif(MESHLAB_ALLOW_DOWNLOAD_SOURCE_CGAL)
	set(CGAL_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/CGAL-${CGAL_VER}")
	set(CGAL_CHECK "${CGAL_DIR}/include/CGAL/version.h")
	set(CGAL_WIN_CHECK "${CGAL_DIR}/auxiliary/gmp/lib/libmpfr-4.lib")

	set(CGAL_DOWNLOAD_SUCCESS TRUE)

	if (NOT EXISTS ${CGAL_CHECK})
		set(CGAL_LINK
			https://github.com/CGAL/cgal/releases/download/v${CGAL_VER}/CGAL-${CGAL_VER}.zip
			https://www.meshlab.net/data/libs/CGAL-${CGAL_VER}.zip
			) 
		set(CGAL_MD5 6d1d067b88e20f7080d07d5108b4c772)
		download_and_unzip(
			NAME "CGAL"
			LINK ${CGAL_LINK}
			MD5 ${CGAL_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- CGAL - download failed.")
			set(CGAL_DOWNLOAD_SUCCESS FALSE)
		endif()
	endif()

	if (WIN32 AND NOT EXISTS ${CGAL_WIN_CHECK})
		set(CGAL_AUX_LINK
			https://github.com/CGAL/cgal/releases/download/v${CGAL_VER}/CGAL-${CGAL_VER}-win64-auxiliary-libraries-gmp-mpfr.zip
			https://www.meshlab.net/data/libs/CGAL-${CGAL_VER}-win64-auxiliary-libraries-gmp-mpfr.zip
			) 
		set(CGAL_AUX_MD5 247f4dca741c6b9a9be76286414070fa)
		download_and_unzip(
			NAME "CGAL auxiliary libraries"
			LINK ${CGAL_AUX_LINK}
			MD5 ${CGAL_AUX_MD5}
			DIR ${CGAL_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- CGAL auxiliary libraries - download failed.")
			set(CGAL_DOWNLOAD_SUCCESS FALSE)
		endif()
	endif()

	if (CGAL_DOWNLOAD_SUCCESS)
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
	endif()
else()
	message(STATUS "- CGAL - skipping CGAL library")
endif()
