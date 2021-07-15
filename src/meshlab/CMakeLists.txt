# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

set(SOURCES
	additionalgui.cpp
	glarea.cpp
	glarea_setting.cpp
	layerDialog.cpp
	main.cpp
	mainwindow_Init.cpp
	mainwindow_RunTime.cpp
	ml_default_decorators.cpp
	ml_render_gui.cpp
	ml_rendering_actions.cpp
	ml_std_par_dialog.cpp
	multiViewer_Container.cpp
	dialogs/about_dialog.cpp
	dialogs/congrats_dialog.cpp
	dialogs/filter_script_dialog.cpp
	dialogs/options_dialog.cpp
	dialogs/plugin_info_dialog.cpp
	dialogs/save_mesh_attributes_dialog.cpp
	dialogs/save_snapshot_dialog.cpp
	dialogs/setting_dialog.cpp
	rich_parameter_gui/richparameterlistdialog.cpp
	rich_parameter_gui/richparameterlistframe.cpp
	rich_parameter_gui/richparameterwidgets.cpp
	${VCGDIR}/wrap/gui/trackball.cpp
	${VCGDIR}/wrap/gui/trackmode.cpp
	${VCGDIR}/wrap/gui/coordinateframe.cpp)

set(HEADERS
	additionalgui.h
	glarea.h
	glarea_setting.h
	layerDialog.h
	mainwindow.h
	ml_default_decorators.h
	ml_render_gui.h
	ml_rendering_actions.h
	ml_std_par_dialog.h
	multiViewer_Container.h
	snapshotsetting.h
	dialogs/about_dialog.h
	dialogs/congrats_dialog.h
	dialogs/filter_script_dialog.h
	dialogs/options_dialog.h
	dialogs/plugin_info_dialog.h
	dialogs/save_mesh_attributes_dialog.h
	dialogs/save_snapshot_dialog.h
	dialogs/setting_dialog.h
	rich_parameter_gui/richparameterlistdialog.h
	rich_parameter_gui/richparameterlistframe.h
	rich_parameter_gui/richparameterwidgets.h
	${VCGDIR}/wrap/gui/trackball.h
	${VCGDIR}/wrap/gui/trackmode.h
	${VCGDIR}/wrap/gl/trimesh.h)

set(RESOURCES meshlab.qrc)

set(UI
	dialogs/about_dialog.ui
	dialogs/congrats_dialog.ui
	dialogs/filter_script_dialog.ui
	dialogs/plugin_info_dialog.ui
	dialogs/save_mesh_attributes_dialog.ui
	dialogs/save_snapshot_dialog.ui
	ui/layerDialog.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

if(WIN32)
	list(APPEND SOURCES meshlab.rc)
endif()

add_executable(meshlab WIN32 MACOSX_BUNDLE
	${SOURCES} ${HEADERS} ${RESOURCES}
	${UI})

target_compile_definitions(meshlab PUBLIC QT_DISABLE_DEPRECATED_BEFORE=0x000000)
target_include_directories(meshlab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
	meshlab
	PUBLIC meshlab-common OpenGL::GLU Qt5::Network
	)

set_property(TARGET meshlab PROPERTY FOLDER Core)
if (APPLE)
	set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/images/meshlab.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images/meshlab.icns DESTINATION "${MESHLAB_BUILD_DISTRIB_DIR}/meshlab.app/Contents/Resources/")
	set_target_properties(meshlab PROPERTIES 
		MACOSX_BUNDLE_ICON_FILE meshlab.icns
		MACOSX_BUNDLE_BUNDLE_VERSION "${MESHLAB_VERSION}"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "${MESHLAB_VERSION}"
		MACOSX_BUNDLE_INFO_STRING "MeshLab ${MESHLAB_VERSION}"
		MACOSX_BUNDLE_COPYRIGHT "Copyright VCG-ISTI-CNR © 2005-2021. All rights reserved."
		)
	
	set_additional_settings_info_plist(
		TARGET meshlab
		FILE ${MESHLAB_BUILD_DISTRIB_DIR}/meshlab.app/Contents/Info.plist)
endif()

install(
	TARGETS meshlab
	DESTINATION ${MESHLAB_BIN_INSTALL_DIR}
	COMPONENT MeshLab)
