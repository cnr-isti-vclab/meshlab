# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(ALLOW_SYSTEM_XERCES "Allow use of system-provided Xerces" ON)
option(ALLOW_BUNDLED_XERCES "Allow use of bundled Xercers sources" ON)

set(XERCES_DIR ${EXTERNAL_DIR}/xerces)

find_package(XercesC)

if(ALLOW_SYSTEM_XERCES AND TARGET XercesC::XercesC)

	message(STATUS "- XercesC - using system-provided library")
	add_library(external-xerces INTERFACE)
	target_link_libraries(external-xerces INTERFACE XercesC::XercesC)

elseif(ALLOW_BUNDLED_XERCES AND EXISTS "${XERCES_DIR}/CMakeLists.txt")

	message(STATUS "- XercesC - using bundled source")
	add_subdirectory(${XERCES_DIR})
	add_library(external-xerces INTERFACE)
	target_link_libraries(external-xerces INTERFACE xerces-c)
	target_include_directories(
		external-xerces INTERFACE
		${XERCES_DIR}/src
		${CMAKE_CURRENT_BINARY_DIR}/xerces/src)

endif()
