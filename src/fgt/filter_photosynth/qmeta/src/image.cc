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
// This file implements the detail of the Image class.

#include "qmeta/image.h"

#include <QtCore>

namespace qmeta {

Image::Image(QByteArray *data) : File(data) {
  GuessType();
}

Image::Image(QIODevice *file) : File(file) {
  GuessType();
}

Image::Image(const QString &file_name) : File(file_name) {
  GuessType();
}

// Guesses the file type of the tracked file. It calls the GuessType(FileType)
// function for all file types it guesses. If there is no matched file type
// or there is no tracked file object, sets the file type to kInvalidFileType.
void Image::GuessType() {
  if (file()) {
    // Guess the file type as JPEG.
    if (GuessType<Jpeg>(kJpegFileType))
      return;
    // Guess the file type as Tiff.
    else if (GuessType<Tiff>(kTiffFileType))
      return;
  }
  set_file_type(kInvalidFileType);
}

// Guesses the file type of the tracked file. Tries to creates a new image
// object using the specified type T, if the image is valid, sets the specified
// file_type to current file type and binds the image's metadata objects.
// Returns true if the specified type T is correct.
template<class T> bool Image::GuessType(FileType file_type) {
  T *image = new T(file());
  if (image->IsValid()) {
    image->setParent(this);
    set_file_type(file_type);
    set_exif(image->exif());
    set_iptc(image->iptc());
    set_xmp(image->xmp());
    return true;
  } else {
    delete image;
    return false;
  }
}

// Reimplements the File::IsValid().
bool Image::IsValid() {
  if (file_type() == kInvalidFileType)
    return false;
  else
    return true;
}

}
