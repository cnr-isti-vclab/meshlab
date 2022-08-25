include (CoMISoExample)

if (WIN32)
  acg_add_executable (small_sparseqr WIN32 ${sources} ${headers} )
else ()
  acg_add_executable (small_sparseqr ${sources} ${headers} )
endif ()

# enable rpath linking
set_target_properties(small_sparseqr PROPERTIES INSTALL_RPATH_USE_LINK_PATH 1)

target_link_libraries (small_sparseqr
  CoMISo
  ${COMISO_LINK_LIBRARIES}
)
