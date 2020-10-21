include (CoMISoExample)

if (WIN32)
  acg_add_executable (quadratic_solver WIN32 ${sources} ${headers} )
else ()
  acg_add_executable (quadratic_solver ${sources} ${headers} )
endif ()

# enable rpath linking
set_target_properties(quadratic_solver PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

target_link_libraries (quadratic_solver
  CoMISo
  ${COMISO_LINK_LIBRARIES}
)
