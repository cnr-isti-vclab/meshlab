set(SOURCES
	ioraster_base.cpp
	${EXTERNAL_DIR}/easyexif/exif.cpp)

set(HEADERS
	ioraster_base.h)

add_library(ioraster_base MODULE ${SOURCES} ${HEADERS})

target_include_directories(ioraster_base PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${EXTERNAL_DIR}/easyexif/)

target_link_libraries(ioraster_base PUBLIC meshlab-common)

target_link_libraries(ioraster_base PRIVATE OpenGL::GLU)

set_property(TARGET ioraster_base PROPERTY FOLDER Plugins)

set_property(TARGET ioraster_base PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET ioraster_base PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                     ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS ioraster_base
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
