# Copyright 2019, 2021, Visual Computing Lab, ISTI - Italian National Research Council

set(SOURCES src/filter_icp.cpp src/align/icp_align_parameter.cpp src/stats_builder/stats_builder.cpp)

set(HEADERS src/filter_icp.h src/align/icp_align_parameter.h src/stats_builder/stats_builder.h)

add_meshlab_plugin(filter_icp ${SOURCES} ${HEADERS})
