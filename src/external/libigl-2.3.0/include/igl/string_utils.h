// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2020 Jérémie Dumas <jeremie.dumas@ens-lyon.org>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_STRING_UTILS_H
#define IGL_STRING_UTILS_H

#include "igl_inline.h"

#include <string>

namespace igl {

IGL_INLINE bool starts_with(const std::string &str, const std::string &prefix);

IGL_INLINE bool starts_with(const char *str, const char* prefix);

}

#ifndef IGL_STATIC_LIBRARY
#  include "string_utils.cpp"
#endif

#endif
