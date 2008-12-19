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

#ifndef MLSMARCHINGCUBES_H
#define MLSMARCHINGCUBES_H

#include <vcg/space/point3.h>
#include <vcg/space/box3.h>
#include <map>
#include "mlssurface.h"
#include "mlsutils.h"

namespace vcg {
namespace tri {

template <class MeshType, class SurfaceType>
class MlsWalker
{
private:
	typedef int VertexIndex;
	typedef typename MeshType::ScalarType ScalarType;
	typedef vcg::Point3f VectorType;
	typedef typename MeshType::VertexPointer VertexPointer;
	//typedef GaelMls::MlsSurface<MeshType> SurfaceType;
	typedef long long unsigned int Key;
	typedef std::map<Key,VertexIndex> MapType;

	struct GridElement
	{
		VectorType position;
		ScalarType value;
	};

	template <typename T>
	inline bool IsFinite(T value)
	{
		return (value>=-std::numeric_limits<T>::max()) && (value<=std::numeric_limits<T>::max());
	}

public:

	int resolution;

	MlsWalker()
	{
		resolution = 150;
		mMaxBlockSize = 64;
		mIsoValue = 0;
	}

	template<class EXTRACTOR_TYPE>
	void BuildMesh(MeshType &mesh, SurfaceType &surface, EXTRACTOR_TYPE &extractor, CallBackPos *cb = 0)
	{
		mpSurface = &surface;
		const ScalarType invalidValue = SurfaceType::InvalidValue();

		// precomputed offsets to access the cell corners
		const int offsets[8] = {
				0,
				1,
				1+mMaxBlockSize*mMaxBlockSize,
				mMaxBlockSize*mMaxBlockSize,
				mMaxBlockSize,
				1+mMaxBlockSize,
				1+mMaxBlockSize+mMaxBlockSize*mMaxBlockSize,
				mMaxBlockSize+mMaxBlockSize*mMaxBlockSize
		};

		mAABB = mpSurface->boundingBox();
		VectorType diag = mAABB.max - mAABB.min;
		mAABB.min -= diag * 0.1f;
		mAABB.max += diag * 0.1f;
		diag = mAABB.max - mAABB.min;

		if (   (diag[0]<=0.)
				|| (diag[1]<=0.)
				|| (diag[2]<=0.)
				|| (resolution==0))
		{
			return;
		}

		mCache = new GridElement[(mMaxBlockSize)*(mMaxBlockSize)*(mMaxBlockSize)];
		ScalarType step = vcg::MaxCoeff(diag)/ScalarType(resolution);

		unsigned int nofCells[3];
		unsigned int nofBlocks[3];

		for (uint k=0 ; k<3 ; ++k)
		{
			nofCells[k] = int(diag[k]/step)+2;
			nofBlocks[k] = nofCells[k]/mMaxBlockSize + ( (nofCells[k]%mMaxBlockSize)==0 ? 0 : 1);
		}

		_mesh = &mesh;
		_mesh->Clear();

		int countSubSlice = 0;
		int totalSubSlices = nofBlocks[2] * nofBlocks[1] * nofCells[0];

		extractor.Initialize();
		// for each macro block
		vcg::Point3i bi; // block id
		for (bi[2]=0 ; bi[2]<nofBlocks[2] ; ++bi[2])
		for (bi[1]=0 ; bi[1]<nofBlocks[1] ; ++bi[1])
		for (bi[0]=0 ; bi[0]<nofBlocks[0] ; ++bi[0])
		{
			mBlockOrigin = bi * (mMaxBlockSize-1);

			// compute the size of the local grid
			for (uint k=0 ; k<3 ; ++k)
			{
				mGridSize[k] = std::min<int>(mMaxBlockSize, nofCells[k]-(mMaxBlockSize-1)*bi[k]);
			}
			VectorType origin = mAABB.min + VectorType(bi[0],bi[1],bi[2]) * (step * (mMaxBlockSize-1));

			// fill the grid
			vcg::Point3i ci; // local cell id

			// for each corners...
			for (ci[0]=0 ; ci[0]<mGridSize[0] ; ++ci[0])
			{
				if (cb)
					cb((100*(++countSubSlice))/totalSubSlices, "Marching cube...");
				for (ci[1]=0 ; ci[1]<mGridSize[1] ; ++ci[1])
				for (ci[2]=0 ; ci[2]<mGridSize[2] ; ++ci[2])
				{
					GridElement& el = mCache[(ci[2]*mMaxBlockSize + ci[1])*mMaxBlockSize + ci[0]];
					el.position = origin + VectorType(ci[0],ci[1],ci[2]) * step;
					el.value = mpSurface->potential(el.position);
					if (!mpSurface->isInDomain(el.position))
						el.value = invalidValue;
				}
			}

			// polygonize the grid (marching cube)
			// for each cell...
			for (ci[0]=0 ; ci[0]<mGridSize[0]-1 ; ++ci[0])
			for (ci[1]=0 ; ci[1]<mGridSize[1]-1 ; ++ci[1])
			for (ci[2]=0 ; ci[2]<mGridSize[2]-1 ; ++ci[2])
			{
				uint cellId = ci[0]+mMaxBlockSize*(ci[1]+mMaxBlockSize*ci[2]);
				// check if one corner is outside the surface definition domain
				bool out =false;
				for (int k=0; k<8 && (!out); ++k)
					out = out || (!IsFinite(mCache[cellId+offsets[k]].value))
										|| mCache[cellId+offsets[k]].value==invalidValue;

				if (!out)
				{
					extractor.ProcessCell(ci+mBlockOrigin, ci+mBlockOrigin+vcg::Point3i(1,1,1));
				}
			}
		}
		extractor.Finalize();
		_mesh   = NULL;
		delete[] mCache;
	};

	int GetLocalCellId(const vcg::Point3i& p)
	{
		return p[0] + (p[1] + p[2]*mMaxBlockSize)*mMaxBlockSize;
	}

	int GetLocalCellIdFromGlobal(vcg::Point3i p)
	{
		p -= mBlockOrigin;
		return GetLocalCellId(p);
	}

	float V(int pi, int pj, int pk)
	{
		return mCache[GetLocalCellIdFromGlobal(vcg::Point3i(pi, pj, pk))].value;
	}

	int GetGlobalCellId(const vcg::Point3i &p)
	{
		return p[0] + p[1] * resolution + p[2] * resolution * resolution;
	}

	void GetIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v, bool create)
	{
		int id1 = GetGlobalCellId(p1);
		int id2 = GetGlobalCellId(p2);
		if (id1>id2)
			std::swap(id1,id2);
		Key k = Key(id1) + (Key(id2)<<32);
		MapType::iterator it = mVertexMap.find(k);
		if (it!=mVertexMap.end())
		{
			// a vertex already exist
			v = &_mesh->vert[it->second];
		}
		else if (create)
		{
			// let's create a new vertex
			VertexIndex vi = (VertexIndex) _mesh->vert.size();
			Allocator<MeshType>::AddVertices( *_mesh, 1 );
			mVertexMap[k] = vi;
			v = &_mesh->vert[vi];
			id1 = GetLocalCellIdFromGlobal(p1);
			id2 = GetLocalCellIdFromGlobal(p2);
			// interpol along the edge
			ScalarType epsilon = ScalarType(1e-5);
			const GridElement& c1 = mCache[id1];
			const GridElement& c2 = mCache[id2];
			if (fabs(mIsoValue-c1.value) < epsilon)
				v->P() = c1.position;
			else if (fabs(mIsoValue-c2.value) < epsilon)
				v->P() = c2.position;
			else if (fabs(c1.value-c2.value) < epsilon)
				v->P() = (c1.position+c1.position)*0.5;
			else
			{
				ScalarType a = (mIsoValue - c1.value) / (c2.value - c1.value);
				v->P() = c1.position + (c2.position - c1.position) * a;
			}
		}
		else
		{
			v = 0;
		}
	}

	bool Exist(const vcg::Point3i &p0, const vcg::Point3i &p1, VertexPointer &v)
	{
		GetIntercept(p0, p1, v, false);
		return v!=0;
	}

	void GetXIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v)
	{
		GetIntercept(p1, p2, v, true);
	}
	void GetYIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v)
	{
		GetIntercept(p1, p2, v, true);
	}
	void GetZIntercept(const vcg::Point3i &p1, const vcg::Point3i &p2, VertexPointer &v)
	{
		GetIntercept(p1, p2, v, true);
	}

protected:
	Box3f mAABB;
	MapType mVertexMap;
	MeshType    *_mesh;
	SurfaceType* mpSurface;
	GridElement* mCache;
	vcg::Point3i mBlockOrigin;
	vcg::Point3i mGridSize;
	int mMaxBlockSize;
	ScalarType mIsoValue;

};
} // end namespace
} // end namespace

#endif

