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
#include "Face.h"
#include "Vertex.h"
#include "Pair.h"

unsigned long GeometryObject::currentTime = 1;

Face::Face () :	a(0), b(0), c(0), index(0)
{ 
	A=0.0f; B=0.0f;  C=0.0f;  D= 0.0f; area = -1.0f;
}

void Face::Set(Pair *pa, Pair *pb, Pair *pc, int idx)
{
	a = pa;
	b = pb;
	c = pc;
	index = idx;

	// Each pair now has this face on its face list:
	a->AddFace (this);
	b->AddFace (this);
	c->AddFace (this);
}

void Face::remove()
{
	if (a) a->RemoveFace(this);
	if (b) b->RemoveFace(this);
	if (c) c->RemoveFace(this);
	a = b = c = NULL;
}

// Returns TRUE if xy is a counter clockwise edge on this face:
BOOL Face::isCounterClockwise (Vertex *x, Vertex *y)
{
	Vertex *va, *vb, *vc;

	va = a->getCommonVertex(b);
	vb = b->getCommonVertex(c);
	vc = c->getCommonVertex(a);

	if ((x == va) && (y == vb)) return TRUE;
	if ((x == vb) && (y == vc)) return TRUE;
	if ((x == vc) && (y == va)) return TRUE;
	return FALSE;
}

void Face::computeArea()
{
	Vertex *va, *vb, *vc;

	va = a->getCommonVertex(b);
	vb = b->getCommonVertex(c);
	vc = c->getCommonVertex(a);

	area = triangleArea((IV3D*)&va->v, (IV3D*)&vb->v, (IV3D*)&vc->v);
}

// kv = keep vertex, rv = removed vertex
float Face::computeNewArea(Vertex *kv, Vertex *rv)
{
	Vertex *v1,*v2,*v3;
	v1 = a->getCommonVertex(b);
	v2 = b->getCommonVertex(c);
	v3 = c->getCommonVertex(a);
	if (v1 == rv) v1 = kv;
	if (v2 == rv) v2 = kv;
	if (v3 == rv) v3 = kv;
	return triangleArea((IV3D*)&v1->v, (IV3D*)&v2->v, (IV3D*)&v3->v);
}


void Face::computeQuadric()
{
	// A Face's quadric is its Kp term (Garland, Heckbert)

	// Compute A, B, C, D...the equation of the plane for this face:
	
	Vertex *v1 = a->getCommonVertex(b);
	Vertex *v2 = b->getCommonVertex(c);
	Vertex *v3 = c->getCommonVertex(a);

	computePlaneEquation ((IV3D*)&v1->v, (IV3D*)&v2->v, (IV3D*)&v3->v, &A, &B, &C, &D);

	// The quadric is simply: p * p^T where P is the vector A, B, C, D.
	float q[16]; // q is row major:
	q[0]=A*A;		q[1]=A*B;		q[2]=A*C;		q[3]=A*D;
	q[4]=A*B;		q[5]=B*B;		q[6]=B*C;		q[7]=B*D;
	q[8]=A*C;		q[9]=B*C;		q[10]=C*C;		q[11]=C*D;
	q[12]=A*D;		q[13]=B*D;		q[14]=C*D;		q[15]=D*D;
	
	quadric.set(q);

	// This quadric, Q, is allows us to easily compute the squared
	// distance to the plane A, B, C, D.  We do so for a point p
	// by multiplying p^T * Q * p.  You'll see this in 
	// QEPair::computeCost();

	makeCurrent();
}
