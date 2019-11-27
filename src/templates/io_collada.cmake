{% extends "CMakeLists.template.cmake" %}

{% block sources %}
{{ super() }}
    ${VCGDIR}/wrap/dae/xmldocumentmanaging.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ${VCGDIR}/wrap/dae/colladaformat.h
    ${VCGDIR}/wrap/dae/util_dae.h
    ${VCGDIR}/wrap/dae/xmldocumentmanaging.h
    ${VCGDIR}/wrap/io_trimesh/export_dae.h
    ${VCGDIR}/wrap/io_trimesh/import_dae.h
{% endblock %}
