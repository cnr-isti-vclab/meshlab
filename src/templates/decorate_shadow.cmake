{% extends "plugin_with_glu.cmake" %}

{% block sources %}
{{ super() }}
    ${VCGDIR}/wrap/gui/trackball.cpp
    ${VCGDIR}/wrap/gui/trackmode.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ${VCGDIR}/wrap/gui/trackball.h
{% endblock %}
