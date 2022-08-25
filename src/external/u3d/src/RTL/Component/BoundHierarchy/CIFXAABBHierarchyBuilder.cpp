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
//
//  CIFXAABBHierarchyBuilder.cpp
//
//  DESCRIPTION
//
//    Implementation of the CIFXAABBHierarchyBuilder class.  Contains the
//    functionality to build each node in the hierarchy.
//
//  NOTES
//
//*****************************************************************************

#include "CIFXAABBHierarchyBuilder.h"
#include "CIFXBoundUtil.h"
#include "IFXCoreCIDs.h"

#include "IFXCOM.h"

#define IFX_MAX_NUMFACES 30000


IFXRESULT CIFXAABBHierarchyBuilder::Build(IFXBoundHierarchy** ppBoundHierarchy,
                      U32                 uType,
                      IFXMeshGroup*       pMeshGroup)
{
  IFXRESULT result = IFX_OK;

  if( pMeshGroup && ppBoundHierarchy )
  {
    // Create the face and vertex lists for the initial bounding box
    CIFXAxisAlignedBBox* pBound      = NULL;
    CIFXBoundUtil        boundUtil;
    CIFXBoundFace       *pFaceList   = NULL;
    IFXVector3          *pVertexList = NULL;
    U32                  uNumFaces;
    U32                  uNumVerts;

    if( IFXSUCCESS(result) )
      result = boundUtil.InitFaceNode(&pFaceList, &uNumFaces, &pVertexList, &uNumVerts, pMeshGroup);

    // Allocate memory for a new boundHierarchy
    if( IFXSUCCESS(result) )
      result = IFXCreateComponent( CID_IFXBoundHierarchy,
                     IID_IFXBoundHierarchy,
                     (void**)ppBoundHierarchy);
    // Initialize the hierarchy
    if( IFXSUCCESS(result) )
      result = (*ppBoundHierarchy)->InitHierarchy(pMeshGroup, &pVertexList);

    if( IFXSUCCESS(result) )
    {
      if( result != IFX_CANCEL )
      {
        // Check Model size: if too large we should  do less accurate split
        if( uNumFaces > IFX_MAX_NUMFACES )
          (*ppBoundHierarchy)->SetSplitType(IFXBoundHierarchy::MEDIAN);

        // Allocate memory for a new AABB and set values
        pBound = new CIFXAxisAlignedBBox;

        if( pBound )
        {
          if( IFXSUCCESS(result) )
            result = pBound->Initialize(*ppBoundHierarchy);

          if( IFXSUCCESS(result) )
            result = pBound->SetFaceList(&pFaceList);

          if( IFXSUCCESS(result) )
          {
            pBound->SetNumFaces(uNumFaces);

            // Allocate memory for the root node
            CIFXBTreeNode *pRoot = NULL;
            result = (*ppBoundHierarchy)->GetRoot(&pRoot);

            // Initialize the root node
            if( IFXSUCCESS(result) )
              result = pRoot->Initialize(0, pBound);

            // Create the binary tree with pRoot as the root node
            if( IFXSUCCESS(result) )
              result = CreateNode(pRoot);
          }
        }
        else
        {
          // Clean up allocated memory if error code present
          IFXRELEASE(*ppBoundHierarchy);
          IFXDELETE(pBound);

          result = IFX_E_OUT_OF_MEMORY;
        }
      }
    }
  }
  else
    result = IFX_E_INVALID_POINTER;

  if (IFX_E_UNSUPPORTED == result)
  {
    // make sure member vars are OK
    result = IFX_OK;
  }

  return result;
}


IFXRESULT CIFXAABBHierarchyBuilder::CreateNode(CIFXBTreeNode *pNextNode)
{
  IFXRESULT result = IFX_OK;

  if( pNextNode )
  {
    CIFXAxisAlignedBBox* pBox        = NULL;
    CIFXAxisAlignedBBox* pLeftBound  = NULL;
    CIFXAxisAlignedBBox* pRightBound = NULL;

    // Construct a new axis aligned bounding box
    IFXBoundVolume* pBoundVolume = pNextNode->GetBound();

    if( pBoundVolume )
    {
      pBox = (CIFXAxisAlignedBBox *)pBoundVolume;

      result = pBox->Build();

      switch( result )
      {
      case IFX_BV_SINGLE_TRI:
        // Single triangle found -- don't split hierarchy any further
        result = IFX_OK;
        break;
      case IFX_BV_MULTI_TRI:
        result = pBox->SplitBound(&pLeftBound, &pRightBound);

        if( IFXSUCCESS(result) )
        {
          // Create new LEFT and RIGHT child nodes
          CIFXBTreeNode *pLeftNode  = new CIFXBTreeNode;
          CIFXBTreeNode *pRightNode = new CIFXBTreeNode;

          if( pLeftNode && pRightNode )
          {
            // Initialize LEFT child node
            result = pLeftNode->Initialize(pNextNode->GetLevel()+1, pLeftBound);
            if( IFXSUCCESS(result) )
              result = pNextNode->SetLeftTreeNode(pLeftNode);
            if( IFXSUCCESS(result) )
              result = CreateNode(pLeftNode);

            // Initialize RIGHT child node
            if( IFXSUCCESS(result) )
              result = pRightNode->Initialize(pNextNode->GetLevel()+1, pRightBound);
            if( IFXSUCCESS(result) )
              result = pNextNode->SetRightTreeNode(pRightNode);
            if( IFXSUCCESS(result) )
              result = CreateNode(pRightNode);
          }
          else
          {
            IFXDELETE(pLeftNode);
            IFXDELETE(pRightNode);

            result = IFX_E_OUT_OF_MEMORY;
          }
        }
        break;
      default:
        break;  // Result Code passed through
      }
    }
  }
  else
    result = IFX_E_INVALID_POINTER;

  return result;
}
