# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES TexturePainter.cpp VisibilityCheck.cpp VisibleSet.cpp
            filter_img_patch_param.cpp)

set(HEADERS Patch.h TexturePainter.h VisibilityCheck.h VisibleSet.h
            filter_img_patch_param.h)

add_meshlab_plugin(filter_img_patch_param ${SOURCES} ${HEADERS})
