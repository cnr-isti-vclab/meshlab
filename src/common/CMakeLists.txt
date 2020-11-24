if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION")
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../../ML_VERSION" MESHLAB_VERSION)
	if (BUILD_WITH_DOUBLE_SCALAR)
		set(MESHLAB_VERSION "${MESHLAB_VERSION}d")
	endif()
	add_definitions(-DMESHLAB_VERSION=${MESHLAB_VERSION})
	message(STATUS "MeshLab version: ${MESHLAB_VERSION}")
endif()

set(SOURCES
	filter_parameter/rich_parameter.cpp
	filter_parameter/rich_parameter_list.cpp
	filter_parameter/value.cpp
	interfaces/decorate_plugin_interface.cpp
	interfaces/filter_plugin_interface.cpp
	interfaces/plugin_interface.cpp
	mesh_data_structures/cmesh.cpp
	mesh_data_structures/mesh_document.cpp
	mesh_data_structures/mesh_model.cpp
	mesh_data_structures/raster_model.cpp
	mesh_data_structures/render_raster.cpp
	GLExtensionsManager.cpp
	GLLogStream.cpp
	filterscript.cpp
	meshlabdocumentbundler.cpp
	meshlabdocumentxml.cpp
	ml_selection_buffers.cpp
	ml_shared_data_context.cpp
	ml_thread_safe_memory_info.cpp
	mlapplication.cpp
	pluginmanager.cpp
	searcher.cpp
	${EXTERNAL_DIR}/easyexif/exif.cpp)

set(HEADERS
	filter_parameter/rich_parameter.h
	filter_parameter/rich_parameter_list.h
	filter_parameter/value.h
	interfaces/decorate_plugin_interface.h
	interfaces/edit_plugin_interface.h
	interfaces/filter_plugin_interface.h
	interfaces/iomesh_plugin_interface.h
	interfaces/mainwindow_interface.h
	interfaces/plugin_interface.h
	interfaces/render_plugin_interface.h
	mesh_data_structures/cmesh.h
	mesh_data_structures/mesh_document.h
	mesh_data_structures/mesh_model.h
	mesh_data_structures/raster_model.h
	mesh_data_structures/render_raster.h
	GLExtensionsManager.h
	GLLogStream.h
	filterscript.h
	meshlabdocumentbundler.h
	meshlabdocumentxml.h
	ml_selection_buffers.h
	ml_shared_data_context.h
	ml_thread_safe_memory_info.h
	mlapplication.h
	mlexception.h
	pluginmanager.h
	searcher.h)

set(TARGET_TYPE SHARED)
if(WIN32)
	set(TARGET_TYPE STATIC)
endif()
add_library(meshlab-common ${TARGET_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(meshlab-common PRIVATE ${EXTERNAL_DIR}/easyexif/)
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

