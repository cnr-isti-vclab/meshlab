# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have lib3ds
if(TARGET external-lib3ds)

    set(SOURCES meshio.cpp)

    set(HEADERS import_3ds.h io_3ds.h meshio.h)

	add_meshlab_plugin(io_3ds ${SOURCES} ${HEADERS})

    target_link_libraries(io_3ds PRIVATE external-lib3ds)

else()
    message(
        STATUS
            "Skipping io_3ds - missing lib3ds in external directory as well as on system."
    )
endif()
