include (../../shared.pri)

HEADERS       += \
    ../../external/maxflow-v2.21.src/adjacency_list/graph.h \
    ../../external/maxflow-v2.21.src/adjacency_list/block.h \
    graph_builder.h \
    filter_decomposer.h

SOURCES       += \
../../external/maxflow-v2.21.src/adjacency_list/maxflow.cpp \
../../external/maxflow-v2.21.src/adjacency_list/graph.cpp \
../../../../vcglib/wrap/ply/plylib.cpp \
    filter_decomposer.cpp

TARGET        = filter_decomposer


