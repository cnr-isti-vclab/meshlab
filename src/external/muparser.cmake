# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_MUPARSER "Allow use of bundled muparser source" ON)
option(ALLOW_SYSTEM_MUPARSER "Allow use of system-provided muparser" ON)

find_package(muparser)

if(ALLOW_SYSTEM_MUPARSER AND TARGET muparser::muparser)
	message(STATUS "- muparser - using system-provided library")
	add_library(external-muparser INTERFACE)
	target_link_libraries(external-muparser INTERFACE muparser::muparser)
elseif(ALLOW_BUNDLED_MUPARSER AND EXISTS "${MUPARSER_DIR}/src/muParser.cpp")
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
	target_link_libraries(external-muparser PRIVATE external-disable-warnings)
endif()
