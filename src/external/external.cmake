# newuoa - optional and header-only, for several plugins including all that use levmar
set(NEWUOA_DIR ${VCGDIR}/wrap/newuoa)
message(STATUS "- newuoa - using bundled source")
add_library(external-newuoa INTERFACE)
target_include_directories(external-newuoa INTERFACE ${NEWUOA_DIR}/include)

# levmar - optional, for several plugins
set(LEVMAR_DIR ${EXTERNAL_DIR}/levmar-2.3)
add_subdirectory(${LEVMAR_DIR})

# lib3ds - optional, for io_3ds
set(LIB3DS_DIR ${EXTERNAL_DIR}/lib3ds-1.3.0)
add_subdirectory(${LIB3DS_DIR})

# gmp or mpir - optional, for filter_csg
if(GMP_FOUND)
    message(STATUS "- GMP/MPIR - using system-provided GMP library")
    add_library(external-gmp INTERFACE)
    target_include_directories(external-gmp SYSTEM INTERFACE ${GMP_INCLUDE_DIRS})
    target_link_libraries(external-gmp INTERFACE ${GMP_LIBRARIES})
else()
    # TODO set up external-mpir per-os for bundled mpir
endif()

# muparser - optional, for filter_func
set(MUPARSER_DIR ${EXTERNAL_DIR}/muparser_v225)
add_subdirectory(${MUPARSER_DIR})

# OpenCTM - optional, for io_ctm
set(OPENCTM_DIR ${EXTERNAL_DIR}/OpenCTM-1.0.3)
add_subdirectory(${OPENCTM_DIR})

# structure-synth - optional, for filter_ssynth
set(SSYNTH_DIR ${EXTERNAL_DIR}/structuresynth-1.5)
add_subdirectory(${SSYNTH_DIR})

# qhull - optional, for filter_qhull
set(QHULL_DIR ${EXTERNAL_DIR}/qhull-2003.1)
add_subdirectory(${QHULL_DIR})

# u3d - optional, for io_u3d
set(U3D_DIR ${EXTERNAL_DIR}/u3d)
add_subdirectory(${U3D_DIR})
#set_property(TARGET external-IDTFConverter PROPERTY FOLDER External)

# opengr - optional and header-only, for filter_globalregistration
set(OPENGR_DIR ${EXTERNAL_DIR}/OpenGR)
message(STATUS "- OpenGR - using bundled source")
add_library(external-opengr INTERFACE)
target_include_directories(external-opengr INTERFACE ${OPENGR_DIR}/src/)

