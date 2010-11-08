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
// QMeta - a library to manipulate file_types metadata based on Qt.
//
// This file implements the detail of the Jpeg class.

#include "qmeta/jpeg.h"

#include <QtCore>
#include <qitty/byte_array.h>

#include "qmeta/exif.h"
#include "qmeta/iptc.h"
#include "qmeta/tiff_header.h"
#include "qmeta/xmp.h"

namespace qmeta {

Jpeg::Jpeg(QByteArray *data) : File(data) {
  InitMetadata();
}

Jpeg::Jpeg(QIODevice *file) : File(file) {
  InitMetadata();
}

Jpeg::Jpeg(const QString &file_name) : File(file_name) {
  InitMetadata();
}

// Reimplements the File::IsValid().
bool Jpeg::IsValid() {
  if (!file())
    return false;

  // Checks the first 2 bytes if equals to the SOI marker.
  file()->seek(0);
  if (file()->read(2).toHex() != "ffd8")
    return false;

  return true;
}

// Reimplements the File::InitExif().
void Jpeg::InitExif() {
  file()->seek(2);
  while (!file()->atEnd()) {
    // Finds APP1 marker.
    if (file()->read(1).toHex() != "ff")
      continue;
    if (file()->read(1).toHex() != "e1")
      continue;

    // Retrieves the APP1 length. The length doesn't include the APP1 marker.
    file()->read(2).toHex().toInt(NULL, 16);

    // Checks the Exif signature.
    if (QString(file()->read(6)) == "Exif")
      break;
  }
  if (file()->atEnd())
    return;

  TiffHeader *tiff_header = new TiffHeader(this);
  if (tiff_header->Init(file(), file()->pos())) {
    // Creates the Exif object.
    Exif *exif = new Exif(this);
    if (exif->Init(file(), tiff_header))
      set_exif(exif);
    else
      delete exif;
  } else {
    delete tiff_header;
  }
}

// Reimplements the File::InitIptc().
void Jpeg::InitIptc() {
  // Finds the APP13 marker.
  file()->seek(2);
  while (!file()->atEnd()) {
    if (file()->read(1).toHex() != "ff")
      continue;
    if (file()->read(1).toHex() != "ed")
      continue;
    break;
  }
  // Returns if there is not APP13 marker.
  if (file()->atEnd())
    return;

  // Skips segment size marker.
  file()->read(2);

  // Checks the Photoshop signature.
  if (QString(file()->read(14)) != "Photoshop 3.0")
    return;

  bool found_iptc = false;
  // Interators the Image Resource Blocks to find IPTC data. If found, sets the
  // `found_iptc` to true and gets out of the loop.
  while (QString(file()->read(4)) == "8BIM") {
    int identifier = file()->read(2).toHex().toInt(NULL, 16);
    // Skips the variable name in Pascal string, padded to make the size even.
    // A null name consists of two bytes of 0.
    int name_length = file()->read(1).toHex().toInt(NULL, 16);
    if (name_length == 0)
      file()->read(1);
    else if (name_length % 2 == 1)
      file()->read(name_length);
    else
      file()->read(name_length + 1);
    // Determines the actual size of resource data that follows.
    int data_length = file()->read(4).toHex().toInt(NULL, 16);
    // Determines if the current block is used to record the IPTC data.
    // If true, the identifier should be 1028 in decimal.
    if (identifier == 1028) {
      found_iptc = true;
      break;
    } else {
      file()->read(data_length);
    }
  }
  // Returns if there is no IPTC data.
  if (!found_iptc)
    return;

  // Creates the Iptc object.
  Iptc *iptc = new Iptc(this);
  if (iptc->Init(file(), file()->pos()))
    set_iptc(iptc);
  else
    delete iptc;
}

// Reimplements the File::InitXmp().
void Jpeg::InitXmp() {
  file()->seek(2);
  while (!file()->atEnd()) {
    // Finds APP1 marker.
    if (file()->read(1).toHex() != "ff")
      continue;
    if (file()->read(1).toHex() != "e1")
      continue;

    // Retrieves the APP1 length. The length doesn't include the APP1 marker.
    file()->read(2).toHex().toInt(NULL, 16);

    // Checks the XMP signature.
    if (QString(file()->read(29)) == "http://ns.adobe.com/xap/1.0/")
      break;
  }
  if (file()->atEnd())
    return;

  Xmp *xmp = new Xmp(this);
  if (xmp->Init(file(), file()->pos()))
    set_xmp(xmp);
  else
    delete xmp;
}

}  // namespace qmeta
