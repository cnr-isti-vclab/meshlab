#############################################################################
# MeshLab                                                           o o     #
# A versatile mesh processing toolbox                             o     o   #
#                                                                _   O  _   #
# Copyright(C) 2023 - 2024                                         \/)\/    #
# Visual Computing Lab                                            /\/|      #
# ISTI - Italian National Research Council                           |      #
#                                                                    \      #
# All rights reserved.                                                      #
#                                                                           #
# This program is free software; you can redistribute it and/or modify      #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 2 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# This program is distributed in the hope that it will be useful,           #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          #
# for more details.                                                         #
#                                                                           #
#############################################################################

set(HEADERS
	io_3mf.h
	)

set(SOURCES
	io_3mf.cpp
	)

add_meshlab_plugin(io_3mf ${SOURCES} ${HEADERS})

target_link_libraries(io_3mf PUBLIC external-lib3mf)
