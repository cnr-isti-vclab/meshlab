//***************************************************************************
//
//  Copyright (c) 2000 - 2006 Intel Corporation
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
#ifndef IFXACCONTEXT_H__
#define IFXACCONTEXT_H__

//***************************************************************************
//  Constants
//***************************************************************************

static const U32 uACStaticFull = 0x00000400;
static const U32 uACMaxRange = uACStaticFull + 0x00003FFF;

// BlockType_MotionCompressed = 0xFFFFFF64;
static const U32 uACContextMotionSignTime = 1;
static const U32 uACContextMotionDiffTime = 2;
static const U32 uACContextMotionSignDisplacement = 3;
static const U32 uACContextMotionDiffDisplacement = 4;
static const U32 uACContextMotionSignRotation = 5;
static const U32 uACContextMotionDiffRotation = 6;
static const U32 uACContextMotionSignScale = 7;
static const U32 uACContextMotionDiffScale = 8;

// BlockType_AuthorCLODStatic
static const U32 uACContextBaseShadingID = 1;

// BlockType_AuthorCLODProgressive
/// @todo: Renumber contexts
static const U8 u8AMPOrientationLeft = 1;
static const U8 u8AMPOrientationRight = 2;
static const U8 u8AMPThirdIndexLocal = 1;
static const U8 u8AMPThirdIndexGlobal = 2;

static const U32 uACContextNumNewFaces = 1;
static const U32 uACContextNumNewTexCoords   = 64;

static const U32 uACContextShadingID = 65; /// @todo: Reorder and renumber contexts

static const U32 uACContextOrientationType = 2;
static const U32 uACContextThirdIndexType = 3;
static const U32 uACContextLocal3rdPosition = 4;

static const U32 uAMPPredictStay2 = 4;
static const U32 uAMPPredictMove2 = 3;
static const U32 uAMPPredictStay = 2;
static const U32 uAMPPredictMove = 1;
static const U32 uAMPPredictNoGuess = 0;
static const U8 u8AMPStay = 0;
static const U8 u8AMPMove = 1;
static const U32 uACContextStayMove = 15;
// reserved for Stay/Move:
// [uACContextStayMove ... uACContextStayMove + uAMPPredictStay2]
// currently that is [15 ... 19]
static const U32 uACContextPositionDiffSigns = 20;
static const U32 uACContextPositionDiffMagX = 21;
static const U32 uACContextPositionDiffMagY = 22;
static const U32 uACContextPositionDiffMagZ = 23;

static const U32 uACContextNewDiffuseColorsCount = 99;
static const U32 uACContextDiffuseColorSign = 100;
static const U32 uACContextNewSpecularColorsCount = 101;
static const U32 uACContextSpecularColorSign = 102;
static const U32 uACContextTexCoordSign = 103;

static const U8 uAMPUpdateChange = 1;
static const U8 uAMPUpdateKeep = 2;
static const U8 uAMPUpdateNew = 1;
static const U8 uAMPUpdateLocal = 2;
static const U8 uAMPUpdateGlobal = 3;

static const U32 uACContextDiffuseKeepChange = 104;
static const U32 uACContextDiffuseChangeType = 105;
static const U32 uACContextDiffuseChangeIndexNew = 106;
static const U32 uACContextDiffuseChangeIndexLocal = 107;
static const U32 uACContextDiffuseChangeIndexGlobal = 108;

static const U32 uACContextSpecularKeepChange = 109;
static const U32 uACContextSpecularChangeType = 110;
static const U32 uACContextSpecularChangeIndexNew = 111;
static const U32 uACContextSpecularChangeIndexLocal = 112;
static const U32 uACContextSpecularChangeIndexGlobal = 113;

static const U32 uACContextTexCoordKeepChange = 114;
static const U32 uACContextTexCoordChangeType = 115;
static const U32 uACContextTexCoordChangeIndexNew = 116;
static const U32 uACContextTexCoordChangeIndexLocal = 117;
static const U32 uACContextTexCoordChangeIndexGlobal = 118;

static const U32 uACContextVertColorSplitIndexLocal = 119;
static const U32 uACContextVertColorSplitIndexGlobal = 120;

static const U32 uACContextTexCoordSplitIndexLocal = 121;
static const U32 uACContextTexCoordSplitIndexGlobal = 122;
static const U32 uACContextNewTexCoordsCount = 123;

static const U32 uACContextTexCoordUsedType  = 24;
static const U32 uACContextTexCoordIndexType = 25;
static const U32 uACContextTexCoordCandType  = 26;
static const U32 uACContextTexCoordListType  = 27;
//static const U32 uACContextTexCoordChangeType= 28;
static const U32 uACContextTexCoordSplitType = 29;
static const U32 uACContextCandidateIndex    = 30;
static const U32 uACContextListIndex     = 31;
static const U32 uACContextTexCoordDiffSigns = 32;
static const U32 uACContextTexCoordDiffMagU  = 33;
static const U32 uACContextTexCoordDiffMagV  = 34;
static const U32 uACContextTexCoordDiffMagS  = 35;
static const U32 uACContextTexCoordDiffMagT  = 36;
static const U32 uACContextTexCoordAttribType = 37;
static const U32 uACContextTexCoordFaceUpdate = 38;
static const U32 uACContextTexCoordDupType    = 39;

static const U8  u8AMPOldTexCoord = 0;
static const U8  u8AMPNewTexCoord = 1;
static const U8  u8AMPRawTexCoord = 5;
static const U8  u8AMPCurTexCoord = 0; //Indicates to use the current TexCoords.
static const U8  u8AMPSplitTexCoord = 1;
static const U8  u8AMPThirdTexCoord = 2;
static const U8  u8AMPOneLessTexCoord=3;
static const U8  u8AMPTwoLessTexCoord=4;
static const U8  u8AMPCandidate   = 0;
static const U8  u8AMPNotCandidate  = 1;
static const U8  u8AMPNotList   = 0;
static const U8  u8AMPList      = 1;
static const U8  u8AMPKeep      = 0;
static const U8  u8AMPChange    = 1;
static const U8  u8AMPGood      = 0;
static const U8  u8AMPBad     = 1;
static const U8  u8AMPSplitTexCoordDup  = 1;
static const U8  u8AMPUpdateTexCoordDup = 2;
static const U8  u8AMPThirdTexCoordDup  = 4;

static const U32 uACContextNumLocalNormals    = 40;
static const U32 uACContextNormalDiffSigns    = 41;
static const U32 uACContextNormalDiffMagX   = 42;
static const U32 uACContextNormalDiffMagY   = 43;
static const U32 uACContextNormalDiffMagZ   = 44;
static const U32 uACContextNormalLocalIndex   = 45;
static const U32 uACContextNumNewDiffuseColors  = 46;
static const U32 uACContextNumNewSpecularColors = 47;

static const U32 uACContextVertColorCount   = 50;
static const U32 uACContextVertColorUsedType  = 51;
static const U32 uACContextVertColorCandType  = 52;
static const U32 uACContextVertColorListType  = 53;
static const U32 uACContextVertColorChangeType  = 54;
static const U32 uACContextVertColorSplitType = 55;
static const U32 uACContextVertColorDupType   = 56;
static const U32 uACContextVertexColorDiffMagR  = 60;
static const U32 uACContextVertexColorDiffMagG  = 61;
static const U32 uACContextVertexColorDiffMagB  = 62;
static const U32 uACContextVertexColorDiffMagA  = 63;

static const U8  u8AMPOldVertColor   = 0;
static const U8  u8AMPNewVertColor   = 1;
static const U8  u8AMPCurVertColor   = 2; //Indicates to use the current TexCoords.
static const U8  u8AMPSplitVertColor = 3;
static const U8  u8AMPThirdVertColor = 4;
static const U8  u8AMPSplitColorDup  = 1;
static const U8  u8AMPUpdateColorDup = 2;
static const U8  u8AMPThirdColorDup  = 4;

static const U32 uACContextBoneWeightCount = 66;
static const U32 uACContextBoneWeightBoneID = 67;
static const U32 uACContextBoneWeightBoneWeight = 68;

// BlockType_LineSet
static const U32 uACContextLineShadingID = 1;

static const U32 uACContextPointShadingID = 1;

#endif // IFXACCONTEXT_H__
