/*****************************************************************************
 * MeshLab                                                           o o     *
 * A versatile mesh processing toolbox                             o     o   *
 *                                                                _   O  _   *
 * Copyright(C) 2004-2022                                           \/)\/    *
 * Visual Computing Lab                                            /\/|      *
 * ISTI - Italian National Research Council                           |      *
 *                                                                    \      *
 * All rights reserved.                                                      *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
 * for more details.                                                         *
 *                                                                           *
 ****************************************************************************/

#ifndef MESHLAB_RICH_PARAMETERS_H
#define MESHLAB_RICH_PARAMETERS_H

#include "rich_parameter/rich_bool.h"
#include "rich_parameter/rich_color.h"
#include "rich_parameter/rich_direction.h"
#include "rich_parameter/rich_dynamic_float.h"
#include "rich_parameter/rich_enum.h"
#include "rich_parameter/rich_float.h"
#include "rich_parameter/rich_file_open.h"
#include "rich_parameter/rich_file_save.h"
#include "rich_parameter/rich_int.h"
#include "rich_parameter/rich_matrix44.h"
#include "rich_parameter/rich_mesh.h"
#include "rich_parameter/rich_percentage.h"
#include "rich_parameter/rich_position.h"
#include "rich_parameter/rich_shot.h"
#include "rich_parameter/rich_string.h"
#include "rich_parameter/rich_parameter.h"

class RichParameterAdapter
{
public:
	static bool create(const QDomElement& np, RichParameter*& par);
};

#endif // MESHLAB_RICH_PARAMETERS_H
