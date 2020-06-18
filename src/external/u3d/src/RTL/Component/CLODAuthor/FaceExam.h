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
#ifndef FACEEXAM_DOT_H
#define FACEEXAM_DOT_H

// We use this class for book keeping when computing normal update records
// in ContractionRecorder::record();
class FaceExam
{
public:
	U32 faceIndex;
	IV3D normal;
	U32 smoothID;	
	unsigned short marked;
};

// A static collection of FaceExams...so we don't have to allocate/deallocate
// Memory on the fly:
#define MAX_NUM_FACEEXAMS	200

class FaceExamList
{
public:
	FaceExam *pFaceExam;
	
	inline FaceExamList();
	inline ~FaceExamList();
};

inline FaceExamList::FaceExamList()
{
	pFaceExam = new FaceExam[MAX_NUM_FACEEXAMS];
}

inline FaceExamList::~FaceExamList()
{
	delete[] pFaceExam;
}

#endif // FACEEXAM_DOT_H
