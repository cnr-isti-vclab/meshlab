{% extends "CMakeLists.template.cmake" %}

{% block sources %}
filter_texture.cpp
${VCGDIR}/wrap/ply/plylib.cpp
${VCGDIR}/wrap/qt/outline2_rasterizer.cpp
{% endblock sources %}

{% block headers %}
rastering.h
filter_texture.h
pushpull.h
${VCGDIR}/vcg/complex/algorithms/parametrization/voronoi_atlas.h
{% endblock headers %}

{% block linking %}
{{ super() }}
if(MSVC)
    target_compile_definitions({{name}} PRIVATE _USE_MATH_DEFINES)
endif()
{% endblock %}
