# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/edit_ocme.cmake,
### then re-run ./make-cmake.py

set(SOURCES
    edit_ocme.cpp
    edit_ocme_factory.cpp
    src/cache/benchmark/main.cpp
    src/cache/cache_gl.cpp
    src/cache/heaptest.cpp
    src/cache/main.cpp
    src/cache/mapmain.cpp
    src/ocme/berkeleydb/ocme_simpledb.cpp
    src/ocme/borderindex_chunk.cpp
    src/ocme/cell.cpp
    src/ocme/impostor_create.cpp
    src/ocme/impostor_render.cpp
    src/ocme/main.cpp
    src/ocme/ocme.cpp
    src/ocme/ocme_debug.cpp
    src/ocme/ocme_disk_loader.cpp
    src/ocme/ocme_garbage.cpp
    src/ocme/ocme_impostor.cpp
    src/ocme/ocme_render.cpp
    src/ocme/plane_box_quantization.cpp
    src/ocme/simpledb/ocme_kcdb.cpp
    src/ocme/simpledb/ocme_simpledb.cpp
    src/ooc_vector/berkeleydb/ooc_chunks.cpp
    src/ooc_vector/cache_policy.cpp
    src/ooc_vector/io/ooc_chunks.cpp
    src/ooc_vector/io/ooc_chunks_kcdb.cpp
    src/ooc_vector/main.cpp
    src/ooc_vector/ooc_chains.cpp
    src/ooc_vector/ooc_chains_kcdb.cpp
    src/ooc_vector/simpledb.cpp
    src/utils/string_serialize.cpp)

set(HEADERS
    edit_ocme.h
    edit_ocme_factory.h
    src/cache/cache.h
    src/cache/controller.h
    src/cache/door.h
    src/cache/iheap.h
    src/cache/mapcache.h
    src/cache/mapcontroller.h
    src/cache/mapmy_widget.h
    src/cache/my_widget.h
    src/cache/old/double_heap.h
    src/cache/old/itreeheap.h
    src/cache/old/mmheap.h
    src/cache/old/mmheap_orig.h
    src/cache/old/mmtreeheap.h
    src/ocme/FBool.h
    src/ocme/berkeleydb/chain_mem_simpledb.h
    src/ocme/boolvector.h
    src/ocme/borderindex_chunk.h
    src/ocme/cell.h
    src/ocme/cell_attributes.h
    src/ocme/globals.h
    src/ocme/import_ocm_ply.h
    src/ocme/import_ply_to_ocme.h
    src/ocme/impostor.h
    src/ocme/impostor_create.h
    src/ocme/impostor_definition.h
    src/ocme/multiple_std_iterator.h
    src/ocme/ocme.h
    src/ocme/ocme_add.h
    src/ocme/ocme_assert.h
    src/ocme/ocme_commit.h
    src/ocme/ocme_definition.h
    src/ocme/ocme_disk_loader.h
    src/ocme/ocme_extract.h
    src/ocme/ocme_impostor.h
    src/ocme/plane_box_intersection.h
    src/ocme/plane_box_quantization.h
    src/ocme/simpledb/chain_mem_simpledb.h
    src/ocme/stdmatrix.h
    src/ocme/stdmatrix3.h
    src/ocme/type_traits.h
    src/ocme/vcg_mesh.h
    src/ooc_vector/cache_policy.h
    src/ooc_vector/ooc_chains.h
    src/ooc_vector/ooc_vector.h
    src/ooc_vector/simpledb.h
    src/ooc_vector/type_traits.h
    src/utils/logging.h
    src/utils/memory_debug.h
    src/utils/name_access_function_bounds.h
    src/utils/release_assert.h
    src/utils/std_util.h
    src/utils/string_serialize.h
    src/utils/timing.h)

set(RESOURCES edit_ocme.qrc)

set(UI ocme.ui ocme_widget.ui)

add_library(edit_ocme MODULE ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})

target_include_directories(edit_ocme PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(edit_ocme PUBLIC common)

set_property(TARGET edit_ocme PROPERTY FOLDER Plugins)

set_property(TARGET edit_ocme PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET edit_ocme PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                       ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS edit_ocme
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
