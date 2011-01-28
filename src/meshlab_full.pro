#config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS       = common \
                meshlab \
                meshlabserver \
# IO plugins
                meshlabplugins/io_base\
                meshlabplugins/io_3ds\
                meshlabplugins/io_bre\
                meshlabplugins/io_collada \
                meshlabplugins/io_epoch \
				meshlabplugins/io_json \
                meshlabplugins/io_u3d\
                meshlabplugins/io_tri\
                meshlabplugins/io_x3d \
                meshlabplugins/io_gts \
                meshlabplugins/io_expe \
                meshlabplugins/io_pdb \
# Filters
                meshlabplugins/filter_ao \
                meshlabplugins/filter_autoalign \
                meshlabplugins/filter_camera \
                meshlabplugins/filter_clean \
                meshlabplugins/filter_colorize \
                meshlabplugins/filter_create \
                meshlabplugins/filter_fractal \
                meshlabplugins/filter_isoparametrization \
                meshlabplugins/filter_layer \
                meshlabplugins/filter_measure \
                meshlabplugins/filter_meshing \
                meshlabplugins/filter_mls \
                meshlabplugins/filter_plymc \
                meshlabplugins/filter_poisson \
                meshlabplugins/filter_sampling \
                meshlabplugins/filter_slice \
                meshlabplugins/filter_select \
                meshlabplugins/filter_texture \
                meshlabplugins/filter_unsharp \
                meshlabplugins/filter_zippering \
# rendering stuff
                meshlabplugins/render_splatting \
                meshlabplugins/render_gdp \
                meshlabplugins/render_radiance_scaling \
                fgt/render_rfx \
# decorations
                meshlabplugins/decorate_base \
                meshlabplugins/decorate_background \
                meshlabplugins/decorate_shadow \
# editing tools
                meshlabplugins/edit_select \
                meshlabplugins/edit_pickpoints \
                meshlabplugins/editalign \
                meshlabplugins/editmeasure \
                meshlabplugins/editsegment \
# Sample plugins
                sampleplugins/sampleedit \
                sampleplugins/samplefilter \
                sampleplugins/samplefilterdyn \
                sampleplugins/io_m\
                sampleplugins/filtergeodesic \
                sampleplugins/filtercreateiso \
# projects
                fgt/filter_aging \
                fgt/filter_colorproc \
				fgt/filter_csg \
                fgt/filter_func \
                fgt/filter_ssynth \
                fgt/filter_qhull \
                fgt/filter_quality \
                fgt/filter_trioptimize \
                fgt/filter_photosynth \
                fgt/edit_hole \
                fgt/editpaint\
                fgt/edit_quality \
                fgt/edit_texture \
                ##				fgt/edit_topo \
