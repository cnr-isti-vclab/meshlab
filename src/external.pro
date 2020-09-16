#this project file builds just the external libraries

TEMPLATE = subdirs

message("MeshLab External")
message("DISTRIB_DIRECTORY: "$$MESHLAB_DISTRIB_DIRECTORY)

SUBDIRS = \
    external
