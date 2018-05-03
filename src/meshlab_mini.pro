#
# meshlab_mini.pro
#
# This is a minimal project file for compiling as less as possible and having a minimal meshlab setup.
# Compiling this minimal subset does not require any additional library (except obviously qt and vcg).
#

TEMPLATE      = subdirs
CONFIG       += ordered


                           # the common framework, used by all the plugins,
SUBDIRS       = common \
                meshlab \                         # the GUI framework
                meshlabserver \
                meshlabplugins/io_base\           # a few basic file formats (ply, obj, off), without this you cannot open anything
                meshlabplugins/filter_meshing \
                meshlabplugins/decorate_base \
                meshlabplugins/filter_measure \
#
# Next some other useful, but still easy to be compiled, plugins
# Uncomment them if you succeed in compiling the above ones.
#                sampleplugins/sample_xmlfilter \
#                meshlabplugins/decorate_background \
#                meshlabplugins/edit_align \
#                meshlabplugins/edit_manipulators \
#                meshlabplugins/edit_select \
#                meshlabplugins/filter_clean\
#                meshlabplugins/filter_colorproc\
#                meshlabplugins/filter_create\
#                meshlabplugins/filter_layer\
#                meshlabplugins/filter_mutualinfoxml\
#                meshlabplugins/filter_quality \
#                meshlabplugins/filter_sampling\
#                meshlabplugins/filter_select \
#                meshlabplugins/filter_sketchfab \
#                meshlabplugins/filter_texture \
#                meshlabplugins/filter_unsharp \
#                meshlabplugins/io_collada \
#                meshlabplugins/io_x3d \

