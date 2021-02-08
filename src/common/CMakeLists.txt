if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION")
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION" MESHLAB_VERSION)
	if (BUILD_WITH_DOUBLE_SCALAR)
		set(MESHLAB_VERSION "${MESHLAB_VERSION}d")
	endif()
	add_definitions(-DMESHLAB_VERSION=${MESHLAB_VERSION})
	message(STATUS "MeshLab version: ${MESHLAB_VERSION}")
endif()

set(HEADERS
	globals.h
	plugins/interfaces/plugin_file_interface.h
	plugins/interfaces/decorate_plugin_interface.h
	plugins/interfaces/edit_plugin_interface.h
	plugins/interfaces/filter_plugin_interface.h
	plugins/interfaces/iomesh_plugin_interface.h
	plugins/interfaces/ioraster_plugin_interface.h
	plugins/interfaces/plugin_interface.h
	plugins/interfaces/render_plugin_interface.h
	plugins/plugin_manager_iterators.h
	plugins/plugin_manager.h
	ml_document/helpers/mesh_document_state_data.h
	ml_document/helpers/mesh_model_state_data.h
	ml_document/base_types.h
	ml_document/cmesh.h
	ml_document/mesh_document.h
	ml_document/mesh_model.h
	ml_document/mesh_model_state.h
	ml_document/raster_model.h
	ml_document/render_raster.h
	parameters/rich_parameter.h
	parameters/rich_parameter_list.h
	parameters/value.h
	utilities/file_format.h
	GLExtensionsManager.h
	GLLogStream.h
	filterscript.h
	mainwindow_interface.h
	meshlabdocumentbundler.h
	meshlabdocumentxml.h
	ml_selection_buffers.h
	ml_shared_data_context.h
	ml_thread_safe_memory_info.h
	mlapplication.h
	mlexception.h
	searcher.h)

set(SOURCES
	globals.cpp
	plugins/interfaces/decorate_plugin_interface.cpp
	plugins/interfaces/filter_plugin_interface.cpp
	plugins/interfaces/plugin_interface.cpp
	plugins/plugin_manager.cpp
	ml_document/helpers/mesh_document_state_data.cpp
	ml_document/cmesh.cpp
	ml_document/mesh_document.cpp
	ml_document/mesh_model.cpp
	ml_document/mesh_model_state.cpp
	ml_document/raster_model.cpp
	ml_document/render_raster.cpp
	parameters/rich_parameter.cpp
	parameters/rich_parameter_list.cpp
	parameters/value.cpp
	GLExtensionsManager.cpp
	GLLogStream.cpp
	filterscript.cpp
	meshlabdocumentbundler.cpp
	meshlabdocumentxml.cpp
	ml_selection_buffers.cpp
	ml_shared_data_context.cpp
	ml_thread_safe_memory_info.cpp
	mlapplication.cpp
	searcher.cpp
	${EXTERNAL_DIR}/easyexif/exif.cpp)

set(TARGET_TYPE SHARED)
if(WIN32)
	set(TARGET_TYPE STATIC)
endif()
add_library(meshlab-common ${TARGET_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(meshlab-common
	PRIVATE
		${EXTERNAL_DIR}/easyexif/
	PUBLIC
		${VCGDIR}
		${EIGEN_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(
	meshlab-common
	PUBLIC 
		Qt5::Core
		Qt5::OpenGL
		Qt5::Widgets
		Qt5::Xml
		external-glew
)

set_property(TARGET meshlab-common PROPERTY FOLDER Core)

set_property(TARGET meshlab-common
	PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

set_property(TARGET meshlab-common
	PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

if (WIN32)
	set_property(TARGET meshlab-common
		PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})
endif()

install(TARGETS meshlab-common DESTINATION ${MESHLAB_LIB_INSTALL_DIR})

