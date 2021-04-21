# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES
    common/meshmethods.cpp
    common/transferfunction.cpp
    common/util.cpp
    edit_quality_factory.cpp
    eqhandle.cpp
    handle.cpp
    qualitymapper.cpp
    qualitymapperdialog.cpp
    tfhandle.cpp
    common/meshmethods.cpp
    common/transferfunction.cpp
    common/util.cpp)

set(HEADERS
    common/const_types.h
    common/meshmethods.h
    common/transferfunction.h
    common/util.h
    edit_quality_factory.h
    eqhandle.h
    handle.h
    qualitymapper.h
    qualitymapperdialog.h
    tfhandle.h
    common/const_types.h
    common/meshmethods.h
    common/transferfunction.h
    common/util.h)

set(RESOURCES qualitymapper.qrc)

set(UI qualitymapperdialog.ui)

add_meshlab_plugin(edit_quality ${SOURCES} ${HEADERS} ${RESOURCES} ${UI})
