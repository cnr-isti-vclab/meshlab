/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#pragma once
namespace vcg
{

class CICubeMap 
{
public:
	GLuint ti;

	GLuint oti[6];
	
	bool Load(const char *basename);
	bool LoadExt(const char *basename);
	bool LoadOld(const char *basename);
	void DrawEnvCubeExt(Matrix44f &tr);
	void DrawEnvCubeOld(Matrix44f &tr);
	void DrawEnvCube(Matrix44f &tr);
	bool GetName(int i, QString basename, QString &filename);
  bool IsValid() {
		return !( (ti == 0) && (oti[0]==0) ); 
	}
  void SetInvalid() { ti=0; oti[0]=0; }
	CICubeMap() {radius=10; ti=0; for(int i=0;i<6;++i) oti[i]=0;}
	float radius;

};

} // end namespace