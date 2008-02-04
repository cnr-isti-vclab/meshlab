/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
 Revision 1.2  2008/02/04 13:20:33  gianpaolopalma
 Added error codes

 Revision 1.1  2008/02/02 13:46:00  gianpaolopalma
 Defined AdditionalInfoX3D with information required to parse correctly X3D scene
 Defined return code error

 
 *****************************************************************************/
#ifndef UTILX3D
#define UTILX3D

#include<QtXml/QDomDocument>


namespace vcg {
namespace tri {
namespace io {
	
	class AdditionalInfoX3D : public AdditionalInfo
	{
	public: 
		QDomDocument* doc;
		QString filename;

		std::map<QString, QDomNode*> inlineNodeMap;

		std::map<QString, QDomNode*> protoDeclareNodeMap;

		std::vector<QString> textuxeFile;

		int lineNumberError;
		std::vector<QString> filenameStack;
		
		AdditionalInfoX3D()
		:AdditionalInfo()
		{
			mask	= 0;
			numvert = 0;
			numface = 0;
			doc = NULL;
		}

		~AdditionalInfoX3D()
		{
			delete doc;
		}
	};

	class UtilX3D
	{
	public:
		
		enum X3DError 
		{
			E_NOERROR,				// 0
			E_CANTOPEN,				// 1
			E_INVALIDXML,			// 2
			E_NO3DSCENE,			// 3
			E_MULTISCENE,			// 4
			E_NODEFFORUSE,			// 5
			E_INVALIDINLINE,		// 6
			E_INVALIDINLINEURL,		// 7
			E_INVALIDPROTOURL,		// 8
			E_INVALIDPROTO,			// 9
			E_INVALIDINSTFIELD,		// 10
			E_INVALIDPROTOINST,		// 11
			E_INVALIDPROTODECL,		// 12
			E_INVALIDPROTODECLFIELD,// 13
			E_INVALIDISCONNECTOR,	// 14
			E_INVALIDDEFINFIELD,	// 15
			E_MULTINAMEPROTODECL,   // 16
			E_MISMATCHDEFUSETYPE,	// 17
			E_LOOPDEPENDENCE,		// 18
			E_UNREFERREDTEXTCOOR,	// 19
			E_MULTITEXT,			// 20
			E_INVALIDFANSTRIP,		// 21
			E_INVALIDINDEXED,		// 22
			E_INVALIDINDEXEDFANSTRIP, //23

		};


		static const char *ErrorMsg(int error)
		{
			static const char * x3d_error_msg[] =
			{
				"No errors",
				"Can't open file",
				"Invalid XML file",
				"There isn't any scene in X3D file",
				"The file contains more Scene elements",
				"There isn't any DEF for the USE",
				"Invalid Inline element. File not found",
				"Invalid url in Inline element",
				"Invalid url in ExternProtoDeclare element",
				"Invalid ExternProtoDeclare. File not found",
				"Invalid field initialization in ProtoInstance element",
				"There isn't any ExternProtoDeclare or ProtoDeclare in the file for the ProtoInstance",
				"Invalid ProtoDeclare. There isn't any ProtoInterface or any ProtoBody",
				"Invalid field declaration in ProtoInterface element",
				"Invalid IS element in ProtoBody elment",
				"Invalid DEF in ProtoInstance field", 
				"There are more ProroDeclare and ExternProtoDeclare elements with the same name",
				"Distint type between DEF and USE",
				"There is a loop in the dependences between the files",
				"Texture coordinate referred to any source texture",
				"There are more sources texture without a MultiTexture element",
				"Invalid TriangleFanSet or TriangleStripSet element. It contains fan or strip with less than three vertex",
				"Invalid vertex index in the index field", 
				"Invalid IndexedTriangleFanSet or IndexedTriangleStripSet element. It contais fan or strip with less than three vertex",
			};

			if(error > 23 || error < 0) return "Unknown error";
			else return x3d_error_msg[error];
		};

		
	};


	class TextureInfo
	{
	public:

		int textureIndex;
		
		vcg::Matrix33f textureTransform;

		QStringList textureCoordList;

		bool repeatS, repeatT;

		QString mode, parameter;

		bool isCoordGenerator;

		bool isValid;

		TextureInfo()
		{
			textureTransform.SetIdentity();
			isValid = true;
			isCoordGenerator = false;
			repeatS = true;
			repeatT = true;
		}

		~TextureInfo()
		{
		
		}


	};

}
}
}

#endif
