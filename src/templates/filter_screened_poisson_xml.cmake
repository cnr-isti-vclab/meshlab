{% set name = "filter_screened_poisson" %}
{% extends "CMakeLists.template.cmake" %}
{# subset of the sources #}
{% block sources %}
filter_screened_poisson.cpp
Src/MarchingCubes.cpp
# Src/CmdLineParser.cpp
Src/Factor.cpp
Src/Geometry.cpp 
{% endblock %}

{% block linking %}
target_compile_definitions({{name}} PRIVATE BRUNO_LEVY_FIX FOR_RELEASE)

if(OpenMP_CXX_FOUND)
    target_link_libraries({{name}} PRIVATE OpenMP::OpenMP_CXX)
endif()

{{ super() }}
{% endblock %}
