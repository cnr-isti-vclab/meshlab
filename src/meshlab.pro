# MeshLab main configuration file for qmake
# To compile meshlab:
# - qmake
# - make
#
# If you want to compile meshlab_mini (just a minimal subset of plugins):
# - qmake "CONFIG += meshlab_mini"
#
# MeshLab qmake config uses the following variables:
#
# MESHLAB_SOURCE_DIRECTORY: the directory where is placed the main meshlab.pro
# MESHLAB_BUILD_DIRECTORY: the directory where the meshlab build files are placed
# MESHLAB_DISTRIB_DIRECTORY: the directory that will contain all the files necessary
#                            for a portable version (after build and deploy)
#                            (if shadow build, will be MESHLAB_BUILD_DIRECTORY/distrib)
# MESHLAB_EXTERNAL_DIRECTORY: the directory where external libraries are placed
#

TEMPLATE = subdirs

message("DISTRIB_DIRECTORY: "$$MESHLAB_DISTRIB_DIRECTORY)

#the following sub projects are compiled ALSO with MeshLab Mini
SUBDIRS = \ #sub projects names
    external \
    common \
    meshlab \
    meshlabserver \
    io_base \        # a few basic file formats (ply, obj, off), without this you cannot open anything
    decorate_base \
    filter_measure \
    filter_meshing

## where to find the sub projects - give the folders ##
external.subdir = external
common.subdir = common
meshlab.subdir = meshlab
meshlabserver.subdir = meshlabserver
io_base.subdir = meshlabplugins/io_base
decorate_base.subdir = meshlabplugins/decorate_base
filter_measure.subdir = meshlabplugins/filter_measure
filter_meshing.subdir = meshlabplugins/filter_meshing

## what subproject depends on others ##
common.depends = external
meshlab.depends = common
meshlabserver.depends = common
io_base.depends = common
decorate_base.depends = common
filter_measure.depends = common
filter_meshing.depends = common

meshlab_mini {
    message(Compiling only MeshLab Mini!)
}
!meshlab_mini {

#Other sub project, compiled only when config is not MeshLab Mini
SUBDIRS += \ #sub projects names
# IO plugins
    io_3ds \
    io_bre \
    io_collada \
    io_ctm \
    io_expe \
    io_json \
    io_tri \
    io_x3d \
    io_pdb \
    io_txt \
    io_u3d \
# Filter samples
    sampleedit \
    samplefilter \
    samplefilterdyn \
    filter_createiso \
    filter_geodesic \
    sample_filtergpu \
# Filter plugins
    filter_ao \
    filter_camera \
    filter_clean \
    filter_color_projection \
    filter_colorproc \
    filter_create \
    filter_csg \
    filter_dirt \
    filter_fractal \
    filter_func \
    filter_img_patch_param \
    filter_isoparametrization \
    filter_layer \
    filter_mls \
    filter_mutualglobal \
    filter_mutualinfoxml \
    filter_plymc \
    filter_qhull \
    filter_quality \
    filter_sampling \
    filter_screened_poisson \
    filter_sdfgpu \
    filter_select \
    filter_sketchfab \
    filter_ssynth \
    filter_texture \
    filter_trioptimize \
    filter_unsharp \
    filter_voronoi \
# Rendering and Decoration Plugins
    decorate_background \
    decorate_raster_proj \
    decorate_shadow \
    render_gdp \
    render_radiance_scaling \
# Edit Plugins
    edit_align \
    edit_manipulators \
    edit_measure \
    edit_mutualcorrs \
    edit_paint \
    edit_point \
    edit_referencing \
    edit_quality \
    edit_select

## where to find the sub projects - give the folders ##
# IO plugins
io_3ds.subdir = meshlabplugins/io_3ds
io_bre.subdir = meshlabplugins/io_bre
io_collada.subdir = meshlabplugins/io_collada
io_ctm.subdir = meshlabplugins/io_ctm
io_expe.subdir = meshlabplugins/io_expe
io_json.subdir = meshlabplugins/io_json
io_tri.subdir = meshlabplugins/io_tri
io_x3d.subdir = meshlabplugins/io_x3d
io_pdb.subdir = meshlabplugins/io_pdb
io_txt.subdir = meshlabplugins/io_txt
io_u3d.subdir = meshlabplugins/io_u3d
# Filter samples
sampleedit.subdir = sampleplugins/sampleedit
samplefilter.subdir = sampleplugins/samplefilter
samplefilterdyn.subdir = sampleplugins/samplefilterdyn
filter_createiso.subdir = sampleplugins/filter_createiso
filter_geodesic.subdir = sampleplugins/filter_geodesic
sample_filtergpu.subdir = sampleplugins/sample_filtergpu
# Filter plugins
filter_ao.subdir = meshlabplugins/filter_ao
filter_camera.subdir = meshlabplugins/filter_camera
filter_clean.subdir = meshlabplugins/filter_clean
filter_color_projection.subdir = meshlabplugins/filter_color_projection
filter_colorproc.subdir = meshlabplugins/filter_colorproc
filter_create.subdir = meshlabplugins/filter_create
filter_csg.subdir = meshlabplugins/filter_csg
filter_dirt.subdir = meshlabplugins/filter_dirt
filter_fractal.subdir = meshlabplugins/filter_fractal
filter_func.subdir = meshlabplugins/filter_func
filter_img_patch_param.subdir = meshlabplugins/filter_img_patch_param
filter_isoparametrization.subdir = meshlabplugins/filter_isoparametrization
filter_layer.subdir = meshlabplugins/filter_layer
filter_mls.subdir = meshlabplugins/filter_mls
filter_mutualglobal.subdir = meshlabplugins/filter_mutualglobal
filter_mutualinfoxml.subdir = meshlabplugins/filter_mutualinfoxml
filter_plymc.subdir = meshlabplugins/filter_plymc
filter_qhull.subdir = meshlabplugins/filter_qhull
filter_quality.subdir = meshlabplugins/filter_quality
filter_sampling.subdir = meshlabplugins/filter_sampling
filter_screened_poisson.subdir = meshlabplugins/filter_screened_poisson
filter_sdfgpu.subdir = meshlabplugins/filter_sdfgpu
filter_select.subdir = meshlabplugins/filter_select
filter_sketchfab.subdir = meshlabplugins/filter_sketchfab
filter_ssynth.subdir = meshlabplugins/filter_ssynth
filter_texture.subdir = meshlabplugins/filter_texture
filter_trioptimize.subdir = meshlabplugins/filter_trioptimize
filter_unsharp.subdir = meshlabplugins/filter_unsharp
filter_voronoi.subdir = meshlabplugins/filter_voronoi
# Rendering and Decoration Plugins
decorate_background.subdir = meshlabplugins/decorate_background
decorate_raster_proj.subdir = meshlabplugins/decorate_raster_proj
decorate_shadow.subdir = meshlabplugins/decorate_shadow
render_gdp.subdir = meshlabplugins/render_gdp
render_radiance_scaling.subdir = meshlabplugins/render_radiance_scaling
# Edit Plugins
edit_align.subdir = meshlabplugins/edit_align
edit_manipulators.subdir = meshlabplugins/edit_manipulators
edit_measure.subdir = meshlabplugins/edit_measure
edit_mutualcorrs.subdir = meshlabplugins/edit_mutualcorrs
edit_paint.subdir = meshlabplugins/edit_paint
edit_point.subdir = meshlabplugins/edit_point
edit_referencing.subdir = meshlabplugins/edit_referencing
edit_quality.subdir = meshlabplugins/edit_quality
edit_select.subdir = meshlabplugins/edit_select

## what subproject depends on others ##
# IO plugins
io_3ds.depends = common
io_bre.depends = common
io_collada.depends = common
io_ctm.depends = common
io_expe.depends = common
io_json.depends = common
io_tri.depends = common
io_x3d.depends = common
io_pdb.depends = common
io_txt.depends = common
io_u3d.depends = common
# Filter samples
sampleedit.depends = common
samplefilter.depends = common
samplefilterdyn.depends = common
filter_createiso.depends = common
filter_geodesic.depends = common
sample_filtergpu.depends = common
# Filter plugins
filter_ao.depends = common
filter_camera.depends = common
filter_clean.depends = common
filter_color_projection.depends = common
filter_colorproc.depends = common
filter_create.depends = common
filter_csg.depends = common
filter_dirt.depends = common
filter_fractal.depends = common
filter_func.depends = common
filter_img_patch_param.depends = common
filter_isoparametrization.depends = common
filter_layer.depends = common
filter_mls.depends = common
filter_mutualglobal.depends = common
filter_mutualinfoxml.depends = common
filter_plymc.depends = common
filter_qhull.depends = common
filter_quality.depends = common
filter_sampling.depends = common
filter_screened_poisson.depends = common
filter_sdfgpu.depends = common
filter_select.depends = common
filter_sketchfab.depends = common
filter_ssynth.depends = common
filter_texture.depends = common
filter_trioptimize.depends = common
filter_unsharp.depends = common
filter_voronoi.depends = common
# Rendering and Decoration Plugins
decorate_background.depends = common
decorate_raster_proj.depends = common
decorate_shadow.depends = common
render_gdp.depends = common
render_radiance_scaling.depends = common
# Edit Plugins
edit_align.depends = common
edit_manipulators.depends = common
edit_measure.depends = common
edit_mutualcorrs.depends = common
edit_paint.depends = common
edit_point.depends = common
edit_referencing.depends = common
edit_quality.depends = common
edit_select.depends = common

#no longer needed# meshlabplugins/filter_aging \
#no longer needed# meshlabplugins/filter_bnpts \
#no longer needed# meshlabplugins/filter_colorize \
#no longer supported#   meshlabplugins/edit_pickpoints \

}


# if distrib folder is not in $$PWD/../distrib (shadow build case),
# we need to copy all the files inside $$PWD/../distrib in the actual
# distrib folder ($$OUT_PWD/distrib or $$MESHLAB_DISTRIB_DIRECTORY)
!equals(PWD, $$OUT_PWD) {

    #copying the "lib" folder inside the $$OUT_PWD/distrib
    win32:copydir.commands = $(COPY_DIR) \"$$shell_path($$PWD/../distrib)\" \"$$shell_path($$OUT_PWD/distrib)\"
    !win32:copydir.commands = $(COPY_DIR) \"$$shell_path($$PWD/../distrib)\" \"$$shell_path($$OUT_PWD)\"
    first.depends += $(first) copydir
    export(first.depends)
    export(copydir.commands)

    QMAKE_EXTRA_TARGETS += first copydir
}
