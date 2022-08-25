# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES io_collada.cpp ${VCGDIR}/wrap/dae/xmldocumentmanaging.cpp)

set(HEADERS
    io_collada.h
    ${VCGDIR}/wrap/dae/colladaformat.h
    ${VCGDIR}/wrap/dae/util_dae.h
    ${VCGDIR}/wrap/dae/xmldocumentmanaging.h
    ${VCGDIR}/wrap/io_trimesh/export_dae.h
    ${VCGDIR}/wrap/io_trimesh/import_dae.h)

add_meshlab_plugin(io_collada ${SOURCES} ${HEADERS})
