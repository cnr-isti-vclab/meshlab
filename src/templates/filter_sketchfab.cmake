{% extends "CMakeLists.template.cmake" %}

{% block sources %}
{{ super() }}
    ${VCGDIR}/wrap/ply/plylib.cpp
{% endblock %}
