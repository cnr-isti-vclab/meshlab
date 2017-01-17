include (../../shared.pri)

HEADERS = edit_panosample_factory.h \
    cloneview.h \
    colorframe.h \
    paintbox.h \
    edit_panosample.h

SOURCES = 	edit_panosample_factory.cpp \
			../../meshlab/ml_selection_buffers.cpp \
			paintbox.cpp \
			edit_panosample.cpp \
			
TARGET = edit_panosample
RESOURCES = edit_paint.qrc
FORMS = paintbox.ui
