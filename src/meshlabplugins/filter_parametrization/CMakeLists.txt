# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council

set(SOURCES filter_parametrization.cpp)

set(HEADERS filter_parametrization.h)

add_meshlab_plugin(filter_parametrization ${SOURCES} ${HEADERS})
