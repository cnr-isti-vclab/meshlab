{% extends "plugin_with_glu.cmake" %}
# Needed because GenericParamDialog is in meshlab, not common.
{% block sources %}
{{ super() }}
    ../../meshlab/stdpardialog.cpp
{% endblock %}
