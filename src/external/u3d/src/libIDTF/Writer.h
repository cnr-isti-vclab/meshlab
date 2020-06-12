//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#ifndef __IFXWRITER_H__
#define __IFXWRITER_H__

//------------- INCLUDES ----------------------------------------------------

#include "IFXDataTypes.h"
#include "IFXResult.h"

namespace U3D_IDTF{}
using namespace U3D_IDTF;

#include "SceneResources.h"
#include "Node.h"
#include "ViewNode.h"
#include "ModelNode.h"
#include "NodeList.h"
#include "ModifierList.h"
#include "SceneData.h"
#include "FileReference.h"

#define MAX_TOKEN_LEN 128
#define DEFAULT_PRECISION 6

//------------- CLASS, STRUCTURE AND TYPES ----------------------------------

/**
*  Interface for the IFXWriter class.
*/
class IFXWriter
{
public:
	~IFXWriter();
	IFXWriter();
	IFXWriter( const char* );

	bool open();
	bool close();

	void begin( const IFXCHAR* );
	void begin( const IFXCHAR*, const int );
	void begin( const IFXCHAR*, const char*, bool = false );
	void begin( const IFXCHAR*, const IFXCHAR*, bool = false );
	void begin( const IFXCHAR*, const IFXString&, bool = false );
	void end();

	void output( const IFXCHAR*, const char*, bool = true );
	void output( const IFXCHAR*, const IFXCHAR*, bool = true );
	void output( const IFXCHAR*, const IFXString&, bool = true );
	void output( const IFXCHAR*, const float, bool = true );
	void output( const IFXCHAR*, const int, bool = true );
	void output( const IFXCHAR*, const unsigned int, bool = true );
	void output( const IFXCHAR*, const Color& );
	void output( const IFXCHAR*, const Point&, bool = true );
	void output( const IFXCHAR*, const U32* );
	void output( const IFXCHAR*, const IFXMatrix4x4& );
	void output( const IFXCHAR*, const Quat& );

	void setPlain( bool plain ) { m_plain = plain; }
	void setPrecision( U32 prec ) { m_precision = prec; }
	void restorePrecision() { m_precision = DEFAULT_PRECISION; }
	void setFixed( bool fixed ) { m_fixed = fixed; }
	void setExportDefaults( bool exportDefaults ) { m_exportDefaults = exportDefaults; }

	void newline();
	void exportFileHeader();
	void exportSceneData( const SceneData* );
	void exportFileReference( const FileReference* );
	void exportNodes( const NodeList* );
	void exportNode( const Node* );
	void exportMeta( const MetaDataList* );
	void exportResources( const SceneResources* );
	void exportViewResource( const ViewResource& );
	void exportLightResource( const LightResource& );
	void exportModelResource( const ModelResource* );
	void exportShaderResource( const Shader& );
	void exportMaterialResource( const Material& );
	void exportTextureResource( const Texture& );
	void exportMotionResource( const MotionResource& );
	void exportShadingDescription( const ModelResource* );
	void exportModifiers( const ModifierList* );
	void exportModifier( const Modifier* );
	void exportShadingModifier( const ShadingModifier* );
	void exportAnimationModifier( const AnimationModifier* );
	void exportBoneWeightModifier( const BoneWeightModifier* );
	void exportCLODModifier( const CLODModifier* );
	void exportSubdivisionModifier( const SubdivisionModifier* pSubdivisionModifier );
	void exportGlyphModifier( const GlyphModifier* );
	void exportInt3List( const IFXCHAR* , const I32, const IFXArray< Int3 >& );
	void exportInt2List( const IFXCHAR* , const I32, const IFXArray< Int2 >& );
	void exportIntList( const IFXCHAR* , const I32, const IFXArray< I32 >& );
	void exportPointList( const IFXCHAR* , const I32, const IFXArray< Point >& );
	void exportColorList( const IFXCHAR* , const I32, const IFXArray< Color >& );
	void exportUrlList( const UrlList& );
	void exportUrlList( const IFXArray<IFXString>& );
	
protected:
	void indent() const;
	void incIndent() { m_indent++; }
	void decIndent() { m_indent--; }

	void output( const Point* );
	void output( const Color* );

	void output( const IFXCHAR*, bool = true );
	void output( const int ) const;
	void output( const unsigned int ) const;
	void output( const float ) const;
	void outputeol( const float ) const;

private:

	char* m_fileName;
	FILE* m_pFileHandle;
	int m_indent;
	bool m_plain;
	U32 m_precision;
	bool m_fixed;
	bool m_exportDefaults;
};

#endif
