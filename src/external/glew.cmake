# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_GLEW "Allow use of bundled GLEW source" ON)
option(ALLOW_SYSTEM_GLEW "Allow use of system-provided GLEW" ON)

set(GLEW_DIR ${CMAKE_CURRENT_LIST_DIR}/glew-2.1.0)

unset(HAVE_SYSTEM_GLEW)
if(DEFINED GLEW_VERSION)
	if((TARGET GLEW::GLEW) AND (${GLEW_VERSION} VERSION_GREATER_EQUAL "2.0.0"))
		set(HAVE_SYSTEM_GLEW TRUE)
	endif()
endif()

if(ALLOW_SYSTEM_GLEW AND HAVE_SYSTEM_GLEW)
	message(STATUS "- glew - using system-provided library")
	add_library(external-glew INTERFACE)
	target_link_libraries(external-glew INTERFACE GLEW::GLEW)
	if(TARGET OpenGL::OpenGL)
		target_link_libraries(external-glew INTERFACE OpenGL::OpenGL)
	elseif(TARGET OpenGL::GL)
		target_link_libraries(external-glew INTERFACE OpenGL::GL)
	else()
		message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
	endif()
elseif(ALLOW_BUNDLED_GLEW AND EXISTS "${GLEW_DIR}/src/glew.c")
	message(STATUS "- glew - using bundled source")
	add_library(external-glew STATIC "${GLEW_DIR}/src/glew.c")
	target_compile_definitions(external-glew PUBLIC GLEW_STATIC)
	target_include_directories(external-glew SYSTEM PUBLIC ${GLEW_DIR}/include)
	if(TARGET OpenGL::OpenGL)
		target_link_libraries(external-glew PUBLIC OpenGL::OpenGL)
	elseif(TARGET OpenGL::GL)
		target_link_libraries(external-glew PUBLIC OpenGL::GL)
	else()
		message(FATAL_ERROR "OpenGL not found or your CMake version is too old!")
	endif()
	if(TARGET OpenGL::GLX)
		target_link_libraries(external-glew PUBLIC OpenGL::GLX)
	endif()
	set_property(TARGET external-glew PROPERTY FOLDER External)

	target_link_libraries(external-glew PRIVATE external-disable-warnings)
else()
	message(
		FATAL_ERROR
			"GLEW is required - at least one of ALLOW_SYSTEM_GLEW or ALLOW_BUNDLED_GLEW must be enabled and found.")
endif()
