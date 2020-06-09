TEMPLATE = lib
CONFIG += dll
TARGET = IDTFConverter

INCLUDEPATH += \
	../RTL/Component/Include \
	../RTL/Kernel/Include \
	../RTL/Platform/Include \
	. \
	Include/ \
	Common/


HEADERS += \
	BlockParser.h \
	Converter.h \
	DefaultSettings.h \
	File.h \
	FileParser.h \
	FileReferenceConverter.h \
	FileReferenceParser.h \
	FileScanner.h \
	IConverter.h \
	LineSetConverter.h \
	LineSetResourceParser.h \
	MaterialParser.h \
	MeshConverter.h \
	MeshResourceParser.h \
	MetaDataConverter.h \
	MetaDataParser.h \
	ModelConverter.h \
	ModelResourceParser.h \
	ModifierConverter.h \
	ModifierParser.h \
	NodeConverter.h \
	NodeParser.h \
	PointSetConverter.h \
	PointSetResourceParser.h \
	ResourceConverter.h \
	ResourceListParser.h \
	SceneConverter.h \
	ShaderParser.h \
	TextureConverter.h \
	TextureParser.h \
	UrlListParser.h \
	Include/ConverterHelpers.h \
	Include/ConverterOptions.h \
	Include/ConverterResult.h \
	Include/DebugInfo.h \
	Include/SceneUtilities.h \
	Include/TGAImage.h \
	Include/U3DHeaders.h \
	Common/AnimationModifier.h \
	Common/BoneWeightModifier.h \
	Common/CLODModifier.h \
	Common/Color.h \
	Common/FileReference.h \
	Common/GlyphCommandList.h \
	Common/GlyphCommands.h \
	Common/GlyphModifier.h \
	Common/INode.h \
	Common/Int2.h \
	Common/Int3.h \
	Common/IResource.h \
	Common/LightNode.h \
	Common/LightResource.h \
	Common/LightResourceList.h \
	Common/LineSetResource.h \
	Common/MaterialResource.h \
	Common/MaterialResourceList.h \
	Common/MeshResource.h \
	Common/MetaDataList.h \
	Common/ModelNode.h \
	Common/ModelResource.h \
	Common/ModelResourceList.h \
	Common/ModelSkeleton.h \
	Common/Modifier.h \
	Common/ModifierList.h \
	Common/MotionResource.h \
	Common/MotionResourceList.h \
	Common/Node.h \
	Common/NodeList.h \
	Common/ParentData.h \
	Common/ParentList.h \
	Common/Point.h \
	Common/PointSetResource.h \
	Common/Quat.h \
	Common/Resource.h \
	Common/ResourceList.h \
	Common/SceneData.h \
	Common/SceneResources.h \
	Common/ShaderResource.h \
	Common/ShaderResourceList.h \
	Common/ShadingDescription.h \
	Common/ShadingDescriptionList.h \
	Common/ShadingModifier.h \
	Common/SubdivisionModifier.h \
	Common/TextureResource.h \
	Common/TextureResourceList.h \
	Common/Tokens.h \
	Common/UrlList.h \
	Common/ViewNodeData.h \
	Common/ViewNode.h \
	Common/ViewResource.h \
	Common/ViewResourceList.h

SOURCES += \
	Converter.cpp \
	FileParser.cpp \
	SceneConverter.cpp \
	PointSetResourceParser.cpp \
	UrlListParser.cpp \
	NodeParser.cpp \
	ModifierParser.cpp \
	PointSetConverter.cpp \
	MaterialParser.cpp \
	MetaDataConverter.cpp \
	MeshResourceParser.cpp \
	ResourceConverter.cpp \
	TextureConverter.cpp \
	ResourceListParser.cpp \
	File.cpp \
	LineSetConverter.cpp \
	#ConverterDriver.cpp \
	ModelConverter.cpp \
	TextureParser.cpp \
	NodeConverter.cpp \
	MeshConverter.cpp \
	BlockParser.cpp \
	ModelResourceParser.cpp \
	FileReferenceConverter.cpp \
	ShaderParser.cpp \
	FileScanner.cpp \
	FileReferenceParser.cpp \
	ModifierConverter.cpp \
	MetaDataParser.cpp \
	LineSetResourceParser.cpp \
	Helpers/MiscUtilities.cpp \
	Helpers/TGAImage.cpp \
	Helpers/ModifierUtilities.cpp \
	Helpers/ConverterHelpers.cpp \
	Helpers/SceneUtilities.cpp \
	Helpers/DebugInfo.cpp \
	Helpers/Guids.cpp \
	Common/GlyphModifier.cpp \
	Common/ModelResource.cpp \
	Common/ModifierList.cpp \
	Common/NodeList.cpp \
	Common/FileReference.cpp \
	Common/ResourceList.cpp \
	Common/SceneResources.cpp \
	Common/ModelResourceList.cpp \
	Common/MetaDataList.cpp \
	Common/ParentList.cpp \
	Common/GlyphCommandList.cpp

LIBS += -L../RTL/ -lIFXCoreStatic
