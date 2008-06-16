/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#ifndef HOLEPATCH_H
#define HOLEPATCH_H

#include <utility>
#include <vector>
#include "fgtHole.h"
#include "vcg/simplex/face/pos.h"
#include "vcg/simplex/faceplus/component_ocf.h"

template <class MESH>
class HolePatch {
public:
	typedef typename MESH::FaceType FaceType;
    typedef typename MESH::FacePointer FacePointer;
	typedef typename MESH::ConstFaceIterator	FaceIterator;
	
	//typedef typename vcg::face::vector_ocf<FaceType> FaceVector;
	//typedef typename vcg::face::Pos<FaceType> PosType;

	MESH patchMesh;

	// Invocazione apertura nuova mesh
	//
	//MeshModel *mm= new MeshModel();	
	//			if (!pCurrentIOPlugin->open(extension, fileName, *mm ,mask,QCallBack,this /*gla*/))
	//				delete mm;
	//


	
	void Draw() const {
		glColor3f( 0.75, 0.94, 1.0 );
		glBegin(GL_TRIANGLES);
		FaceIterator it = patchMesh.face.begin();
		for( ; it!=patchMesh.face.end() ; ++it) {					
			 glVertex( (*it).V(0)->P() );
			 //glVertex( (*it).V(1)->P() );
			 glVertex( (*it).V(2)->P() );
			 glVertex( (*it).V(1)->P() );
		}
		glEnd();

		glDisable( GL_LIGHTING );
		glColor3f( 0.0, 0.0, 1.0 );
		
		it = patchMesh.face.begin();
		for( ; it!=patchMesh.face.end() ; ++it) {
			glBegin(GL_LINE_STRIP);
			glVertex( (*it).V(0)->P() );
			glVertex( (*it).V(1)->P() );
			glVertex( (*it).V(2)->P() );
			glVertex( (*it).V(0)->P() );
			glEnd();
		}
		
		glEnable( GL_LIGHTING );
	}
};


#endif