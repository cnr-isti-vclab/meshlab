{% extends "CMakeLists.template.cmake" %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE OpenGL::GLU)
{% endblock %}
