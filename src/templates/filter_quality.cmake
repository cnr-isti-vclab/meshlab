{% extends "CMakeLists.template.cmake" %}

{% block sources %}
{{ super() }}
    ../edit_quality/common/meshmethods.cpp
    ../edit_quality/common/transferfunction.cpp
    ../edit_quality/common/util.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ../edit_quality/common/const_types.h
    ../edit_quality/common/meshmethods.h
    ../edit_quality/common/transferfunction.h
    ../edit_quality/common/util.h
{% endblock %}
