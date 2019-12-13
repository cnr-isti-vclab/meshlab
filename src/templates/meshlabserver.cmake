{% extends "CMakeLists.template.cmake" %}
{% block add_target %}
list(APPEND RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../meshlab/meshlab.qrc)
if(WIN32)
    list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../meshlab/meshlab.rc)
endif()

add_executable({{name}}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
    ${UI}
)
{% endblock %}

{% block linking %}
target_compile_definitions({{name}}
    PUBLIC
    QT_DISABLE_DEPRECATED_BEFORE=0x000000
    NO_XSERVER_DEPENDENCY)
target_include_directories({{name}} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries({{name}}
    PUBLIC
    common
    Qt5::Network)
{% endblock %}

{% block output_dir %}{% endblock %}
{% block install %}

install(TARGETS {{name}}
    DESTINATION ${MESHLAB_BIN_INSTALL_DIR} COMPONENT MeshLab-Server)

{% endblock %}
{% block targetfolder %}Core{% endblock %}
