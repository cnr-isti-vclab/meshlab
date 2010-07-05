include (../../shared.pri)

HEADERS       = textureParams.h textureFormat.h texture2D.h framebufferObject.h gpuShader.h gpuProgram.h radianceScalingRenderer.h shaderDialog.h
SOURCES       = textureParams.cpp textureFormat.cpp framebufferObject.cpp gpuShader.cpp gpuProgram.cpp radianceScalingRenderer.cpp shaderDialog.cpp $$GLEWCODE

TARGET        = render_radiance_scaling
FORMS         = shaderDialog.ui

RESOURCES     = radianceScalingRenderer.qrc

QT            += opengl

# CONFIG += debug
