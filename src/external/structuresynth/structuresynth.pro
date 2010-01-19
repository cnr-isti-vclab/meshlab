TEMPLATE = lib
TARGET = ssynth
CONFIG += staticlib

DEPENDPATH += . \
              ssynth \
              ssynth\StructureSynth\Model \
              ssynth\StructureSynth\Parser \
              ssynth\SyntopiaCore\Exceptions \
              ssynth\SyntopiaCore\GLEngine \
              ssynth\SyntopiaCore\Logging \
              ssynth\SyntopiaCore\Math \
              ssynth\SyntopiaCore\Misc \
              ssynth\ThirdPartyCode\MersenneTwister \
              ssynth\StructureSynth\Model\Rendering


INCLUDEPATH += ssynth

# Input
HEADERS += ssynth/StructureSynth/Model/Action.h \
           ssynth/StructureSynth/Model/AmbiguousRule.h \
           ssynth/StructureSynth/Model/Builder.h \
           ssynth/StructureSynth/Model/ColorPool.h \
           ssynth/StructureSynth/Model/CustomRule.h \
           ssynth/StructureSynth/Model/ExecutionStack.h \
           ssynth/StructureSynth/Model/PrimitiveClass.h \
           ssynth/StructureSynth/Model/PrimitiveRule.h \
           ssynth/StructureSynth/Model/RandomStreams.h \
           ssynth/StructureSynth/Model/Rule.h \
           ssynth/StructureSynth/Model/RuleRef.h \
           ssynth/StructureSynth/Model/RuleSet.h \
           ssynth/StructureSynth/Model/State.h \
           ssynth/StructureSynth/Model/Transformation.h \
           ssynth/StructureSynth/Model/TransformationLoop.h \
           ssynth/StructureSynth/Parser/EisenParser.h \
           ssynth/StructureSynth/Parser/Preprocessor.h \
           ssynth/StructureSynth/Parser/Tokenizer.h \
           ssynth/SyntopiaCore/Exceptions/Exception.h \
           ssynth/SyntopiaCore/GLEngine/Box.h \
           ssynth/SyntopiaCore/GLEngine/Dot.h \
           ssynth/SyntopiaCore/GLEngine/EngineWidget.h \
           ssynth/SyntopiaCore/GLEngine/Grid.h \
           ssynth/SyntopiaCore/GLEngine/Line.h \
           ssynth/SyntopiaCore/GLEngine/Mesh.h \
           ssynth/SyntopiaCore/GLEngine/Object3D.h \
           ssynth/SyntopiaCore/GLEngine/RayTracer.h \
           ssynth/SyntopiaCore/GLEngine/RaytraceTriangle.h \
           ssynth/SyntopiaCore/GLEngine/Sphere.h \
           ssynth/SyntopiaCore/GLEngine/Triangle.h \
           ssynth/SyntopiaCore/Logging/ListWidgetLogger.h \
           ssynth/SyntopiaCore/Logging/Logging.h \
           ssynth/SyntopiaCore/Math/Matrix4.h \
           ssynth/SyntopiaCore/Math/Random.h \
           ssynth/SyntopiaCore/Math/Vector3.h \
           ssynth/SyntopiaCore/Misc/ColorUtils.h \
           ssynth/SyntopiaCore/Misc/MiniParser.h \
           ssynth/SyntopiaCore/Misc/Persistence.h \
           ssynth/SyntopiaCore/Misc/Version.h \
           ssynth/ThirdPartyCode/MersenneTwister/MersenneTwister.h \
           ssynth/StructureSynth/Model/Rendering/OpenGLRenderer.h \
           ssynth/StructureSynth/Model/Rendering/Renderer.h \
           ssynth/StructureSynth/Model/Rendering/TemplateRenderer.h
SOURCES += ssynth/StructureSynth/Model/Action.cpp \
           ssynth/StructureSynth/Model/AmbiguousRule.cpp \
           ssynth/StructureSynth/Model/Builder.cpp \
           ssynth/StructureSynth/Model/ColorPool.cpp \
           ssynth/StructureSynth/Model/CustomRule.cpp \
           ssynth/StructureSynth/Model/ExecutionStack.cpp \
           ssynth/StructureSynth/Model/PrimitiveClass.cpp \
           ssynth/StructureSynth/Model/PrimitiveRule.cpp \
           ssynth/StructureSynth/Model/RandomStreams.cpp \
           ssynth/StructureSynth/Model/Rule.cpp \
           ssynth/StructureSynth/Model/RuleRef.cpp \
           ssynth/StructureSynth/Model/RuleSet.cpp \
           ssynth/StructureSynth/Model/State.cpp \
           ssynth/StructureSynth/Model/Transformation.cpp \
           ssynth/StructureSynth/Model/TransformationLoop.cpp \
           ssynth/StructureSynth/Parser/EisenParser.cpp \
           ssynth/StructureSynth/Parser/Preprocessor.cpp \
           ssynth/StructureSynth/Parser/Tokenizer.cpp \
           ssynth/SyntopiaCore/GLEngine/Box.cpp \
           ssynth/SyntopiaCore/GLEngine/Dot.cpp \
           ssynth/SyntopiaCore/GLEngine/EngineWidget.cpp \
           ssynth/SyntopiaCore/GLEngine/Grid.cpp \
           ssynth/SyntopiaCore/GLEngine/Line.cpp \
           ssynth/SyntopiaCore/GLEngine/Mesh.cpp \
           ssynth/SyntopiaCore/GLEngine/Object3D.cpp \
           ssynth/SyntopiaCore/GLEngine/RayTracer.cpp \
           ssynth/SyntopiaCore/GLEngine/RaytraceTriangle.cpp \
           ssynth/SyntopiaCore/GLEngine/Sphere.cpp \
           ssynth/SyntopiaCore/GLEngine/Triangle.cpp \
           ssynth/SyntopiaCore/Logging/ListWidgetLogger.cpp \
           ssynth/SyntopiaCore/Logging/Logging.cpp \
           ssynth/SyntopiaCore/Math/Matrix4.cpp \
           ssynth/SyntopiaCore/Math/Random.cpp \
           ssynth/SyntopiaCore/Math/Vector3.cpp \
           ssynth/SyntopiaCore/Misc/ColorUtils.cpp \
           ssynth/SyntopiaCore/Misc/MiniParser.cpp \
           ssynth/SyntopiaCore/Misc/Persistence.cpp \
           ssynth/SyntopiaCore/Misc/Version.cpp \
           ssynth/StructureSynth/Model/Rendering/OpenGLRenderer.cpp \
           ssynth/StructureSynth/Model/Rendering/Renderer.cpp \
           ssynth/StructureSynth/Model/Rendering/TemplateRenderer.cpp
CONFIG+=opengl
QT+=xml opengl script
macx:DESTDIR       = ../lib/macx
win32-g++:DESTDIR       = ../lib/win32-gcc
win32-msvc2005:DESTDIR       = ../lib/win32-msvc2005
win32-msvc2008:DESTDIR       = ../lib/win32-msvc2008
linux-g++-32:DESTDIR = ../lib/linux-g++-32
linux-g++-64:DESTDIR = ../lib/linux-g++-64
linux-g++:DESTDIR = ../lib/linux-g++
