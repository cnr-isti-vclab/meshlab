#ifndef __VCG_TRIVIAL_WALKER
#define __VCG_TRIVIAL_WALKER

namespace vcg {
  namespace tri {


// La classe Walker implementa la politica di visita del volume; conoscendo l'ordine di visita del volume
// è conveniente che il Walker stesso si faccia carico del caching dei dati utilizzati durante l'esecuzione 
// degli algoritmi MarchingCubes ed ExtendedMarchingCubes, in particolare il calcolo del volume ai vertici
// delle celle e delle intersezioni della superficie con le celle. In questo esempio il volume da processare
// viene suddiviso in fette; in questo modo se il volume ha dimensione h*l*w (rispettivamente altezza,
// larghezza e profondità), lo spazio richiesto per il caching dei vertici già allocati passa da O(h*l*w)
// a O(h*l). 

template <class MeshType, class VolumeType>
class TrivialWalker
{
private:
	typedef int VertexIndex;
  typedef typename MeshType::ScalarType ScalarType;
  typedef typename MeshType::VertexPointer VertexPointer;
	public:

  // bbox is the portion of the volume to be computed
  // resolution determine the sampling step:
  // should be a divisor of bbox size (e.g. if bbox size is 256^3 resolution could be 128,64, etc)


  void Init(const Box3i &bbox, const vcg::Point3i &resolution)
	{
		_bbox				= bbox;
		_resolution = resolution;
		_cell_size.X() = _bbox.DimX()/_resolution.X();
		_cell_size.Y() = _bbox.DimY()/_resolution.Y();
		_cell_size.Z() = _bbox.DimZ()/_resolution.Z();
		_slice_dimension = resolution.X()*resolution.Z();

		_x_cs = new VertexIndex[ _slice_dimension ];
		_y_cs = new VertexIndex[ _slice_dimension ];
		_z_cs = new VertexIndex[ _slice_dimension ];
		_x_ns = new VertexIndex[ _slice_dimension ];
		_z_ns = new VertexIndex[ _slice_dimension ];
		_v_cs = new float[_slice_dimension];
		_v_ns = new float[_slice_dimension];

	};

	~TrivialWalker()
	{}

  template<class EXTRACTOR_TYPE>
	void BuildMesh(MeshType &mesh, VolumeType &volume, EXTRACTOR_TYPE &extractor)
  {
    Box3i tmp(Point3i(0,0,0),volume.ISize());
    BuildMesh(mesh,volume,extractor,tmp,volume.ISize());
  }
	
	template<class EXTRACTOR_TYPE>
	void BuildMesh(MeshType &mesh, VolumeType &volume, EXTRACTOR_TYPE &extractor,const Box3i &bbox, const vcg::Point3i &resolution )
	{
    Init(bbox,resolution);
		_volume = &volume;
		_mesh		= &mesh;
		_mesh->Clear();
		vcg::Point3i p1, p2;

		Begin();
		extractor.Initialize();
		for (int j=_bbox.min.Y(); j<(_bbox.max.Y()-_cell_size.Y())-1; j+=_cell_size.Y())
		{		 if((j%10)==0) 	printf("Marching volume z %i (%i ..%i)\r",j,_bbox.min.Y(),_bbox.max.Y());

			for (int i=_bbox.min.X(); i<(_bbox.max.X()-_cell_size.X())-1; i+=_cell_size.X())
			{
				for (int k=_bbox.min.Z(); k<(_bbox.max.Z()-_cell_size.Z())-1; k+=_cell_size.Z())
				{
					p1.X()=i;									p1.Y()=j;									p1.Z()=k;
					p2.X()=i+_cell_size.X();	p2.Y()=j+_cell_size.Y();	p2.Z()=k+_cell_size.Z();
          if(ValidCell(p1, p2))	
          extractor.ProcessCell(p1, p2);
				}
			}
			NextSlice();
		}
		extractor.Finalize();
		_volume = NULL;
		_mesh		= NULL;
	};

  bool ValidCell(const Point3i &p1, const Point3i &p2)
  {
    if(V(p1.X(),p1.Y(),p1.Z()) == 1000 ) return false;
    if(V(p2.X(),p1.Y(),p1.Z()) == 1000 ) return false;
    if(V(p1.X(),p2.Y(),p1.Z()) == 1000 ) return false;
    if(V(p2.X(),p2.Y(),p1.Z()) == 1000 ) return false;
    if(V(p1.X(),p1.Y(),p2.Z()) == 1000 ) return false;
    if(V(p2.X(),p1.Y(),p2.Z()) == 1000 ) return false;
    if(V(p1.X(),p2.Y(),p2.Z()) == 1000 ) return false;
    if(V(p2.X(),p2.Y(),p2.Z()) == 1000 ) return false;
    return true;
  }

	float V(int pi, int pj, int pk)
	{
		int i = pi - _bbox.min.X();
		int k = pk - _bbox.min.Z();
		return (pj==_current_slice) ? _v_cs[i+k*_resolution.X()] : _v_ns[i+k*_resolution.X()];
	}

	// this function seems to be called only by the ComputeCVertex function of the Marching cube.h
	// and in very rare situations (on a random mesh it happens  1:10^6 intersected cells)
	bool Exist(const vcg::Point3i &p0, const vcg::Point3i &p1, VertexPointer &v)
	{ 
		// orginal version of Fiorin
		// int pos = p0.X()+p0.Z()*_resolution.X();
		
    // new version, with correct offsetting 
		int pos = p0.X()-_bbox.min.X()+(p0.Z()-_bbox.min.Z())*_resolution.X();
		assert(pos<_slice_dimension);
		
		int vidx;

		if (p0.X()!=p1.X()) // punti allineati lungo l'asse X
			vidx = (p0.Y()==_current_slice) ? _x_cs[pos] : _x_ns[pos];
		else if (p0.Y()!=p1.Y()) // punti allineati lungo l'asse Y
			vidx = _y_cs[pos];
		else if (p0.Z()!=p1.Z()) // punti allineati lungo l'asse Z
			vidx = (p0.Y()==_current_slice)? _z_cs[pos] : _z_ns[pos];
		else
			assert(false);

		v = (vidx!=-1)? &_mesh->vert[vidx] : NULL;
		return v!=NULL;
	}

	void GetXIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v) 
	{ 
		int i = p1.X() - _bbox.min.X();
		int z = p1.Z() - _bbox.min.Z();
		VertexIndex index = i+z*_resolution.X();
		VertexIndex pos;
		if (p1.Y()==_current_slice)
		{
			if ((pos=_x_cs[index])==-1)
			{
				_x_cs[index] = (VertexIndex) _mesh->vert.size();
				pos = _x_cs[index];
        Allocator<MeshType>::AddVertices( *_mesh, 1 );
				v = &_mesh->vert[pos];
				_volume->GetXIntercept/*<VertexPointer>*/(p1, p2, v);
				return;
			}
		}
		if (p1.Y()==_current_slice+_cell_size.Y())
		{
			if ((pos=_x_ns[index])==-1)
			{
				_x_ns[index] = (VertexIndex) _mesh->vert.size();
				pos = _x_ns[index];
				Allocator<MeshType>::AddVertices( *_mesh, 1 );
				v = &_mesh->vert[pos];
				_volume->GetXIntercept(p1, p2, v);
				return;
			}
		}
		v = &_mesh->vert[pos];
	}
	void GetYIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v) 
	{
		int i = p1.X() - _bbox.min.X();
		int z = p1.Z() - _bbox.min.Z();
		VertexIndex index = i+z*_resolution.X();
		VertexIndex pos;
		if ((pos=_y_cs[index])==-1)
		{
			_y_cs[index] = (VertexIndex) _mesh->vert.size();
			pos = _y_cs[index];
			Allocator<MeshType>::AddVertices( *_mesh, 1);
			v = &_mesh->vert[ pos ];
			_volume->GetYIntercept(p1, p2, v);
		}
		v = &_mesh->vert[pos];
	}
	void GetZIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v) 
	{
		int i = p1.X() - _bbox.min.X();
		int z = p1.Z() - _bbox.min.Z();
		VertexIndex index = i+z*_resolution.X();
		VertexIndex pos;
		if (p1.Y()==_current_slice)
		{
			if ((pos=_z_cs[index])==-1)
			{
				_z_cs[index] = (VertexIndex) _mesh->vert.size();
				pos = _z_cs[index];
				Allocator<MeshType>::AddVertices( *_mesh, 1 );
				v = &_mesh->vert[pos];
				_volume->GetZIntercept(p1, p2, v);
				return;
			}
		}
		if (p1.Y()==_current_slice+_cell_size.Y())
		{
			if ((pos=_z_ns[index])==-1)
			{
				_z_ns[index] = (VertexIndex) _mesh->vert.size();
				pos = _z_ns[index];
				Allocator<MeshType>::AddVertices( *_mesh, 1 );
				v = &_mesh->vert[pos];
				_volume->GetZIntercept(p1, p2, v);
				return;
			}
		}
		v = &_mesh->vert[pos];
	}

protected:
	Box3i		_bbox;
	vcg::Point3i	_resolution;
	vcg::Point3i	_cell_size;

	int _slice_dimension;
	int	_current_slice;
  
	float *_v_cs; // il valore del campo campionato nella fetta di volumecorrente 
	float *_v_ns; // il valore del campo campionato nella prossima fetta di volume
	
	VertexIndex *_x_cs; // indici dell'intersezioni della superficie lungo gli Xedge della fetta corrente
	VertexIndex	*_y_cs; // indici dell'intersezioni della superficie lungo gli Yedge della fetta corrente
	VertexIndex *_z_cs; // indici dell'intersezioni della superficie lungo gli Zedge della fetta corrente
	VertexIndex *_x_ns; // indici dell'intersezioni della superficie lungo gli Xedge della prossima fetta 
	VertexIndex *_z_ns; // indici dell'intersezioni della superficie lungo gli Zedge della prossima fetta 

	MeshType		*_mesh;
	VolumeType	*_volume;

	void NextSlice() 
	{
		memset(_x_cs, -1, _slice_dimension*sizeof(VertexIndex));
		memset(_y_cs,	-1, _slice_dimension*sizeof(VertexIndex));
		memset(_z_cs, -1, _slice_dimension*sizeof(VertexIndex));

		std::swap(_x_cs, _x_ns);
		std::swap(_z_cs, _z_ns);		
		std::swap(_v_cs, _v_ns);
		
		_current_slice += _cell_size.Y();
		int j						= _current_slice + _cell_size.Y();
		int k_idx, i_idx, index;
		for (int i=_bbox.min.X(); i<_bbox.max.X(); i+=_cell_size.X())
		{
			i_idx = i-_bbox.min.X();
			for (int k=_bbox.min.Z(); k<_bbox.max.Z(); k+=_cell_size.Z())
			{
				k_idx = k-_bbox.min.Z();
				index = i_idx+k_idx*_resolution.X();
				_v_ns[ index ] = _volume->Val(i, j, k);
			}
		}
	}

	void Begin()
	{
		_current_slice = _bbox.min.Y();

		memset(_x_cs, -1, _slice_dimension*sizeof(VertexIndex));
		memset(_y_cs, -1, _slice_dimension*sizeof(VertexIndex));
		memset(_z_cs, -1, _slice_dimension*sizeof(VertexIndex));
		memset(_x_ns, -1, _slice_dimension*sizeof(VertexIndex));
		memset(_z_ns, -1, _slice_dimension*sizeof(VertexIndex));
		
		int index;
		int j = _current_slice;
		int i_idx, k_idx;
		for (int i=_bbox.min.X(); i<_bbox.max.X(); i+=_cell_size.X())
		{
			i_idx = i-_bbox.min.X();
			for (int k=_bbox.min.Z(); k<_bbox.max.Z(); k+=_cell_size.Z())
			{
				k_idx = k-_bbox.min.Z();
				index = i_idx+k_idx*_resolution.X();
				_v_cs[index] = _volume->Val(i, j, k);
				_v_ns[index] = _volume->Val(i, j+_cell_size.Y(), k);
			}
		}
	}
};
} // end namespace 
} // end namespace 
#endif // __VCGTEST_WALKER
