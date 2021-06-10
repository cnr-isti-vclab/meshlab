#version management
string(TIMESTAMP MESHLAB_VERSION "%Y.%m")

if (MESHLAB_IS_NIGHTLY_VERSION)
	# nightly version is in the form:
	# YYYY.mm[d]_nightly_GIT_SHA1
	# YYYY and mm are computed by cmake and not read from file
	find_package(Git)
	execute_process(COMMAND
		"${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=7
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_SHA1
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	if (BUILD_WITH_DOUBLE_SCALAR)
		set(MESHLAB_VERSION "${MESHLAB_VERSION}d")
	endif()
	set(MESHLAB_VERSION "${MESHLAB_VERSION}_nightly_${GIT_SHA1}")
else()
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION")
		# YYYY and mm are read from file
		file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION" MESHLAB_VERSION)
	endif()
	# if double precision version, "d" is appended
	if (BUILD_WITH_DOUBLE_SCALAR)
		set(MESHLAB_VERSION "${MESHLAB_VERSION}d")
	endif()
endif()
message(STATUS "MeshLab version: ${MESHLAB_VERSION}")

if (BUILD_WITH_DOUBLE_SCALAR)
	message(STATUS "Building with double precision")
	set(MESHLAB_SCALAR "double")
else()
	message(STATUS "Building with single precision")
	set(MESHLAB_SCALAR "float")
endif()

set(HEADERS
	ml_document/helpers/mesh_document_state_data.h
	ml_document/helpers/mesh_model_state_data.h
	ml_document/base_types.h
	ml_document/cmesh.h
	ml_document/mesh_document.h
	ml_document/mesh_model.h
	ml_document/mesh_model_state.h
	ml_document/raster_model.h
	ml_document/render_raster.h
	ml_shared_data_context/ml_plugin_gl_context.h
	ml_shared_data_context/ml_scene_gl_shared_data_context.h
	ml_shared_data_context/ml_shared_data_context.h
	parameters/rich_parameter.h
	parameters/rich_parameter_list.h
	parameters/value.h
	plugins/containers/generic_container_iterator.h
	plugins/containers/decorate_plugin_container.h
	plugins/containers/edit_plugin_container.h
	plugins/containers/filter_plugin_container.h
	plugins/containers/io_plugin_container.h
	plugins/containers/render_plugin_container.h
	plugins/interfaces/meshlab_plugin.h
	plugins/interfaces/meshlab_plugin_logger.h
	plugins/interfaces/decorate_plugin.h
	plugins/interfaces/edit_plugin.h
	plugins/interfaces/filter_plugin.h
	plugins/interfaces/io_plugin.h
	plugins/interfaces/render_plugin.h
	plugins/meshlab_plugin_type.h
	plugins/plugin_manager.h
	python/function.h
	python/function_parameter.h
	python/function_set.h
	python/python_utils.h
	utilities/file_format.h
	utilities/load_save.h
	globals.h
	GLExtensionsManager.h
	GLLogStream.h
	filterscript.h
	ml_selection_buffers.h
	ml_thread_safe_memory_info.h
	mlapplication.h
	mlexception.h
	searcher.h)

set(SOURCES
	ml_document/helpers/mesh_document_state_data.cpp
	ml_document/cmesh.cpp
	ml_document/mesh_document.cpp
	ml_document/mesh_model.cpp
	ml_document/mesh_model_state.cpp
	ml_document/raster_model.cpp
	ml_document/render_raster.cpp
	ml_shared_data_context/ml_plugin_gl_context.cpp
	ml_shared_data_context/ml_scene_gl_shared_data_context.cpp
	ml_shared_data_context/ml_shared_data_context.cpp
	parameters/rich_parameter.cpp
	parameters/rich_parameter_list.cpp
	parameters/value.cpp
	plugins/containers/decorate_plugin_container.cpp
	plugins/containers/edit_plugin_container.cpp
	plugins/containers/filter_plugin_container.cpp
	plugins/containers/io_plugin_container.cpp
	plugins/containers/render_plugin_container.cpp
	plugins/interfaces/meshlab_plugin_logger.cpp
	plugins/interfaces/decorate_plugin.cpp
	plugins/interfaces/filter_plugin.cpp
	plugins/interfaces/io_plugin.cpp
	plugins/meshlab_plugin_type.cpp
	plugins/plugin_manager.cpp
	python/function.cpp
	python/function_parameter.cpp
	python/function_set.cpp
	python/python_utils.cpp
	utilities/load_save.cpp
	globals.cpp
	GLExtensionsManager.cpp
	GLLogStream.cpp
	filterscript.cpp
	ml_selection_buffers.cpp
	ml_thread_safe_memory_info.cpp
	mlapplication.cpp
	searcher.cpp)

set(RESOURCES meshlab-common.qrc)

set(TARGET_TYPE SHARED)
add_library(meshlab-common ${TARGET_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

target_compile_definitions(meshlab-common
	PUBLIC
		MESHLAB_VERSION=${MESHLAB_VERSION}
		MESHLAB_SCALAR=${MESHLAB_SCALAR})

target_include_directories(meshlab-common
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(
	meshlab-common
	PUBLIC
		Qt5::Core
		Qt5::OpenGL
		Qt5::Widgets
		Qt5::Xml
		Qt5::Network
		vcglib
		external-glew
		external-exif
)

set_property(TARGET meshlab-common PROPERTY FOLDER Core)

set_property(TARGET meshlab-common
	PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

set_property(TARGET meshlab-common
	PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

if (WIN32)
	add_compile_definitions(meshlab-common PRIVATE ML_EXPORT_SYMBOLS)
	set_property(TARGET meshlab-common
		PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})
endif()

set_target_properties(meshlab-common PROPERTIES
	INSTALL_RPATH "${MESHLAB_LIB_INSTALL_DIR}:${INSTALL_RPATH}")

install(TARGETS meshlab-common DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
