TEMPLATE = subdirs
#CONFIG += ORDERED

SUBDIRS = \
	IFXCoreStatic \
	IFXCore \
	IFXExporting \
	IFXScheduling \
	IDTFConverter

#SUBDIRS += IDTFTest

IFXCoreStatic.subdir = IFXCoreStatic
IFXCore.subdir = IFXCore
IFXExporting.subdir = IFXExporting
IFXScheduling.subdir = IFXScheduling
IDTFConverter.subdir = IDTFConverter
IDTFTest.subdir = IDTFTest

IFXExporting.depends = IFXCore
IFXScheduling.depends = IFXCore
IDTFConverter.depends = IFXCoreStatic
IDTFTest.depends = IDTFConverter
