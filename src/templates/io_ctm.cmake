{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have OpenCTM
if(TARGET external-openctm)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - missing OpenCTM.")
endif()
{% endblock %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-openctm)
{% endblock %}
