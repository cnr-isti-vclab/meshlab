# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_STRUCTURE_SYNTH "Allow use of bundled structure-synth source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_STRUCTURE_SYNTH)
	set(SSYNTH_VER 1.5.1)
	set(SSYNTH_DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/StructureSynth-${SSYNTH_VER})

	if (NOT EXISTS "${SSYNTH_DIR}/StructureSynth/Model/Action.h")
		set(SSYNTH_LINK "https://github.com/alemuntoni/StructureSynth/archive/refs/tags/${SSYNTH_VER}.zip")
		download_and_unzip(${SSYNTH_LINK} ${MESHLAB_EXTERNAL_DOWNLOAD_DIR} "Structure Synth")
	endif()

	message(STATUS "- structure-synth - using downloaded source")

	add_subdirectory(${SSYNTH_DIR})
	add_library(external-ssynth INTERFACE)
	target_link_libraries(external-ssynth INTERFACE structure-synth)
endif()
