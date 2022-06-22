#!/bin/bash
# this is a script shell for deploying a meshlab-portable app.
# Requires a properly built meshlab.
#
# Without given arguments, the folder that will be deployed is meshlab/install.
#
# You can give as argument the INSTALL_PATH in the following way:
# bash 2_deploy.sh  --install_path=/path/to/install
# -i argument is also supported.
#
# After running this script, $INSTALL_PATH/meshlab.app will be a portable meshlab application.

SCRIPTS_PATH=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

INSTALL_PATH=$SCRIPTS_PATH/../../install
QT_DIR=""
APPNAME="meshlab.app"

#checking for parameters
for i in "$@"
do
case $i in
    -i=*|--install_path=*)
        INSTALL_PATH="${i#*=}"
        shift # past argument=value
        ;;
    -qt=*|--qt_dir=*)
        QT_DIR=${i#*=}/bin/
        shift # past argument=value
        ;;
    *)
        # unknown option
        ;;
esac
done

${QT_DIR}macdeployqt $INSTALL_PATH/$APPNAME \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_align.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_csg.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_isoparametrization.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_mesh_booleans.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libdecorate_background.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libdecorate_base.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libdecorate_raster_proj.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libdecorate_shadow.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_manipulators.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_measure.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_mutualcorrs.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_paint.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_pickpoints.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_point.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_quality.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_referencing.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_sample.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libedit_select.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_ao.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_camera.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_clean.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_color_projection.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_colorproc.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_create.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_createiso.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_dirt.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_fractal.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_func.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_geodesic.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_icp.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_img_patch_param.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_io_nxs.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_layer.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_measure.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_meshing.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_mls.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_mutualglobal.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_mutualinfo.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_parametrization.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_plymc.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_qhull.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_quality.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sample.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sample_gpu.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sampling.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_screened_poisson.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sdfgpu.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_select.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_sketchfab.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_e57.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_ssynth.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_texture.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_texture_defragmentation.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_trioptimize.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_unsharp.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libfilter_voronoi.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_3ds.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_base.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_bre.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_collada.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_ctm.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_expe.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_gltf.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_json.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_pdb.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_tri.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_txt.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_u3d.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/libio_x3d.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/librender_gdp.so \
    -executable=$INSTALL_PATH/$APPNAME/Contents/PlugIns/librender_radiance_scaling.so
