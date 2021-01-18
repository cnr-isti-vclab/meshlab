project (OpenGR-IOLib)

set(io_ROOT "${SRC_DIR}/gr/io")

file(GLOB_RECURSE io_sources ${io_ROOT}/*.cc)
file(GLOB_RECURSE io_headers ${io_ROOT}/*.h ${io_ROOT}/*.hpp)

add_library(opengr_io ${io_sources} ${io_headers})

if(IO_USE_BOOST)
    find_package(Boost 1.54 COMPONENTS system filesystem REQUIRED)
    if (Boost_FOUND)
        message(STATUS "[Deps] Use Boost ${Boost_VERSION} for image loading")
        add_definitions(-DUSE_BOOST)
        target_include_directories(opengr_io PRIVATE ${Boost_INCLUDE_DIRS} )
        target_link_libraries(opengr_io ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})
    else ()
        message(STATUS "[Deps] Boost not found")
    endif (Boost_FOUND)
endif(IO_USE_BOOST)

set(targets_export_name "${PROJECT_NAME}Targets")
install( TARGETS opengr_io
    EXPORT "${targets_export_name}"
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include  )
