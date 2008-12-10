include (../../shared.pri)

QT      += opengl xml

HEADERS  = rmmeshrender.h \
           rmshaderdialog.h \
           glstateholder.h \
           parser/GlState.h \
           parser/RmEffect.h \
           parser/RmPass.h \
           parser/RmXmlParser.h \
           parser/UniformVar.h

SOURCES  = rmmeshrender.cpp \
           rmshaderdialog.cpp \
           glstateholder.cpp \
           parser/RmPass.cpp \
           parser/RmXmlParser.cpp \
           parser/UniformVar.cpp \
		   $$GLEWCODE

FORMS    = rmShadowDialog.ui

TARGET   = render_rm
