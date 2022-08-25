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
#ifndef COSTMAP_DOT_H
#define COSTMAP_DOT_H
#include <float.h>

// these constants partition the cost space.

// all base mesh vertices have costs >= to this.
#define BASE_VERTEX_COST FLT_MAX/2
#define BASE_VERTEX_DELTA FLT_MAX/200000

// all normal flips have costs >= to this and < BASE_VERTEX_COST
#define NORMAL_FLIP_COST FLT_MAX/4

#endif // COSTMAP_DOT_H
