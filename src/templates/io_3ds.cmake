{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have lib3ds
if(TARGET external-lib3ds)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - missing lib3ds in external directory as well as on system.")
endif()
{% endblock %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-lib3ds)
{% endblock %}
