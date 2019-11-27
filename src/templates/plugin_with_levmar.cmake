{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have newuoa and levmar
if(TARGET external-newuoa AND TARGET external-levmar)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - missing either newuoa or levmar in external directory.")
endif()
{% endblock %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-newuoa external-levmar)
{% endblock %}
