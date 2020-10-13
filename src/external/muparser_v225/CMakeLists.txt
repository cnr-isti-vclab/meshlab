if(TARGET muparser::muparser)
    message(STATUS "- muparser - using system-provided library")
    add_library(external-muparser INTERFACE)
    target_link_libraries(external-muparser INTERFACE muparser::muparser)
else()
    message(STATUS "- muparser - using bundled source")
    add_library(
        external-muparser STATIC
        "${MUPARSER_DIR}/src/muParser.cpp"
        "${MUPARSER_DIR}/src/muParserBase.cpp"
        "${MUPARSER_DIR}/src/muParserBytecode.cpp"
        "${MUPARSER_DIR}/src/muParserCallback.cpp"
        "${MUPARSER_DIR}/src/muParserDLL.cpp"
        "${MUPARSER_DIR}/src/muParserError.cpp"
        "${MUPARSER_DIR}/src/muParserInt.cpp"
        "${MUPARSER_DIR}/src/muParserTest.cpp"
        "${MUPARSER_DIR}/src/muParserTokenReader.cpp")
    target_include_directories(external-muparser SYSTEM PUBLIC ${MUPARSER_DIR}/include)
    target_compile_definitions(external-muparser PUBLIC _UNICODE)
    set_property(TARGET external-muparser PROPERTY FOLDER External)
endif()
