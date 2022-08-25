/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/


#ifndef _PATATE_GRENAILLE_
#define _PATATE_GRENAILLE_

// First inclue Eigen Core
#include <Eigen/Core>

// Include common stuff
#include "common/defines.h"

// Include Grenaille Core components
#include "Grenaille/Core/enums.h"
#include "Grenaille/Core/basket.h"

#include "Grenaille/Core/weightKernel.h"
#include "Grenaille/Core/weightFunc.h"

#include "Grenaille/Core/plane.h"
#include "Grenaille/Core/covariancePlaneFit.h"

#include "Grenaille/Core/orientedSphereFit.h"
#include "Grenaille/Core/curvature.h"
#include "Grenaille/Core/gls.h"

// not supported on cuda
#ifndef __CUDACC__
# include "Grenaille/Core/unorientedSphereFit.h"
#endif


// Include Grenaille Algorithms

#endif //_PATATE_GRENAILLE_
