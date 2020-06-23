
# EXTERNALS
# have ExternalProject available
include(ExternalProject)
message(STATUS "[Deps] Use Chealpix")

ExternalProject_Add(
        cfitsio
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cfitsio"
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/"

        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}

        # override default behaviours
        UPDATE_COMMAND ""
)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty/chealpix)
add_definitions(-DSUPER4PCS_USE_CHEALPIX)
include_directories(${Chealpix_INCLUDE_DIR})
