/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <QFileDialog>

#include <cmath>
#include <cstdlib>
#include <ctime>

#include <vcg/space/colorspace.h>

#include "filter_color_projection.h"

#include "floatbuffer.cpp"

#include "render_helper.cpp"

#include "pushpull.h"
#include "rastering.h"
#include <vcg/complex/algorithms/update/texture.h>


using namespace std;
using namespace vcg;

// utility---------------------------------

#define CheckError(x,y); if ((x)) {this->errorMessage = (y); return false;}

static QString extractFilenameWOExt(MeshModel* mm)
{
    QFileInfo fi(mm->fullName());
    return fi.baseName();
}
//-----------------------------------------

// Constructor
FilterColorProjectionPlugin::FilterColorProjectionPlugin()
{
    typeList
        <<  FP_SINGLEIMAGEPROJ
        <<  FP_MULTIIMAGETRIVIALPROJ
        <<  FP_MULTIIMAGETRIVIALPROJTEXTURE;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);
}

QString FilterColorProjectionPlugin::pluginName() const
{
    return "FilterColorProjection";
}

// ST() return the very short string describing each filtering action
QString FilterColorProjectionPlugin::filterName(FilterIDType filterId) const
{
    switch(filterId) {
    case FP_SINGLEIMAGEPROJ	:	return QString("Project current raster color to current mesh");
    case FP_MULTIIMAGETRIVIALPROJ	:	return QString("Project active rasters color to current mesh");
    case FP_MULTIIMAGETRIVIALPROJTEXTURE	:	return QString("Project active rasters color to current mesh, filling the texture");
    default : assert(0);
    }
	return NULL;
}

// Info() return the longer string describing each filtering action
QString FilterColorProjectionPlugin::filterInfo(FilterIDType filterId) const
{
    switch(filterId) {
    case FP_SINGLEIMAGEPROJ	:	return QString("Color information from the current raster is perspective-projected on the current mesh");
    case FP_MULTIIMAGETRIVIALPROJ	:	return QString("Color information from all the active rasters is perspective-projected on the current mesh using basic weighting");
    case FP_MULTIIMAGETRIVIALPROJTEXTURE  :	return QString("Color information from all the active rasters is perspective-projected on the current mesh, filling the texture, using basic weighting");
    default : assert(0);
    }
	return NULL;
}

// What "new" properties the plugin requires
int FilterColorProjectionPlugin::getRequirements(const QAction *action){
    switch(ID(action)){
    case FP_SINGLEIMAGEPROJ:  return MeshModel::MM_VERTCOLOR;
    case FP_MULTIIMAGETRIVIALPROJ:  return MeshModel::MM_VERTCOLOR;
    case FP_MULTIIMAGETRIVIALPROJTEXTURE : return 0;
    default: assert(0); return 0;
    }
    return 0;
}


// This function define the needed parameters for each filter.
void FilterColorProjectionPlugin::initParameterList(const QAction *action, MeshDocument &md, RichParameterList & parlst)
{
    switch(ID(action))
    {
    case FP_SINGLEIMAGEPROJ :
        {
            parlst.addParam(RichBool ("usedepth",
                true,
                "Use depth for projection",
                "If true, depth is used to restrict projection on visible faces"));
            parlst.addParam(RichFloat ("deptheta",
                0.5,
                "depth threshold",
                "threshold value for depth buffer projection (shadow buffer)"));
            parlst.addParam(RichBool ("onselection",
                false,
                "Only on selection",
                "If true, projection is only done for selected vertices"));
			QColor color1 = QColor(0, 0, 0, 255);
			parlst.addParam(RichColor("blankColor", color1, "Color for unprojected areas", "Areas that cannot be projected willb e filled using this color. If R=0 G=0 B=0 A=0 old color is preserved"));
        }
        break;

    case FP_MULTIIMAGETRIVIALPROJ :
        {
            parlst.addParam(RichFloat ("deptheta",
                0.5,
                "depth threshold",
                "threshold value for depth buffer projection (shadow buffer)"));
            parlst.addParam(RichBool ("onselection",
                false,
                "Only on selection",
                "If true, projection is only done for selected vertices"));
            parlst.addParam(RichBool ("useangle",
                true,
                "use angle weight",
                "If true, color contribution is weighted by pixel view angle"));
            parlst.addParam(RichBool ("usedistance",
                true,
                "use distance weight",
                "If true, color contribution is weighted by pixel view distance"));
            parlst.addParam(RichBool ("useborders",
                true,
                "use image borders weight",
                "If true, color contribution is weighted by pixel distance from image boundaries"));
            parlst.addParam(RichBool ("usesilhouettes",
                true,
                "use depth discontinuities weight",
                "If true, color contribution is weighted by pixel distance from depth discontinuities (external and internal silhouettes)"));
            parlst.addParam(RichBool ("usealpha",
                false,
                "use image alpha weight",
                "If true, alpha channel of the image is used as additional weight. In this way it is possible to mask-out parts of the images that should not be projected on the mesh. Please note this is not a transparency effect, but just influences the weigthing between different images"));
			QColor color1 = QColor(0, 0, 0, 255);
			parlst.addParam(RichColor("blankColor", color1, "Color for unprojected areas", "Areas that cannot be projected willb e filled using this color. If R=0 G=0 B=0 A=0 old color is preserved"));
        }
        break;

    case FP_MULTIIMAGETRIVIALPROJTEXTURE :
        {
            QString fileName = extractFilenameWOExt(md.mm());
            fileName = fileName.append("_color.png");
            parlst.addParam(RichString("textName",
                fileName,
                "Texture file",
                "The texture file to be created"));
            parlst.addParam(RichInt ("texsize",
                1024,
                "pixel size of texture image",
                "pixel size of texture image, the image will be a square tsize X tsize, most applications do require that tsize is a power of 2"));
            parlst.addParam(RichBool ("dorefill",
                true,
                "fill atlas gaps",
                "If true, unfilled areas of the mesh are interpolated, to avoid visible seams while mipmapping"));
            parlst.addParam(RichFloat ("deptheta",
                0.5,
                "depth threshold",
                "threshold value for depth buffer projection (shadow buffer)"));
            parlst.addParam(RichBool ("onselection",
                false,
                "Only on selection",
                "If true, projection is only done for selected vertices"));
            parlst.addParam(RichBool ("useangle",
                true,
                "use angle weight",
                "If true, color contribution is weighted by pixel view angle"));
            parlst.addParam(RichBool ("usedistance",
                true,
                "use distance weight",
                "If true, color contribution is weighted by pixel view distance"));
            parlst.addParam(RichBool ("useborders",
                true,
                "use image borders weight",
                "If true, color contribution is weighted by pixel distance from image boundaries"));
            parlst.addParam(RichBool ("usesilhouettes",
                true,
                "use depth discontinuities weight",
                "If true, color contribution is weighted by pixel distance from depth discontinuities (external and internal silhouettes)"));
            parlst.addParam(RichBool ("usealpha",
                false,
                "use image alpha weight",
                "If true, alpha channel of the image is used as additional weight. In this way it is possible to mask-out parts of the images that should not be projected on the mesh. Please note this is not a transparency effect, but just influences the weigthing between different images"));
        }
        break;

    default: break; // do not add any parameter for the other filters
    }
}

// Core Function doing the actual mesh processing.
bool FilterColorProjectionPlugin::applyFilter(const QAction *filter, MeshDocument &md, std::map<std::string, QVariant>&, unsigned int& /*postConditionMask*/, const RichParameterList & par, vcg::CallBackPos *cb)
{
    //CMeshO::FaceIterator fi;
    CMeshO::VertexIterator vi;

    RenderHelper *rendermanager=NULL;

    switch(ID(filter))
    {

        ////--------------------------- project single trivial ----------------------------------

    case FP_SINGLEIMAGEPROJ :
        {
            bool use_depth = par.getBool("usedepth");
            bool onselection = par.getBool("onselection");
            float eta = par.getFloat("deptheta");
            QColor blank = par.getColor("blankColor");


            float depth=0;     // depth of point (distance from camera)
            float pdepth=0;    // depth value of projected point (from depth map)

            // get current raster and model
            RasterModel *raster   = md.rm();
            MeshModel   *model    = md.mm();

            // no projection if camera not valid
            if(!raster->shot.IsValid())
                return false;

            // the mesh has to be correctly transformed before mapping
            tri::UpdatePosition<CMeshO>::Matrix(model->cm,model->cm.Tr,true);
            tri::UpdateBounding<CMeshO>::Box(model->cm);

            // making context current
            glContext->makeCurrent();

            if(use_depth)
            {
                // init rendermanager
                rendermanager = new RenderHelper();
                if( rendermanager->initializeGL(cb) != 0 )
                {
                    delete rendermanager;
                    return false;
                }
                log("init GL");
                //if( rendermanager->initializeMeshBuffers(model, cb) != 0 )
                //    return false;
                //Log("init Buffers");

                // render depth
                rendermanager->renderScene(raster->shot, model, RenderHelper::FLAT, glContext);
            }

            // unmaking context current
            glContext->doneCurrent();

            qDebug("Viewport %i %i",raster->shot.Intrinsics.ViewportPx[0],raster->shot.Intrinsics.ViewportPx[1]);
            for(vi=model->cm.vert.begin();vi!=model->cm.vert.end();++vi)
            {
                if(!(*vi).IsD() && (!onselection || (*vi).IsS()))
                {
                    Point2m pp = raster->shot.Project((*vi).P());
                    // pray is the vector from the point-to-be-colored to the camera center
                    Point3m pray = (raster->shot.GetViewPoint() - (*vi).P()).Normalize();

                    if ((blank.red() != 0) || (blank.green() != 0) || (blank.blue() != 0) || (blank.alpha() != 0))
                        (*vi).C() = vcg::Color4b(blank.red(), blank.green(), blank.blue(), blank.alpha());

                    //if inside image
                    if(pp[0]>0 && pp[1]>0 && pp[0]<raster->shot.Intrinsics.ViewportPx[0] && pp[1]<raster->shot.Intrinsics.ViewportPx[1])
                    {
                        if((pray.dot(-raster->shot.Axis(2))) <= 0.0)
                        {
                            if(use_depth)
                            {
                                depth  = raster->shot.Depth((*vi).P());
                                pdepth = rendermanager->depth->getval(int(pp[0]), int(pp[1]));//rendermanager->depth[(int(pp[1]) * raster->shot.Intrinsics.ViewportPx[0]) + int(pp[0])];
                            }

                            if(!use_depth || (depth <= (pdepth + eta)))
                            {
                                QRgb pcolor = raster->currentPlane->image.pixel(pp[0],raster->shot.Intrinsics.ViewportPx[1] - pp[1]);
                                (*vi).C() = vcg::Color4b(qRed(pcolor), qGreen(pcolor), qBlue(pcolor), 255);
                            }
                        }
                    }
                }
            }

            // the mesh has to return to its original position
            tri::UpdatePosition<CMeshO>::Matrix(model->cm,Inverse(model->cm.Tr),true);
            tri::UpdateBounding<CMeshO>::Box(model->cm);

            // delete rendermanager
            if(rendermanager != NULL)
                delete rendermanager;
        }

        break;


        ////--------------------------- project multi trivial ----------------------------------

    case FP_MULTIIMAGETRIVIALPROJ :
        {
            bool onselection = par.getBool("onselection");
            float eta = par.getFloat("deptheta");
            bool  useangle = par.getBool("useangle");
            bool  usedistance = par.getBool("usedistance");
            bool  useborders = par.getBool("useborders");
            bool  usesilhouettes = par.getBool("usesilhouettes");
            bool  usealphamask =  par.getBool("usealpha");
            QColor blank = par.getColor("blankColor");

            float  depth=0;     // depth of point (distance from camera)
            float  pdepth=0;    // depth value of projected point (from depth map)
            double pweight;     // pixel weight
            MeshModel *model;
            bool do_project;
            int cam_ind;

            // min max depth for depth weight normalization
            float allcammaxdepth;
            float allcammindepth;

            // max image size for border weight normalization
            float allcammaximagesize;

            // accumulation buffers for colors and weights
            int buff_ind;
            double *weights;
            double *acc_red;
            double *acc_grn;
            double *acc_blu;

            // get current model
            model = md.mm();

            // the mesh has to be correctly transformed before mapping
            tri::UpdatePosition<CMeshO>::Matrix(model->cm,model->cm.Tr,true);
            tri::UpdateBounding<CMeshO>::Box(model->cm);

            // init accumulation buffers for colors and weights
            log("init color accumulation buffers");
            weights = new double[model->cm.vn];
            acc_red = new double[model->cm.vn];
            acc_grn = new double[model->cm.vn];
            acc_blu = new double[model->cm.vn];
            for(int buff_ind=0; buff_ind<model->cm.vn; buff_ind++)
            {
                weights[buff_ind] = 0.0;
                acc_red[buff_ind] = 0.0;
                acc_grn[buff_ind] = 0.0;
                acc_blu[buff_ind] = 0.0;
            }

            // calculate accuratenear/far for all cameras
            std::vector<float> my_near;
            std::vector<float> my_far;
            calculateNearFarAccurate(md, &my_near, &my_far);

            allcammaxdepth =  -1000000;
            allcammindepth =   1000000;
            allcammaximagesize = -1000000;
            for(cam_ind = 0; cam_ind < md.rasterList.size(); cam_ind++)
            {
                if(my_far[cam_ind] > allcammaxdepth)
                    allcammaxdepth = my_far[cam_ind];
                if(my_near[cam_ind] < allcammindepth)
                    allcammindepth = my_near[cam_ind];

                float imgdiag = sqrt(double(md.rasterList[cam_ind]->shot.Intrinsics.ViewportPx[0] * md.rasterList[cam_ind]->shot.Intrinsics.ViewportPx[1]));
                if (imgdiag > allcammaximagesize)
                    allcammaximagesize = imgdiag;
            }

            //-- cycle all cameras
            cam_ind = 0;
            for(RasterModel *raster : md.rasterList){
                if(raster->visible)
                {
                    do_project = true;

                    // no drawing if camera not valid
                    if(!raster->shot.IsValid())
                        do_project = false;

                    // no drawing if raster is not active
                    //if(!raster->shot.IsValid())
                    //  do_project = false;

                    if(do_project)
                    {
                        // making context current
                        glContext->makeCurrent();

                        // delete & reinit rendermanager
                        if(rendermanager != NULL)
                            delete rendermanager;
                        rendermanager = new RenderHelper();
                        if( rendermanager->initializeGL(cb) != 0 )
                            return false;
                        log("init GL");
                        /*if( rendermanager->initializeMeshBuffers(model, cb) != 0 )
                            return false;
                        Log("init Buffers");*/

                        // render normal & depth
                        rendermanager->renderScene(raster->shot, model, RenderHelper::NORMAL, glContext, my_near[cam_ind]*0.5, my_far[cam_ind]*1.25);

                        // unmaking context current
                        glContext->doneCurrent();

                        buff_ind=0;

                        // If should be used silhouette weighting, it is needed to compute depth discontinuities
                        // and per-pixel distance from detected borders on the entire image here
                        // the weight is then applied later, per-vertex, when needed
                        floatbuffer *silhouette_buff=NULL;
                        float maxsildist = rendermanager->depth->sx + rendermanager->depth->sy;
                        if(usesilhouettes)
                        {
                            silhouette_buff = new floatbuffer();
                            silhouette_buff->init(rendermanager->depth->sx, rendermanager->depth->sy);

                            silhouette_buff->applysobel(rendermanager->depth);
                            //sprintf(dumpFileName,"Abord%i.pfm",cam_ind);
                            //silhouette_buff->dumppfm(dumpFileName);

                            silhouette_buff->initborder(rendermanager->depth);
                            //sprintf(dumpFileName,"Bbord%i.pfm",cam_ind);
                            //silhouette_buff->dumppfm(dumpFileName);

                            maxsildist = silhouette_buff->distancefield();
                            //sprintf(dumpFileName,"Cbord%i.pfm",cam_ind);
                            //silhouette_buff->dumppfm(dumpFileName);
                        }

                        for(vi=model->cm.vert.begin();vi!=model->cm.vert.end();++vi)
                        {
                            if(!(*vi).IsD() && (!onselection || (*vi).IsS()))
                            {
                                // pp is the projected point in image space
                                Point2m pp = raster->shot.Project((*vi).P());
                                // pray is the vector from the point-to-be-colored to the camera center
                                Point3m pray = (raster->shot.GetViewPoint() - (*vi).P()).Normalize();

                                //if inside image
                                if(pp[0]>=0 && pp[1]>=0 && pp[0]<raster->shot.Intrinsics.ViewportPx[0] && pp[1]<raster->shot.Intrinsics.ViewportPx[1])
                                {
                                    if((pray.dot(-raster->shot.Axis(2))) <= 0.0)
                                    {

                                        depth  = raster->shot.Depth((*vi).P());
                                        pdepth = rendermanager->depth->getval(int(pp[0]), int(pp[1])); //  rendermanager->depth[(int(pp[1]) * raster->shot.Intrinsics.ViewportPx[0]) + int(pp[0])];

                                        if(depth <= (pdepth + eta))
                                        {
                                            // determine color
                                            QRgb pcolor = raster->currentPlane->image.pixel(pp[0],raster->shot.Intrinsics.ViewportPx[1] - pp[1]);
                                            // determine weight
                                            pweight = 1.0;

                                            if(useangle)
                                            {
                                                Point3m pixnorm = (*vi).N();
                                                Point3m viewaxis  = raster->shot.GetViewPoint() - (*vi).P();
                                                pixnorm.Normalize();
                                                viewaxis.Normalize();

                                                float ang = abs(pixnorm * viewaxis);
                                                ang = min(1.0f, ang);

                                                pweight *= ang;
                                            }

                                            if(usedistance)
                                            {
                                                float distw = depth;
                                                distw = 1.0 - (distw - (allcammindepth*0.99)) / ((allcammaxdepth*1.01) - (allcammindepth*0.99));

                                                pweight *= distw;
                                                pweight *= distw;
                                            }

                                            if(useborders)
                                            {
                                                double xdist = 1.0 - (abs(pp[0] - (raster->shot.Intrinsics.ViewportPx[0] / 2.0)) / (raster->shot.Intrinsics.ViewportPx[0] / 2.0));
                                                double ydist = 1.0 - (abs(pp[1] - (raster->shot.Intrinsics.ViewportPx[1] / 2.0)) / (raster->shot.Intrinsics.ViewportPx[1] / 2.0));
                                                double borderw = min (xdist , ydist);
                                                //borderw = min(1.0,borderw); //debug debug
                                                //borderw = max(0.0,borderw); //debug debug

                                                pweight *= borderw;
                                            }

                                            if(usesilhouettes)
                                            {
                                                // here the silhouette weight is applied, but it is calculated before, on a per-image basis
                                                float silw = 1.0;
                                                silw = silhouette_buff->getval(int(pp[0]), int(pp[1])) / maxsildist;
                                                //silw = min(1.0f,silw); //debug debug
                                                //silw = max(0.0f,silw); //debug debug

                                                pweight *= silw;
                                            }

                                            if(usealphamask) //alpha channel of image is an additional mask
                                            {
                                                pweight *= (qAlpha(pcolor) / 255.0);
                                            }

                                            weights[buff_ind] += pweight;
                                            acc_red[buff_ind] += (qRed(pcolor) * pweight / 255.0);
                                            acc_grn[buff_ind] += (qGreen(pcolor) * pweight / 255.0);
                                            acc_blu[buff_ind] += (qBlue(pcolor) * pweight / 255.0);
                                        }
                                    }
                                }
                            }
                            buff_ind++;
                        }
                        cam_ind ++;

                        if(usesilhouettes)
                        {
                            delete silhouette_buff;
                        }

                    } // end foreach camera
                } // end foreach camera
            }

            buff_ind = 0;
            for(vi=model->cm.vert.begin();vi!=model->cm.vert.end();++vi)
            {
                if(!(*vi).IsD() && (!onselection || (*vi).IsS()))
                {
                    if (weights[buff_ind] != 0) // if 0, it has not found any valid projection on any camera
                    {
                        (*vi).C() = vcg::Color4b( (acc_red[buff_ind] / weights[buff_ind]) *255.0,
                            (acc_grn[buff_ind] / weights[buff_ind]) *255.0,
                            (acc_blu[buff_ind] / weights[buff_ind]) *255.0,
                            255);
                    }
                    else
                    {
                        if ((blank.red() != 0) || (blank.green() != 0) || (blank.blue() != 0) || (blank.alpha() != 0))
                            (*vi).C() = vcg::Color4b(blank.red(), blank.green(), blank.blue(), blank.alpha());
                    }
                }
                buff_ind++;
            }

            // the mesh has to return to its original position
            tri::UpdatePosition<CMeshO>::Matrix(model->cm,Inverse(model->cm.Tr),true);
            tri::UpdateBounding<CMeshO>::Box(model->cm);

            // delete rendermanager
            if(rendermanager != NULL)
                delete rendermanager;

            // delete accumulation buffers
            delete[]  weights;
            delete[]  acc_red;
            delete[]  acc_grn;
            delete[]  acc_blu;

        }
        break;


    case FP_MULTIIMAGETRIVIALPROJTEXTURE :
        {

            if(!tri::HasPerWedgeTexCoord(md.mm()->cm))
            {
                errorMessage="Warning: nothing have been done. Mesh has no Texture Coordinates.";
                return false;
            }

            //bool onselection = par.getBool("onselection");
            int texsize = par.getInt("texsize");
            bool  dorefill = par.getBool("dorefill");
            float eta = par.getFloat("deptheta");
            bool  useangle = par.getBool("useangle");
            bool  usedistance = par.getBool("usedistance");
            bool  useborders = par.getBool("useborders");
            bool  usesilhouettes = par.getBool("usesilhouettes");
            bool  usealphamask =  par.getBool("usealpha");
            QString textName = par.getString("textName");

            int textW = texsize;
            int textH = texsize;

            float  depth=0;     // depth of point (distance from camera)
            float  pdepth=0;    // depth value of projected point (from depth map)
            double pweight;     // pixel weight
            MeshModel *model;
            bool do_project;
            int cam_ind;

            // min max depth for depth weight normalization
            float allcammaxdepth;
            float allcammindepth;

            // max image size for border weight normalization
            float allcammaximagesize;

            // get the working model
            model = md.mm();

            // the mesh has to be correctly transformed before mapping
            tri::UpdatePosition<CMeshO>::Matrix(model->cm,model->cm.Tr,true);
            tri::UpdateBounding<CMeshO>::Box(model->cm);

            // texture file name
            QString filePath(model->fullName());
            filePath = filePath.left(std::max<int>(filePath.lastIndexOf('\\'),filePath.lastIndexOf('/'))+1);
            // Check textName and eventually add .png ext
            CheckError(textName.length() == 0, "Texture file not specified");
            CheckError(std::max<int>(textName.lastIndexOf("\\"),textName.lastIndexOf("/")) != -1, "Path in Texture file not allowed");
            if (!textName.endsWith(".png", Qt::CaseInsensitive))
                textName.append(".png");
            filePath.append(textName);

            // Image creation
            CheckError(textW <= 0, "Texture Width has an incorrect value");
            CheckError(textH <= 0, "Texture Height has an incorrect value");
            QImage img(QSize(textW,textH), QImage::Format_ARGB32);
            img.fill(qRgba(0,0,0,0)); // transparent black

            // Compute (texture-space) border edges
            if(dorefill)
            {
                model->updateDataMask(MeshModel::MM_FACEFACETOPO);
                tri::UpdateTopology<CMeshO>::FaceFaceFromTexCoord(model->cm);
                tri::UpdateFlags<CMeshO>::FaceBorderFromFF(model->cm);
            }

            // create a list of to-be-filled texels and accumulators
            // storing texel 2d coords, texel mesh-space point, texel mesh normal

            vector<TexelDesc> texels;
            texels.clear();
            texels.reserve(textW*textH);  // just to avoid the 2x reallocate rule...

            vector<TexelAccum> accums;
            accums.clear();
            accums.reserve(textW*textH);  // just to avoid the 2x reallocate rule...

            // Rasterizing triangles in the list of voxels
            TexFillerSampler tfs(img);
            tfs.texelspointer = &texels;
            tfs.accumpointer  = &accums;
            tfs.InitCallback(cb, model->cm.fn, 0, 80);
            tri::SurfaceSampling<CMeshO,TexFillerSampler>::Texture(model->cm,tfs,textW,textH,true);

            // Revert alpha values for border edge pixels to 255
            cb(81, "Cleaning up texture ...");
            for (int y=0; y<textH; ++y)
            {
                for (int x=0; x<textW; ++x)
                {
                    QRgb px = img.pixel(x,y);
                    if (qAlpha(px) < 255 && qAlpha(px) > 0)
                        img.setPixel(x,y, px | 0xff000000);
                }
            }

            // calculate accuratenear/far for all cameras
            std::vector<float> my_near;
            std::vector<float> my_far;
            calculateNearFarAccurate(md, &my_near, &my_far);

            allcammaxdepth =  -1000000;
            allcammindepth =   1000000;
            allcammaximagesize = -1000000;
            for(cam_ind = 0; cam_ind < md.rasterList.size(); cam_ind++)
            {
                if(my_far[cam_ind] > allcammaxdepth)
                    allcammaxdepth = my_far[cam_ind];
                if(my_near[cam_ind] < allcammindepth)
                    allcammindepth = my_near[cam_ind];

                float imgdiag = sqrt(double(md.rasterList[cam_ind]->shot.Intrinsics.ViewportPx[0] * md.rasterList[cam_ind]->shot.Intrinsics.ViewportPx[1]));
                if (imgdiag > allcammaximagesize)
                    allcammaximagesize = imgdiag;
            }

            //-- cycle all cameras
            cam_ind = 0;
            for(RasterModel *raster : md.rasterList)
            {
                if(raster->visible)
                {
                    do_project = true;

                    // no drawing if camera not valid
                    if(!raster->shot.IsValid())
                        do_project = false;

                    // no drawing if raster is not active
                    //if(!raster->shot.IsValid())
                    //  do_project = false;

                    if(do_project)
                    {
                        // making context current
                        glContext->makeCurrent();

                        // delete & reinit rendermanager
                        if(rendermanager != NULL)
                            delete rendermanager;
                        rendermanager = new RenderHelper();
                        if( rendermanager->initializeGL(cb) != 0 )
                            return false;
                        log("init GL");
                        /*if( rendermanager->initializeMeshBuffers(model, cb) != 0 )
                            return false;
                        Log("init Buffers");*/

                        // render normal & depth
                        rendermanager->renderScene(raster->shot, model, RenderHelper::NORMAL, glContext, my_near[cam_ind]*0.5, my_far[cam_ind]*1.25);

                        // unmaking context current
                        glContext->doneCurrent();

                        // If should be used silhouette weighting, it is needed to compute depth discontinuities
                        // and per-pixel distance from detected borders on the entire image here
                        // the weight is then applied later, per-vertex, when needed
                        floatbuffer *silhouette_buff=NULL;
                        float maxsildist = rendermanager->depth->sx + rendermanager->depth->sy;
                        if(usesilhouettes)
                        {
                            silhouette_buff = new floatbuffer();
                            silhouette_buff->init(rendermanager->depth->sx, rendermanager->depth->sy);

                            silhouette_buff->applysobel(rendermanager->depth);
                            //sprintf(dumpFileName,"Abord%i.bmp",cam_ind);
                            //silhouette_buff->dumpbmp(dumpFileName);

                            silhouette_buff->initborder(rendermanager->depth);
                            //sprintf(dumpFileName,"Bbord%i.bmp",cam_ind);
                            //silhouette_buff->dumpbmp(dumpFileName);

                            maxsildist = silhouette_buff->distancefield();
                            //sprintf(dumpFileName,"Cbord%i.bmp",cam_ind);
                            //silhouette_buff->dumpbmp(dumpFileName);
                        }

                        for(size_t texcount=0; texcount < texels.size(); texcount++)
                        {
                            Point2m pp = raster->shot.Project(texels[texcount].meshpoint);
                            // pray is the vector from the point-to-be-colored to the camera center
                            Point3m pray = (raster->shot.GetViewPoint() - texels[texcount].meshpoint).Normalize();

                            //if inside image
                            if(pp[0]>0 && pp[1]>0 && pp[0]<raster->shot.Intrinsics.ViewportPx[0] && pp[1]<raster->shot.Intrinsics.ViewportPx[1])
                            {
                                if((pray.dot(-raster->shot.Axis(2))) <= 0.0)
                                {

                                    depth  = raster->shot.Depth(texels[texcount].meshpoint);
                                    pdepth = rendermanager->depth->getval(int(pp[0]), int(pp[1])); //  rendermanager->depth[(int(pp[1]) * raster->shot.Intrinsics.ViewportPx[0]) + int(pp[0])];

                                    if(depth <= (pdepth + eta))
                                    {
                                        // determine color
                                        QRgb pcolor = raster->currentPlane->image.pixel(pp[0],raster->shot.Intrinsics.ViewportPx[1] - pp[1]);
                                        // determine weight
                                        pweight = 1.0;

                                        if(useangle)
                                        {
                                            Point3m pixnorm = texels[texcount].meshnormal;
                                            pixnorm.Normalize();

                                            Point3m viewaxis = raster->shot.GetViewPoint() - texels[texcount].meshpoint;
                                            viewaxis.Normalize();

                                            float ang = abs(pixnorm * viewaxis);
                                            ang = min(1.0f, ang);

                                            pweight *= ang;
                                        }

                                        if(usedistance)
                                        {
                                            float distw = depth;
                                            distw = 1.0 - (distw - (allcammindepth*0.99)) / ((allcammaxdepth*1.01) - (allcammindepth*0.99));

                                            pweight *= distw;
                                            pweight *= distw;
                                        }

                                        if(useborders)
                                        {
                                            double xdist = 1.0 - (abs(pp[0] - (raster->shot.Intrinsics.ViewportPx[0] / 2.0)) / (raster->shot.Intrinsics.ViewportPx[0] / 2.0));
                                            double ydist = 1.0 - (abs(pp[1] - (raster->shot.Intrinsics.ViewportPx[1] / 2.0)) / (raster->shot.Intrinsics.ViewportPx[1] / 2.0));
                                            double borderw = min (xdist , ydist);

                                            pweight *= borderw;
                                        }

                                        if(usesilhouettes)
                                        {
                                            // here the silhouette weight is applied, but it is calculated before, on a per-image basis
                                            float silw = 1.0;
                                            silw = silhouette_buff->getval(int(pp[0]), int(pp[1])) / maxsildist;
                                            pweight *= silw;
                                        }

                                        if(usealphamask) //alpha channel of image is an additional mask
                                        {
                                            pweight *= (qAlpha(pcolor) / 255.0);
                                        }

                                        accums[texcount].weights += pweight;
                                        accums[texcount].acc_red += (qRed(pcolor) * pweight / 255.0);
                                        accums[texcount].acc_grn += (qGreen(pcolor) * pweight / 255.0);
                                        accums[texcount].acc_blu += (qBlue(pcolor) * pweight / 255.0);
                                    }
                                }
                            }

                        } // end foreach texel
                        cam_ind ++;

                        if(usesilhouettes)
                        {
                            delete silhouette_buff;
                        }

                    } // end if(do_project)

                }
            } // end foreach camera

            // for each texel.... divide accumulated values by weight and write to texture
            for(size_t texcount=0; texcount < texels.size(); texcount++)
            {
                if(accums[texcount].weights > 0.0)
                {
                    float texel_red   =  accums[texcount].acc_red / accums[texcount].weights;
                    float texel_green =  accums[texcount].acc_grn / accums[texcount].weights;
                    float texel_blue  =  accums[texcount].acc_blu / accums[texcount].weights;

                    img.setPixel(texels[texcount].texcoord.X(), img.height() - 1 - texels[texcount].texcoord.Y(), qRgba(texel_red*255.0, texel_green*255.0, texel_blue*255.0, 255));
                }
                else         // if no projected data available, black (to be refilled later on
                {
                    img.setPixel(texels[texcount].texcoord.X(), img.height() - 1 - texels[texcount].texcoord.Y(), qRgba(0, 0, 0, 0));
                }
            }

            // cleaning
            texels.clear();
            accums.clear();

            // PullPush
            if(dorefill)
            {
                cb(85, "Filling texture holes...");

                PullPush(img, qRgba(0,0,0,0));      // atlas gaps
            }

            // Undo topology changes
            if(dorefill)
            {
                tri::UpdateTopology<CMeshO>::FaceFace(model->cm);
                tri::UpdateFlags<CMeshO>::FaceBorderFromFF(model->cm);
            }

            // Save texture
            cb(90, "Saving texture ...");
            CheckError(!img.save(filePath), "Texture file cannot be saved");
            log( "Texture \"%s\" Created", filePath.toStdString().c_str());
            assert(QFile(filePath).exists());

            // Assign texture
            model->cm.textures.clear();
            model->cm.textures.push_back(textName.toStdString());

            // the mesh has to return to its original position
            tri::UpdatePosition<CMeshO>::Matrix(model->cm,Inverse(model->cm.Tr),true);
            tri::UpdateBounding<CMeshO>::Box(model->cm);


        } break;


    }

    return true;
}

FilterColorProjectionPlugin::FilterClass FilterColorProjectionPlugin::getClass(const QAction *a) const
{
    switch(ID(a)) {
    case FP_SINGLEIMAGEPROJ:
        return FilterClass(Camera + VertexColoring);
        break;
    case FP_MULTIIMAGETRIVIALPROJ:
        return FilterClass(Camera + VertexColoring);
        break;
    case FP_MULTIIMAGETRIVIALPROJTEXTURE:
        return FilterClass(Camera + Texture);
        break;
    default :  assert(0);
        return FilterPluginInterface::Generic;
    }
}

int FilterColorProjectionPlugin::postCondition( const QAction* a ) const{
    switch(ID(a)) {
    case FP_SINGLEIMAGEPROJ:
        return MeshModel::MM_VERTCOLOR;
        break;
    case FP_MULTIIMAGETRIVIALPROJ:
        return MeshModel::MM_VERTCOLOR;
        break;
    case FP_MULTIIMAGETRIVIALPROJTEXTURE:
		return MeshModel::MM_WEDGTEXCOORD;
        break;
    default: 
        return MeshModel::MM_ALL;
    }
}

//--- this function calculates the near and far values
int FilterColorProjectionPlugin::calculateNearFarAccurate(MeshDocument &md, std::vector<float> *near_acc, std::vector<float> *far_acc)
{
    CMeshO::VertexIterator vi;
    int rasterindex;

    if(near_acc != NULL)
    {
        near_acc->clear();
        near_acc->resize(md.rasterList.size());
    }
    else
        return -1;

    if(far_acc != NULL)
    {
        far_acc->clear();
        far_acc->resize(md.rasterList.size());
    }
    else
        return -1;

    // init near and far vectors
    for(rasterindex = 0; rasterindex < md.rasterList.size(); rasterindex++)
    {
        (*near_acc)[rasterindex] =  1000000;
        (*far_acc)[rasterindex]  = -1000000;
    }

    // current model
    MeshModel *model = md.mm();

    // scan all vertices
    for(vi=model->cm.vert.begin();vi!=model->cm.vert.end();++vi)
    {
        if(!(*vi).IsD())
        {
            // check against all cameras
            rasterindex = 0;
            foreach(RasterModel *raster, md.rasterList)
            {
                if(raster->shot.IsValid())
                {
                    Point2m pp = raster->shot.Project((*vi).P());

                    Point3m viewaxis = raster->shot.GetViewPoint() - (*vi).P();
                    viewaxis.Normalize();

                    //if(viewaxis * raster->shot.GetViewDir() > 0.0)     // if facing
                    if(pp[0]>0 && pp[1]>0 && pp[0]<raster->shot.Intrinsics.ViewportPx[0] && pp[1]<raster->shot.Intrinsics.ViewportPx[1]) // if inside image
                    {
                        // then update near and far
                        if(raster->shot.Depth((*vi).P()) < (*near_acc)[rasterindex])
                            (*near_acc)[rasterindex] = raster->shot.Depth((*vi).P());
                        if(raster->shot.Depth((*vi).P()) > (*far_acc)[rasterindex])
                            (*far_acc)[rasterindex]  = raster->shot.Depth((*vi).P());
                    }

                }
                rasterindex++;
            }
        }
    }

    for(rasterindex = 0; rasterindex < md.rasterList.size(); rasterindex++) // set to 0 0 invalid and "strange" cameras
    {
        if ( ((*near_acc)[rasterindex] == 1000000)  ||  ((*far_acc)[rasterindex] == -1000000) )
        {
            (*near_acc)[rasterindex] = 0;
            (*far_acc)[rasterindex]  = 0;
        }
    }

    return 0;
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterColorProjectionPlugin)
