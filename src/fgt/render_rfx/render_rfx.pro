include (../../shared.pri)

QT           += opengl xml

HEADERS       = render_rfx.h         \
                rfx_shader.h         \
                rfx_glpass.h         \
                rfx_uniform.h        \
                rfx_state.h          \
                rfx_parser.h         \
                rfx_dialog.h         \
                rfx_rendertarget.h

SOURCES       = render_rfx.cpp       \
                rfx_shader.cpp       \
                rfx_glpass.cpp       \
                rfx_uniform.cpp      \
                rfx_state.cpp        \
                rfx_parser.cpp       \
                rfx_dialog.cpp       \
                rfx_rendertarget.cpp \
                $$GLEWCODE

FORMS         = rfx_dialog.ui

TARGET        = render_rfx
