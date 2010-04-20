/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
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

#ifndef SLICE_KD_TREE
#define SLICE_KD_TREE
#include <vcg/space/point3.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include "filter_slice_functors.h"
#include "filter_slice.h"

template<typename MeshType>
class KDTree
{
  protected:
		KDTree<MeshType> *leftChild, *rightChild;
		MeshType *slice;
		MeshModel* mm;
		Point3f planeAxis;
		MeshDocument *m;
		int axisIndex;
		float eps;
		QString name;
  public:
		bool delfather;
		enum{X,Y,Z};
		KDTree(MeshDocument *_m, MeshModel* _mm, float _eps, int _axisIndex=X)
		{
			name="";
			m=_m;
			mm=_mm;
			eps=_eps;
			planeAxis=Point3f(0,0,0);
			leftChild=0;
			rightChild=0;
			slice=0;
			axisIndex=_axisIndex;
			assert(axisIndex>=0 && axisIndex<3);			
			planeAxis[axisIndex]=1;
			delfather=false;
		}
		~KDTree(){}
		KDTree<MeshType>* L(){return leftChild;}
		KDTree<MeshType>* R(){return rightChild;}
		MeshType* S(){return slice;}
		void Slice(vcg::CallBackPos *cb)
		{
			Plane3f slicingPlane;
			vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);
			slicingPlane.Init(mm->cm.bbox.Center(),planeAxis);
			actual_slice(slicingPlane,cb);
		}

		void actual_slice(Plane3f slicingPlane,vcg::CallBackPos *cb=0)
		{	
			if (mm->cm.vn<=0)
				return;
			mm->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
      if ( tri::Clean<CMeshO>::CountNonManifoldEdgeFF(mm->cm)>0 || tri::Clean<CMeshO>::CountNonManifoldVertexFF(mm->cm,false) != 0)
				return;

			if (rightChild!=0 && leftChild!=0)
			{
				//find a plane
				Plane3f slicingPlane;
				vcg::Box3f leftbox=leftChild->mm->cm.bbox;
				vcg::Box3f rightbox=rightChild->mm->cm.bbox;
				leftbox.Translate(planeAxis*((leftbox.Dim()*.2)*planeAxis));
				rightbox.Translate(planeAxis*(-1)*((rightbox.Dim()*.2)*planeAxis));
				leftbox.Intersect(rightbox);
				slicingPlane.Init(leftbox.Center(),leftChild->planeAxis);

				leftChild->actual_slice(slicingPlane,cb);
				rightChild->actual_slice(slicingPlane,cb);
				return;
			}
			//bool oriented;
	    //bool orientable;
			//tolto per controllare le normali
	    //tri::Clean<CMeshO>::IsOrientedMesh(mm->cm, oriented,orientable);
			//actual slicing
			

			//vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(mm->cm);
			
			SlicedEdge<CMeshO> slicededge(slicingPlane);
      SlicingFunction<CMeshO> slicingfunc(slicingPlane);
      //after the RefineE call, the mesh will be half vertices selected
      vcg::RefineE<CMeshO, SlicingFunction<CMeshO>, SlicedEdge<CMeshO> >
           (mm->cm, slicingfunc, slicededge, false, cb);
			//vcg::tri::UpdateTopology<MeshType>::FaceFace(mm->cm);
      vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(mm->cm);
			//vcg::tri::Clean<CMeshO>::RemoveDuplicateVertex(mm->cm);
			vcg::tri::UpdateTopology<CMeshO>::FaceFace(mm->cm);
			
			

			
				
				MeshModel *slice1= new MeshModel();
				m->meshList.push_back(slice1);
				QString layername;
				layername=name+"L.ply";
				slice1->setFileName(layername);								// mesh name
				slice1->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(mm->cm,VERTEX_LEFT,VERTEX_LEFT);
        vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(mm->cm);
				//if (hideSlices)
					//slice1->visible=false;
        //createSlice(mm->cm,slice1);
				tri::Append<CMeshO,CMeshO>::Mesh(slice1->cm, mm->cm, true);
				tri::UpdateTopology<CMeshO>::FaceFace(slice1->cm);
				tri::UpdateBounding<CMeshO>::Box(slice1->cm);						// updates bounding box
				slice1->cm.Tr = (mm->cm).Tr;								// copy transformation
				vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(slice1->cm);
				MeshModel* cap= new MeshModel();
				m->meshList.push_back(cap);
				layername=name+"_slice.ply";
				cap->setFileName(layername);			
				cap->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				ExtraFilter_SlicePlugin::capHole(slice1,cap);

				if (eps!=0)
				{
					MeshModel* dup= new MeshModel();
					m->meshList.push_back(dup);
					layername=name+"_extr.ply";
					dup->setFileName(layername);											// mesh name
					dup->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
					ExtraFilter_SlicePlugin::extrude(cap, dup, eps, planeAxis);
				}

				tri::Append<CMeshO,CMeshO>::Mesh(slice1->cm, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(slice1->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice1->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(slice1->cm);
				
				MeshModel* slice2= new MeshModel();
				m->meshList.push_back(slice2);
				layername=name+"R.ply";
				slice2->setFileName(layername);											// mesh name
				slice2->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(mm->cm,VERTEX_RIGHT,VERTEX_RIGHT);
				vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(mm->cm);
				//createSlice(mesh,slice2);
				tri::Append<CMeshO,CMeshO>::Mesh(slice2->cm, mm->cm, true);	
				tri::UpdateTopology<CMeshO>::FaceFace(slice2->cm);
				tri::UpdateBounding<CMeshO>::Box(slice2->cm);						// updates bounding box
				slice2->cm.Tr = (mm->cm).Tr;								// copy transformation
				tri::Clean<CMeshO>::FlipMesh(cap->cm);
				vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(slice2->cm);
				tri::Append<CMeshO,CMeshO>::Mesh(slice2->cm, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(slice2->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice2->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(slice2->cm);

				leftChild=new KDTree<MeshType>(m,slice1,eps,(axisIndex+1)%3);
				leftChild->delfather=delfather;
				layername=name+"L";
				leftChild->name=layername.toStdString().c_str();
				rightChild=new KDTree<MeshType>(m,slice2,eps,(axisIndex+1)%3);
				rightChild->delfather=delfather;
				layername=name+"R";
				rightChild->name=layername.toStdString().c_str();

				slice1->visible=false;				
				slice1->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice1->cm);
				vcg::tri::UpdateBounding<CMeshO>::Box(slice1->cm);
				
				slice2->visible=false;
				slice2->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice2->cm);
				vcg::tri::UpdateBounding<CMeshO>::Box(slice2->cm);
				
				cap->visible=false;
				//if (delfather)
					//m->delMesh(mm);
					
		}

};

#endif
