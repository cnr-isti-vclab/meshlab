/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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
/****************************************************************************
  History

 $Log$
 Revision 1.2  2005/12/11 00:34:22  fmazzant
 bug-fix: added the type of return of the MaskObj method::MaskObjToInt()

 Revision 1.1  2005/12/09 16:37:20  fmazzant
 maskobj for select element to save

 
*****************************************************************************/


#include "maskobj.h"
#include <wrap/ply/io_mask.h>

MaskObj::MaskObj()
{
	this->isfirst=true;

	this->faces = true;
	this->vertexs = true;

	this->binary = false;
	this->colorV = false;
	this->normal = false;
	this->texture = false;
}

int MaskObj::MaskObjToInt()
{
	int mask;
	if(faces)	{mask |= vcg::ply::PLYMask::PM_FACEQUALITY;}
	if(vertexs)	{mask |= vcg::ply::PLYMask::PM_VERTQUALITY;}
	if(colorV)	{mask |= vcg::ply::PLYMask::PM_VERTCOLOR;}
	if(normal)	{mask |= vcg::ply::PLYMask::PM_WEDGNORMAL;}
	if(texture)	{mask |= vcg::ply::PLYMask::PM_WEDGTEXCOORD;}
	//if(colorF){}
	return mask;
}