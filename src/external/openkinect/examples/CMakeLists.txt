######################################################################################
# Packages needed for examples
######################################################################################

add_executable(glview glview.c)
add_executable(cppview cppview.cpp)
add_executable(glpclview glpclview.c)

# We need to include libfreenect_sync.h for glpclview
include_directories (../wrappers/c_sync/)

# Mac just has everything already
if(APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "-framework OpenGL -framework GLUT")
  target_link_libraries(glview freenect)
  target_link_libraries(cppview freenect)
  target_link_libraries(glpclview freenect_sync)
# Linux, not so much
else()
  find_package(Threads REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(GLUT REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS} ${USB_INCLUDE_DIRS})
  target_link_libraries(glview freenect ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} m)
  target_link_libraries(cppview freenect ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} m)
  target_link_libraries(glpclview freenect_sync ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} ${CMAKE_THREAD_LIBS_INIT} m)
endif()

install (TARGETS glview glpclview
  DESTINATION bin)

install (TARGETS cppview
  DESTINATION bin)
