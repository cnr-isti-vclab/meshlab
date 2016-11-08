#config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS       = common \
                meshlab \
                meshlabserver \
# IO plugins
                meshlabplugins/io_base\
				plugins_unsupported/io_bre \
                meshlabplugins/io_3ds\
                meshlabplugins/io_collada \
                meshlabplugins/io_ctm \
                meshlabplugins/io_json \
                meshlabplugins/io_u3d\
                meshlabplugins/io_tri\
                meshlabplugins/io_x3d \
                meshlabplugins/io_gts \
                meshlabplugins/io_expe \
                meshlabplugins/io_pdb \
                plugins_experimental/io_TXT \
# Filter plugins
                meshlabplugins/filter_aging \
                meshlabplugins/filter_ao \
                meshlabplugins/filter_camera \
                meshlabplugins/filter_bnpts \
                meshlabplugins/filter_clean \
                meshlabplugins/filter_colorize \
                meshlabplugins/filter_colorproc \
                meshlabplugins/filter_color_projection \
                meshlabplugins/filter_create \
				sampleplugins/filter_createiso \
                meshlabplugins/filter_csg \
                meshlabplugins/filter_dirt \
                meshlabplugins/filter_fractal \
                meshlabplugins/filter_func \
				sampleplugins/filter_geodesic \
				plugins_experimental/filter_harmonic \
                meshlabplugins/filter_img_patch_param \
                meshlabplugins/filter_isoparametrization \
                meshlabplugins/filter_layer \
                meshlabplugins/filter_measure \
                meshlabplugins/filter_meshing \
				plugins_experimental/filter_mutualglobal \
                meshlabplugins/filter_mutualinfoxml \
                meshlabplugins/filter_mls \
                meshlabplugins/filter_photosynth \
                meshlabplugins/filter_plymc \
                meshlabplugins/filter_screened_poisson \
                meshlabplugins/filter_qhull \
                meshlabplugins/filter_quality \
                meshlabplugins/filter_sampling \
                meshlabplugins/filter_sdfgpu \
                meshlabplugins/filter_select \
				plugins_experimental/filter_sketchfab \
                meshlabplugins/filter_texture \
                meshlabplugins/filter_trioptimize \
                meshlabplugins/filter_unsharp \
# Rendering and Decoration Plugins
                meshlabplugins/render_gdp \
                meshlabplugins/render_radiance_scaling \
                meshlabplugins/decorate_base \
                meshlabplugins/decorate_background \
                meshlabplugins/decorate_shadow \
                meshlabplugins/decorate_raster_proj \
# Edit Plugins
                meshlabplugins/edit_align \
                meshlabplugins/edit_arc3D \
                meshlabplugins/edit_manipulators \
                meshlabplugins/edit_measure \
				plugins_experimental/edit_mutualcorrs \
                meshlabplugins/edit_paint\
                meshlabplugins/edit_pickpoints \
                meshlabplugins/edit_point\
                plugins_experimental/edit_referencing \
                meshlabplugins/edit_quality \
                meshlabplugins/edit_select \
# Sample Plugins
                sampleplugins/sampleedit \
                sampleplugins/samplefilter \
                sampleplugins/samplefilterdyn \
                ## sampleplugins/io_m          