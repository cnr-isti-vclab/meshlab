TEMPLATE = lib
CONFIG += dll
TARGET = IFXCoreStatic

linux{
	U3D_PLATFORM=Lin32
}
macx {
	U3D_PLATFORM=Mac32
}

win32 {
	U3D_PLATFORM=Win32
}

INCLUDEPATH += \
	Component/Include/ \
	Kernel/Include/ \
	Platform/Include \
	Component/Base \
	Component/Rendering \
	Dependencies/WildCards

HEADERS += \
	Component/Base/IFXVectorHasher.h \
	Component/Rendering/CIFXDeviceBase.h \
	Component/Rendering/CIFXDeviceLight.h \
	Component/Rendering/CIFXDeviceTexture.h \
	Component/Rendering/CIFXDeviceTexUnit.h \
	Component/Rendering/CIFXRender.h \
	Component/Rendering/CIFXRenderContext.h \
	Component/Rendering/CIFXRenderDevice.h \
	Component/Rendering/CIFXRenderServices.h \
	Component/Rendering/DX7/CIFXDeviceLightDX7.h \
	Component/Rendering/DX7/CIFXDeviceTextureDX7.h \
	Component/Rendering/DX7/CIFXDeviceTexUnitDX7.h \
	Component/Rendering/DX7/CIFXDirectX7.h \
	Component/Rendering/DX7/CIFXRenderDeviceDX7.h \
	Component/Rendering/DX7/CIFXRenderDX7.h \
	Component/Rendering/DX7/IFXRenderPCHDX7.h \
	Component/Rendering/DX8/CIFXDeviceLightDX8.h \
	Component/Rendering/DX8/CIFXDeviceTextureDX8.h \
	Component/Rendering/DX8/CIFXDeviceTexUnitDX8.h \
	Component/Rendering/DX8/CIFXDirectX8.h \
	Component/Rendering/DX8/CIFXRenderDeviceDX8.h \
	Component/Rendering/DX8/CIFXRenderDX8.h \
	Component/Rendering/DX8/IFXRenderPCHDX8.h \
	Component/Rendering/IFXAAFilter.h \
	Component/Rendering/IFXRenderPCH.h \
	Component/Rendering/Null/CIFXDeviceLightNULL.h \
	Component/Rendering/Null/CIFXDeviceTextureNULL.h \
	Component/Rendering/Null/CIFXDeviceTexUnitNULL.h \
	Component/Rendering/Null/CIFXRenderDeviceNULL.h \
	Component/Rendering/Null/CIFXRenderNULL.h \
	Component/Rendering/Null/IFXRenderPCHNULL.h \
	Component/Rendering/OpenGL/CIFXDeviceLightOGL.h \
	Component/Rendering/OpenGL/CIFXDeviceTextureOGL.h \
	Component/Rendering/OpenGL/CIFXDeviceTexUnitOGL.h \
	Component/Rendering/OpenGL/CIFXOpenGL.h \
	Component/Rendering/OpenGL/CIFXRenderDeviceOGL.h \
	Component/Rendering/OpenGL/CIFXRenderOGL.h \
	Component/Rendering/OpenGL/IFXRenderPCHOGL.h \
	Dependencies/WildCards/wcmatch.h

SOURCES += \
	IFXCoreStatic/IFXCoreStatic.cpp \
	Component/Common/IFXDids.cpp \
	Component/Base/IFXCoincidentVertexMap.cpp \
	Component/Base/IFXCornerIter.cpp \
	Component/Base/IFXEuler.cpp \
	Component/Base/IFXFatCornerIter.cpp \
	Component/Base/IFXTransform.cpp \
	Component/Base/IFXVectorHasher.cpp \
	Component/Base/IFXVertexMap.cpp \
	Component/Base/IFXVertexMapGroup.cpp \
	Kernel/DataTypes/IFXCoreArray.cpp \
	Kernel/DataTypes/IFXCoreList.cpp \
	Kernel/DataTypes/IFXFastAllocator.cpp \
	Kernel/DataTypes/IFXListNode.cpp \
	Kernel/DataTypes/IFXMatrix4x4.cpp \
	Kernel/DataTypes/IFXQuaternion.cpp \
	Kernel/DataTypes/IFXString.cpp \
	Kernel/DataTypes/IFXUnitAllocator.cpp \
	Kernel/DataTypes/IFXVector3.cpp \
	Kernel/DataTypes/IFXVector4.cpp \
	Dependencies/WildCards/wcmatch.cpp \
	Kernel/Common/IFXDebug.cpp \
	Platform/$${U3D_PLATFORM}/Common/IFXOSUtilities.cpp \
	Platform/$${U3D_PLATFORM}/Common/IFXOSLoader.cpp \
	Platform/$${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
