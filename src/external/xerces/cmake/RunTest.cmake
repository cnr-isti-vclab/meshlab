# CMake tests for libtiff (common functionality)
#
# Copyright © 2015 Open Microscopy Environment / University of Dundee
# Written by Roger Leigh <rleigh@codelibre.net>
#
# Permission to use, copy, modify, distribute, and sell this software and
# its documentation for any purpose is hereby granted without fee, provided
# that (i) the above copyright notices and this permission notice appear in
# all copies of the software and related documentation, and (ii) the names of
# Sam Leffler and Silicon Graphics may not be used in any advertising or
# publicity relating to the software without the specific, prior written
# permission of Sam Leffler and Silicon Graphics.
#
# THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
# EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
# WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
#
# IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
# ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
# OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
# WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
# LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
# OF THIS SOFTWARE.

# Run a test command, and print out any observed output discrepancies
macro(test_command command)
  file(MAKE_DIRECTORY "${OBSERVED_DIR}")

  if(STDIN)
    set(input INPUT_FILE "${STDIN}")
  endif()

  string(REGEX REPLACE ";" " " display_args "${ARGN}")
  message(STATUS "Running ${command} ${display_args}")

  set(ENV{XERCESC_NLS_HOME} "${NLS_HOME}")

  execute_process(
    COMMAND "${command}" ${ARGN}
    RESULT_VARIABLE TEST_STATUS
    ${input}
    OUTPUT_FILE "${OBSERVED_DIR}/${NAME}.log"
    ERROR_FILE "${OBSERVED_DIR}/${NAME}.log"
    WORKING_DIRECTORY "${WORKDIR}")
  if(TEST_STATUS AND NOT EXPECT_FAIL)
    message(FATAL_ERROR "Returned failed status ${TEST_STATUS}")
  endif()
  if(NOT TEST_STATUS AND EXPECT_FAIL)
    message(FATAL_ERROR "Unexpected success")
  endif()

  # Replace timings in output, and convert newlines for safe comparison
  file(READ "${OBSERVED_DIR}/${NAME}.log" output)
  string(REGEX REPLACE " *[0-9]+ *ms *" "{timing removed}" output "${output}")
  file(WRITE "${OBSERVED_DIR}/${NAME}-mod.log" "${output}")
  configure_file("${OBSERVED_DIR}/${NAME}-mod.log" "${OBSERVED_DIR}/${NAME}.log"
    @ONLY NEWLINE_STYLE UNIX)
  configure_file("${EXPECTED_DIR}/${NAME}.log" "${OBSERVED_DIR}/${NAME}-exp.log"
    @ONLY NEWLINE_STYLE UNIX)
  file(REMOVE "${OBSERVED_DIR}/${NAME}-mod.log")

  file(READ "${OBSERVED_DIR}/${NAME}.log" observed)
  file(READ "${OBSERVED_DIR}/${NAME}-exp.log" expected)

  # If the observed and expected logs differ, output a diff and fail
  if(NOT observed STREQUAL expected)
    if(DIFF)
      execute_process(COMMAND ${DIFF} -u "${OBSERVED_DIR}/${NAME}-exp.log" "${OBSERVED_DIR}/${NAME}.log")
    endif()
    file(REMOVE "${OBSERVED_DIR}/${NAME}-exp.log")
    message(FATAL_ERROR "Observed output does not match expected output")
  endif()
  file(REMOVE "${OBSERVED_DIR}/${NAME}-exp.log")
endmacro()

# Add the directory containing libxerces-c to the PATH (Windows only)
if(WIN32)
  get_filename_component(LIBXERCES_C_DIR "${LIBXERCES_C}" DIRECTORY)
  file(TO_NATIVE_PATH "${LIBXERCES_C_DIR}" LIBXERCES_C_DIR)
  set(ENV{PATH} "${LIBXERCES_C_DIR};$ENV{PATH}")
endif()
if(CYGWIN)
  get_filename_component(LIBXERCES_C_DIR "${LIBXERCES_C}" DIRECTORY)
  file(TO_NATIVE_PATH "${LIBXERCES_C_DIR}" LIBXERCES_C_DIR)
  set(ENV{PATH} "${LIBXERCES_C_DIR}:$ENV{PATH}")
endif()

test_command("${PROGRAM}" ${ARGS})
