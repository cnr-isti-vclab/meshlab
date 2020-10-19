# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/meshlab.cmake (custom for this directory),
### then re-run ./make-cmake.py

set(SOURCES
	additionalgui.cpp
	changetexturename.cpp
	filterScriptDialog.cpp
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
	plugindialog.cpp
	saveSnapshotDialog.cpp
	savemaskexporter.cpp
	rich_parameter_gui/richparameterlistdialog.cpp
	rich_parameter_gui/richparameterlistframe.cpp
	rich_parameter_gui/richparameterwidgets.cpp
	meshlab_settings/settingdialog.cpp
	meshlab_settings/meshlabsettingsdialog.cpp
	${VCGDIR}/wrap/gui/trackball.cpp
	${VCGDIR}/wrap/gui/trackmode.cpp
	${VCGDIR}/wrap/gui/coordinateframe.cpp
	${EXTERNAL_DIR}/easyexif/exif.cpp)

set(HEADERS
	additionalgui.h
	changetexturename.h
	filterScriptDialog.h
	glarea.h
	glarea_setting.h
	layerDialog.h
	mainwindow.h
	meshlabeditdockwidget.h
	ml_default_decorators.h
	ml_render_gui.h
	ml_rendering_actions.h
	ml_std_par_dialog.h
	multiViewer_Container.h
	plugindialog.h
	saveSnapshotDialog.h
	savemaskexporter.h
	snapshotsetting.h
	rich_parameter_gui/richparameterlistdialog.h
	rich_parameter_gui/richparameterlistframe.h
	rich_parameter_gui/richparameterwidgets.h
	meshlab_settings/settingdialog.h
	meshlab_settings/meshlabsettingsdialog.h
	${VCGDIR}/wrap/gui/trackball.h
	${VCGDIR}/wrap/gui/trackmode.h
	${VCGDIR}/wrap/gl/trimesh.h)

set(RESOURCES meshlab.qrc)

set(UI
	ui/aboutDialog.ui
	ui/congratsDialog.ui
	ui/filterScriptDialog.ui
	ui/filtercreatortab.ui
	ui/filtergui.ui
	ui/layerDialog.ui
	ui/renametexture.ui
	ui/savemaskexporter.ui
	ui/savesnapshotDialog.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

if(WIN32)
	list(APPEND SOURCES meshlab.rc)
endif()

add_executable(meshlab WIN32 MACOSX_BUNDLE
	${SOURCES} ${HEADERS} ${RESOURCES}
	${UI})

target_compile_definitions(meshlab PUBLIC QT_DISABLE_DEPRECATED_BEFORE=0x000000)
target_include_directories(meshlab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${EXTERNAL_DIR}/easyexif/)
target_link_libraries(
	meshlab
	PUBLIC common OpenGL::GLU Qt5::Network
	)

set_property(TARGET meshlab PROPERTY FOLDER Core)
if (APPLE)
	set_target_properties(meshlab PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
	set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/images/meshlab.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images/meshlab.icns DESTINATION "${MESHLAB_BUILD_DISTRIB_DIR}/meshlab.app/Contents/Resources/")
	set_target_properties(meshlab PROPERTIES MACOSX_BUNDLE_ICONFILE meshlab.icns)
endif()

install(
	TARGETS meshlab
	DESTINATION ${MESHLAB_BIN_INSTALL_DIR}
	COMPONENT MeshLab)
