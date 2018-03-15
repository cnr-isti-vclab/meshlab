#config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS       = common \
                meshlab \
                meshlabserver \
# IO plugins
                meshlabplugins/io_3ds\
                meshlabplugins/io_base\
                meshlabplugins/io_bre \
                meshlabplugins/io_collada \
                meshlabplugins/io_ctm \
                meshlabplugins/io_expe \
                meshlabplugins/io_json \
                meshlabplugins/io_tri\
                meshlabplugins/io_x3d \
                meshlabplugins/io_pdb \
                meshlabplugins/io_txt \
                meshlabplugins/io_u3d\
# Filter plugins
                meshlabplugins/filter_aging \
                meshlabplugins/filter_ao \
                meshlabplugins/filter_bnpts \
                meshlabplugins/filter_camera \
                meshlabplugins/filter_clean \
                meshlabplugins/filter_color_projection \
                meshlabplugins/filter_colorize \
                meshlabplugins/filter_colorproc \
                meshlabplugins/filter_create \
                meshlabplugins/filter_csg \
                meshlabplugins/filter_dirt \
                meshlabplugins/filter_fractal \
                meshlabplugins/filter_func \
                meshlabplugins/filter_img_patch_param \
                meshlabplugins/filter_isoparametrization \
                meshlabplugins/filter_layer \
                meshlabplugins/filter_measure \
                meshlabplugins/filter_meshing \
                meshlabplugins/filter_mls \
                meshlabplugins/filter_mutualglobal \
                meshlabplugins/filter_mutualinfoxml \
                meshlabplugins/filter_plymc \
                meshlabplugins/filter_qhull \
                meshlabplugins/filter_quality \
                meshlabplugins/filter_sampling \
                meshlabplugins/filter_screened_poisson \
                meshlabplugins/filter_sdfgpu \
                meshlabplugins/filter_select \
                meshlabplugins/filter_sketchfab \
                meshlabplugins/filter_ssynth \
                meshlabplugins/filter_texture \
                meshlabplugins/filter_trioptimize \
                meshlabplugins/filter_unsharp \
                meshlabplugins/filter_voronoi \
# Rendering and Decoration Plugins
                meshlabplugins/render_gdp \
                meshlabplugins/render_radiance_scaling \
                meshlabplugins/decorate_base \
                meshlabplugins/decorate_background \
                meshlabplugins/decorate_raster_proj \
                meshlabplugins/decorate_shadow \
# Edit Plugins
                meshlabplugins/edit_align \
                meshlabplugins/edit_manipulators \
                meshlabplugins/edit_measure \
                meshlabplugins/edit_mutualcorrs \
                meshlabplugins/edit_paint\
#no longer supported#   meshlabplugins/edit_pickpoints \
                meshlabplugins/edit_point\
                meshlabplugins/edit_referencing \
                meshlabplugins/edit_quality \
                meshlabplugins/edit_select \
# Sample Plugins
                sampleplugins/sampleedit \
                sampleplugins/samplefilter \
                sampleplugins/samplefilterdyn \     
                sampleplugins/filter_createiso \
                sampleplugins/filter_geodesic \
				sampleplugins/sample_filtergpu \


