include (../../shared.pri)

HEADERS       += sample_pclfilter.h

SOURCES       += sample_pclfilter.cpp  
		
TARGET        = sample_pclfilter

INCLUDEPATH  += $(PCL_ROOT)/3rdParty/Boost/include/ $(PCL_ROOT)/3rdParty/Eigen/include $(PCL_ROOT)/3rdParty/Flann/include $(PCL_ROOT)/include/pcl-1.5 
QMAKE_LIBDIR += $(PCL_ROOT)/lib
LIBS += pcl_common_debug.lib pcl_io_debug.lib



