TEMPLATE      = subdirs
SUBDIRS       = meshfilter

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaintplugins
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS plugandpaintplugins.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/plugandpaintplugins
INSTALLS += target sources
