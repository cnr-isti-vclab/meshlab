{% extends "CMakeLists.template.cmake" %}
{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE vcglib_trackball)
{% endblock %}
