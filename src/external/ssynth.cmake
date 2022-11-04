# Copyright 2019, 2020, Collabora, Ltd.
# Copyright 2019, 2020, Visual Computing Lab, ISTI - Italian National Research Council
# SPDX-License-Identifier: BSL-1.0

option(MESHLAB_ALLOW_DOWNLOAD_SOURCE_STRUCTURE_SYNTH "Allow use of bundled Structure Synth source" ON)

if(MESHLAB_ALLOW_DOWNLOAD_SOURCE_STRUCTURE_SYNTH)
	set(SSYNTH_DIR "${MESHLAB_EXTERNAL_DOWNLOAD_DIR}/StructureSynth-1.5.1")
	set(SSYNTH_CHECK "${SSYNTH_DIR}/StructureSynth/Model/Action.h")

	if (NOT EXISTS ${SSYNTH_CHECK})
		set(SSYNTH_LINK
			https://github.com/alemuntoni/StructureSynth/archive/refs/tags/1.5.1.zip
			https://www.meshlab.net/data/libs/StructureSynth-1.5.1.zip)
		set(SSYNTH_MD5 e2122c5e6e370fb36c885bccf59562b7)
		download_and_unzip(
			NAME "Structure Synth"
			LINK ${SSYNTH_LINK}
			MD5 ${SSYNTH_MD5}
			DIR ${MESHLAB_EXTERNAL_DOWNLOAD_DIR})
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
