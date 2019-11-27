{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have muparser
if(TARGET external-muparser)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - don't have muparser.")
endif()
{% endblock %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-muparser)
{% endblock %}
