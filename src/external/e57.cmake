# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, 2021 Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_BUNDLED_LIBE57_FORMAT "Allow use of bundled libE57Format source" ON)
option(ALLOW_SYSTEM_LIBE57_FORMAT "Allow use of system-provided libE57Format" ON)

set(E57Format_DIR ${EXTERNAL_DIR}/e57)

if (TARGET external-xerces)
	if(ALLOW_SYSTEM_LIBE57_FORMAT AND TARGET E57Format::E57Format)

		message(STATUS "- libE57Format - using system-provided library")
		add_library(external-libE57Format INTERFACE)
		target_link_libraries(external-libE57Format INTERFACE E57Format::E57Format)

	elseif(ALLOW_BUNDLED_LIBE57_FORMAT)

		message(STATUS "- libE57Format - using bundled source")

		set(E57_BUILD_SHARED OFF)
		set(MESSAGE_QUIET ON)
		add_subdirectory(e57 EXCLUDE_FROM_ALL)
		unset(MESSAGE_QUIET)

		add_library(external-libE57Format INTERFACE)
		target_link_libraries(external-libE57Format INTERFACE E57Format)

	endif()
endif()
