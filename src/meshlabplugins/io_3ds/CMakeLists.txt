# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/io_3ds.cmake (custom for this directory),
### then re-run ./make-cmake.py

# Only build if we have lib3ds
if(TARGET external-lib3ds)

    set(SOURCES meshio.cpp)

    set(HEADERS import_3ds.h io_3ds.h meshio.h)

    add_library(io_3ds MODULE ${SOURCES} ${HEADERS})

    target_include_directories(io_3ds PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(io_3ds PUBLIC meshlab-common)

    target_link_libraries(io_3ds PRIVATE external-lib3ds)

    set_property(TARGET io_3ds PROPERTY FOLDER Plugins)

    set_property(TARGET io_3ds PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET io_3ds PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                        ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(
        TARGETS io_3ds
        DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
        COMPONENT Plugins)

else()
    message(
        STATUS
            "Skipping io_3ds - missing lib3ds in external directory as well as on system."
    )
endif()
