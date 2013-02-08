include (CoMISoExample)

if (WIN32)
  acg_add_executable (small_cplex_soc WIN32 ${sources} ${headers} )
else ()
  acg_add_executable (small_cplex_soc ${sources} ${headers} )
endif ()

# enable rpath linking
set_target_properties(small_cplex_soc PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

target_link_libraries (small_cplex_soc
  CoMISo
  ${COMISO_LINK_LIBRARIES}
)
