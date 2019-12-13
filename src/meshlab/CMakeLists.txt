# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

set(SOURCES
    additionalgui.cpp
    changetexturename.cpp
    customDialog.cpp
    filterScriptDialog.cpp
    filterthread.cpp
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
    stdpardialog.cpp
    xmlgeneratorgui.cpp
    xmlstdpardialog.cpp
    ${VCGDIR}/wrap/gui/trackball.cpp
    ${VCGDIR}/wrap/gui/trackmode.cpp
    ${VCGDIR}/wrap/gui/coordinateframe.cpp)

set(HEADERS
    additionalgui.h
    alnParser.h
    changetexturename.h
    customDialog.h
    filterScriptDialog.h
    filterthread.h
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
    stdpardialog.h
    xmlgeneratorgui.h
    xmlstdpardialog.h
    ${VCGDIR}/wrap/gui/trackball.h
    ${VCGDIR}/wrap/gui/trackmode.h
    ${VCGDIR}/wrap/gl/trimesh.h)

set(RESOURCES meshlab.qrc)

set(UI
    ui/aboutDialog.ui
    ui/congratsDialog.ui
    ui/customDialog.ui
    ui/filterScriptDialog.ui
    ui/filtercreatortab.ui
    ui/filtergui.ui
    ui/layerDialog.ui
    ui/renametexture.ui
    ui/savemaskexporter.ui
    ui/savesnapshotDialog.ui)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
if(APPLE)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../install/macx/info.plist
                images/meshlab.icns)
endif()

if(WIN32)
    list(APPEND SOURCES meshlab.rc)
endif()

add_executable(meshlab WIN32 MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${RESOURCES}
                                           ${UI})

target_compile_definitions(meshlab PUBLIC QT_DISABLE_DEPRECATED_BEFORE=0x000000)
target_include_directories(meshlab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(
    meshlab
    PUBLIC common OpenGL::GLU Qt5::Network
    PRIVATE external-jhead)

set_property(TARGET meshlab PROPERTY FOLDER Core)

install(TARGETS meshlab DESTINATION ${MESHLAB_BIN_INSTALL_DIR} COMPONENT
                MeshLab)

if(Qt5_windeployqt_EXECUTABLE AND BUILD_WITH_WINDEPLOYQT_POST_BUILD)
    add_custom_command(
        TARGET meshlab POST_BUILD
        COMMAND "${Qt5_windeployqt_EXECUTABLE}" --no-translations
                $<TARGET_FILE_NAME:meshlab>
        WORKING_DIRECTORY $<TARGET_FILE_DIR:meshlab>
        COMMENT "Deploying Qt to the build directory of meshlab"
        VERBATIM USES_TERMINAL)
endif()
