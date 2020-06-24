cmake_minimum_required(VERSION 2.6)
project(chealpix)

include_directories(${CFITSIO_INCLUDE_DIRS})

# Set variables
set(Chealpix_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include" CACHE INTERNAL "Chealpix include dir" FORCE)
set(Chealpix_LIB_DIR "${PROJECT_BINARY_DIR}")
set(Chealpix_LIBS ${PROJECT_NAME} CACHE INTERNAL "Chealpix lib dir" FORCE)

# create the c-based healpix library.
include_directories(${Chealpix_INCLUDE_DIR})

add_library(${PROJECT_NAME} SHARED ${PROJECT_SOURCE_DIR}/src/chealpix.c)
target_link_libraries(${PROJECT_NAME} ${CFITSIO_LIBRARIES})
add_dependencies(${PROJECT_NAME}  cfitsio)

set(targets_export_name "${PROJECT_NAME}Targets")
install( TARGETS ${PROJECT_NAME}
    EXPORT "${targets_export_name}"
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include )
