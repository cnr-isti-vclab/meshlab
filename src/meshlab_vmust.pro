TEMPLATE      = subdirs
CONFIG       += ordered

SUBDIRS       = common \                                       # the common framework  
                meshlab \                                      # the GUI framework
                meshlabplugins/io_base\                        # to import basic file formats (ply, obj, off)
				meshlabplugins/io_x3d\                         # to import X3D
				meshlabplugins/io_collada \                    # to import COLLADA
				meshlabplugins/filter_select\                  # selection (e.g. self-intersection faces)
				meshlabplugins/filter_clean\                   # cleaning operations
				meshlabplugins/filter_meshing\                 # simplification and other algorithms
                plugins_experimental/filter_web_export_vmust   # plugin to prepare the 3D model for the Web (based on the CIF API developed in the ambit of the V-Must NoE (FP7))