//***************************************************************************
//
//  Copyright (c) 1999 - 2006 Intel Corporation
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//***************************************************************************
#ifndef VERTEX_DOT_H
#define VERTEX_DOT_H

#include "Primitives.h"
#include "SmallPtrSet.h"
#include "Matrix4x4.h"
#include "IFXVector3.h"

class Pair;

class Vertex 
{
private:
	struct 
	{
		unsigned int is_boundary:1;
		unsigned int is_base	:1;  // for artist control, don't remove til end.
		unsigned int is_textureBoundary:1;
		U32 FORCE_DWORD:29;
	} flags;
protected:
	SmallPtrSet m_Pairs;  // list of all pairs that this vertex is in.
public:
	IFXVector3 v;

	Vertex();  // see if this is being called correctly.
	virtual ~Vertex() {};

	virtual void IFXAPI init();
	
	// Pairs
	inline void AddPair(Pair* pp) { m_Pairs.Insert(pp); };
	inline void RemovePair(Pair* pp) { m_Pairs.Remove(pp); };
	SmallPtrSet* GetPairSet() {return &m_Pairs;};
	inline int getNumPairs() {return m_Pairs.Size();};

	void updatePairCosts(Vertex *removed);

	// Faces
	BOOL HasFaces();

	void computeFaceSet(SmallPtrSet&faces); 

	void updateFaces(Vertex *remaining);

	// Boundries
	void initBoundary();
	
	inline BOOL getBoundary() { return flags.is_boundary; };
	inline void setBoundary() { flags.is_boundary = 1; };
	inline void clearBoundary() { flags.is_boundary = 0; };

	inline BOOL getTextureBoundary() { return flags.is_textureBoundary; };
	inline void setTextureBoundary() { flags.is_textureBoundary = 1; };
	inline void clearTextureBoundary() { flags.is_textureBoundary = 0; };

	inline BOOL getBase() { return flags.is_base; };
	inline void setBase() { flags.is_base = 1; };
	inline void clearBase() { flags.is_base = 0; };

private:
	Matrix4x4 quadric;  
	Matrix4x1 v4x1;
	Matrix1x4 v1x4;

	void computeQuadric();
	void setMultiplyVectors();

public:
	inline Matrix4x4 &Quadric() { return quadric; };
	
	inline Matrix4x1 & vec4x1() { return v4x1; };
	inline Matrix1x4 & vec1x4() { return v1x4; };
};

#endif
