include(../ext_common.pri)
linux{
	U3D_PLATFORM=Lin32
}
macx {
	U3D_PLATFORM=Mac32
}

win32 {
	U3D_PLATFORM=Win32
}

TEMPLATE = lib
TARGET = IDTFConverter
CONFIG += staticlib

INCLUDEPATH += \
	RTL/Component/Include/ \
	RTL/Kernel/Include/ \
	RTL/Platform/Include \
	RTL/Component/Base \
	RTL/Component/Rendering \
	RTL/Dependencies/WildCards \
	IDTF/ \
	IDTF/Include \
	IDTF/Common


HEADERS += \
	#RTL headers
	RTL/Component/Base/IFXVectorHasher.h \
	RTL/Component/Rendering/CIFXDeviceBase.h \
	RTL/Component/Rendering/CIFXDeviceLight.h \
	RTL/Component/Rendering/CIFXDeviceTexture.h \
	RTL/Component/Rendering/CIFXDeviceTexUnit.h \
	RTL/Component/Rendering/CIFXRender.h \
	RTL/Component/Rendering/CIFXRenderContext.h \
	RTL/Component/Rendering/CIFXRenderDevice.h \
	RTL/Component/Rendering/CIFXRenderServices.h \
	RTL/Component/Rendering/DX7/CIFXDeviceLightDX7.h \
	RTL/Component/Rendering/DX7/CIFXDeviceTextureDX7.h \
	RTL/Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h \
	RTL/Component/Rendering/DX7/CIFXDirectX7.h \
	RTL/Component/Rendering/DX7/CIFXRenderDeviceDX7.h \
	RTL/Component/Rendering/DX7/CIFXRenderDX7.h \
	RTL/Component/Rendering/DX7/IFXRenderPCHDX7.h \
	RTL/Component/Rendering/DX8/CIFXDeviceLightDX8.h \
	RTL/Component/Rendering/DX8/CIFXDeviceTextureDX8.h \
	RTL/Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h \
	RTL/Component/Rendering/DX8/CIFXDirectX8.h \
	RTL/Component/Rendering/DX8/CIFXRenderDeviceDX8.h \
	RTL/Component/Rendering/DX8/CIFXRenderDX8.h \
	RTL/Component/Rendering/DX8/IFXRenderPCHDX8.h \
	RTL/Component/Rendering/IFXAAFilter.h \
	RTL/Component/Rendering/IFXRenderPCH.h \
	RTL/Component/Rendering/Null/CIFXDeviceLightNULL.h \
	RTL/Component/Rendering/Null/CIFXDeviceTextureNULL.h \
	RTL/Component/Rendering/Null/CIFXDeviceTexUnitNULL.h \
	RTL/Component/Rendering/Null/CIFXRenderDeviceNULL.h \
	RTL/Component/Rendering/Null/CIFXRenderNULL.h \
	RTL/Component/Rendering/Null/IFXRenderPCHNULL.h \
	RTL/Component/Rendering/OpenGL/CIFXDeviceLightOGL.h \
	RTL/Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h \
	RTL/Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h \
	RTL/Component/Rendering/OpenGL/CIFXOpenGL.h \
	RTL/Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h \
	RTL/Component/Rendering/OpenGL/CIFXRenderOGL.h \
	RTL/Component/Rendering/OpenGL/IFXRenderPCHOGL.h \
	RTL/Dependencies/WildCards/wcmatch.h \
	#IDTF headers
	IDTF/BlockParser.h \
	IDTF/Converter.h \
	IDTF/DefaultSettings.h \
	IDTF/File.h \
	IDTF/FileParser.h \
	IDTF/FileReferenceConverter.h \
	IDTF/FileReferenceParser.h \
	IDTF/FileScanner.h \
	IDTF/IConverter.h \
	IDTF/LineSetConverter.h \
	IDTF/LineSetResourceParser.h \
	IDTF/MaterialParser.h \
	IDTF/MeshConverter.h \
	IDTF/MeshResourceParser.h \
	IDTF/MetaDataConverter.h \
	IDTF/MetaDataParser.h \
	IDTF/ModelConverter.h \
	IDTF/ModelResourceParser.h \
	IDTF/ModifierConverter.h \
	IDTF/ModifierParser.h \
	IDTF/NodeConverter.h \
	IDTF/NodeParser.h \
	IDTF/PointSetConverter.h \
	IDTF/PointSetResourceParser.h \
	IDTF/ResourceConverter.h \
	IDTF/ResourceListParser.h \
	IDTF/SceneConverter.h \
	IDTF/ShaderParser.h \
	IDTF/TextureConverter.h \
	IDTF/TextureParser.h \
	IDTF/UrlListParser.h \
	IDTF/Include/ConverterHelpers.h \
	IDTF/Include/ConverterOptions.h \
	IDTF/Include/ConverterResult.h \
	IDTF/Include/DebugInfo.h \
	IDTF/Include/SceneUtilities.h \
	IDTF/Include/TGAImage.h \
	IDTF/Include/U3DHeaders.h \
	IDTF/Common/AnimationModifier.h \
	IDTF/Common/BoneWeightModifier.h \
	IDTF/Common/CLODModifier.h \
	IDTF/Common/Color.h \
	IDTF/Common/FileReference.h \
	IDTF/Common/GlyphCommandList.h \
	IDTF/Common/GlyphCommands.h \
	IDTF/Common/GlyphModifier.h \
	IDTF/Common/INode.h \
	IDTF/Common/Int2.h \
	IDTF/Common/Int3.h \
	IDTF/Common/IResource.h \
	IDTF/Common/LightNode.h \
	IDTF/Common/LightResource.h \
	IDTF/Common/LightResourceList.h \
	IDTF/Common/LineSetResource.h \
	IDTF/Common/MaterialResource.h \
	IDTF/Common/MaterialResourceList.h \
	IDTF/Common/MeshResource.h \
	IDTF/Common/MetaDataList.h \
	IDTF/Common/ModelNode.h \
	IDTF/Common/ModelResource.h \
	IDTF/Common/ModelResourceList.h \
	IDTF/Common/ModelSkeleton.h \
	IDTF/Common/Modifier.h \
	IDTF/Common/ModifierList.h \
	IDTF/Common/MotionResource.h \
	IDTF/Common/MotionResourceList.h \
	IDTF/Common/Node.h \
	IDTF/Common/NodeList.h \
	IDTF/Common/ParentData.h \
	IDTF/Common/ParentList.h \
	IDTF/Common/Point.h \
	IDTF/Common/PointSetResource.h \
	IDTF/Common/Quat.h \
	IDTF/Common/Resource.h \
	IDTF/Common/ResourceList.h \
	IDTF/Common/SceneData.h \
	IDTF/Common/SceneResources.h \
	IDTF/Common/ShaderResource.h \
	IDTF/Common/ShaderResourceList.h \
	IDTF/Common/ShadingDescription.h \
	IDTF/Common/ShadingDescriptionList.h \
	IDTF/Common/ShadingModifier.h \
	IDTF/Common/SubdivisionModifier.h \
	IDTF/Common/TextureResource.h \
	IDTF/Common/TextureResourceList.h \
	IDTF/Common/Tokens.h \
	IDTF/Common/UrlList.h \
	IDTF/Common/ViewNodeData.h \
	IDTF/Common/ViewNode.h \
	IDTF/Common/ViewResource.h \
	IDTF/Common/ViewResourceList.h

SOURCES += \
	#RTL Sources
	RTL/IFXCoreStatic/IFXCoreStatic.cpp \
	RTL/Component/Common/IFXDids.cpp \
	RTL/Component/Base/IFXCoincidentVertexMap.cpp \
	RTL/Component/Base/IFXCornerIter.cpp \
	RTL/Component/Base/IFXEuler.cpp \
	RTL/Component/Base/IFXFatCornerIter.cpp \
	RTL/Component/Base/IFXTransform.cpp \
	RTL/Component/Base/IFXVectorHasher.cpp \
	RTL/Component/Base/IFXVertexMap.cpp \
	RTL/Component/Base/IFXVertexMapGroup.cpp \
	RTL/Kernel/DataTypes/IFXCoreArray.cpp \
	RTL/Kernel/DataTypes/IFXCoreList.cpp \
	RTL/Kernel/DataTypes/IFXFastAllocator.cpp \
	RTL/Kernel/DataTypes/IFXListNode.cpp \
	RTL/Kernel/DataTypes/IFXMatrix4x4.cpp \
	RTL/Kernel/DataTypes/IFXQuaternion.cpp \
	RTL/Kernel/DataTypes/IFXString.cpp \
	RTL/Kernel/DataTypes/IFXUnitAllocator.cpp \
	RTL/Kernel/DataTypes/IFXVector3.cpp \
	RTL/Kernel/DataTypes/IFXVector4.cpp \
	RTL/Dependencies/WildCards/wcmatch.cpp \
	RTL/Kernel/Common/IFXDebug.cpp \
	RTL/Platform/$${U3D_PLATFORM}/Common/IFXOSUtilities.cpp \
	RTL/Platform/$${U3D_PLATFORM}/Common/IFXOSLoader.cpp \
	RTL/Platform/$${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp \
	#IDTF Sources
	IDTF/Converter.cpp \
	IDTF/FileParser.cpp \
	IDTF/SceneConverter.cpp \
	IDTF/PointSetResourceParser.cpp \
	IDTF/UrlListParser.cpp \
	IDTF/NodeParser.cpp \
	IDTF/ModifierParser.cpp \
	IDTF/PointSetConverter.cpp \
	IDTF/MaterialParser.cpp \
	IDTF/MetaDataConverter.cpp \
	IDTF/MeshResourceParser.cpp \
	IDTF/ResourceConverter.cpp \
	IDTF/TextureConverter.cpp \
	IDTF/ResourceListParser.cpp \
	IDTF/File.cpp \
	IDTF/LineSetConverter.cpp \
	#ConverterDriver.cpp \
	IDTF/ModelConverter.cpp \
	IDTF/TextureParser.cpp \
	IDTF/NodeConverter.cpp \
	IDTF/MeshConverter.cpp \
	IDTF/BlockParser.cpp \
	IDTF/ModelResourceParser.cpp \
	IDTF/FileReferenceConverter.cpp \
	IDTF/ShaderParser.cpp \
	IDTF/FileScanner.cpp \
	IDTF/FileReferenceParser.cpp \
	IDTF/ModifierConverter.cpp \
	IDTF/MetaDataParser.cpp \
	IDTF/LineSetResourceParser.cpp \
	IDTF/Helpers/MiscUtilities.cpp \
	IDTF/Helpers/TGAImage.cpp \
	IDTF/Helpers/ModifierUtilities.cpp \
	IDTF/Helpers/ConverterHelpers.cpp \
	IDTF/Helpers/SceneUtilities.cpp \
	IDTF/Helpers/DebugInfo.cpp \
	IDTF/Helpers/Guids.cpp \
	IDTF/Common/GlyphModifier.cpp \
	IDTF/Common/ModelResource.cpp \
	IDTF/Common/ModifierList.cpp \
	IDTF/Common/NodeList.cpp \
	IDTF/Common/FileReference.cpp \
	IDTF/Common/ResourceList.cpp \
	IDTF/Common/SceneResources.cpp \
	IDTF/Common/ModelResourceList.cpp \
	IDTF/Common/MetaDataList.cpp \
	IDTF/Common/ParentList.cpp \
	IDTF/Common/GlyphCommandList.cpp

