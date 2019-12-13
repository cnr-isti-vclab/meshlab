{% extends "plugin_with_levmar.cmake" %}

{% block linking %}
{{ super() }}

if(OpenMP_CXX_FOUND)
    target_link_libraries({{name}} PRIVATE OpenMP::OpenMP_CXX)
    target_compile_definitions({{name}} PRIVATE _USE_OMP)
endif()

{% endblock %}
