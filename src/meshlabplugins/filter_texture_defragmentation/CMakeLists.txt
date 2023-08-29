set(SOURCES
    filter_texture_defragmentation.cpp

    TextureDefragmentation/src/intersection.cpp
    TextureDefragmentation/src/mesh_attribute.cpp
    TextureDefragmentation/src/packing.cpp
    TextureDefragmentation/src/seam_remover.cpp
    TextureDefragmentation/src/seams.cpp
    TextureDefragmentation/src/texture_optimization.cpp
    TextureDefragmentation/src/mesh_graph.cpp
    TextureDefragmentation/src/gl_utils.cpp
    TextureDefragmentation/src/mesh.cpp
    TextureDefragmentation/src/texture_rendering.cpp
    TextureDefragmentation/src/logging.cpp
    TextureDefragmentation/src/matching.cpp
    TextureDefragmentation/src/arap.cpp
    TextureDefragmentation/src/shell.cpp
    TextureDefragmentation/src/texture_object.cpp

    ${VCGDIR}/wrap/ply/plylib.cpp
    ${VCGDIR}/wrap/openfbx/src/ofbx.cpp
    ${VCGDIR}/wrap/openfbx/src/miniz.c
    ${VCGDIR}/wrap/qt/outline2_rasterizer.cpp
)

set(HEADERS
    filter_texture_defragmentation.h
    TextureDefragmentation/src/intersection.h
    TextureDefragmentation/src/mesh.h
    TextureDefragmentation/src/packing.h
    TextureDefragmentation/src/seam_remover.h
    TextureDefragmentation/src/seams.h
    TextureDefragmentation/src/timer.h
    TextureDefragmentation/src/types.h
    TextureDefragmentation/src/mesh_graph.h
    TextureDefragmentation/src/texture_rendering.h
    TextureDefragmentation/src/math_utils.h
    TextureDefragmentation/src/texture_optimization.h
    TextureDefragmentation/src/pushpull.h
    TextureDefragmentation/src/gl_utils.h
    TextureDefragmentation/src/mesh_attribute.h
    TextureDefragmentation/src/logging.h
    TextureDefragmentation/src/utils.h
    TextureDefragmentation/src/matching.h
    TextureDefragmentation/src/arap.h
    TextureDefragmentation/src/shell.h
    TextureDefragmentation/src/texture_object.h
)

add_meshlab_plugin(filter_texture_defragmentation ${SOURCES} ${HEADERS})

target_link_libraries(filter_texture_defragmentation PRIVATE OpenGL::GLU)

if(MSVC)
    target_compile_definitions(filter_texture_defragmentation PRIVATE _USE_MATH_DEFINES)
endif()
