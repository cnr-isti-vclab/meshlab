{% extends "plugin_with_glu.cmake" %}

{% block sources %}
{{ super() }}
${VCGDIR}/wrap/gui/rubberband.cpp
{% endblock %}
