{% extends "CMakeLists.template.cmake" %}

{% block sources %}
{{ super() }}
    common/meshmethods.cpp
    common/transferfunction.cpp
    common/util.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    common/const_types.h
    common/meshmethods.h
    common/transferfunction.h
    common/util.h
{% endblock %}
