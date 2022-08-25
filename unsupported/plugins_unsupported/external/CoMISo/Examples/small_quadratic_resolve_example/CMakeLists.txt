include (CoMISoExample)

if (WIN32)
  acg_add_executable (small_quadratic_resolve WIN32 ${sources} ${headers} )
else ()
  acg_add_executable (small_quadratic_resolve ${sources} ${headers} )
endif ()

# enable rpath linking
set_target_properties(small_quadratic_resolve PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

target_link_libraries (small_quadratic_resolve
  CoMISo
  ${COMISO_LINK_LIBRARIES}
)
