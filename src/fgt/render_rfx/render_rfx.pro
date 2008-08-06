include (../../shared.pri)

QT           += opengl xml

HEADERS       = render_rfx.h  \
                rfx_shader.h  \
                rfx_glpass.h  \
                rfx_uniform.h \
                rfx_state.h   \
                rfx_parser.h

SOURCES       = render_rfx.cpp  \
                rfx_shader.cpp  \
                rfx_glpass.cpp  \
                rfx_uniform.cpp \
                rfx_state.cpp   \
                rfx_parser.cpp  \
                $$GLEWCODE

TARGET        = render_rfx
