config += debug_and_release

TEMPLATE = subdirs

!meshlab_mini {
SUBDIRS	+= \
	levmar-2.3/levmar-2.3.pro \
	structuresynth-1.5/structuresynth.pro \
	u3d/u3d.pro
#   openkinect/openkinect.pro

include(../find_system_libs.pri)

!CONFIG(system_qhull)    SUBDIRS += qhull-2003.1/qhull2003.1.pro
!CONFIG(system_muparser) SUBDIRS += muparser_v225/muParser.pro
!CONFIG(system_openctm)  SUBDIRS += OpenCTM-1.0.3/openctm.pro
!CONFIG(system_lib3ds)   SUBDIRS += lib3ds-1.3.0/lib3ds.pro
}



# Can't easily make muparser conditional because of the fact it relies on the _UNICODE define
