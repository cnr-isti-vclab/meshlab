// Copyright 2010, Ollix
// All rights reserved.
//
// This file is part of Qitty.
//
// Qitty is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or any later version.
//
// Qitty is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with Qitty. If not, see <http://www.gnu.org/licenses/>.

// ---
// Author: olliwang@ollix.com (Olli Wang)
//
// Qitty - a toolkit to enhance the Qt library.
//
// Defines utility functions for Qt's QByteArray class.

#ifndef QITTY_UTILS_BYTE_ARRAY_H_
#define QITTY_UTILS_BYTE_ARRAY_H_

class QByteArray;

namespace qitty_utils {

bool EqualToInt(QByteArray byte_array, int value);
QByteArray ReverseByteArray(QByteArray byte_array);
int ToInt(QByteArray byte_array);

}  // namespace qitty_utils

#endif  // QITTY_UTILS_BYTE_ARRAY_H_
