{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have newuoa
if(TARGET external-newuoa)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - missing newuoa in external directory.")
endif()
{% endblock %}

{% block sources %}
align/align_parameter.cpp
align/AlignGlobal.cpp
align/AlignPair.cpp
align/OccupancyGrid.cpp
alignDialog.cpp
AlignPairDialog.cpp
AlignPairWidget.cpp
edit_align_factory.cpp
edit_align.cpp
meshtree.cpp
point_matching_scale.cpp
../../meshlab/stdpardialog.cpp
${VCGDIR}/wrap/ply/plylib.cpp
{% endblock %}

{% block headers %}
align/align_parameter.h
align/AlignGlobal.h
align/AlignPair.h
align/OccupancyGrid.h
alignDialog.h
AlignPairDialog.h
AlignPairWidget.h
edit_align_factory.h
edit_align.h
meshtree.h
../../meshlab/stdpardialog.h
${VCGDIR}/wrap/gl/trimesh.h
{% endblock %}

{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-newuoa OpenGL::GLU vcglib_trackball)
if(OpenMP_CXX_FOUND)
    target_link_libraries({{name}} PRIVATE OpenMP::OpenMP_CXX)
endif()
{% endblock %}
