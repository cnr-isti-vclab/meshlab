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
#ifndef IFXTQTVERTEX_DOT_H
#define IFXTQTVERTEX_DOT_H

#include "IFXVector3.h"

typedef struct 
{
    enum        {NotLabeled=0xFFFFFFFF};
    U32         m_uRenderIndex;
    IFXVector3  m_position;
    IFXVector3  m_normal;
    IFXVector2  m_texCoord;
} IFXTQTVertex;

#endif
