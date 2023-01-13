#version management
string(TIMESTAMP MESHLAB_VERSION "%Y.%m")

if (MESHLAB_IS_NIGHTLY_VERSION)
	# nightly version is in the form:
	# YYYY.mm[d]_nightly_GIT_SHA1
	# YYYY and mm are computed by cmake and not read from file
	# the GIT_SHA1 is stored in file, which will be read at runtime
	find_package(Git)
	execute_process(COMMAND
		"${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=7
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_SHA1
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)
	file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/resources/git_sha.txt ${GIT_SHA1})
else()
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION")
		# YYYY and mm are read from file
		file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION" MESHLAB_VERSION)
	endif()
endif()

if (MESHLAB_BUILD_WITH_DOUBLE_SCALAR)
	set(MESHLAB_VERSION "${MESHLAB_VERSION}d")
endif()


set(MESHLAB_VERSION ${MESHLAB_VERSION} PARENT_SCOPE)
if (MESHLAB_IS_NIGHTLY_VERSION)
	message(STATUS "MeshLab version: ${MESHLAB_VERSION}_nightly_${GIT_SHA1}")
else()
	message(STATUS "MeshLab version: ${MESHLAB_VERSION}")
endif()

if (MESHLAB_BUILD_WITH_DOUBLE_SCALAR)
	message(STATUS "Building with double precision")
	set(MESHLAB_SCALAR "double")
else()
	message(STATUS "Building with single precision")
	set(MESHLAB_SCALAR "float")
endif()

set(HEADERS
	filter_history/filter.h
	filter_history/filter_history.h
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
	parameters/rich_parameters.h
	parameters/values.h
	parameters/rich_parameter_list.h
	parameters/rich_parameter/rich_bool.h
	parameters/rich_parameter/rich_color.h
	parameters/rich_parameter/rich_direction.h
	parameters/rich_parameter/rich_dynamic_float.h
	parameters/rich_parameter/rich_enum.h
	parameters/rich_parameter/rich_file_open.h
	parameters/rich_parameter/rich_file_save.h
	parameters/rich_parameter/rich_float.h
	parameters/rich_parameter/rich_int.h
	parameters/rich_parameter/rich_matrix44.h
	parameters/rich_parameter/rich_mesh.h
	parameters/rich_parameter/rich_parameter.h
	parameters/rich_parameter/rich_percentage.h
	parameters/rich_parameter/rich_position.h
	parameters/rich_parameter/rich_shot.h
	parameters/rich_parameter/rich_string.h
	parameters/value/bool_value.h
	parameters/value/color_value.h
	parameters/value/float_value.h
	parameters/value/int_value.h
	parameters/value/matrix44_value.h
	parameters/value/point3_value.h
	parameters/value/shot_value.h
	parameters/value/string_value.h
	parameters/value/value.h
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
	plugins/action_searcher.h
	plugins/meshlab_plugin_type.h
	plugins/plugin_manager.h
	python/function.h
	python/function_parameter.h
	python/function_set.h
	python/python_utils.h
	utilities/eigen_mesh_conversions.h
	utilities/file_format.h
	utilities/load_save.h
	globals.h
	GLExtensionsManager.h
	GLLogStream.h
	filterscript.h
	ml_selection_buffers.h
	ml_thread_safe_memory_info.h
	mlapplication.h
	mlexception.h)

set(SOURCES
	filter_history/filter.cpp
	filter_history/filter_history.cpp
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
	parameters/rich_parameters.cpp
	parameters/rich_parameter_list.cpp
	parameters/rich_parameter/rich_bool.cpp
	parameters/rich_parameter/rich_color.cpp
	parameters/rich_parameter/rich_direction.cpp
	parameters/rich_parameter/rich_dynamic_float.cpp
	parameters/rich_parameter/rich_enum.cpp
	parameters/rich_parameter/rich_file_open.cpp
	parameters/rich_parameter/rich_file_save.cpp
	parameters/rich_parameter/rich_float.cpp
	parameters/rich_parameter/rich_int.cpp
	parameters/rich_parameter/rich_matrix44.cpp
	parameters/rich_parameter/rich_mesh.cpp
	parameters/rich_parameter/rich_parameter.cpp
	parameters/rich_parameter/rich_percentage.cpp
	parameters/rich_parameter/rich_position.cpp
	parameters/rich_parameter/rich_shot.cpp
	parameters/rich_parameter/rich_string.cpp
	parameters/value/bool_value.cpp
	parameters/value/color_value.cpp
	parameters/value/float_value.cpp
	parameters/value/int_value.cpp
	parameters/value/matrix44_value.cpp
	parameters/value/point3_value.cpp
	parameters/value/shot_value.cpp
	parameters/value/string_value.cpp
	plugins/containers/decorate_plugin_container.cpp
	plugins/containers/edit_plugin_container.cpp
	plugins/containers/filter_plugin_container.cpp
	plugins/containers/io_plugin_container.cpp
	plugins/containers/render_plugin_container.cpp
	plugins/interfaces/meshlab_plugin_logger.cpp
	plugins/interfaces/decorate_plugin.cpp
	plugins/interfaces/filter_plugin.cpp
	plugins/interfaces/io_plugin.cpp
	plugins/action_searcher.cpp
	plugins/meshlab_plugin_type.cpp
	plugins/plugin_manager.cpp
	python/function.cpp
	python/function_parameter.cpp
	python/function_set.cpp
	python/python_utils.cpp
	utilities/eigen_mesh_conversions.cpp
	utilities/load_save.cpp
	globals.cpp
	GLExtensionsManager.cpp
	GLLogStream.cpp
	filterscript.cpp
	ml_selection_buffers.cpp
	ml_thread_safe_memory_info.cpp
	mlapplication.cpp)

set(RESOURCES meshlab-common.qrc)

set(TARGET_TYPE SHARED)
add_library(meshlab-common ${TARGET_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

target_compile_definitions(meshlab-common
	PUBLIC
		MESHLAB_VERSION=${MESHLAB_VERSION}
		MESHLAB_SCALAR=${MESHLAB_SCALAR})

if (MESHLAB_IS_NIGHTLY_VERSION)
	target_compile_definitions(meshlab-common
		PUBLIC
			MESHLAB_IS_NIGHTLY)
endif()

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
		external-easyexif
)

set_property(TARGET meshlab-common PROPERTY FOLDER Core)

set_property(TARGET meshlab-common
	PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

set_property(TARGET meshlab-common
	PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

if (WIN32)
	target_compile_definitions(meshlab-common PRIVATE ML_EXPORT_SYMBOLS)
	set_property(TARGET meshlab-common
		PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})
endif()

if(MESHLAB_ENABLE_DEBUG_LOG_FILE)
	target_compile_definitions(meshlab-common PUBLIC MESHLAB_LOG_FILE_ENABLED)
endif()

set_target_properties(meshlab-common PROPERTIES
	INSTALL_RPATH "$ORIGIN")

install(TARGETS meshlab-common DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
