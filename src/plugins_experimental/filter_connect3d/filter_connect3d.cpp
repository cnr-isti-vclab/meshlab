
#include "reconstruction/precompiled.h"

#include "reconstruction/c3dDataStructures.h"
#include "reconstruction/connect3d.h"

#include "filter_connect3d.h"


//#define RESIZE_TO_UNIT_CUBE
#define PRESERVE_RENDERING

#ifdef PRESERVE_RENDERING
std::vector<vcg::Point3<float> > vertexNormals;
#endif

Connect3dFilterPlugin::Connect3dFilterPlugin()
{
  typeList << FP_CONNECT3D;
 
  FilterIDType tt;
  foreach(tt , types())
	    actionList << new QAction(filterName(tt), this);
}

Connect3dFilterPlugin::~Connect3dFilterPlugin()
{
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

QString Connect3dFilterPlugin::filterName(FilterIDType filter) const
{
	switch(filter)
	{
	case FP_CONNECT3D:		return tr("Connect3D Mesh Reconstruction");
	default: assert(0);
	}
	return QString("This shouldn't happen!");
}

QString Connect3dFilterPlugin::filterInfo(FilterIDType filter) const
{
	switch(filter)
	{
	case FP_CONNECT3D:		return tr(Connect3D::info().c_str());
	default: assert(0);
	}
	return QString("This shouldn't happen!");
}

MeshFilterInterface::FilterClass Connect3dFilterPlugin::getClass(QAction *action)
{
	switch(ID(action))
	{
	case FP_CONNECT3D : return FilterClass::PointSet;    
	default			  : return MeshFilterInterface::Generic;
	}
}

void Connect3dFilterPlugin::initParameterSet(QAction *, MeshModel &, RichParameterSet &)
{
	//currently no parameters used
}

void updateVCGDatastructures(c3dModelData &model, CMeshO &cmesh)
{
	assert(cmesh.VN() == model.numVertices);

#ifdef PRESERVE_RENDERING
	vertexNormals.reserve(cmesh.VN());
	for(auto v: cmesh.vert)
		vertexNormals.push_back(v.cN());

	auto backupVertexNormal = vertexNormals.begin();
#endif

	//delete all faces and vertices
	cmesh.Clear();

	//count triangles:
	size_t numTriangles = 0;
	for(std::vector<Triangle>::iterator t = model.dtTriangles.begin(); t != model.dtTriangles.end(); t++)
		if(t->exists())
			++numTriangles;

	

	vcg::tri::Allocator<CMeshO>::AddVertices(cmesh, model.numVertices);
	vcg::tri::Allocator<CMeshO>::AddFaces(cmesh, numTriangles + model.colorTriangles.size());

	Vector3D *vec3Iter = model.vertices.get();

	for(auto vcgVertexIter = cmesh.vert.begin(); vcgVertexIter != cmesh.vert.end(); ++vcgVertexIter, ++vec3Iter)
	{
		vcgVertexIter->P() = CMeshO::CoordType(vec3Iter[0][0], vec3Iter[0][1], vec3Iter[0][2]);
#ifdef PRESERVE_RENDERING
		vcgVertexIter->N() = *backupVertexNormal;
		++backupVertexNormal;
#endif
	}

#ifdef PRESERVE_RENDERING
	vertexNormals.clear();
#endif

//	cmesh.face.EnableColor();

	//add the new faces generated with connect3d
	auto vcgFaceIter = cmesh.face.begin();
	for (std::vector<Triangle>::iterator t = model.dtTriangles.begin(); t != model.dtTriangles.end(); t++)
	{
		if (t->exists())
		{
			int ind0 = t->vertex(0)->info().index;
			int ind1 = t->vertex(1)->info().index;
			int ind2 = t->vertex(2)->info().index;

			vcgFaceIter->V(0) = &cmesh.vert[ind0];
			vcgFaceIter->V(1) = &cmesh.vert[ind1];
			vcgFaceIter->V(2) = &cmesh.vert[ind2];

#ifdef PRESERVE_RENDERING

			Vector3D p0 = model.vertices[ind0];
			Vector3D p1 = model.vertices[ind1];
			Vector3D p2 = model.vertices[ind2];

			Vector3D faceNormal = Vector3D::crossProduct((p1 - p0), (p2 - p0));
			faceNormal.normalize();

			vcgFaceIter->N() = vcg::Point3<float>(faceNormal[0], faceNormal[1], faceNormal[2]);
#endif
//			vcgFaceIter->C().Import(vcg::Color4f(1.0f, 1.0f, 1.0f, 0.2f));
			++vcgFaceIter;
		}
	}

	//Debugging only
//	for(auto t: model.colorTriangles)
//	{
//		int ind0 = t->vertex(0)->info().index;
//		int ind1 = t->vertex(1)->info().index;
//		int ind2 = t->vertex(2)->info().index;
//
//		vcgFaceIter->V(0) = &cmesh.vert[ind0];
//		vcgFaceIter->V(1) = &cmesh.vert[ind1];
//		vcgFaceIter->V(2) = &cmesh.vert[ind2];
//
//#ifdef PRESERVE_RENDERING
//
//		Vector3D p0 = model.vertices[ind0];
//		Vector3D p1 = model.vertices[ind1];
//		Vector3D p2 = model.vertices[ind2];
//
//		Vector3D faceNormal = Vector3D::crossProduct((p1 - p0), (p2 - p0));
//		faceNormal.normalize();
//
//		vcgFaceIter->N() = vcg::Point3<float>(faceNormal[0], faceNormal[1], faceNormal[2]);
//#endif
//
//		vcgFaceIter->C().Import(vcg::Color4f(1.0f, 0.0f, 0.0f, 1.0f));
//		
//
//		++vcgFaceIter;
//	}

}

vcg::CallBackPos * g_cb = nullptr;

bool pCallback(const int percent, const std::string &msg)
{
	if(g_cb != nullptr)
	{
		g_cb(percent, msg.c_str());
		return true;
	}
	assert(false);
	return false;
}


bool Connect3dFilterPlugin::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet &, vcg::CallBackPos * cb)
{
	g_cb = cb;

	//initialize model data and resize to unit cube:
	c3dModelData modelData;

	CMeshO &cmesh = md.mm()->cm;
	modelData.numVertices = cmesh.VertexNumber();
	modelData.vertices.reset(new Vector3D[modelData.numVertices]);

	Vector3D *v = modelData.vertices.get();
	size_t debugNumVert = 0;

	cb(1, "Initializing Datastructures");

	for(auto vP = cmesh.vert.begin(); vP != cmesh.vert.end(); ++vP, ++v, ++debugNumVert)
	{
		assert(vP->IsR() && ! vP->IsD());

		CVertexO cVertex = *vP;
		(*v)[0] = cVertex.cP().X();
		(*v)[1] = cVertex.cP().Y();
		(*v)[2] = cVertex.cP().Z();


#ifdef RESIZE_TO_UNIT_CUBE

	float maxCoord[3] = {-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()}; 
	float minCoord[3] = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};

		for (size_t j = 0; j < 3; j++)
		{
			if ((*v)[j] < minCoord[j])
				minCoord[j] = (*v)[j];

			if ((*v)[j] > maxCoord[j])
				maxCoord[j] = (*v)[j];
		}
#endif
	}

	assert(debugNumVert == modelData.numVertices);

#ifdef RESIZE_TO_UNIT_CUBE
	float maxExtent = 0.0;
	for (size_t j = 0; j < 3; j++)
	{
		float extent = (maxCoord[j] - minCoord[j]);

		if (extent > maxExtent)
			maxExtent = extent;
	}

	for (size_t i = 0; i < modelData.numVertices; ++i)
	{
		for (size_t j = 0; j < 3; j++)
			modelData.vertices[i][j] = static_cast<float>(((double)modelData.vertices[i][j] - minCoord[j])/maxExtent);
	}
#endif

	//start mesh reconstruction:
	cb(2, "starting mesh reconstruction");
	Connect3D::reconstruct(modelData, &pCallback);

	//convert mesh into vcg data structure:
	cb(95, "updating vcg mesh datastructure");
	updateVCGDatastructures(modelData, cmesh);

	return true;
}

int Connect3dFilterPlugin::postCondition(QAction *filter) const
{
	switch(ID(filter))
	{
	case FP_CONNECT3D :	return MeshModel::MM_UNKNOWN;
	default: assert(0);
	}
	return 0;
}

int Connect3dFilterPlugin::getPreCondition(QAction *filter) const
{
	switch(ID(filter))
	{
	case FP_CONNECT3D :	return MeshModel::MM_VERTCOORD;
	default: assert(0);
	}
	return 0;
}


MESHLAB_PLUGIN_NAME_EXPORTER(Connect3dFilterPlugin)
