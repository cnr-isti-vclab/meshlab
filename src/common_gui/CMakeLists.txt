set(HEADERS
	gui_utils/clickable_label.h
	gui_utils/vertical_scroll_area.h
	rich_parameter_gui/richparameterlistdialog.h
	rich_parameter_gui/richparameterlistframe.h
	rich_parameter_gui/richparameterwidgets.h
)

set(SOURCES
	gui_utils/clickable_label.cpp
	gui_utils/vertical_scroll_area.cpp
	rich_parameter_gui/richparameterlistdialog.cpp
	rich_parameter_gui/richparameterlistframe.cpp
	rich_parameter_gui/richparameterwidgets.cpp
)

set(RESOURCES )

set(TARGET_TYPE SHARED)
add_library(meshlab-common-gui ${TARGET_TYPE} ${SOURCES} ${HEADERS} ${RESOURCES})

target_link_libraries(
	meshlab-common-gui
	PUBLIC
		meshlab-common
)

set_property(TARGET meshlab-common-gui PROPERTY FOLDER Core)

set_property(TARGET meshlab-common-gui
	PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

set_property(TARGET meshlab-common-gui
	PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_LIB_OUTPUT_DIR})

set_target_properties(meshlab-common-gui PROPERTIES
	INSTALL_RPATH "${MESHLAB_LIB_INSTALL_DIR}:${INSTALL_RPATH}")

install(TARGETS meshlab-common-gui DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
