{% extends "CMakeLists.template.cmake" %}


{% block whole_build %}
# Only build if we have structure-synth
if(TARGET external-ssynth)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - missing structure-synth sources.")
endif()
{% endblock %}

{% block sources %}
{{ super() }}
    ../../meshlabplugins/io_x3d/vrml/Parser.cpp
    ../../meshlabplugins/io_x3d/vrml/Scanner.cpp
{% endblock %}

{% block headers %}
{{ super() }}
    ../../meshlabplugins/io_x3d/import_x3d.h
    ../../meshlabplugins/io_x3d/util_x3d.h
    ../../meshlabplugins/io_x3d/vrml/Parser.h
    ../../meshlabplugins/io_x3d/vrml/Scanner.h
{% endblock %}


{% block linking %}
{{ super() }}
target_link_libraries({{name}} PRIVATE external-ssynth)
{% endblock %}
