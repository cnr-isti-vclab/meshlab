/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_FEM_UTILS_
#define _VITELOTTE_FEM_UTILS_

#include <Eigen/Core>


namespace Vitelotte
{


template <class Derived0, class Derived1>
inline typename Eigen::MatrixBase<Derived0>::Scalar det2(
        const Eigen::MatrixBase<Derived0>& _v0, const Eigen::MatrixBase<Derived1>& _v1);


} // namespace Vitelotte

#include "femUtils.hpp"


#endif
