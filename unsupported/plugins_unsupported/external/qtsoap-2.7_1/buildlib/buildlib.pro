TEMPLATE=lib
CONFIG += qt dll qtsoap-buildlib
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
include(../src/qtsoap.pri)
TARGET = $$QTSOAP_LIBNAME
DESTDIR = $$QTSOAP_LIBDIR
win32 {
    DLLDESTDIR = $$[QT_INSTALL_BINS]
    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${QTSOAP_LIBNAME}.dll
}
target.path = $$DESTDIR
INSTALLS += target
