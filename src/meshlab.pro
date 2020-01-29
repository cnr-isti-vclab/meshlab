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
CONFIG  += ordered

message("DISTRIB_DIRECTORY: "$$MESHLAB_DISTRIB_DIRECTORY)

SUBDIRS  = \
    external \
    common \
    meshlab \
    meshlabserver \
    meshlabplugins/io_base \        # a few basic file formats (ply, obj, off), without this you cannot open anything
    meshlabplugins/decorate_base \
    meshlabplugins/filter_measure \
    meshlabplugins/filter_meshing

           
!meshlab_mini {
    
SUBDIRS += \
# IO plugins
    meshlabplugins/io_3ds \
    meshlabplugins/io_bre \
    meshlabplugins/io_collada \
    meshlabplugins/io_ctm \
    meshlabplugins/io_expe \
    meshlabplugins/io_json \
    meshlabplugins/io_tri \
    meshlabplugins/io_x3d \
    meshlabplugins/io_pdb \
    meshlabplugins/io_txt \
    meshlabplugins/io_u3d \
# Filter plugins
    meshlabplugins/filter_ao \
    meshlabplugins/filter_camera \
    meshlabplugins/filter_clean \
    meshlabplugins/filter_color_projection \
    meshlabplugins/filter_colorproc \
    meshlabplugins/filter_create \
    meshlabplugins/filter_csg \
    meshlabplugins/filter_dirt \
    meshlabplugins/filter_fractal \
    meshlabplugins/filter_func \
    meshlabplugins/filter_img_patch_param \
    meshlabplugins/filter_isoparametrization \
    meshlabplugins/filter_layer \
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
    meshlabplugins/decorate_background \
    meshlabplugins/decorate_raster_proj \
    meshlabplugins/decorate_shadow \
    meshlabplugins/render_gdp \
    meshlabplugins/render_radiance_scaling \
# Edit Plugins
    meshlabplugins/edit_align \
    meshlabplugins/edit_manipulators \
    meshlabplugins/edit_measure \
    meshlabplugins/edit_mutualcorrs \
    meshlabplugins/edit_paint \
    meshlabplugins/edit_point \
    meshlabplugins/edit_referencing \
    meshlabplugins/edit_quality \
    meshlabplugins/edit_select

#no longer needed# meshlabplugins/filter_aging \
#no longer needed# meshlabplugins/filter_bnpts \
#no longer needed# meshlabplugins/filter_colorize \
#no longer supported#   meshlabplugins/edit_pickpoints \

}

meshlab_samples {

SUBDIRS += \
# Sample Plugins
    sampleplugins/sampleedit \
    sampleplugins/samplefilter \
    sampleplugins/samplefilterdyn \
    sampleplugins/filter_createiso \
    sampleplugins/filter_geodesic \
    sampleplugins/sample_filtergpu
}

# if distrib folder is not in $$PWD/../distrib (shadow build case),
# we need to copy all the files inside $$PWD/../distrib in the actual
# distrib folder ($$OUT_PWD/distrib or $$MESHLAB_DISTRIB_DIRECTORY)
!equals(PWD, $$OUT_PWD) {

    #copying the "lib" folder inside the $$OUT_PWD/distrib
    distrib.commands = $(COPY_DIR) \"$$shell_path($$PWD/../distrib)\" \"$$shell_path($$OUT_PWD/distrib)\"
    first.depends += $(first) distrib
    export(first.depends)
    export(distrib.commands)

    QMAKE_EXTRA_TARGETS += first distrib
}
