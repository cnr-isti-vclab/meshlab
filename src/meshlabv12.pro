config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS       = common \
                meshlab \
                meshlabserver \
                meshlabplugins/meshlabpluginsv12.pro
