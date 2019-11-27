{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have qhull
if(TARGET external-qhull)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - missing qhull")
endif()
{% endblock %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-qhull)
{% endblock %}
