TEMPLATE      = subdirs
CONFIG       += ordered

SUBDIRS       = common \                                 # the common framework  
                meshlabserver\                           # meshlab server
                meshlabplugins/io_base\                  # a few basic file formats (ply, obj, off)
				meshlabplugins/io_x3d\                   # to import X3D
				meshlabplugins/filter_select\            # selection (e.g. self-intersection faces)
				meshlabplugins/filter_clean\             # cleaning operations
				meshlabplugins/filter_meshing\           # simplification and other algorithms
                plugins_experimental/filter_info_vmust   # information extraction