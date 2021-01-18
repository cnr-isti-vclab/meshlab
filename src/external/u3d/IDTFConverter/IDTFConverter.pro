include(../../ext_common.pri)
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11
TARGET = IDTFConverter
#DESTDIR = ../

BASE_SRC_IDTF = $${PWD}/../src/IDTF
BASE_SRC_RTL = $${PWD}/../src/RTL

INCLUDEPATH += \
	$${BASE_SRC_RTL}/Component/Include \
	$${BASE_SRC_RTL}/Kernel/Include \
	$${BASE_SRC_RTL}/Platform/Include \
	$${BASE_SRC_IDTF} \
	$${BASE_SRC_IDTF}/Include/ \
	$${BASE_SRC_IDTF}/Common/

SOURCES += \
	$${BASE_SRC_IDTF}/Converter.cpp \
	$${BASE_SRC_IDTF}/FileParser.cpp \
	$${BASE_SRC_IDTF}/SceneConverter.cpp \
	$${BASE_SRC_IDTF}/PointSetResourceParser.cpp \
	$${BASE_SRC_IDTF}/UrlListParser.cpp \
	$${BASE_SRC_IDTF}/NodeParser.cpp \
	$${BASE_SRC_IDTF}/ModifierParser.cpp \
	$${BASE_SRC_IDTF}/PointSetConverter.cpp \
	$${BASE_SRC_IDTF}/MaterialParser.cpp \
	$${BASE_SRC_IDTF}/MetaDataConverter.cpp \
	$${BASE_SRC_IDTF}/MeshResourceParser.cpp \
	$${BASE_SRC_IDTF}/ResourceConverter.cpp \
	$${BASE_SRC_IDTF}/TextureConverter.cpp \
	$${BASE_SRC_IDTF}/ResourceListParser.cpp \
	$${BASE_SRC_IDTF}/File.cpp \
	$${BASE_SRC_IDTF}/LineSetConverter.cpp \
	$${BASE_SRC_IDTF}/ModelConverter.cpp \
	$${BASE_SRC_IDTF}/TextureParser.cpp \
	$${BASE_SRC_IDTF}/NodeConverter.cpp \
	$${BASE_SRC_IDTF}/MeshConverter.cpp \
	$${BASE_SRC_IDTF}/BlockParser.cpp \
	$${BASE_SRC_IDTF}/ModelResourceParser.cpp \
	$${BASE_SRC_IDTF}/FileReferenceConverter.cpp \
	$${BASE_SRC_IDTF}/ShaderParser.cpp \
	$${BASE_SRC_IDTF}/FileScanner.cpp \
	$${BASE_SRC_IDTF}/FileReferenceParser.cpp \
	$${BASE_SRC_IDTF}/ModifierConverter.cpp \
	$${BASE_SRC_IDTF}/MetaDataParser.cpp \
	$${BASE_SRC_IDTF}/LineSetResourceParser.cpp \
	$${BASE_SRC_IDTF}/Helpers/MiscUtilities.cpp \
	$${BASE_SRC_IDTF}/Helpers/TGAImage.cpp \
	$${BASE_SRC_IDTF}/Helpers/ModifierUtilities.cpp \
	$${BASE_SRC_IDTF}/Helpers/ConverterHelpers.cpp \
	$${BASE_SRC_IDTF}/Helpers/SceneUtilities.cpp \
	$${BASE_SRC_IDTF}/Helpers/DebugInfo.cpp \
	$${BASE_SRC_IDTF}/Helpers/Guids.cpp \
	$${BASE_SRC_IDTF}/Common/GlyphModifier.cpp \
	$${BASE_SRC_IDTF}/Common/ModelResource.cpp \
	$${BASE_SRC_IDTF}/Common/ModifierList.cpp \
	$${BASE_SRC_IDTF}/Common/NodeList.cpp \
	$${BASE_SRC_IDTF}/Common/FileReference.cpp \
	$${BASE_SRC_IDTF}/Common/ResourceList.cpp \
	$${BASE_SRC_IDTF}/Common/SceneResources.cpp \
	$${BASE_SRC_IDTF}/Common/ModelResourceList.cpp \
	$${BASE_SRC_IDTF}/Common/MetaDataList.cpp \
	$${BASE_SRC_IDTF}/Common/ParentList.cpp \
	$${BASE_SRC_IDTF}/Common/GlyphCommandList.cpp

macx {
	QMAKE_LFLAGS += -exported_symbols_list  -undefined dynamic_lookup
}
