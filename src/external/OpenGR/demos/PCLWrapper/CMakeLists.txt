project(OpenGR-PCLWrapper)

file(GLOB_RECURSE Demo_PCL_headers
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

find_package( PCL 1.8 QUIET COMPONENTS common io search visualization)

if( PCL_FOUND )
  message (STATUS "[Demos] PCL found, creating target OpenGR-PCLWrapper")
  set(Demo_PCL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp )

  include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(${PROJECT_NAME} ${Demo_PCL_SRC} ${Demo_PCL_headers})
  target_link_libraries(${PROJECT_NAME} opengr_algo opengr_utils ${PCL_LIBRARIES})

  # [C/C++]>[General]>[Additional Include Directories]
  include_directories( ${PCL_INCLUDE_DIRS} )

  # [C/C++]>[Preprocessor]>[Preprocessor Definitions]
  add_definitions( ${PCL_DEFINITIONS} )

  # For Use Not PreCompiled Features
  #add_definitions( -DPCL_NO_PRECOMPILE )

  # [Linker]>[General]>[Additional Library Directories]
  link_directories( ${PCL_LIBRARY_DIRS} )

  install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

endif()


install( DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
         DESTINATION include
         FILES_MATCHING REGEX ".*(h|hpp)$")
