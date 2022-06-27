#include "glarea.h"

inline MultiViewer_Container* GLArea::mvc()
{
	return parentmultiview;
}

inline MeshDocument* GLArea::md()
{
	if (mvc() == nullptr)
		return nullptr;
	return &(mvc()->meshDoc);
}

inline MLSceneGLSharedDataContext* GLArea::getSceneGLSharedContext()
{
	return ((mvc() != NULL) ? mvc()->sharedDataContext() : NULL);
}

inline void GLArea::updateSelection(int meshid, bool vertsel, bool facesel)
{
	makeCurrent();
	if (md() != NULL)
	{
		MeshModel* mm = md()->getMesh(meshid);
		if (mm != NULL)
		{
			CMeshO::PerMeshAttributeHandle< MLSelectionBuffers* > selbufhand = vcg::tri::Allocator<CMeshO>::GetPerMeshAttribute<MLSelectionBuffers* >(mm->cm, MLDefaultMeshDecorators::selectionAttName());
			if ((selbufhand() != NULL) && (facesel))
				selbufhand()->updateBuffer(MLSelectionBuffers::ML_PERFACE_SEL);

			if ((selbufhand() != NULL) && (vertsel))
				selbufhand()->updateBuffer(MLSelectionBuffers::ML_PERVERT_SEL);
		}
	}
}

/*WARNING!!!!! HORRIBLE THING!!!!! Added just to avoid to include the multiViewer_container.cpp file in a MeshLab plugins project in case it needs to update all the GLArea and not just the one passed as parameter*/

inline void GLArea::updateAllSiblingsGLAreas()
{
	if (mvc() == NULL)
		return;
	foreach(GLArea* viewer, mvc()->viewerList)
	{
		if (viewer != NULL)
			viewer->update();
	}
}

template <typename... Ts>
void GLArea::Logf(int Level, const char * f, Ts&&... ts)
{
	makeCurrent();
	if( this->md() != nullptr){
		this->md()->Log.logf(Level, f, std::forward<Ts>(ts)...);
	}
}

/*
Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
to apply "refCamera" o "track" (via GPU).
*/
template <class T>
vcg::Shot<T> GLArea::track2ShotGPU(vcg::Shot<T> &refCamera, vcg::Trackball *track){
	vcg::Shot<T> view;

	double _near, _far;
	_near = 0.1;
	_far = 100;

		 //get OpenGL modelview matrix after applying the trackball
	GlShot<vcg::Shot<T> >::SetView(refCamera, _near, _far);
	glPushMatrix();
	track->GetView();
	track->Apply();
	vcg::Matrix44d model;
	glGetv(GL_MODELVIEW_MATRIX, model);
	glPopMatrix();
	GlShot<vcg::Shot<T> >::UnsetView();

		 //get translation out of modelview
	vcg::Point3d tra;
	tra[0] = model[0][3]; tra[1] = model[1][3]; tra[2] = model[2][3];
	model[0][3] = model[1][3] = model[2][3] = 0;

		 //get pure rotation out of modelview
	double det = model.Determinant();
	double idet = 1/pow(det, 1/3.0); //inverse of the determinant
	model *= idet;
	model[3][3] = 1;
	view.Extrinsics.SetRot(model);

		 //get pure translation out of modelview
	vcg::Matrix44d imodel = model;
	vcg::Transpose(imodel);
	tra = -(imodel*tra);
	tra *= idet;
	view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

		 //use same current intrinsics
	view.Intrinsics = refCamera.Intrinsics;

	return view;
}

/*
Given a shot "refCamera" and a trackball "track", computes a new shot which is equivalent
to apply "refCamera" o "track" (via CPU).
*/
template <class T>
vcg::Shot<T> GLArea::track2ShotCPU(vcg::Shot<T> &refCamera, vcg::Trackball *track){
	vcg::Shot<T> view;

		 //double _near, _far;
		 //_near = 0.1;
		 //_far = 100;

		 //get shot extrinsics matrix
	vcg::Matrix44<T> shotExtr;
	refCamera.GetWorldToExtrinsicsMatrix().ToMatrix(shotExtr);

	vcg::Matrix44<T> model2;
	model2 = (shotExtr)* vcg::Matrix44<T>::Construct(track->Matrix());
	vcg::Matrix44<T> model;
	model2.ToMatrix(model);

		 //get translation out of modelview
	vcg::Point3<T> tra;
	tra[0] = model[0][3]; tra[1] = model[1][3]; tra[2] = model[2][3];
	model[0][3] = model[1][3] = model[2][3] = 0;

		 //get pure rotation out of modelview
	double det = model.Determinant();
	double idet = 1/pow(det, 1/3.0); //inverse of the determinant
	model *= idet;
	model[3][3] = 1;
	view.Extrinsics.SetRot(model);

		 //get pure translation out of modelview
	vcg::Matrix44<T> imodel = model;
	vcg::Transpose(imodel);
	tra = -(imodel*tra);
	tra *= idet;
	view.Extrinsics.SetTra(vcg::Point3<T>::Construct(tra));

		 //use same current intrinsics
	view.Intrinsics = refCamera.Intrinsics;

	return view;
}

/*
Given a shot "from" and a trackball "track", updates "track" with "from" extrinsics.
A translation involving cameraDistance is included. This is necessary to compensate
a transformation that OpenGL performs at the end of the graphic pipeline.
*/
template <class T>
void GLArea::shot2Track(const vcg::Shot<T> &from, const float cameraDist, vcg::Trackball &tb){

	vcg::Quaternion<T> qfrom; qfrom.FromMatrix(from.Extrinsics.Rot());

	tb.track.rot = vcg::Quaternionf::Construct(qfrom);
	tb.track.tra =  (vcg::Point3f::Construct(-from.Extrinsics.Tra()));
	tb.track.tra += vcg::Point3f::Construct(tb.track.rot.Inverse().Rotate(vcg::Point3f(0,0,cameraDist)))*(1/tb.track.sca);
}
