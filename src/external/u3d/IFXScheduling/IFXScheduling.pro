include(../../ext_common.pri)
TEMPLATE = lib
CONFIG += dll
TARGET = IFXScheduling
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
	$${BASE_SRC_RTL}/Component/ModifierChain \
	$${BASE_SRC_RTL}/Component/SceneGraph \
	$${BASE_SRC_RTL}/Component/Scheduling \
	$${BASE_SRC_RTL}/Dependencies/WildCards

SOURCES += \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/IFXScheduling/IFXSchedulingDllMain.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXClock.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXErrorInfo.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXNotificationInfo.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXNotificationManager.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXScheduler.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXSchedulerInfo.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXSimulationInfo.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXSimulationManager.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXSystemManager.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXTaskCallback.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXTaskData.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXTaskManager.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXTaskManagerNode.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXTaskManagerView.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/CIFXTimeManager.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/IFXScheduling.cpp \
	$${BASE_SRC_RTL}/Component/Scheduling/IFXSchedulingGuids.cpp \
	$${BASE_SRC_RTL}/IFXCorePluginStatic/IFXCorePluginStatic.cpp \
	$${BASE_SRC_RTL}/Platform/$${U3D_PLATFORM}/Common/IFXOSUtilities.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXSubject.cpp \
	$${BASE_SRC_RTL}/Component/ModifierChain/CIFXModifier.cpp \
	$${BASE_SRC_RTL}/Component/SceneGraph/CIFXMarker.cpp \
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
	$${BASE_SRC_RTL}/Kernel/Common/IFXDebug.cpp

macx {
    QMAKE_LFLAGS += -exported_symbols_list $${BASE_SRC_RTL}/Platform/Mac32/IFXScheduling/IFXScheduling.def   -undefined dynamic_lookup
	QMAKE_LFLAGS_PLUGIN -= -dynamiclib
	QMAKE_LFLAGS_PLUGIN += -bundle
	QMAKE_EXTENSION_SHLIB = so
}

win32 {
	RC_FILE = $${BASE_SRC_RTL}/Platform/Win32/IFXScheduling/IFXScheduling.rc
	HEADERS += \
		$${BASE_SRC_RTL}//Platform/Win32/IFXScheduling/IFXResource.h
	DEF_FILE = $${BASE_SRC_RTL}/Platform/Win32/IFXScheduling/IFXScheduling.def
	LIBS += winmm.lib user32.lib -L$$DESTDIR/ -lIFXCore
}
