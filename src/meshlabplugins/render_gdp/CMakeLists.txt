# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES meshrender.cpp shaderDialog.cpp textfile.cpp)

set(HEADERS meshrender.h shaderDialog.h shaderStructs.h textfile.h)

set(UI shaderDialog.ui)

add_meshlab_plugin(render_gdp ${SOURCES} ${HEADERS} ${UI})
