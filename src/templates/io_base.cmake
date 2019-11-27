{% extends "plugin_with_glu.cmake" %}

{% block sources %}
{{ super() }}
    ${VCGDIR}/wrap/openfbx/src/miniz.c
    ${VCGDIR}/wrap/openfbx/src/ofbx.cpp
    ${VCGDIR}/wrap/ply/plylib.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ${VCGDIR}/wrap/io_trimesh/export_obj.h
    ${VCGDIR}/wrap/io_trimesh/export_off.h
    ${VCGDIR}/wrap/io_trimesh/export_ply.h
    ${VCGDIR}/wrap/io_trimesh/import_fbx.h
    ${VCGDIR}/wrap/io_trimesh/import_obj.h
    ${VCGDIR}/wrap/io_trimesh/import_off.h
    ${VCGDIR}/wrap/io_trimesh/import_ply.h
    ${VCGDIR}/wrap/io_trimesh/import_ptx.h
    ${VCGDIR}/wrap/io_trimesh/import_stl.h
    ${VCGDIR}/wrap/io_trimesh/io_material.h
    ${VCGDIR}/wrap/ply/plylib.h
{% endblock %}
