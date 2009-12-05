include (../../shared.pri)

QT           += opengl xml

HEADERS       = render_rfx.h             \
		    rfx_colorbox.h           \
		    rfx_specialuniform.h     \
		    rfx_specialattribute.h   \
                rfx_shader.h             \
                rfx_glpass.h             \
                rfx_uniform.h            \
                rfx_state.h              \
                rfx_parser.h             \
                rfx_dialog.h             \
                rfx_rendertarget.h       \
                rfx_textureloader.h      \
                plugins/rfx_qimage.h     \
                plugins/rfx_dds.h        \
                plugins/rfx_tga.h	 

SOURCES       = render_rfx.cpp           \
	  	    rfx_colorbox.cpp	     \
		    rfx_specialuniform.cpp   \
		    rfx_specialattribute.cpp \
                rfx_shader.cpp           \
                rfx_glpass.cpp           \
                rfx_uniform.cpp          \
                rfx_state.cpp            \
                rfx_parser.cpp           \
                rfx_dialog.cpp           \
                rfx_rendertarget.cpp     \
                rfx_textureloader.cpp    \
                plugins/rfx_qimage.cpp   \
                plugins/rfx_dds.cpp      \
                plugins/rfx_tga.cpp      

FORMS         = rfx_dialog.ui
RESOURCES 	  = render_rfx.qrc
TARGET        = render_rfx
