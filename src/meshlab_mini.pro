#
# meshlab_mini.pro
#
# This is a minimal project file for compiling as less as possible and having a minimal meshlab setup.
# Compiling this minimal subset does not require any additional library (except obviously qt and vcg).
#

TEMPLATE      = subdirs
CONFIG       += ordered

SUBDIRS       = common \                          # the common framework, used by all the plugins,
                meshlab \                         # the GUI framework
                meshlabplugins/io_base\           # a few basic file formats (ply, obj, off), without this you cannot open anything
                meshlabplugins/filter_meshing \   # a few basic filtering operations, including the well know simplification
#
# Next some other useful, but still easy to be compiled, plugins
# Uncomment them if you succeed in compiling the above ones.
#                meshlabplugins/edit_select \
#                meshlabplugins/filter_create\
#                meshlabplugins/filter_select \
#                meshlabplugins/decorate_base \
#                meshlabplugins/filter_colorize\
#                meshlabplugins/filter_measure\
#                meshlabplugins/filter_sampling\
#                meshlabplugins/filter_layer\
#                meshlabserver \

#

