set(SOURCES filter_heatgeodesic.cpp)

set(HEADERS filter_heatgeodesic.h trimesh_heatmethod.h)

add_meshlab_plugin(filter_heatgeodesic ${SOURCES} ${HEADERS})
