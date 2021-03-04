set(SOURCES
	ioraster_base.cpp
	${EXTERNAL_DIR}/easyexif/exif.cpp)

set(HEADERS
	ioraster_base.h)

add_meshlab_plugin(ioraster_base ${SOURCES} ${HEADERS})

target_include_directories(ioraster_base PRIVATE ${EXTERNAL_DIR}/easyexif/)

target_link_libraries(ioraster_base PRIVATE OpenGL::GLU)
