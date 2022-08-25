######################################################################################
# C Synchronous Interface
######################################################################################
add_library (freenect_sync SHARED libfreenect_sync.c)
set_target_properties (freenect_sync PROPERTIES
  VERSION ${PROJECT_VER}
  SOVERSION ${PROJECT_APIVER})

target_link_libraries (freenect_sync freenect)

install (TARGETS freenect_sync
  DESTINATION "${PROJECT_LIBRARY_INSTALL_DIR}")
install (FILES "libfreenect_sync.h"
  DESTINATION ${PROJECT_INCLUDE_INSTALL_DIR})