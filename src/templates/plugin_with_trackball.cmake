{% extends "CMakeLists.template.cmake" %}
{% block sources %}
{{ super() }}
    ${VCGDIR}/wrap/gui/trackball.cpp
    ${VCGDIR}/wrap/gui/trackmode.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ${VCGDIR}/wrap/gui/trackball.h
{% endblock %}
