# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


# Only build if we have newuoa and levmar
if(TARGET external-newuoa AND TARGET external-levmar)

	set(SOURCES
		alignset.cpp
		filter_mutualglobal.cpp
		levmarmethods.cpp
		mutual.cpp
		parameters.cpp
		pointCorrespondence.cpp
		solver.cpp)

	set(HEADERS
		alignset.h
		alignGlobal.h
		filter_mutualglobal.h
		levmarmethods.h
		mutual.h
		parameters.h
		pointCorrespondence.h
		pointOnLayer.h
		shutils.h
		solver.h)

	add_meshlab_plugin(filter_mutualglobal ${SOURCES} ${HEADERS})

	target_link_libraries(filter_mutualglobal PRIVATE external-newuoa
													  external-levmar)

else()
	message(
		STATUS
			"Skipping filter_mutualglobal - missing either newuoa or levmar in external directory."
	)
endif()
