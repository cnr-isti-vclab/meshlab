# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

{% block whole_build %}
{% block before_vars %}{% endblock %}

set(SOURCES
{% block sources %}
    {{ sources | join('\n') | indent(4) }}
{% endblock sources %}
)
{% if headers %}
set(HEADERS
{% block headers %}
    {{ headers | join('\n') | indent(4) }}
{% endblock headers %}
)
{% endif %}
{% if resources %}
set(RESOURCES
    {{ resources | join('\n') | indent(4) }}
)
{% endif %}
{% if ui %}
set(UI
    {{ ui | join('\n') | indent(4) }}
)
{% endif %}
{% if xml %}
set(XML
    {{ xml | join('\n') | indent(4) }}
)
meshlab_install_plugin_xml(${CMAKE_CURRENT_SOURCE_DIR}/${XML} XML_OUT)
list(APPEND SOURCES ${XML_OUT})
{% endif %}


{% block add_target %}
add_library({{name}} MODULE
    ${SOURCES}
{% if headers %}    ${HEADERS}{% endif -%}
{% if resources %}    ${RESOURCES}{% endif -%}
{% if ui %}    ${UI}{% endif -%}
)

{% endblock %}
{% block linking %}
target_include_directories({{name}} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries({{name}}
    PUBLIC
    common)
{% endblock %}

set_property(TARGET {{name}} PROPERTY
    FOLDER {% block targetfolder %}Plugins{% endblock %})

{% block output_dir %}
set_property(TARGET {{name}} PROPERTY
    RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET {{name}} PROPERTY
    LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})
{% endblock %}

{% block install %}

install(TARGETS {{name}}
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR} COMPONENT Plugins)

{% endblock %}

{% endblock whole_build %}
