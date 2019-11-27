# Copyright 2019 Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates then re-run ./make-cmake.py

# Only build if we have gmp/gmpxx or mpir
if(TARGET external-gmp OR TARGET external-mpir)

    set(SOURCES filter_csg.cpp)

    set(HEADERS filter_csg.h gmpfrac.h intercept.h)

    add_library(filter_csg MODULE ${SOURCES} ${HEADERS})

    target_include_directories(filter_csg PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(filter_csg PUBLIC common)

    if(TARGET external-gmp)
        target_compile_definitions(filter_csg PRIVATE BUILD_WITH_GMP)
        target_link_libraries(filter_csg PRIVATE external-gmp)
    else()
        target_link_libraries(filter_csg PRIVATE external-mpir)
    endif()

    set_property(TARGET filter_csg PROPERTY FOLDER Plugins)

    set_property(TARGET filter_csg PROPERTY RUNTIME_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

    set_property(TARGET filter_csg PROPERTY LIBRARY_OUTPUT_DIRECTORY
                                            ${MESHLAB_PLUGIN_OUTPUT_DIR})

    install(TARGETS filter_csg DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
                    COMPONENT Plugins)

else()
    message(
        STATUS
            "Skipping filter_csg - don't know about gmp or mpir on this system."
    )
endif()
