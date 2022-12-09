# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_BUNDLED_SOURCE_GLEW "Allow use bundled source of GLEW" ON)
option(MESHLAB_ALLOW_SYSTEM_GLEW "Allow use of system-provided GLEW" ON)

unset(HAVE_SYSTEM_GLEW)
if(DEFINED GLEW_VERSION)
	if((TARGET GLEW::GLEW) AND (${GLEW_VERSION} VERSION_GREATER_EQUAL "2.0.0"))
		set(HAVE_SYSTEM_GLEW TRUE)
	endif()
endif()

if(MESHLAB_ALLOW_SYSTEM_GLEW AND HAVE_SYSTEM_GLEW)
	message(STATUS "- GLEW - using system-provided library")
	add_library(external-glew INTERFACE)
	target_link_libraries(external-glew INTERFACE GLEW::GLEW)
	if(TARGET OpenGL::GL)
		target_link_libraries(external-glew INTERFACE OpenGL::GL)
	elseif(TARGET OpenGL::OpenGL)
		target_link_libraries(external-glew INTERFACE OpenGL::OpenGL)
	else()
		message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
	endif()
elseif(MESHLAB_ALLOW_BUNDLED_SOURCE_GLEW)
	set(GLEW_DIR "${MESHLAB_EXTERNAL_DIR}/glew-2.2.0")
	set(GLEW_CHECK "${GLEW_DIR}/src/glew.c")

	# just to be sure, but this library is bundled in meshlab repo and download shouldn't be necessary
	if (NOT EXISTS ${GLEW_CHECK})
		set(GLEW_LINK
			https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.zip
			https://www.meshlab.net/data/libs/glew-2.2.0.zip)
		set(GLEW_MD5 970535b75b1b69ebd018a0fa05af63d1)
		download_and_unzip(
			NAME "GLEW"
			LINK ${GLEW_LINK}
			MD5 ${GLEW_MD5}
			DIR ${MESHLAB_EXTERNAL_DIR})
		if (NOT download_and_unzip_SUCCESS)
			message(FATAL_ERROR "- GLEW - download failed.")
		endif()
	endif()

	if (EXISTS ${GLEW_CHECK})
		message(STATUS "- GLEW - using bundled source")
		add_library(external-glew SHARED "${GLEW_DIR}/src/glew.c")
		target_include_directories(external-glew SYSTEM PUBLIC ${GLEW_DIR}/include)
		if(TARGET OpenGL::GL)
			target_link_libraries(external-glew PUBLIC OpenGL::GL)
		elseif(TARGET OpenGL::OpenGL)
			target_link_libraries(external-glew PUBLIC OpenGL::OpenGL)
		else()
			message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
		endif()

		target_link_libraries(external-glew PRIVATE external-disable-warnings)
		install(TARGETS external-glew DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
	endif()
else()
	message(
		FATAL_ERROR
			"GLEW is required - at least one of MESHLAB_ALLOW_SYSTEM_GLEW or MESHLAB_ALLOW_BUNDLED_SOURCE_GLEW must be ON and found.")
endif()
