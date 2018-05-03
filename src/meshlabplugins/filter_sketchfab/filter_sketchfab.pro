include (../../shared.pri)

HEADERS += filter_sketchfab.h
SOURCES += filter_sketchfab.cpp $$VCGDIR//wrap/ply/plylib.cpp
TARGET   = filter_sketchfab

include (../../shared_post.pri)