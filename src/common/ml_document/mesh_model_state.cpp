#include "mesh_model_state.h"

#include "mesh_model.h"

void MeshModelState::create(int _mask, MeshModel* _m)
{
	m=_m;
	changeMask=_mask;
	if(changeMask & MeshModel::MM_VERTCOLOR)
	{
		vertColor.resize(m->cm.vert.size());
		std::vector<vcg::Color4b>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertColor.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*ci)=(*vi).C();
	}
	
	if(changeMask & MeshModel::MM_VERTQUALITY)
	{
		vertQuality.resize(m->cm.vert.size());
		std::vector<float>::iterator qi;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), qi = vertQuality.begin(); vi != m->cm.vert.end(); ++vi, ++qi)
			if(!(*vi).IsD()) (*qi)=(*vi).Q();
	}
	
	if(changeMask & MeshModel::MM_VERTCOORD)
	{
		vertCoord.resize(m->cm.vert.size());
		std::vector<Point3m>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertCoord.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*ci)=(*vi).P();
	}
	
	if(changeMask & MeshModel::MM_VERTNORMAL)
	{
		vertNormal.resize(m->cm.vert.size());
		std::vector<Point3m>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertNormal.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*ci)=(*vi).N();
	}
	
	if(changeMask & MeshModel::MM_FACENORMAL)
	{
		faceNormal.resize(m->cm.face.size());
		std::vector<Point3m>::iterator ci;
		CMeshO::FaceIterator fi;
		for(fi = m->cm.face.begin(), ci = faceNormal.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			if(!(*fi).IsD()) (*ci) = (*fi).N();
	}
	
	if(changeMask & MeshModel::MM_FACECOLOR)
	{
		m->updateDataMask(MeshModel::MM_FACECOLOR);
		faceColor.resize(m->cm.face.size());
		std::vector<vcg::Color4b>::iterator ci;
		CMeshO::FaceIterator fi;
		for(fi = m->cm.face.begin(), ci = faceColor.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			if(!(*fi).IsD()) (*ci) = (*fi).C();
	}
	
	if(changeMask & MeshModel::MM_FACEFLAGSELECT)
	{
		faceSelection.resize(m->cm.face.size());
		std::vector<bool>::iterator ci;
		CMeshO::FaceIterator fi;
		for(fi = m->cm.face.begin(), ci = faceSelection.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			if(!(*fi).IsD()) (*ci) = (*fi).IsS();
	}
	
	if(changeMask & MeshModel::MM_VERTFLAGSELECT)
	{
		vertSelection.resize(m->cm.vert.size());
		std::vector<bool>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertSelection.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*ci) = (*vi).IsS();
	}
	
	if(changeMask & MeshModel::MM_TRANSFMATRIX)
		Tr = m->cm.Tr;
	if(changeMask & MeshModel::MM_CAMERA)
		this->shot = m->cm.shot;
}

bool MeshModelState::apply(MeshModel *_m)
{
	if(_m != m)
		return false;
	if(changeMask & MeshModel::MM_VERTCOLOR)
	{
		if(vertColor.size() != m->cm.vert.size()) return false;
		std::vector<vcg::Color4b>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertColor.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*vi).C()=(*ci);
	}
	if(changeMask & MeshModel::MM_FACECOLOR)
	{
		if(faceColor.size() != m->cm.face.size()) return false;
		std::vector<vcg::Color4b>::iterator ci;
		CMeshO::FaceIterator fi;
		for(fi = m->cm.face.begin(), ci = faceColor.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			if(!(*fi).IsD()) (*fi).C()=(*ci);
	}
	if(changeMask & MeshModel::MM_VERTQUALITY)
	{
		if(vertQuality.size() != m->cm.vert.size()) return false;
		std::vector<float>::iterator qi;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), qi = vertQuality.begin(); vi != m->cm.vert.end(); ++vi, ++qi)
			if(!(*vi).IsD()) (*vi).Q()=(*qi);
	}
	
	if(changeMask & MeshModel::MM_VERTCOORD)
	{
		if(vertCoord.size() != m->cm.vert.size()) 
			return false;
		std::vector<Point3m>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertCoord.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) 
				(*vi).P()=(*ci);
	}
	
	if(changeMask & MeshModel::MM_VERTNORMAL)
	{
		if(vertNormal.size() != m->cm.vert.size()) return false;
		std::vector<Point3m>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci=vertNormal.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
			if(!(*vi).IsD()) (*vi).N()=(*ci);
	}
	
	if(changeMask & MeshModel::MM_FACENORMAL)
	{
		if(faceNormal.size() != m->cm.face.size()) return false;
		std::vector<Point3m>::iterator ci;
		CMeshO::FaceIterator fi;
		for(fi = m->cm.face.begin(), ci=faceNormal.begin(); fi != m->cm.face.end(); ++fi, ++ci)
			if(!(*fi).IsD()) (*fi).N()=(*ci);
	}
	
	if(changeMask & MeshModel::MM_FACEFLAGSELECT)
	{
		if(faceSelection.size() != m->cm.face.size()) return false;
		std::vector<bool>::iterator ci;
		CMeshO::FaceIterator fi;
		for(fi = m->cm.face.begin(), ci = faceSelection.begin(); fi != m->cm.face.end(); ++fi, ++ci)
		{
			if((*ci))
				(*fi).SetS();
			else
				(*fi).ClearS();
		}
	}
	
	if(changeMask & MeshModel::MM_VERTFLAGSELECT)
	{
		if(vertSelection.size() != m->cm.vert.size()) return false;
		std::vector<bool>::iterator ci;
		CMeshO::VertexIterator vi;
		for(vi = m->cm.vert.begin(), ci = vertSelection.begin(); vi != m->cm.vert.end(); ++vi, ++ci)
		{
			if((*ci))
				(*vi).SetS();
			else
				(*vi).ClearS();
		}
	}
	
	
	if(changeMask & MeshModel::MM_TRANSFMATRIX)
		m->cm.Tr=Tr;
	if(changeMask & MeshModel::MM_CAMERA)
		m->cm.shot = this->shot;
	
	return true;
}

int MeshModelState::maskChangedAtts() const
{
	return changeMask;
}
