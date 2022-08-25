#ifndef SceneConverter_H
#define SceneConverter_H


//***************************************************************************
//  Includes
//***************************************************************************

#include "ConverterResult.h"
#include "IConverter.h"
#include "NodeList.h"
#include "SceneResources.h"
#include "ModifierList.h"
#include "SceneData.h"
#include "FileReference.h"
#include "DefaultSettings.h"

namespace U3D_IDTF
{
//***************************************************************************
//  Defines
//***************************************************************************


//***************************************************************************
//  Constants
//***************************************************************************
const U32 ATTRMESH = 1;
const U32 ATTRLINE = 1 << 1;
const U32 ATTRPOINT = 1 << 2;
const U32 ATTRGLYPH = 1 << 3;

//***************************************************************************
//  Enumerations
//***************************************************************************


//***************************************************************************
//  Classes, structures and types
//***************************************************************************

class SceneUtilities;
struct ConverterOptions;

/**
This is the implementation of a class that is used to @todo: usage.

It supports the following interfaces:  @todo: interfaces.
*/
class SceneConverter : public IConverter
{
public:
	SceneConverter( SceneUtilities* pSceneUtils,
		ConverterOptions* pConverterOptions );
	virtual ~SceneConverter();

	/**
	*/
	virtual IFXRESULT  Convert();
	void Export( const char* );
	SceneData m_sceneData;
	FileReference m_fileReference;
	NodeList m_nodeList;
	SceneResources m_sceneResources;
	ModifierList m_modifierList;

	SceneUtilities* m_pSceneUtils;
	ConverterOptions* m_pOptions;

protected:
	IFXRESULT ConvertSceneData();
	IFXRESULT ConvertFileReference();
	IFXRESULT ConvertScene();

};

//***************************************************************************
//  Inline functions
//***************************************************************************


//***************************************************************************
//  Global function prototypes
//***************************************************************************


//***************************************************************************
//  Global data
//***************************************************************************

}

#endif
