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
#ifndef FACEUPDATE_DOT_H
#define FACEUPDATE_DOT_H

// Class FaceUpdate encapsulates a changed attribute within a face of
// an CLOD model.  The following list of token identifiers indicate which 
// attributes  may be changed:

// DataUpdate Attribute Tokens:
#define CLOD_VERTEX_A       0
#define CLOD_VERTEX_B       1
#define CLOD_VERTEX_C       2
#define CLOD_NORMAL_A       3
#define CLOD_NORMAL_B       4
#define CLOD_NORMAL_C       5
#define CLOD_TEXCOORD_A     6
#define CLOD_TEXCOORD_B     7
#define CLOD_TEXCOORD_C     8

typedef enum 
{ 
	BadToken,
	VertexA, VertexB, VertexC, 
	NormalA, NormalB, NormalC, 
	TexCoordA, TexCoordB, TexCoordC
} AttribToken;

typedef enum {Decreasing, Increasing} Direction;

class FaceUpdate
{
public:
	int			faceIndex;		// Index of the IFACE to be changed
	AttribToken	attribToken;	// One of CLOD_UPDATE_* tokens (defined above)
	int			value[2];		// [0]:Attribute value for resolution decrease.
								// [1]: Attribute value for resolution increase.
};

#endif // FACEUPDATE_DOT_H
