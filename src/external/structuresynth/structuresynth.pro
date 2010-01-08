TEMPLATE = lib
TARGET = ssynth
CONFIG += staticlib
macx:DESTDIR       = ../lib/macx
win32-g++:DESTDIR       = ../lib/win32-gcc
win32-msvc2005:DESTDIR       = ../lib/win32-msvc2005
win32-msvc2008:DESTDIR       = ../lib/win32-msvc2008
linux-g++-32:DESTDIR = ../lib/linux-g++-32
linux-g++-64:DESTDIR = ../lib/linux-g++-64
linux-g++:DESTDIR = ../lib/linux-g++
DEPENDPATH += .\ssynth \
              ssynth\SyntopiaCore\Exceptions \
              ssynth\SyntopiaCore\GLEngine \
              ssynth\SyntopiaCore\Logging \
              ssynth\SyntopiaCore\Math \
              ssynth\SyntopiaCore\Misc \
              ssynth\ThirdPartyCode\MersenneTwister \
              ssynth\structures\StructureSynth\Model \
              ssynth\structures\StructureSynth\Parser \
              ssynth\structures\StructureSynth\Model\Rendering
INCLUDEPATH += .\ssynth

# Input
HEADERS += ssynth/SyntopiaCore/Exceptions/Exception.h \
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
           ssynth/structures/StructureSynth/Model/Action.h \
           ssynth/structures/StructureSynth/Model/AmbiguousRule.h \
           ssynth/structures/StructureSynth/Model/Builder.h \
           ssynth/structures/StructureSynth/Model/ColorPool.h \
           ssynth/structures/StructureSynth/Model/CustomRule.h \
           ssynth/structures/StructureSynth/Model/ExecutionStack.h \
           ssynth/structures/StructureSynth/Model/PrimitiveClass.h \
           ssynth/structures/StructureSynth/Model/PrimitiveRule.h \
           ssynth/structures/StructureSynth/Model/RandomStreams.h \
           ssynth/structures/StructureSynth/Model/Rule.h \
           ssynth/structures/StructureSynth/Model/RuleRef.h \
           ssynth/structures/StructureSynth/Model/RuleSet.h \
           ssynth/structures/StructureSynth/Model/State.h \
           ssynth/structures/StructureSynth/Model/Transformation.h \
           ssynth/structures/StructureSynth/Model/TransformationLoop.h \
           ssynth/structures/StructureSynth/Parser/EisenParser.h \
           ssynth/structures/StructureSynth/Parser/Preprocessor.h \
           ssynth/structures/StructureSynth/Parser/Tokenizer.h \
           ssynth/structures/StructureSynth/Model/Rendering/OpenGLRenderer.h \
           ssynth/structures/StructureSynth/Model/Rendering/Renderer.h \
           ssynth/structures/StructureSynth/Model/Rendering/TemplateRenderer.h
SOURCES += ssynth/SyntopiaCore/GLEngine/Box.cpp \
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
           ssynth/structures/StructureSynth/Model/Action.cpp \
           ssynth/structures/StructureSynth/Model/AmbiguousRule.cpp \
           ssynth/structures/StructureSynth/Model/Builder.cpp \
           ssynth/structures/StructureSynth/Model/ColorPool.cpp \
           ssynth/structures/StructureSynth/Model/CustomRule.cpp \
           ssynth/structures/StructureSynth/Model/ExecutionStack.cpp \
           ssynth/structures/StructureSynth/Model/PrimitiveClass.cpp \
           ssynth/structures/StructureSynth/Model/PrimitiveRule.cpp \
           ssynth/structures/StructureSynth/Model/RandomStreams.cpp \
           ssynth/structures/StructureSynth/Model/Rule.cpp \
           ssynth/structures/StructureSynth/Model/RuleRef.cpp \
           ssynth/structures/StructureSynth/Model/RuleSet.cpp \
           ssynth/structures/StructureSynth/Model/State.cpp \
           ssynth/structures/StructureSynth/Model/Transformation.cpp \
           ssynth/structures/StructureSynth/Model/TransformationLoop.cpp \
           ssynth/structures/StructureSynth/Parser/EisenParser.cpp \
           ssynth/structures/StructureSynth/Parser/Preprocessor.cpp \
           ssynth/structures/StructureSynth/Parser/Tokenizer.cpp \
           ssynth/structures/StructureSynth/Model/Rendering/OpenGLRenderer.cpp \
           ssynth/structures/StructureSynth/Model/Rendering/Renderer.cpp \
           ssynth/structures/StructureSynth/Model/Rendering/TemplateRenderer.cpp
CONFIG += opengl release
QT+=xml opengl script
