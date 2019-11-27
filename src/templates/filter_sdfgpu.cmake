{% extends "plugin_with_glu.cmake" %}

{% block headers %}
{{ super() }}
    ../render_radiance_scaling/gpuProgram.h
    ../render_radiance_scaling/framebufferObject.h
    ../render_radiance_scaling/gpuShader.h
    ../render_radiance_scaling/textureParams.h
    ../render_radiance_scaling/textureFormat.h
    ../render_radiance_scaling/texture2D.h
{% endblock %}

{% block sources %}
{{ super() }}
    ../render_radiance_scaling/gpuProgram.cpp
    ../render_radiance_scaling/framebufferObject.cpp
    ../render_radiance_scaling/gpuShader.cpp
    ../render_radiance_scaling/textureParams.cpp
    ../render_radiance_scaling/textureFormat.cpp
{% endblock %}

{% block linking %}
{{ super() }}
target_include_directories({{name}} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../render_radiance_scaling)
{% endblock %}
