{% extends "plugin_with_glu.cmake" %}
{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE vcglib_trackball)
{% endblock %}
