# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    filterqualitymapper.cpp ../edit_quality/common/meshmethods.cpp
    ../edit_quality/common/transferfunction.cpp ../edit_quality/common/util.cpp)

set(HEADERS
    filterqualitymapper.h ../edit_quality/common/const_types.h
    ../edit_quality/common/meshmethods.h
    ../edit_quality/common/transferfunction.h ../edit_quality/common/util.h)

add_meshlab_plugin(filter_quality ${SOURCES} ${HEADERS})
