# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_STRUCTURE_SYNTH "Allow use of bundled Structure Synth source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_STRUCTURE_SYNTH)
	set(SSYNTH_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/StructureSynth-1.5.1")
	set(SSYNTH_CHECK "${SSYNTH_DIR}/StructureSynth/Model/Action.h")

	if (NOT EXISTS ${SSYNTH_CHECK})
		set(SSYNTH_LINK "https://github.com/alemuntoni/StructureSynth/archive/refs/tags/1.5.1.zip")
		download_and_unzip(
			LINK ${SSYNTH_LINK}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR}
			NAME "Structure Synth")
		if (NOT download_and_unzip_SUCCESS)
			message(STATUS "- Structure Synth - download failed.")
		endif()
	endif()

	if(EXISTS ${SSYNTH_CHECK})
		message(STATUS "- Structure Synth - using downloaded source")

		add_subdirectory(${SSYNTH_DIR})
		add_library(external-ssynth INTERFACE)
		target_link_libraries(external-ssynth INTERFACE structure-synth)
	endif()
endif()
