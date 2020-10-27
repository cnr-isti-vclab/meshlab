if(TARGET Lib3ds::Lib3ds)
    message(STATUS "- lib3ds - using system-provided library")
    add_library(external-lib3ds INTERFACE)
    target_link_libraries(external-lib3ds INTERFACE Lib3ds::Lib3ds)
else()
    message(STATUS "- lib3ds - using bundled source")
    add_library(
        external-lib3ds STATIC
        "${LIB3DS_DIR}/lib3ds/atmosphere.c"
        "${LIB3DS_DIR}/lib3ds/atmosphere.h"
        "${LIB3DS_DIR}/lib3ds/background.c"
        "${LIB3DS_DIR}/lib3ds/background.h"
        "${LIB3DS_DIR}/lib3ds/camera.c"
        "${LIB3DS_DIR}/lib3ds/camera.h"
        "${LIB3DS_DIR}/lib3ds/chunk.c"
        "${LIB3DS_DIR}/lib3ds/chunk.h"
        "${LIB3DS_DIR}/lib3ds/chunktable.h"
        "${LIB3DS_DIR}/lib3ds/ease.c"
        "${LIB3DS_DIR}/lib3ds/ease.h"
        "${LIB3DS_DIR}/lib3ds/file.c"
        "${LIB3DS_DIR}/lib3ds/file.h"
        "${LIB3DS_DIR}/lib3ds/io.c"
        "${LIB3DS_DIR}/lib3ds/io.h"
        "${LIB3DS_DIR}/lib3ds/light.c"
        "${LIB3DS_DIR}/lib3ds/light.h"
        "${LIB3DS_DIR}/lib3ds/material.c"
        "${LIB3DS_DIR}/lib3ds/material.h"
        "${LIB3DS_DIR}/lib3ds/matrix.c"
        "${LIB3DS_DIR}/lib3ds/matrix.h"
        "${LIB3DS_DIR}/lib3ds/mesh.c"
        "${LIB3DS_DIR}/lib3ds/mesh.h"
        "${LIB3DS_DIR}/lib3ds/node.c"
        "${LIB3DS_DIR}/lib3ds/node.h"
        "${LIB3DS_DIR}/lib3ds/quat.c"
        "${LIB3DS_DIR}/lib3ds/quat.h"
        "${LIB3DS_DIR}/lib3ds/shadow.c"
        "${LIB3DS_DIR}/lib3ds/shadow.h"
        "${LIB3DS_DIR}/lib3ds/tcb.c"
        "${LIB3DS_DIR}/lib3ds/tcb.h"
        "${LIB3DS_DIR}/lib3ds/tracks.c"
        "${LIB3DS_DIR}/lib3ds/tracks.h"
        "${LIB3DS_DIR}/lib3ds/types.h"
        "${LIB3DS_DIR}/lib3ds/vector.c"
        "${LIB3DS_DIR}/lib3ds/vector.h"
        "${LIB3DS_DIR}/lib3ds/viewport.c"
        "${LIB3DS_DIR}/lib3ds/viewport.h")
    target_include_directories(external-lib3ds SYSTEM PUBLIC "${LIB3DS_DIR}")
    target_compile_definitions(external-lib3ds PUBLIC LIB3DS_STATIC)
    set_property(TARGET external-lib3ds PROPERTY FOLDER External)
endif()
