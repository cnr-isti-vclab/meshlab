# This Project file contains all the stable plugins ready for the deployment in version 1.2 
# Plugins should be added here only when they are reasonably complete and stable. 

TEMPLATE      = subdirs
SUBDIRS       = meshfilter \
				baseio\
				meshio \
				colladaio \
				epoch_io\
				io_u3d\
				io_tri\
				io_x3d \
				meshrender \
				cleanfilter \
				filter_ao \ 
				filter_morph \
				filter_poisson \
				filter_unsharp \
				filter_splitter \
#				filter_texture \
				filter_sampling \
				filter_autoalign \
				meshcolorize \
				meshselect \
				meshdecorate \ 
				edit_select \
				edit_pickpoints \
#				edit_phototexturing \
				editslice \ 
#				editpaint \
				editalign \
				editmeasure\
				editrgbtri\
				editsegment\
				edit_straightener \
				../sampleplugins/sampleedit \
				../sampleplugins/samplefilter \
				../sampleplugins/samplefilterdyn \
				../sampleplugins/samplefilterdoc \
				../sampleplugins/sampledecoration \
				../sampleplugins/filterborder \
				../sampleplugins/filtergeodesic \
				../sampleplugins/filtercreateiso \
				../fgt/filter_trioptimize \
				../fgt/filter_aging \
				../fgt/edit_quality \
				../fgt/edit_quality/filter_quality \
				../fgt/edit_texture\
				../fgt/editpaint\
				../fgt/filter_colorproc 