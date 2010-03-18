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
#include <vcg/complex/trimesh/point_sampling.h>
#include <vcg/space/triangle2.h>

class VertexSampler
{
	typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
	typedef vcg::tri::FaceTmark<CMeshO> MarkerFace;
	
	CMeshO &srcMesh;
	QImage &srcImg;
	float dist_upper_bound;
	
	MetroMeshGrid unifGridFace;
	MarkerFace markerFunctor;
	vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
	
	// Callback stuff
	vcg::CallBackPos *cb;
	int vertexNo, vertexCnt, start, offset;
	
public:
	VertexSampler(CMeshO &_srcMesh, QImage &_srcImg, float upperBound) : 
	srcMesh(_srcMesh), srcImg(_srcImg), dist_upper_bound(upperBound) 
	{
		unifGridFace.Set(_srcMesh.face.begin(),_srcMesh.face.end());
		markerFunctor.SetMesh(&_srcMesh);
	}
	
	void InitCallback(vcg::CallBackPos *_cb, int _vertexNo, int _start=0, int _offset=100)
	{
		assert(_vertexNo > 0);
		assert(_start>=0);
		assert(_offset>=0 && _offset <= 100-_start);
		cb = _cb;
		vertexNo = _vertexNo;
		vertexCnt = 0;
		start = _start;
		offset = _offset;
	}
	
	void AddVert(CMeshO::VertexType &v)
	{
		// Get Closest point
		CMeshO::CoordType closestPt;
		float dist=dist_upper_bound;
		CMeshO::FaceType *nearestF;
		nearestF =  unifGridFace.GetClosest(PDistFunct, markerFunctor, v.cP(), dist_upper_bound, dist, closestPt);
		if (dist == dist_upper_bound) return;
		
		// Convert point to barycentric coords
		vcg::Point3f interp;
		int axis = 0;
		float tmp = -1;
		for (int i=0; i<3; ++i)
			if (fabs(nearestF->cN()[i]) > tmp) {tmp = fabs(nearestF->cN()[i]); axis = i;}
		bool ret = InterpolationParameters(*nearestF, axis, closestPt, interp);
		assert(ret);
		interp[2]=1.0-interp[1]-interp[0];
		
		int w=srcImg.width(), h=srcImg.height();
		int x, y;
		x = w * (interp[0]*nearestF->cWT(0).U()+interp[1]*nearestF->cWT(1).U()+interp[2]*nearestF->cWT(2).U());
		y = h * (1.0 - (interp[0]*nearestF->cWT(0).V()+interp[1]*nearestF->cWT(1).V()+interp[2]*nearestF->cWT(2).V()));
		// repeat mode
		x = (x%w + w)%w;
		y = (y%h + h)%h;
		QRgb px = srcImg.pixel(x, y);
		v.C() = CMeshO::VertexType::ColorType(qRed(px), qGreen(px), qBlue(px), 255);
	}
};

class RasterSampler
{
	QImage &trgImg;
	
	// Callback stuff
	vcg::CallBackPos *cb;
	const CMeshO::FaceType *currFace;
	int faceNo, faceCnt, start, offset;
	
public:
	RasterSampler(QImage &_img) : trgImg(_img) {}
	
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
		CMeshO::VertexType::ColorType c;
		/*int alpha = 255;
		if (fabs(p[0]+p[1]+p[2]-1)>=0.00001)
			if (p[0] <.0) {alpha = 254+p[0]*128; bary[0] = 0.;} else
				if (p[1] <.0) {alpha = 254+p[1]*128; bary[1] = 0.;} else
					if (p[2] <.0) {alpha = 254+p[2]*128; bary[2] = 0.;}*/
		int alpha = 255;
		if (edgeDist != 0.0)
			alpha=254-edgeDist*128;
		
		if (alpha==255 || qAlpha(trgImg.pixel(tp.X(), trgImg.height() - tp.Y())) < alpha)
		{
			c.lerp(f.V(0)->cC(), f.V(1)->cC(), f.V(2)->cC(), p);
			trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), qRgba(c[0], c[1], c[2], alpha));
		}
		if (cb)
		{
			if (&f != currFace) {currFace = &f; ++faceCnt;}
			cb(start + faceCnt*offset/faceNo, "Rasterizing faces ...");
		}
	}
};

class TransferColorSampler
{
	typedef vcg::GridStaticPtr<CMeshO::FaceType, CMeshO::ScalarType > MetroMeshGrid;
    typedef vcg::GridStaticPtr<CMeshO::VertexType, CMeshO::ScalarType > VertexMeshGrid;

	QImage &trgImg;
	QImage *srcImg;
	float dist_upper_bound;
	bool fromTexture;
	MetroMeshGrid unifGridFace;
    VertexMeshGrid   unifGridVert;
    bool useVertexSampling;

	// Callback stuff
	vcg::CallBackPos *cb;
	const CMeshO::FaceType *currFace;
    CMeshO *srcMesh;
	int faceNo, faceCnt, start, offset;
	
	typedef vcg::tri::FaceTmark<CMeshO> MarkerFace;
	MarkerFace markerFunctor;
	
	/*QRgb GetBilinearPixelColor(float _u, float _v, int alpha)
	{
		int w = srcImg->width();
		int h = srcImg->height();
		QRgb p00, p01, p10, p11;
		float u = _u * w -0.5;
		float v = _v * h -0.5;
		int x = floor(u);
		int y = floor(v);
		float u_ratio = u - x;
		float v_ratio = v - y;
		x = (x%w + w)%w;
		y = (y%h + h)%h;
		float u_opposite = 1 - u_ratio;
		float v_opposite = 1 - v_ratio;
		
		p00 = srcImg->pixel(x,y);
		p01 = srcImg->pixel(x, (y+1)%h);
		p10 = srcImg->pixel((x+1)%w, y);
		p11 = srcImg->pixel((x+1)%w, (y+1)%h);
		int r,g,b;
		r = (qRed(p00)*u_opposite+qRed(p01)*u_ratio)*v_opposite + 
			(qRed(p01)*u_opposite+qRed(p11)*u_ratio)*v_ratio;
		g = (qGreen(p00)*u_opposite+qGreen(p01)*u_ratio)*v_opposite + 
			(qGreen(p01)*u_opposite+qGreen(p11)*u_ratio)*v_ratio;
		b = (qBlue(p00)*u_opposite+qBlue(p01)*u_ratio)*v_opposite + 
		    (qBlue(p01)*u_opposite+qBlue(p11)*u_ratio)*v_ratio;
		return qRgba(r,g,b, alpha);	
	}*/
	
public:
	TransferColorSampler(CMeshO &_srcMesh, QImage &_trgImg, float upperBound)
	: trgImg(_trgImg), dist_upper_bound(upperBound)
	{
        srcMesh=&_srcMesh;
        useVertexSampling = _srcMesh.face.empty();
        if(useVertexSampling) unifGridVert.Set(_srcMesh.vert.begin(),_srcMesh.vert.end());
                        else  unifGridFace.Set(_srcMesh.face.begin(),_srcMesh.face.end());
		markerFunctor.SetMesh(&_srcMesh);
		fromTexture = false;

	}
	
	TransferColorSampler(CMeshO &_srcMesh, QImage &_trgImg, QImage *_srcImg, float upperBound)
	: trgImg(_trgImg), dist_upper_bound(upperBound)
	{
		assert(_srcImg != NULL);
		srcImg = _srcImg;
        unifGridFace.Set(_srcMesh.face.begin(),_srcMesh.face.end());
        markerFunctor.SetMesh(&_srcMesh);
		fromTexture = true;
        useVertexSampling=false;
	}
	
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
	
	void AddTextureSample(const CMeshO::FaceType &f, const CMeshO::CoordType &p, const vcg::Point2i &tp, float edgeDist=0.0)
	{
		// Calculate correct barycentric coords
		/*CMeshO::CoordType bary = p;
		int alpha = 255;
		if (fabs(p[0]+p[1]+p[2]-1)>=0.00001)
			if (p[0] <.0) {alpha = 254+p[0]*128; bary[0] = 0.;} else
				if (p[1] <.0) {alpha = 254+p[1]*128; bary[1] = 0.;} else
					if (p[2] <.0) {alpha = 254+p[2]*128; bary[2] = 0.;}*/
		
		CMeshO::CoordType bary = p;
		int alpha = 255;
		if (edgeDist != 0.0)
			alpha=254-edgeDist*128;
		
		// Get point on face
		CMeshO::CoordType startPt;
		startPt[0] = bary[0]*f.V(0)->P().X()+bary[1]*f.V(1)->P().X()+bary[2]*f.V(2)->P().X();
		startPt[1] = bary[0]*f.V(0)->P().Y()+bary[1]*f.V(1)->P().Y()+bary[2]*f.V(2)->P().Y();
		startPt[2] = bary[0]*f.V(0)->P().Z()+bary[1]*f.V(1)->P().Z()+bary[2]*f.V(2)->P().Z();
		
		// Retrieve closest point on source mesh

        if(useVertexSampling)
        {
            CMeshO::VertexType   *nearestV=0;
            float dist=dist_upper_bound;
            nearestV =  vcg::tri::GetClosestVertex<CMeshO,VertexMeshGrid>(*srcMesh,unifGridVert,startPt,dist_upper_bound,dist); //(PDistFunct,markerFunctor,startPt,dist_upper_bound,dist,closestPt);
        //if(cb) cb(sampleCnt++*100/sampleNum,"Resampling Vertex attributes");
            //if(storeDistanceAsQualityFlag)  p.Q() = dist;
            if(dist == dist_upper_bound) return ;
            trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), qRgba(nearestV->C()[0], nearestV->C()[1], nearestV->C()[2], 255));
        }
        else // sampling from a mesh
        {
		CMeshO::CoordType closestPt;
		vcg::face::PointDistanceBaseFunctor<CMeshO::ScalarType> PDistFunct;
		float dist=dist_upper_bound;
		CMeshO::FaceType *nearestF;
		nearestF =  unifGridFace.GetClosest(PDistFunct, markerFunctor, startPt, dist_upper_bound, dist, closestPt);
		if (dist == dist_upper_bound) return;
		
		// Convert point to barycentric coords
		vcg::Point3f interp;
		int axis = 0;
		float tmp = -1;
		for (int i=0; i<3; ++i)
			if (fabs(nearestF->cN()[i]) > tmp) {tmp = fabs(nearestF->cN()[i]); axis = i;}
		bool ret = InterpolationParameters(*nearestF, axis, closestPt, interp);
		assert(ret);
		interp[2]=1.0-interp[1]-interp[0];
		
		if (alpha==255 || qAlpha(trgImg.pixel(tp.X(), trgImg.height() - tp.Y())) < alpha)
			if (fromTexture)
			{
				int w=srcImg->width(), h=srcImg->height();
				int x, y;
				x = w * (interp[0]*nearestF->cWT(0).U()+interp[1]*nearestF->cWT(1).U()+interp[2]*nearestF->cWT(2).U());
				y = h * (1.0 - (interp[0]*nearestF->cWT(0).V()+interp[1]*nearestF->cWT(1).V()+interp[2]*nearestF->cWT(2).V()));
				// repeat mode
				x = (x%w + w)%w;
				y = (y%h + h)%h;
				QRgb px = srcImg->pixel(x, y);
				trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), qRgba(qRed(px), qGreen(px), qBlue(px), alpha));
				
				/*float u = interp[0]*nearestF->cWT(0).U()+interp[1]*nearestF->cWT(1).U()+interp[2]*nearestF->cWT(2).U();
				float v = 1.0 - (interp[0]*nearestF->cWT(0).V()+interp[1]*nearestF->cWT(1).V()+interp[2]*nearestF->cWT(2).V());
				trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), GetBilinearPixelColor(u, v, alpha));*/
			}
			else
			{	
				// Calculate and set color 
				CMeshO::VertexType::ColorType c;
				c.lerp(nearestF->V(0)->cC(), nearestF->V(1)->cC(), nearestF->V(2)->cC(), interp);
				trgImg.setPixel(tp.X(), trgImg.height() - tp.Y(), qRgba(c[0], c[1], c[2], alpha));
			}
		
		if (cb)
		{
			if (&f != currFace) {currFace = &f; ++faceCnt;}
			cb(start + faceCnt*offset/faceNo, "Rasterizing faces ...");
		}
    }
	}
};

/*
template <class MetroMesh, class VertexSampler, bool EDGEBARYCENTRIC>
static void SingleFaceRasterWEdge(typename MetroMesh::FaceType &f,  VertexSampler &ps, 
								  const vcg::Point2<typename MetroMesh::ScalarType> & v0, 
								  const vcg::Point2<typename MetroMesh::ScalarType> & v1, 
								  const vcg::Point2<typename MetroMesh::ScalarType> & v2)
{
	typedef typename MetroMesh::ScalarType S;
	// Calcolo bounding box
	vcg::Box2i bbox;
	
	if(v0[0]<v1[0]) { bbox.min[0]=int(v0[0]); bbox.max[0]=int(v1[0]); }
	else            { bbox.min[0]=int(v1[0]); bbox.max[0]=int(v0[0]); }
	if(v0[1]<v1[1]) { bbox.min[1]=int(v0[1]); bbox.max[1]=int(v1[1]); }
	else            { bbox.min[1]=int(v1[1]); bbox.max[1]=int(v0[1]); }
	if(bbox.min[0]>int(v2[0])) bbox.min[0]=int(v2[0]);
	else if(bbox.max[0]<int(v2[0])) bbox.max[0]=int(v2[0]);
	if(bbox.min[1]>int(v2[1])) bbox.min[1]=int(v2[1]);
	else if(bbox.max[1]<int(v2[1])) bbox.max[1]=int(v2[1]);
	
	
	// Calcolo versori degli spigoli
	vcg::Point2<S> d10 = v1 - v0;
	vcg::Point2<S> d21 = v2 - v1;
	vcg::Point2<S> d02 = v0 - v2;
	
	// Preparazione prodotti scalari
	S b0  = (bbox.min[0]-v0[0])*d10[1] - (bbox.min[1]-v0[1])*d10[0];
	S b1  = (bbox.min[0]-v1[0])*d21[1] - (bbox.min[1]-v1[1])*d21[0];
	S b2  = (bbox.min[0]-v2[0])*d02[1] - (bbox.min[1]-v2[1])*d02[0];
	// Preparazione degli steps
	S db0 = d10[1];
	S db1 = d21[1];
	S db2 = d02[1];
	// Preparazione segni
	S dn0 = -d10[0];
	S dn1 = -d21[0];
	S dn2 = -d02[0];
	
	//Calcolo orientamento
        bool flipped = !(d02 * vcg::Point2<S>(-d10[1], d10[0]) >= 0);
	
	// Precalcolo Calcolo edge di bordo
	vcg::Segment2<S> borderEdges[3];
	S edgeLength[3];
	unsigned char edgeMask = 0;
	if (f.IsB(0)) {
		borderEdges[0] = vcg::Segment2<S>(v0, v1);
		edgeLength[0] = borderEdges[0].Length();
		edgeMask |= 1;
	}
	if (f.IsB(1)) {
		borderEdges[1] = vcg::Segment2<S>(v1, v2);
		edgeLength[1] = borderEdges[1].Length();
		edgeMask |= 2;
	}
	if (f.IsB(2)) {
		borderEdges[2] = vcg::Segment2<S>(v2, v0);
		edgeLength[2] = borderEdges[2].Length();
		edgeMask |= 4;
	}
	
	// Rasterizzazione
	double de = v0[0]*v1[1]-v0[0]*v2[1]-v1[0]*v0[1]+v1[0]*v2[1]-v2[0]*v1[1]+v2[0]*v0[1];
	
	for(int x=bbox.min[0]-1;x<=bbox.max[0]+1;++x)
	{
		bool in = false;
		S n[3]  = { b0-db0-dn0, b1-db1-dn1, b2-db2-dn2};
		for(int y=bbox.min[1]-1;y<=bbox.max[1]+1;++y)
		{
			if((n[0]>=0 && n[1]>=0 && n[2]>=0) || (n[0]<=0 && n[1]<=0 && n[2]<=0))
			{
				typename MetroMesh::CoordType baryCoord;
				baryCoord[0] =  double(-y*v1[0]+v2[0]*y+v1[1]*x-v2[0]*v1[1]+v1[0]*v2[1]-x*v2[1])/de;
				baryCoord[1] = -double( x*v0[1]-x*v2[1]-v0[0]*y+v0[0]*v2[1]-v2[0]*v0[1]+v2[0]*y)/de;
				baryCoord[2] = 1-baryCoord[0]-baryCoord[1];
				
				ps.AddTextureSample(f, baryCoord, vcg::Point2i(x,y));
				in = true;
			} else {
				// Check whether a pixel outside (on a border edge side) triangle affects color inside it
				vcg::Point2<S> px(x, y);
				vcg::Point2<S> closePoint;
				int closeEdge = -1;
				S minDst = FLT_MAX;
				
				for (int i=0, t=0; t<2 && i<3 && (edgeMask>>i)%2 ; ++i)
				{
					vcg::Point2<S> close;
					S dst;
                                        if ( (!flipped && n[i]<0 || flipped && n[i]>0) &&
						(dst = ((close = ClosestPoint(borderEdges[i], px)) - px).Norm()) < minDst &&
						close.X() > px.X()-1 && close.X() < px.X()+1 &&
						close.Y() > px.Y()-1 && close.Y() < px.Y()+1)
					{
						minDst = dst;
						closePoint = close;
						closeEdge = i;
						++t;
					}
				}
				
				if (closeEdge >= 0)
				{
					typename MetroMesh::CoordType baryCoord;
					if (EDGEBARYCENTRIC)
					{
						// Add x,y sample with closePoint barycentric coords (on edge)
						baryCoord[closeEdge] = (closePoint - borderEdges[closeEdge].P(1)).Norm()/edgeLength[closeEdge];
						baryCoord[(closeEdge+1)%3] = 1 - baryCoord[closeEdge];
						baryCoord[(closeEdge+2)%3] = 0;
					} else {
						// Add x,y sample with his own barycentric coords (off edge)
						baryCoord[0] =  double(-y*v1[0]+v2[0]*y+v1[1]*x-v2[0]*v1[1]+v1[0]*v2[1]-x*v2[1])/de;
						baryCoord[1] = -double( x*v0[1]-x*v2[1]-v0[0]*y+v0[0]*v2[1]-v2[0]*v0[1]+v2[0]*y)/de;
						baryCoord[2] = 1-baryCoord[0]-baryCoord[1];
					}
					ps.AddTextureSample(f, baryCoord, vcg::Point2i(x,y), minDst);
					in = true;
				} else if (in) break;
			}
			n[0] += dn0;
			n[1] += dn1;
			n[2] += dn2;
		}
		b0 += db0;
		b1 += db1;
		b2 += db2;
	}
}

template <class MetroMesh, class VertexSampler, bool EDGEBARYCENTRIC>	
static void TextureCorrectedWEdge(MetroMesh & m, VertexSampler &ps, int textureWidth, int textureHeight)
{
	typedef typename MetroMesh::FaceIterator FaceIterator;
	FaceIterator fi;
	
	printf("Similar Triangles face sampling\n");
	for(fi=m.face.begin(); fi != m.face.end(); fi++)
		if (!fi->IsD())
		{
			vcg::Point2f ti[3];
			for(int i=0;i<3;++i)
				ti[i]=vcg::Point2f((*fi).WT(i).U() * textureWidth - 0.5, (*fi).WT(i).V() * textureHeight + 0.5);
			//vcg::tri::SurfaceSampling<MetroMesh,VertexSampler>::SingleFaceRaster(*fi,  ps, ti[0],ti[1],ti[2]);
			SingleFaceRasterWEdge<MetroMesh,VertexSampler, EDGEBARYCENTRIC>(*fi,  ps, ti[0],ti[1],ti[2]);
		}
}*/

#endif
