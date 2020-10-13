if(TARGET OpenCTM::OpenCTM)
    message(STATUS "- OpenCTM - using system-provided library")
    add_library(external-openctm INTERFACE)
    target_link_libraries(external-openctm INTERFACE OpenCTM::OpenCTM)
else()
    message(STATUS "- OpenCTM - using bundled source")
    # Modified liblzma included - can't build against system version
    add_library(
        external-openctm STATIC
        "${OPENCTM_DIR}/lib/openctm.c"
        "${OPENCTM_DIR}/lib/stream.c"
        "${OPENCTM_DIR}/lib/compressRAW.c"
        "${OPENCTM_DIR}/lib/compressMG1.c"
        "${OPENCTM_DIR}/lib/compressMG2.c"
        "${OPENCTM_DIR}/lib/liblzma/Alloc.c"
        "${OPENCTM_DIR}/lib/liblzma/LzFind.c"
        "${OPENCTM_DIR}/lib/liblzma/LzmaDec.c"
        "${OPENCTM_DIR}/lib/liblzma/LzmaEnc.c"
        "${OPENCTM_DIR}/lib/liblzma/LzmaLib.c")
    target_compile_definitions(external-openctm PUBLIC OPENCTM_STATIC)
    target_include_directories(
        external-openctm SYSTEM
        PUBLIC "${OPENCTM_DIR}/lib" SYSTEM
        PRIVATE "${OPENCTM_DIR}/lib/liblzma")
    if(MSVC)
        target_compile_definitions(external-openctm PRIVATE _CRT_SECURE_NO_WARNINGS)
    endif()
    set_property(TARGET external-openctm PROPERTY FOLDER External)
endif()
