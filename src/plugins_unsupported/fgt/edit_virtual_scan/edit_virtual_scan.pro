include( ../../shared.pri )

QT          +=  opengl

HEADERS     +=  edit_virtual_scan_factory.h     \
                edit_virtual_scan_plugin.h      \
                edit_vs_widget.h                \
                my_gl_widget.h                  \
                vs/povs_generator.h             \
                vs/resources.h                  \
                vs/sampler.h                    \
                vs/simple_renderer.h            \
                vs/stages.h                     \
                vs/utils.h

SOURCES     +=  edit_virtual_scan_factory.cpp   \
                edit_virtual_scan_plugin.cpp    \
                edit_vs_widget.cpp

TARGET      =   edit_virtual_scan

RESOURCES   +=  edit_virtual_scan.qrc
