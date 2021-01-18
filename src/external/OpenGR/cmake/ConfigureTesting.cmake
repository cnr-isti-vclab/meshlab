


add_custom_target(buildtests)
add_custom_target(check COMMAND "ctest")
add_dependencies(check buildtests)

# check whether /bin/bash exists
find_file(OpenGR_BIN_BASH_EXISTS "/bin/bash" PATHS "/" NO_DEFAULT_PATH)

# CMake/Ctest does not allow us to change the build command,
# so we have to workaround by directly editing the generated DartConfiguration.tcl file
# save CMAKE_MAKE_PROGRAM
set(CMAKE_MAKE_PROGRAM_SAVE ${CMAKE_MAKE_PROGRAM})
# and set a fake one
set(CMAKE_MAKE_PROGRAM "@OpenGR_MAKECOMMAND_PLACEHOLDER@")

# This call activates testing and generates the DartConfiguration.tcl
# This adds another build target, which is test for Makefile generators,
# or RUN_TESTS for integrated development environments (like Visual Studio)
include(CTest)

set(OpenGR_TEST_BUILD_FLAGS "" CACHE STRING "Options passed to the build command of unit tests")

# overwrite default DartConfiguration.tcl
# The worarounds are different for each version of the MSVC IDE
set(OpenGR_TEST_TARGET buildtests)
if(MSVC_IDE)
  if(CMAKE_MAKE_PROGRAM_SAVE MATCHES "devenv") # devenv
    set(OpenGR_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM_SAVE} OpenGR.sln /build Release /project ${OpenGR_TEST_TARGET}")
  else() # msbuild
    set(OpenGR_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM_SAVE} ${OpenGR_TEST_TARGET}.vcxproj /p:Configuration=\${CTEST_CONFIGURATION_TYPE}")
  endif()

  # append the build flags if provided
  if(NOT "${OpenGR_TEST_BUILD_FLAGS}" MATCHES "^[ \t]*$")
    set(OpenGR_BUILD_COMMAND "${OpenGR_BUILD_COMMAND} ${OpenGR_TEST_BUILD_FLAGS}")
  endif()

  # apply the dartconfig hack ...
  set(OpenGR_MAKECOMMAND_PLACEHOLDER "${OpenGR_BUILD_COMMAND}\n#")
else()
  # for make and nmake
  set(OpenGR_BUILD_COMMAND "${CMAKE_MAKE_PROGRAM_SAVE} ${OpenGR_TEST_TARGET} ${OpenGR_TEST_BUILD_FLAGS}")
  set(OpenGR_MAKECOMMAND_PLACEHOLDER "${OpenGR_BUILD_COMMAND}")
endif()

configure_file(${CMAKE_BINARY_DIR}/DartConfiguration.tcl ${CMAKE_BINARY_DIR}/DartConfiguration.tcl)

# restore default CMAKE_MAKE_PROGRAM
set(CMAKE_MAKE_PROGRAM ${CMAKE_MAKE_PROGRAM_SAVE})

# un-set temporary variables so that it is like they never existed
unset(CMAKE_MAKE_PROGRAM_SAVE)
unset(OpenGR_MAKECOMMAND_PLACEHOLDER)


## Configure coverage
#if(CMAKE_COMPILER_IS_GNUCXX)
#  option(OpenGR_COVERAGE_TESTING "Enable/disable gcov" ON)

#  if(OpenGR_COVERAGE_TESTING)
#    set(COVERAGE_FLAGS "-fprofile-arcs -ftest-coverage")
#  else(OpenGR_COVERAGE_TESTING)
#    set(COVERAGE_FLAGS "")
#  endif(OpenGR_COVERAGE_TESTING)

#  if(OpenGR_TEST_C++0x)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")
#  endif(OpenGR_TEST_C++0x)

#  if(CMAKE_SYSTEM_NAME MATCHES Linux)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS} -g2")
#    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${COVERAGE_FLAGS} -O2 -g2")
#    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${COVERAGE_FLAGS} -fno-inline-functions")
#    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${COVERAGE_FLAGS} -O0 -g3")
#  endif(CMAKE_SYSTEM_NAME MATCHES Linux)

#elseif(MSVC)

#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_WARNINGS /D_SCL_SECURE_NO_WARNINGS")

#endif(CMAKE_COMPILER_IS_GNUCXX)

