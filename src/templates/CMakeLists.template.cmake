# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/{{template}},
{% if assumed_custom_template_name -%}
### or create a derived template in src/templates/{{assumed_custom_template_name}},
{% endif -%}
### then re-run ./make-cmake.py

{# To customize this for just one or a few directories, you create a derived template: see any other template for examples. #}
{# Templates are chosen based on the leaf directory name: e.g. src/meshlabplugins/filter_qhull uses templates/filter_qhull.cmake if available. #}
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
