# OS X requires a Mach-O dynamic library to have a baked "install name", that is used by other modules to link to it. Depending
# on how the library is built, the install name is not always an absolute path, nor necessarily the same as the name of the
# library file itself. This macro takes as input the name of a target, and a list of libraries that it links to (the output of
# FIND_PACKAGE or FIND_LIBRARY calls), and generates a set of custom, post-build commands that, for each linked dylib, changes
# the name the target uses to refer to it with a fully-qualified (absolute) version of the library's own install name. This
# helps ensure that the target can be used from any location while still being able to locate the linked dynamic libraries.
#
# Note that this script does NOT handle the case when a linked library itself refers to another library using a non-absolute
# name (Boost is a notorious example). To avoid such issues, it is recommended to use a static library instead of a shared one
# in a non-standard location. Alternatively, set DYLD_LIBRARY_PATH to include these non-standard locations when running the
# program (not recommended).
#
# Author: Siddhartha Chaudhuri, 2009.
#

# Set the minimum required CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# See cmake --help-policy CMP0011 for details on this one
IF(POLICY CMP0011)
  CMAKE_POLICY(SET CMP0011 NEW)
ENDIF(POLICY CMP0011)

# See cmake --help-policy CMP0026 for details on this one
IF(POLICY CMP0026)
  CMAKE_POLICY(SET CMP0026 NEW)
ENDIF(POLICY CMP0026)

# See cmake --help-policy CMP0045 for details on this one
IF(POLICY CMP0045)
  CMAKE_POLICY(SET CMP0045 NEW)
ENDIF(POLICY CMP0045)

MACRO(OSX_FIX_DYLIB_REFERENCES target libraries)

  IF(APPLE)
    SET(OFIN_${target}_RPATHS )

    FOREACH(OFIN_${target}_Library ${libraries})
      IF(${OFIN_${target}_Library} MATCHES "[.]dylib$"
         OR ${OFIN_${target}_Library} MATCHES "[.]framework/.+")

        # Resolve symlinks and get absolute location
        GET_FILENAME_COMPONENT(OFIN_${target}_LibraryAbsolute ${OFIN_${target}_Library} ABSOLUTE)

        # Get the baked install name of the library
        EXECUTE_PROCESS(COMMAND otool -D ${OFIN_${target}_LibraryAbsolute}
                        OUTPUT_VARIABLE OFIN_${target}_LibraryInstallNameOutput
                        OUTPUT_STRIP_TRAILING_WHITESPACE)
        STRING(REGEX REPLACE "[\r\n]" " " OFIN_${target}_LibraryInstallNameOutput ${OFIN_${target}_LibraryInstallNameOutput})
        SEPARATE_ARGUMENTS(OFIN_${target}_LibraryInstallNameOutput)
        LIST(GET OFIN_${target}_LibraryInstallNameOutput 1 OFIN_${target}_LibraryInstallName)

        IF(${OFIN_${target}_LibraryInstallName} MATCHES "^[@]rpath/")

          # Ideally, we want to eliminate the longest common suffix of the install name and the absolute path. Whatever's left
          # will be the desired rpath. But this is difficult to do (especially if there are naming variations, e.g.
          # "Versions/Current" vs "Versions/5" is a common culprit). So we'll add various candidate rpaths and hope at least one
          # is correct.

          # Typically, the rpath to a library within a framework looks like this:
          # @rpath/A.framework/Versions/5/libFoo.dylib
          #
          # Hence, we'll extract for the path unit immediately following the @rpath (in this case A.framework) and then look for
          # it in the library's actual path. Everything before this location will be put in the rpath.
          SET(OFIN_${target}_PathPrefix ${OFIN_${target}_LibraryInstallName})
          SET(OFIN_${target}_RpathFirstChild )
          WHILE(NOT OFIN_${target}_PathPrefix STREQUAL "@rpath")
            GET_FILENAME_COMPONENT(OFIN_${target}_RpathFirstChild  ${OFIN_${target}_PathPrefix} NAME)
            GET_FILENAME_COMPONENT(OFIN_${target}_PathPrefix       ${OFIN_${target}_PathPrefix} PATH)

            IF(NOT OFIN_${target}_PathPrefix)  # should never happen but just in case
              BREAK()
            ENDIF(NOT OFIN_${target}_PathPrefix)

            IF(OFIN_${target}_PathPrefix STREQUAL "/")  # should never happen but just in case
              BREAK()
            ENDIF(OFIN_${target}_PathPrefix STREQUAL "/")
          ENDWHILE(NOT OFIN_${target}_PathPrefix STREQUAL "@rpath")

          IF(OFIN_${target}_RpathFirstChild)
            SET(OFIN_${target}_PathPrefix ${OFIN_${target}_LibraryAbsolute})
            SET(OFIN_${target}_PathUnit )
            WHILE(NOT OFIN_${target}_PathUnit STREQUAL ${OFIN_${target}_RpathFirstChild})
              GET_FILENAME_COMPONENT(OFIN_${target}_PathUnit    ${OFIN_${target}_PathPrefix} NAME)
              GET_FILENAME_COMPONENT(OFIN_${target}_PathPrefix  ${OFIN_${target}_PathPrefix} PATH)

              IF(NOT OFIN_${target}_PathPrefix)
                BREAK()
              ENDIF(NOT OFIN_${target}_PathPrefix)

              IF(OFIN_${target}_PathPrefix STREQUAL "/")
                BREAK()
              ENDIF(OFIN_${target}_PathPrefix STREQUAL "/")
            ENDWHILE(NOT OFIN_${target}_PathUnit STREQUAL ${OFIN_${target}_RpathFirstChild})

            IF(OFIN_${target}_PathPrefix)
              SET(OFIN_${target}_RPATHS ${OFIN_${target}_RPATHS} "${OFIN_${target}_PathPrefix}")
            ENDIF(OFIN_${target}_PathPrefix)
          ENDIF(OFIN_${target}_RpathFirstChild)

          # Add the directory containing the library
          GET_FILENAME_COMPONENT(OFIN_${target}_LibraryAbsolutePath ${OFIN_${target}_LibraryAbsolute} PATH)
          SET(OFIN_${target}_RPATHS ${OFIN_${target}_RPATHS} "${OFIN_${target}_LibraryAbsolutePath}")

          # Add paths specified as library search prefixes
          FOREACH(prefix ${CMAKE_PREFIX_PATH})
            SET(OFIN_${target}_RPATHS ${OFIN_${target}_RPATHS} "${CMAKE_PREFIX_PATH}")
            SET(OFIN_${target}_RPATHS ${OFIN_${target}_RPATHS} "${CMAKE_PREFIX_PATH}/lib")
          ENDFOREACH()

        ELSEIF(NOT ${OFIN_${target}_LibraryInstallName} MATCHES "^[@/]")  # just a relative path

          # Replace the unqualified filename, if it appears, with the absolute location, either by directly changing the path or
          # by editing the rpath

          # -- handle the case when the actual filename is baked in
          GET_FILENAME_COMPONENT(OFIN_${target}_LibraryFilename ${OFIN_${target}_LibraryAbsolute} NAME)
          ADD_CUSTOM_COMMAND(TARGET ${target} POST_BUILD
                             COMMAND install_name_tool
                             ARGS -change
                                  ${OFIN_${target}_LibraryFilename}
                                  ${OFIN_${target}_LibraryAbsolute}
                                  $<TARGET_FILE:${target}>)

          # -- handle the case when the install name is baked in
          ADD_CUSTOM_COMMAND(TARGET ${target} POST_BUILD
                             COMMAND install_name_tool
                             ARGS -change
                                  ${OFIN_${target}_LibraryInstallName}
                                  ${OFIN_${target}_LibraryAbsolute}
                                  $<TARGET_FILE:${target}>)
        ENDIF()

      ENDIF()
    ENDFOREACH(OFIN_${target}_Library)

    # Add the collected rpaths
    IF(OFIN_${target}_RPATHS)
      LIST(REMOVE_DUPLICATES OFIN_${target}_RPATHS)

      FOREACH(rpath ${OFIN_${target}_RPATHS})
        ADD_CUSTOM_COMMAND(TARGET ${target} POST_BUILD
                           COMMAND bash
                           ARGS -c "install_name_tool -add_rpath '${rpath}' '$<TARGET_FILE:${target}>' > /dev/null 2>&1 || true"
                           VERBATIM)
      ENDFOREACH()
    ENDIF()
  ENDIF()

ENDMACRO(OSX_FIX_DYLIB_REFERENCES)
