# File for compiling just meshlab mini (without the external libraries!)
#
# MeshLab qmake config uses the following variables:
#
# MESHLAB_SOURCE_DIRECTORY: the directory where is placed the main meshlab.pro
# MESHLAB_BUILD_DIRECTORY: the directory where the meshlab build files are placed
# MESHLAB_DISTRIB_DIRECTORY: the directory that will contain all the files necessary
#                            for a portable version (after build and deploy)
#                            (if shadow build, will be MESHLAB_BUILD_DIRECTORY/distrib)
# MESHLAB_EXTERNAL_DIRECTORY: the directory where external libraries are placed
#

TEMPLATE = subdirs

message("MeshLab Mini")
message("DISTRIB_DIRECTORY: "$$MESHLAB_DISTRIB_DIRECTORY)

#the following sub projects are compiled ALSO with MeshLab Mini
SUBDIRS = \ #sub projects names
    common \
    meshlab \
    io_base \        # a few basic file formats (ply, obj, off), without this you cannot open anything
    decorate_base \
    filter_measure \
    filter_meshing
    
common.subdir = common
meshlab.subdir = meshlab
io_base.subdir = meshlabplugins/io_base
decorate_base.subdir = meshlabplugins/decorate_base
filter_measure.subdir = meshlabplugins/filter_measure
filter_meshing.subdir = meshlabplugins/filter_meshing

meshlab.depends = common
io_base.depends = common
decorate_base.depends = common
filter_measure.depends = common
filter_meshing.depends = common

# if distrib folder is not in $$PWD/../distrib (shadow build case),
# we need to copy all the files inside $$PWD/../distrib in the actual
# distrib folder ($$OUT_PWD/distrib or $$MESHLAB_DISTRIB_DIRECTORY)
!equals(PWD, $$OUT_PWD) : !equals(PWD, $$OUT_PWD/src) {
    #copying the "lib" folder inside the $$OUT_PWD/distrib
    win32:copydir.commands = $(COPY_DIR) \"$$shell_path($$PWD/../distrib)\" \"$$shell_path($$OUT_PWD/distrib)\"
    !win32:copydir.commands = $(COPY_DIR) \"$$shell_path($$PWD/../distrib)\" \"$$shell_path($$OUT_PWD)\"
    first.depends += $(first) copydir
    export(first.depends)
    export(copydir.commands)

    QMAKE_EXTRA_TARGETS += first copydir
}
