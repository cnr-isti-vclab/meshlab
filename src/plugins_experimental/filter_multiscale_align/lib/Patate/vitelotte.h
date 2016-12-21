/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/. 
*/

#ifndef _PATATE_VITELOTTE_
#define _PATATE_VITELOTTE_

#include <Eigen/Core>

#include "common/surface_mesh/surfaceMesh.h"

#include "Vitelotte/Core/linearElement.h"
#include "Vitelotte/Core/quadraticElement.h"
#include "Vitelotte/Core/morleyElement.h"
#include "Vitelotte/Core/fvElement.h"

#include "Vitelotte/Core/vgMesh.h"
#include "Vitelotte/Utils/mvgReader.h"
#include "Vitelotte/Utils/mvgWriter.h"

#include "Vitelotte/Core/femSolver.h"
#include "Vitelotte/Core/linearElementBuilder.h"
#include "Vitelotte/Core/quadraticElementBuilder.h"
#include "Vitelotte/Core/morleyElementBuilder.h"
#include "Vitelotte/Core/fvElementBuilder.h"
#include "Vitelotte/Core/singularElementDecorator.h"

#include "Vitelotte/Utils/dcMesh.h"
#include "Vitelotte/Utils/mvgWithCurvesReader.h"
#include "Vitelotte/Utils/mvgWithCurvesWriter.h"

#endif
