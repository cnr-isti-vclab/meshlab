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

#ifndef _RASTERING_H
#define _RASTERING_H

#include <QtGui>
#include <common/interfaces.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/space/triangle2.h>


// texel descriptor---------------------------------

typedef struct{

  Point2i texcoord;

  Point3f meshpoint;
  Point3f meshnormal;

} TexelDesc;

typedef struct{

  float weights;
  float acc_red;
  float acc_grn;
  float acc_blu;

} TexelAccum;


//--------------------------------------------------

//----------- TEXTURE FILLING SAMPLER ------------------------------
/*
class TexFillSampler
{
	public:
	TexFillSampler(CMeshO* _m){m=_m; uvSpaceFlag = false; qualitySampling=false; tex=0;};
	CMeshO *m;
	QImage* tex;
	int texSamplingWidth;
	int texSamplingHeight;
	bool uvSpaceFlag;
	bool qualitySampling;
	texwork *thiswork;
	
	void AddVert(const MyMesh::VertexType &p) 
	{
	}
	
	void AddFace(const MyMesh::FaceType &f, MyMesh::CoordType p) 
	{
	}
	
	void AddTextureSample(const MyMesh::FaceType &f, const MyMesh::CoordType &p, const Point2i &tp, float edgeDist=0)
	{
		double rr,gg,bb;
		double count;
		QColor col;
		float  weight;
		Point2f ppoint;
		Point3f pray;
		float dpt,ref;

		Point3f meshpoint = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
		Point3f meshnorm  = f.V(0)->N()*p[0] + f.V(1)->N()*p[1] +f.V(2)->N()*p[2];

		rr = gg = bb = 0;
		count = 0;
		for(int camit=0; camit<thiswork->camnum; camit++)
		{
			ppoint = thiswork->immagini[camit].imshot->Project(meshpoint);
			// pray is the vector from the point-to-be-colored to the camera center
			pray = (thiswork->immagini[camit].imshot->Extrinsics.Tra() - meshpoint).Normalize();


			if((ppoint[0] > 0) && (ppoint[0] < thiswork->immagini[camit].imshot->Intrinsics.ViewportPx.X()) &&
			  (ppoint[1] > 0) && (ppoint[1] < thiswork->immagini[camit].imshot->Intrinsics.ViewportPx.Y()))
				if((meshnorm.dot(-thiswork->immagini[camit].imshot->Axis(2))) <= 0.0)
				if((pray.dot(-thiswork->immagini[camit].imshot->Axis(2))) <= 0.0)
				{

					dpt  = (thiswork->immagini[camit].imshot->Depth(meshpoint) - thiswork->depth_eta);
					ref  = thiswork->immagini[camit].camdepth.getval(ppoint[0],ppoint[1]);

					if(dpt < ref)
					{
						weight = thiswork->immagini[camit].camweight.getval(ppoint[0],ppoint[1]);
 						ppoint[1] = thiswork->immagini[camit].imshot->Intrinsics.ViewportPx.Y() - ppoint[1];
						col = thiswork->immagini[camit].impicture->pixel(ppoint[0],ppoint[1]);

						if((col.red() + col.green() + col.blue()) > 0)
						{
						 rr += col.red()*weight; gg += col.green()*weight; bb += col.blue()*weight;
						 count += weight;
						}
					}
				}
		}
  
		if(count > 0)
		{
			rr = (double)rr/count;
			gg = (double)gg/count;
			bb = (double)bb/count;

			// updating color mean
			if(thiswork->usemean)
			{
				thiswork->meanrr = (thiswork->meanrr * 10.0 + rr) / 11.0;
				thiswork->meangg = (thiswork->meangg * 10.0 + gg) / 11.0;
				thiswork->meanbb = (thiswork->meanbb * 10.0 + bb) / 11.0;
			}
		}
		else	// default color
		{
			rr = thiswork->meanrr;
			gg = thiswork->meangg;
			bb = thiswork->meanbb;
		}

//--------------------------

		tex->setPixel(tp[0], texSamplingHeight - tp[1], qRgb(rr,gg,bb));
		

	
	}
}; // end class TexFillSampler
//------------------------------------------------------------------

*/


class TexFillerSampler
{
public:
  
    QImage &trgImg;

    vector<TexelDesc> *texelspointer; // list of active texels to be filled
    vector<TexelAccum> *accumpointer;  // list of color + weight accumulators

    // Callback stuff
    vcg::CallBackPos *cb;
    const CMeshO::FaceType *currFace;
    int faceNo, faceCnt, start, offset;


    TexFillerSampler(QImage &_img) : trgImg(_img) {}

    void InitCallback(vcg::CallBackPos *_cb, int _faceNo, int _start=0, int _offset=100)
    {
        assert(_faceNo > 0);
        assert(_start>=0);
        assert(_offset>=0 && _offset <= 100-_start);
        cb = _cb;
        faceNo = _faceNo;
        faceCnt = 0;
        start = _start;
        offset = _offset;
        currFace = NULL;
    }

    // expects points outside face (affecting face color) with edge distance > 0
    void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const vcg::Point2i &tp, float edgeDist= 0.0)
    {
  		Point3f meshpoint = f.P(0)*p[0] + f.P(1)*p[1] +f.P(2)*p[2];
	  	Point3f meshnorm  = (f.V(0)->N()*p[0] + f.V(1)->N()*p[1] + f.V(2)->N()*p[2]).Normalize();

      TexelDesc newtexel;
      newtexel.texcoord = tp;
      newtexel.meshpoint = meshpoint;
      newtexel.meshnormal = meshnorm;

      TexelAccum newaccum;
      newaccum.weights = 0.0;
      newaccum.acc_red = 0.0;
      newaccum.acc_grn = 0.0;
      newaccum.acc_blu = 0.0;

      texelspointer->push_back(newtexel);
      accumpointer->push_back(newaccum);
    }
}; // end class TexFillerSampler



#endif
