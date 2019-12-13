{% extends "CMakeLists.template.cmake" %}
{% block outputdir %}{%endblock%}
{% block add_target %}
set(TARGET_TYPE SHARED)
if(WIN32)
    set(TARGET_TYPE STATIC)
endif()
add_library(common ${TARGET_TYPE}
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)
{% endblock %}

{% block linking %}
set_target_properties({{name}} PROPERTIES
    OUTPUT_NAME meshlab-common)
target_link_libraries({{name}}
    PUBLIC
    Qt5::Core
    Qt5::OpenGL
    Qt5::Script
    Qt5::Widgets
    Qt5::Xml
    Qt5::XmlPatterns
    external-glew
    PRIVATE
    external-jhead)
{% endblock %}

{% block output_dir %}{% endblock %}

{% block install %}
if(NOT WIN32)
    install(TARGETS {{name}}
        DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

{% endblock %}
{% block targetfolder %}Core{% endblock %}
