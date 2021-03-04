# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0


set(SOURCES 
	apss.cpp 
	balltree.cpp 
	mlsplugin.cpp 
	rimls.cpp)

set(HEADERS
	apss.h
	balltree.h
	implicits.h
	mlsmarchingcube.h
	mlsplugin.h
	mlssurface.h
	priorityqueue.h
	rimls.h
	smallcomponentselection.h)

SET(TPP_HEADERS
	apss.tpp
	mlssurface.tpp
	rimls.tpp)

add_meshlab_plugin(filter_mls ${SOURCES} ${HEADERS} ${TPP_HEADERS})
