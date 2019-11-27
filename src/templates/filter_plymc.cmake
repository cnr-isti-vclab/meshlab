{% extends "plugin_with_glu.cmake" %}

{# plymc_main doesn't build and isn't part of this target anyway #}
{% block sources %}
filter_plymc.cpp
${VCGDIR}/wrap/ply/plylib.cpp
{% endblock %}

{% block headers %}
filter_plymc.h
{% endblock %}
