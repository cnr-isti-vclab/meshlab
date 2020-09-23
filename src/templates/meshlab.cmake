{% extends "CMakeLists.template.cmake" %}

{% block sources %}
{{ super() }}
    ${VCGDIR}/wrap/gui/trackball.cpp
    ${VCGDIR}/wrap/gui/trackmode.cpp
    ${VCGDIR}/wrap/gui/coordinateframe.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ${VCGDIR}/wrap/gui/trackball.h
    ${VCGDIR}/wrap/gui/trackmode.h 
    ${VCGDIR}/wrap/gl/trimesh.h
{% endblock %}

{% block add_target %}
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)
#if(APPLE)
#    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../install/macx/info.plist
#                images/meshlab.icns)
#endif()

if(WIN32)
    list(APPEND SOURCES meshlab.rc)
endif()

add_executable({{name}} WIN32 MACOSX_BUNDLE
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ${UI}
)
{% endblock %}

{% block linking %}
target_compile_definitions({{name}}
    PUBLIC
    QT_DISABLE_DEPRECATED_BEFORE=0x000000)
target_include_directories({{name}} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries({{name}}
    PUBLIC
    common
    OpenGL::GLU
    Qt5::Network)

{% endblock %}
{% block install %}

install(TARGETS {{name}}
    DESTINATION ${MESHLAB_BIN_INSTALL_DIR} COMPONENT MeshLab)

if(Qt5_windeployqt_EXECUTABLE AND BUILD_WITH_WINDEPLOYQT_POST_BUILD)
    add_custom_command(TARGET {{name}}
        POST_BUILD
        COMMAND
        "${Qt5_windeployqt_EXECUTABLE}" --no-translations $<TARGET_FILE_NAME:{{name}}>
        WORKING_DIRECTORY $<TARGET_FILE_DIR:{{name}}>
        COMMENT "Deploying Qt to the build directory of {{name}}"
        VERBATIM USES_TERMINAL)
endif()
{% endblock %}
{% block output_dir %}{% endblock %}
{% block targetfolder %}Core{% endblock %}
