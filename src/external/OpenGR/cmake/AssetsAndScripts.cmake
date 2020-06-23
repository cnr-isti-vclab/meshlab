
# SCRIPTS
file (GLOB Scripts_SRC ${SCRIPTS_DIR}/*.sh
                       ${SCRIPTS_DIR}/*.bat)

install( FILES ${Scripts_SRC}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/scripts
         PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
add_custom_target(opengr_scripts_IDE SOURCES ${Scripts_SRC})


# ASSETS
if (DL_DATASETS)
    file (GLOB_RECURSE Assets_SRC  ${ASSETS_DIR}/*)
    add_custom_target(opengr_assets SOURCES ${Assets_SRC}
                                       DEPENDS dl-datasets)
endif (DL_DATASETS)

install( DIRECTORY ${ASSETS_DIR}/ DESTINATION ${CMAKE_INSTALL_PREFIX}/assets )
