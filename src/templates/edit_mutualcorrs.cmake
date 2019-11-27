{% extends "plugin_with_levmar.cmake" %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE OpenGL::GLU)
{% endblock %}
