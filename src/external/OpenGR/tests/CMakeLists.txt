project (OpenGR-Tests)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${SRC_DIR})

#############################################
## start by downloading standford datasets, that will be used
## to test the approach
include(ExternalProject)
ExternalProject_Add(dataset-standford-armadillo
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/datasets/armadillo
    URL http://graphics.stanford.edu/pub/3Dscanrep/armadillo/Armadillo_scans.tar.gz
    URL_MD5 437eafa8d3d07067e17a42c727a0c28d
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Add(dataset-standford-bunny
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/datasets/bunny
    URL http://graphics.stanford.edu/pub/3Dscanrep/bunny.tar.gz
    URL_MD5 232bb90cac5f2ac77140e09a59bcffe4
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

#############################################
## testing files
set (testing_SRCS
    testing.h
)

#############################################
## externalAppTestD

include(ExternalProject)
ExternalProject_Add(
        externalAppTestTarget
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/externalAppTest"

        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
            -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
            -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}/lib/cmake
            -DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_PREFIX}/lib
            -DEIGEN3_INCLUDE_DIR=${EIGEN3_INCLUDE_DIR}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CC_COMPILER=${CMAKE_CC_COMPILER}

        # override default behaviours
        UPDATE_COMMAND ""
        EXCLUDE_FROM_ALL
)

add_dependencies(externalAppTestTarget ForceOpenGRInstallation)
add_dependencies(buildtests externalAppTestTarget)
add_test(NAME externalAppTest
         COMMAND OpenGR-externalAppTest)

#############################################
## pair extraction
set(pair_extraction_SRCS
    pair_extraction.cc
)
add_executable(pair_extraction ${pair_extraction_SRCS} ${testing_SRCS})
add_dependencies(buildtests pair_extraction)
add_test(NAME pair_extraction
         #CONFIGURATIONS Release
         COMMAND pair_extraction)
target_link_libraries(pair_extraction opengr_accel opengr_algo opengr_utils)
if(OpenGR_USE_CHEALPIX)
    target_link_libraries(pair_extraction ${Chealpix_LIBS} )
endif(OpenGR_USE_CHEALPIX)

#############################################
## quad extraction
#set(quad_extraction_SRCS
#    quad_extraction.cc
#)
#add_executable(quad_extraction ${quad_extraction_SRCS} ${testing_SRCS})
#add_dependencies(buildtests quad_extraction)
#add_test(NAME quad_extraction
#         #CONFIGURATIONS Release
#         COMMAND quad_extraction)
#target_link_libraries(quad_extraction open_accel open_algo)
#if(OpenGR_USE_CHEALPIX)
#    target_link_libraries(quad_extraction ${Chealpix_LIBS} )
#endif(OpenGR_USE_CHEALPIX)

#############################################
## matching
set(matching_SRCS
    matching.cc)

find_package(Boost 1.54 COMPONENTS system filesystem QUIET)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(matching ${matching_SRCS} ${testing_SRCS})
    add_dependencies(buildtests matching)
    set (matching_subtests 0 1)
    foreach(subtest_id ${matching_subtests})
        add_test(NAME "matching_${subtest_id}"
                 #CONFIGURATIONS Release
                 COMMAND matching ${subtest_id})
        set_tests_properties ( "matching_${subtest_id}" PROPERTIES TIMEOUT 600) # in seconds
    endforeach()
    target_link_libraries(matching opengr_io opengr_accel opengr_algo opengr_utils)
    target_link_libraries(matching ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    add_dependencies(matching dataset-standford-armadillo dataset-standford-bunny)
    if(OpenGR_USE_CHEALPIX)
        target_link_libraries(matching ${Chealpix_LIBS} )
    endif(OpenGR_USE_CHEALPIX)
endif (Boost_FOUND)

#############################################
## matching3pcs
set(matching3pcs_SRCS
        matching3pcs.cc
        )

find_package(Boost 1.54 COMPONENTS system filesystem QUIET)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(matching3pcs ${matching3pcs_SRCS} ${testing_SRCS})
    add_dependencies(buildtests matching3pcs)
    set (matching3pcs_subtests 0 1)
    foreach(subtest_id ${matching3pcs_subtests})
        add_test(NAME "matching3pcs_${subtest_id}"
                #CONFIGURATIONS Release
                COMMAND matching3pcs ${subtest_id})
        set_tests_properties ( "matching3pcs_${subtest_id}" PROPERTIES TIMEOUT 600) # in seconds
    endforeach()
    target_link_libraries(matching3pcs opengr_io opengr_accel opengr_algo opengr_utils)
    target_link_libraries(matching3pcs ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    add_dependencies(matching3pcs dataset-standford-armadillo dataset-standford-bunny)
    if(OpenGR_USE_CHEALPIX)
        target_link_libraries(matching3pcs ${Chealpix_LIBS} )
    endif(OpenGR_USE_CHEALPIX)
endif (Boost_FOUND)