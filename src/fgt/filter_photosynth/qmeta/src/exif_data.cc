// Copyright 2010, Ollix
// All rights reserved.
//
// This file is part of QMeta.
//
// QMeta is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or any later version.
//
// QMeta is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with QMeta. If not, see <http://www.gnu.org/licenses/>.

// ---
// Author: olliwang@ollix.com (Olli Wang)
//
// QMeta - a library to manipulate image metadata based on Qt.
//
// This file implements the details of all classes related to Exif data.

#include "qmeta/exif_data.h"

#include <QtCore>

namespace qmeta {

ExifData::ExifData(const QByteArray &other) : QByteArray(other) {}

// Returns the byte array converted to double. This function works correctly
// if the Type is RATIONAL and the Count is 1.
double ExifData::ToDouble() {
  double numerator = static_cast<double>(mid(0,4).toHex().toUInt(NULL, 16));
  double denominator = static_cast<double>(mid(4).toHex().toUInt(NULL, 16));
  return numerator/ denominator;
}

// Returns the byte array converted to float. This function works correctly
// if the Type is SRATIONAL and the Count is 1.
float ExifData::ToFloat() {
  float numerator = static_cast<float>(mid(0,4).toHex().toInt(NULL, 16));
  float denominator = static_cast<float>(mid(4).toHex().toInt(NULL, 16));
  return numerator/ denominator;
}

// Returns the byte array converted to int in decimal. This function works
// correctly if the Type is one of BYTE, SHORT, or SLONG and the Count is 1.
int ExifData::ToInt() {
  return toHex().toInt(NULL, 16);
}

// Returns the byte array converted to QString. This functions works correctly
// if the Type is ASCII.
QString ExifData::ToString() {
  return QString(data());
}

// Returns the byte array converted to uint in decimal. This  function works
// correctly if the Type is BYTE, SHORT, or LONG and the Count is 1.
uint ExifData::ToUInt() {
  return toHex().toUInt(NULL, 16);
}

}  // namespace qmeta
