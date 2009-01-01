#ifndef SLICE_KD_TREE
#define SLICE_KD_TREE
#include <vcg/space/Point3.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include "filter_slice_functors.h"
#include "filter_slice.h"

template<typename MeshType>
class KDTree
{
  protected:
		KDTree<MeshType> *leftChild, *rightChild;
		MeshType *slice, *mesh;
		Point3f planeAxis;
		MeshDocument *m;
		int axisIndex;
		float eps;
  public:
		static enum{YZ,XZ,XY};
		KDTree(MeshDocument *_m, MeshType* _mesh, float _eps, int _axisIndex=YZ)
		{
			m=_m;
			eps=_eps;
			planeAxis=Point3f(0,0,0);
			leftChild=0;
			rightChild=0;
			slice=0;
			mesh=_mesh;
			axisIndex=_axisIndex;
			assert(axisIndex>=0 && axisIndex<3);			
			planeAxis[axisIndex]=1;
		}
		~KDTree(){}
		KDTree<MeshType>* L(){return leftChild;}
		KDTree<MeshType>* R(){return rightChild;}
		MeshType* S(){return slice;}
		MeshType* M(){return mesh;}
		void Slice(Plane3f p=Plane3f(),vcg::CallBackPos *cb=0)
		{		
			assert (tri::Clean<CMeshO>::IsTwoManifoldFace(*mesh));
			assert (tri::Clean<CMeshO>::CountNonManifoldVertexFF(*mesh,false) == 0);
			if (!tri::Clean<CMeshO>::IsTwoManifoldFace(*mesh) || tri::Clean<CMeshO>::CountNonManifoldVertexFF(*mesh,false) != 0)
				return;
			if (rightChild!=0 && leftChild!=0)
			{
				//find a plane
				leftChild->Slice(p);
				rightChild->Slice(p);
				return;
			}
			bool oriented;
	    bool orientable;
	    tri::Clean<CMeshO>::IsOrientedMesh(*mesh, oriented,orientable);
			//actual slicing
			Plane3f slicingPlane;
			vcg::tri::UpdateBounding<CMeshO>::Box(*mesh);
			slicingPlane.Init(mesh->bbox.Center(),planeAxis);
			
			vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(*mesh);

			SlicedEdge<CMeshO> slicededge(slicingPlane);
      SlicingFunction<CMeshO> slicingfunc(slicingPlane);
      //after the RefineE call, the mesh will be half vertices selected
      vcg::RefineE<CMeshO, SlicingFunction<CMeshO>, SlicedEdge<CMeshO> >
           (*mesh, slicingfunc, slicededge, false, cb);
      vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(*mesh);
				
				MeshModel *slice1= new MeshModel();
				m->meshList.push_back(slice1);
				QString layername;
				//layername.sprintf("slice_%d-%d.ply",i,i+1);
				slice1->fileName = "left.ply";								// mesh name
				slice1->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(*mesh,VERTEX_LEFT,VERTEX_LEFT);
        vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(*mesh);
				//if (hideSlices)
					//slice1->visible=false;
        //createSlice(*mesh,slice1);
				tri::Append<CMeshO,CMeshO>::Mesh(slice1->cm, *mesh, true);
				tri::UpdateTopology<CMeshO>::FaceFace(slice1->cm);
				tri::UpdateBounding<CMeshO>::Box(slice1->cm);						// updates bounding box
				slice1->cm.Tr = (*mesh).Tr;								// copy transformation

				vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(slice1->cm);

				MeshModel* cap= new MeshModel();
				m->meshList.push_back(cap);
				//layername.sprintf("plane_%d.ply",i+1);
				//cap->fileName = layername.toStdString().c_str();								// mesh name
				cap->fileName="slice.ply";
				cap->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				ExtraFilter_SlicePlugin::capHole(slice1,cap);

				if (eps!=0)
				{
					MeshModel* dup= new MeshModel();
					m->meshList.push_back(dup);
					//layername.sprintf("plane_%d_extruded.ply",i+1);
					dup->fileName = "extr";								// mesh name
					dup->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
					ExtraFilter_SlicePlugin::extrude(cap, dup, eps, planeAxis);
				}

				tri::Append<CMeshO,CMeshO>::Mesh(slice1->cm, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(slice1->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice1->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(slice1->cm);

				MeshModel* slice2= new MeshModel();
				m->meshList.push_back(slice2);
				slice2->fileName = "right.ply";								// mesh name
				slice2->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
				vcg::tri::UpdateSelection<CMeshO>::VertexFromQualityRange(*mesh,VERTEX_RIGHT,VERTEX_RIGHT);
				vcg::tri::UpdateSelection<CMeshO>::FaceFromVertexLoose(*mesh);
				//createSlice(mesh,slice2);
				tri::Append<CMeshO,CMeshO>::Mesh(slice2->cm, *mesh, true);	
				tri::UpdateTopology<CMeshO>::FaceFace(slice2->cm);
				tri::UpdateBounding<CMeshO>::Box(slice2->cm);						// updates bounding box
				slice2->cm.Tr = (*mesh).Tr;								// copy transformation
				
				vcg::tri::UpdateFlags<CMeshO>::FaceBorderFromNone(slice2->cm);
				tri::Append<CMeshO,CMeshO>::Mesh(slice2->cm, cap->cm);
				tri::Clean<CMeshO>::RemoveDuplicateVertex(slice2->cm);
				vcg::tri::UpdateTopology<CMeshO>::FaceFace(slice2->cm);
				vcg::tri::UpdateNormals<CMeshO>::PerVertexPerFace(slice2->cm);

			

				leftChild=new KDTree<MeshType>(m,&slice1->cm,eps,(axisIndex+1)%3);
				rightChild=new KDTree<MeshType>(m,&slice2->cm,eps,(axisIndex+1)%3);
				slice1->visible=false;
				slice2->visible=false;
				cap->visible=false;
		}
};

#endif