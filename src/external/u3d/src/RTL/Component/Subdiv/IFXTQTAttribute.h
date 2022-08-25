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
#ifndef IFXTQTATTRIBUTE_DOT_H
#define IFXTQTATTRIBUTE_DOT_H


#include "IFXDataTypes.h"


// IFXEdgeContinuityDescriptor is used to indicate whether or not
// a particular mesh attribute is continuous across an edge or not.
// A zero value indicates that the attribute is continuous, a
// non-zero value indicates discontinous.  Thus, if AllAttributes == 0,
// the all attributes are continous accross the edge.  If AllAttributes
// is non-zero, then one or more of the attributes must be discontinuous.
// The structure is employed by the subdivision neighbor gathering code,
// to build the inputs to the subdivision computation.
#define IFXCONTINUOUS   ((U8) 0)
typedef struct
{
    union
    {
        struct
        {
            U8  bPosition       :1;
            U8  bNormal         :1;
            U8  bTexCoord       :1;
            U8  bMaterial       :1;
            U8  bUserAttribute1 :1;
            U8  bUserAttribute2 :1;
        }bAttribute;
        U8  AllAttributes      :8;
    };
} IFXEdgeContinuityDescriptor;

#endif
