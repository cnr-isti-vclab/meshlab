{% extends "CMakeLists.template.cmake" %}

{% block whole_build %}
# Only build if we have gmp/gmpxx or mpir
if(TARGET external-gmp OR TARGET external-mpir)
    {{ super() | indent(4) }}
else()
    message(STATUS "Skipping {{name}} - don't know about gmp or mpir on this system.")
endif()
{% endblock %}
{% block linking %}
{{ super() }}
if(TARGET external-gmp)
    target_compile_definitions({{name}} PRIVATE BUILD_WITH_GMP)
    target_link_libraries({{name}} PRIVATE external-gmp)
else()
    target_link_libraries({{name}} PRIVATE external-mpir)
endif()
{% endblock %}
