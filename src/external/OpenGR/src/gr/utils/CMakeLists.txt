project (OpenGR-UtilsLib)

set(utils_ROOT "${SRC_DIR}/gr/utils")

file(GLOB_RECURSE utils_relative_INCLUDE ${utils_ROOT}/*.h ${utils_ROOT}/*.hpp)

# compute accel_INCLUDE to get absolute filenames
set(utils_INCLUDE "")
set(utils_installed_INCLUDE "")
foreach(f ${utils_relative_INCLUDE})
    list(APPEND utils_INCLUDE "${f}")
    file(RELATIVE_PATH f_relative "${SRC_DIR}" "${f}")
    list(APPEND utils_installed_INCLUDE "include/${f_relative}")
endforeach(f)

add_library(opengr_utils INTERFACE)
target_sources(opengr_utils INTERFACE
    $<BUILD_INTERFACE:${utils_INCLUDE}>
    $<INSTALL_INTERFACE:${utils_installed_INCLUDE}>
    )
#############################################

set(targets_export_name "${PROJECT_NAME}Targets")
install( TARGETS opengr_utils
    EXPORT "${targets_export_name}"
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include  )

#############################################
#
# HACK: have the files showing in the IDE, under the name 'opengr_utils_IDE'
# Source: https://stackoverflow.com/questions/27039019/how-to-have-cmake-show-headers-that-are-not-part-of-any-binary-target-in-the-ide/29214327#29214327
#
add_custom_target(opengr_utils_IDE SOURCES ${utils_INCLUDE})
