# This Project file contains all the stable plugins ready for the deployment in version 1.2 

TEMPLATE      = subdirs
SUBDIRS       = io_base\
				io_3ds \
				io_collada \
				io_epoch\
				io_u3d\
				io_tri\
				io_x3d \
				io_gts \
				io_expe \
				filter_ao \
				filter_autoalign \
				filter_clean \
				filter_colorize \
				filter_create \
				filter_meshing \
				filter_mls \
				filter_poisson \
				filter_sampling \
				filter_splitter \
				filter_select \
#				filter_texture \
				filter_unsharp \
#				filter_curvature_from_sliding \
				render_splatting \
				render_gdp \
				meshdecorate \
				edit_select \
				edit_pickpoints \
#				edit_phototexturing \
				editslice \
				editalign \
				editmeasure \
				editrgbtri \
				editsegment \
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
				../fgt/filter_colorproc \
				../fgt/edit_hole \
				../fgt/edit_topo \
				../fgt/render_rfx \
				../fgt/filter_func \

