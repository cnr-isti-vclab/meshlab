include(../../ext_common.pri)
TEMPLATE = lib
CONFIG += staticlib
TARGET = IFXCoreStatic
#DESTDIR = ../

BASE_SRC_IDTF = $${PWD}/../src/IDTF
BASE_SRC_RTL = $${PWD}/../src/RTL

linux{
	U3D_PLATFORM=Lin32
}
macx {
	DEFINES+=MAC32
	U3D_PLATFORM=Mac32
}

win32 {
	U3D_PLATFORM=Win32
}

INCLUDEPATH += \
	$${BASE_SRC_RTL}/Component/Include/ \
	$${BASE_SRC_RTL}/Kernel/Include/ \
	$${BASE_SRC_RTL}/Platform/Include \
	$${BASE_SRC_RTL}/Component/Base \
	$${BASE_SRC_RTL}/Component/Rendering \
	$${BASE_SRC_RTL}/Dependencies/WildCards

SOURCES += \
	$${BASE_SRC_RTL}/IFXCoreStatic/IFXCoreStatic.cpp \
	$${BASE_SRC_RTL}/Component/Common/IFXDids.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXCoincidentVertexMap.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXCornerIter.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXEuler.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXFatCornerIter.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXTransform.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXVectorHasher.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXVertexMap.cpp \
	$${BASE_SRC_RTL}/Component/Base/IFXVertexMapGroup.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXCoreArray.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXCoreList.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXFastAllocator.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXListNode.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXMatrix4x4.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXQuaternion.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXString.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXUnitAllocator.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXVector3.cpp \
	$${BASE_SRC_RTL}/Kernel/DataTypes/IFXVector4.cpp \
	$${BASE_SRC_RTL}/Dependencies/WildCards/wcmatch.cpp \
	$${BASE_SRC_RTL}/Kernel/Common/IFXDebug.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSUtilities.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSLoader.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSRenderWindow.cpp
