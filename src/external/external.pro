config += debug_and_release

CONFIG += c++11

TEMPLATE      = subdirs

SUBDIRS	= 	muparser_v225/src \
			levmar-2.3/levmar-2.3.pro \
			structuresynth-1.5/structuresynth.pro \
			jhead-3.04/jhead-3.04.pro \
#			openkinect/openkinect.pro

!CONFIG(system_bzip2)   SUBDIRS	+= 	bzip2-1.0.5/bzip2-1.0.5.pro
!CONFIG(system_openctm) SUBDIRS	+= 	OpenCTM-1.0.3/openctm.pro
!CONFIG(system_lib3ds)  SUBDIRS	+= 	lib3ds-1.3.0/lib3ds/lib3ds.pro

# Can't easily make muparser conditional because of the fact it relies on the _UNICODE define
