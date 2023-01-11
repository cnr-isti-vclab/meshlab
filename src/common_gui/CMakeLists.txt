set(HEADERS
	gui_utils/clickable_label.h
	gui_utils/vertical_scroll_area.h

	rich_parameter/widgets/bool_widget.h
	rich_parameter/widgets/color_widget.h
	rich_parameter/widgets/combo_widget.h
	rich_parameter/widgets/direction_widget.h
	rich_parameter/widgets/dynamic_float_widget.h
	rich_parameter/widgets/enum_widget.h
	rich_parameter/widgets/float_widget.h
	rich_parameter/widgets/int_widget.h
	rich_parameter/widgets/io_file_widget.h
	rich_parameter/widgets/line_edit_widget.h
	rich_parameter/widgets/matrix44_widget.h
	rich_parameter/widgets/mesh_widget.h
	rich_parameter/widgets/open_file_widget.h
	rich_parameter/widgets/percentage_widget.h
	rich_parameter/widgets/point3_widget.h
	rich_parameter/widgets/position_widget.h
	rich_parameter/widgets/rich_parameter_widget.h
	rich_parameter/widgets/rich_parameter_widgets.h
	rich_parameter/widgets/save_file_widget.h
	rich_parameter/widgets/shot_widget.h
	rich_parameter/widgets/string_widget.h

	rich_parameter/richparameterlistdialog.h
	rich_parameter/rich_parameter_list_frame.h
)

set(SOURCES
	gui_utils/clickable_label.cpp
	gui_utils/vertical_scroll_area.cpp

	rich_parameter/widgets/bool_widget.cpp
	rich_parameter/widgets/color_widget.cpp
	rich_parameter/widgets/combo_widget.cpp
	rich_parameter/widgets/direction_widget.cpp
	rich_parameter/widgets/dynamic_float_widget.cpp
	rich_parameter/widgets/enum_widget.cpp
	rich_parameter/widgets/float_widget.cpp
	rich_parameter/widgets/int_widget.cpp
	rich_parameter/widgets/io_file_widget.cpp
	rich_parameter/widgets/line_edit_widget.cpp
	rich_parameter/widgets/matrix44_widget.cpp
	rich_parameter/widgets/mesh_widget.cpp
	rich_parameter/widgets/open_file_widget.cpp
	rich_parameter/widgets/percentage_widget.cpp
	rich_parameter/widgets/point3_widget.cpp
	rich_parameter/widgets/position_widget.cpp
	rich_parameter/widgets/rich_parameter_widget.cpp
	rich_parameter/widgets/save_file_widget.cpp
	rich_parameter/widgets/shot_widget.cpp
	rich_parameter/widgets/string_widget.cpp

	rich_parameter/richparameterlistdialog.cpp
	rich_parameter/rich_parameter_list_frame.cpp
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

set_target_properties(meshlab-common-gui PROPERTIES
	INSTALL_RPATH "$ORIGIN")

install(TARGETS meshlab-common-gui DESTINATION ${MESHLAB_LIB_INSTALL_DIR})
