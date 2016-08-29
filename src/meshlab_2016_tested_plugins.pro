#
# meshlab_mini.pro
#
# This is a minimal project file for compiling as less as possible and having a minimal meshlab setup.
# Compiling this minimal subset does not require any additional library (except obviously qt and vcg).
#

#config += debug_and_release
TEMPLATE      = subdirs
CONFIG += ordered
SUBDIRS       = common \
                meshlab \
# IO plugins
                meshlabplugins/io_base\
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
                meshlabplugins/filter_autoalign \
                meshlabplugins/filter_camera \
                meshlabplugins/filter_bnpts \
                meshlabplugins/filter_clean \
                meshlabplugins/filter_colorize \
                meshlabplugins/filter_colorproc \
                meshlabplugins/filter_color_projection \
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
                meshlabplugins/filter_mutualinfoxml \
                meshlabplugins/filter_mls \
                meshlabplugins/filter_photosynth \
                meshlabplugins/filter_plymc \
				meshlabplugins/filter_poisson \
                meshlabplugins/filter_qhull \
                meshlabplugins/filter_quality \
                meshlabplugins/filter_sampling \
                meshlabplugins/filter_sdfgpu \
                meshlabplugins/filter_select \
                meshlabplugins/filter_ssynth \
                meshlabplugins/filter_texture \
                meshlabplugins/filter_trioptimize \
                meshlabplugins/filter_unsharp \
                meshlabplugins/filter_zippering \			
# Decorator Plugins
				meshlabplugins/decorate_background \
				meshlabplugins/decorate_base \			
				meshlabplugins/decorate_shadow \
				meshlabplugins/decorate_raster_proj \
# Edit Plugins
				meshlabplugins/edit_arc3D \
                meshlabplugins/edit_manipulators \
				meshlabplugins/edit_measure \
				meshlabplugins/edit_quality \
				meshlabplugins/edit_pickpoints \
				meshlabplugins/edit_point \
				meshlabplugins/edit_select \
# Render Plugins
				meshlabplugins/render_radiance_scaling \
# Sample Plugins
				sampleplugins/sampleedit \
                sampleplugins/samplefilter \
                sampleplugins/samplefilterdyn \
                sampleplugins/io_m \
                sampleplugins/filter_geodesic \
                sampleplugins/filter_createiso
				