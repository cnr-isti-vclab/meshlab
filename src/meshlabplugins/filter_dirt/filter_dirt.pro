include (../../shared.pri)

HEADERS = filter_dirt.h \
          particle.h \
          dirt_utils.h \
          $$VCGDIR/vcg/complex/algorithms/point_sampling.h

SOURCES = filter_dirt.cpp dirt_utils.cpp
TARGET = filter_dirt
