include (../../shared.pri)

QTSOAPDIR = ../../external/qtsoap-2.7_1/src
QMETA = ./qmeta/include
QMETADIR = qmeta
QITTYINCLUDE = $$QMETADIR/include/qitty/
QMETAINCLUDE = $$QMETADIR/include/qmeta/

INCLUDEPATH += $$QTSOAPDIR \
               $$QMETA

HEADERS       += filter_photosynth.h \
                 synthData.h \
                 $$QTSOAPDIR/qtsoap.h \
                 $$QITTYINCLUDE/byte_array.h \
                 $$QITTYINCLUDE/image.h \
                 $$QITTYINCLUDE/line_edit.h \
                 $$QITTYINCLUDE/plain_text_edit.h \
                 $$QITTYINCLUDE/qitty.h \
                 $$QITTYINCLUDE/style_sheet.h\
                 $$QMETAINCLUDE/exif.h \
                 $$QMETAINCLUDE/exif_data.h \
                 $$QMETAINCLUDE/file.h \
                 $$QMETAINCLUDE/identifiers.h \
                 #$$QMETAINCLUDE/image.h \
                 $$QMETAINCLUDE/iptc.h \
                 $$QMETAINCLUDE/jpeg.h \
                 $$QMETAINCLUDE/qmeta.h \
                 $$QMETAINCLUDE/standard.h \
                 $$QMETAINCLUDE/tiff.h \
                 $$QMETAINCLUDE/tiff_header.h \
                 $$QMETAINCLUDE/xmp.h

SOURCES       += filter_photosynth.cpp \
                 downloader.cpp \
                 $$QTSOAPDIR/qtsoap.cpp \
                 $$QMETADIR/src/utils/byte_array.cc \
                 #$$QMETADIR/src/utils/image.cc \
                 $$QMETADIR/src/utils/style_sheet.cc \
                 $$QMETADIR/src/widgets/line_edit.cc \
                 $$QMETADIR/src/widgets/plain_text_edit.cc \
                 $$QMETADIR/src/exif.cc \
                 $$QMETADIR/src/exif_data.cc \
                 $$QMETADIR/src/file.cc \
                 $$QMETADIR/src/image.cc \
                 $$QMETADIR/src/iptc.cc \
                 $$QMETADIR/src/jpeg.cc \
                 $$QMETADIR/src/standard.cc \
                 $$QMETADIR/src/tiff.cc \
                 $$QMETADIR/src/tiff_header.cc \
                 $$QMETADIR/src/xmp.cc

TARGET         = filter_photosynth

QT += network
