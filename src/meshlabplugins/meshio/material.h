/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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
 Revision 1.1  2006/02/06 11:04:40  fmazzant
 added file material.h. it include struct Material, CreateNewMaterial(...) and MaterialsCompare(...)


 ****************************************************************************/

#ifndef __VCGLIB_MATERIAL
#define __VCGLIB_MATERIAL

namespace vcg {
namespace tri {
namespace io {
	
	/*
		structures material
	*/
	struct Material
	{
		unsigned int index;//index of material

		Point3f Ka;//ambient
		Point3f Kd;//diffuse
		Point3f Ks;//specular
		
		float d;//alpha
		float Tr;//alpha
		
		int illum;//specular illumination
		float Ns;

		std::string map_Kd; //filename texture
	};

	
	template <class SaveMeshType>
	class Materials
	{
	public:	
		typedef typename SaveMeshType::FaceIterator FaceIterator;
		typedef typename SaveMeshType::VertexIterator VertexIterator;
		typedef typename SaveMeshType::VertexType VertexType;
	
		/*
			creates a new meterial
		*/
		inline static int CreateNewMaterial(SaveMeshType &m, std::vector<Material> &materials, unsigned int index, FaceIterator &fi)
		{			
			unsigned char r = (*fi).C()[0];
			unsigned char g = (*fi).C()[1];
			unsigned char b = (*fi).C()[2];
			unsigned char alpha = (*fi).C()[3];
			
			Point3f diffuse = Point3f((float)r/255.0,(float)g/255.0,(float)b/255.0);//diffuse
			float Tr = (float)alpha/255.0;//alpha
			
			int illum = 2; //default not use Ks!
			float ns = 0.0; //default

			Material mtl;

			mtl.index = index;//index of materials
			mtl.Ka = Point3f(0.2,0.2,0.2);//ambient
			mtl.Kd = diffuse;//diffuse
			mtl.Ks = Point3f(1.0,1.0,1.0);//specular
			mtl.Tr = Tr;//alpha
			mtl.Ns = ns;
			mtl.illum = illum;//illumination
			
			if(m.textures.size() && (*fi).WT(0).n() >=0 ) 
				mtl.map_Kd = m.textures[(*fi).WT(0).n()];
			else
				mtl.map_Kd = "";
			
			int i = -1;
			if((i = MaterialsCompare(materials,mtl)) == -1)
			{
				materials.push_back(mtl);
				return materials.size();
			}
			return i;
		}

		/*
			returns the index of the material if it exists inside the list of the materials, 
			otherwise it returns -1.
		*/
		inline static int MaterialsCompare(std::vector<Material> &materials, Material mtl)
		{
			for(int i=0;i<materials.size();i++)
			{
				bool ka = materials[i].Ka == mtl.Ka;
				bool kd = materials[i].Kd == mtl.Kd;
				bool ks = materials[i].Ks == mtl.Ks;
				bool tr = materials[i].Tr == mtl.Tr;
				bool illum = materials[i].illum == mtl.illum;
				bool ns = materials[i].Ns == mtl.Ns;
				bool map = materials[i].map_Kd == mtl.map_Kd;
				if(ka & kd & ks & tr & illum & ns & map){return i;}
			}
			return -1;
		}
	};
}
}
}
#endif __VCGLIB_MATERIAL