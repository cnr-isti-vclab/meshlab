#config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS       = common \
                meshlab \
                meshlabserver \
                meshlabplugins/io_base\
        		meshlabplugins/io_3ds\
        		meshlabplugins/io_bre\
        		meshlabplugins/io_collada \
				meshlabplugins/io_epoch\
				meshlabplugins/io_u3d\
				meshlabplugins/io_tri\
				meshlabplugins/io_x3d \
				meshlabplugins/io_gts \
				meshlabplugins/io_expe \
				meshlabplugins/io_pdb \
				meshlabplugins/filter_ao \
				meshlabplugins/filter_autoalign \
				meshlabplugins/filter_clean \
				meshlabplugins/filter_colorize \
				meshlabplugins/filter_create \
        meshlabplugins/filter_fractal \
				meshlabplugins/filter_isoparametrization \
				meshlabplugins/filter_measure \
				meshlabplugins/filter_meshing \
				meshlabplugins/filter_mls \
				meshlabplugins/filter_plymc \
				meshlabplugins/filter_poisson \
				meshlabplugins/filter_sampling \
				meshlabplugins/filter_slice \
				meshlabplugins/filter_splitter \
				meshlabplugins/filter_select \
				meshlabplugins/filter_texture \
        		meshlabplugins/filter_unsharp \
				meshlabplugins/filter_zippering \
#			meshlabplugins/filter_curvature_from_sliding \
#			meshlabplugins/filter_perceptualmetric \
#			meshlabplugins/render_splat_pyramid \
			meshlabplugins/render_splatting \
			meshlabplugins/render_gdp \
			meshlabplugins/render_radiance_scaling \
        meshlabplugins/decorate_base \
				meshlabplugins/edit_select \
				meshlabplugins/edit_pickpoints \
##				meshlabplugins/edit_phototexturing \
##		        meshlabplugins/editslice \
				meshlabplugins/editalign \
				meshlabplugins/editmeasure \
#				meshlabplugins/editrgbtri \
				meshlabplugins/editsegment \
##				meshlabplugins/edit_straightener \
				sampleplugins/sampleedit \
				sampleplugins/samplefilter \
				sampleplugins/samplefilterdyn \
				sampleplugins/samplefilterdoc \
				sampleplugins/sampledecoration \
				sampleplugins/io_m\
				sampleplugins/filterborder \
			sampleplugins/filtergeodesic \
			sampleplugins/filtercreateiso \
             fgt/filter_aging \
              fgt/filter_colorproc \
               fgt/filter_func \
              fgt/filter_ssynth \
              fgt/filter_qhull \
              fgt/filter_quality \
               fgt/filter_trioptimize \
                fgt/decorate_shadow \
                fgt/edit_hole \
                fgt/editpaint\
                fgt/edit_quality \
                fgt/edit_texture\
##				fgt/edit_topo \
				fgt/render_rfx 
