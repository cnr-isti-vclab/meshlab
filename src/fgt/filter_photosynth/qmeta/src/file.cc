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
// This file implements the detail of the File class.

#include "qmeta/file.h"

#include <QtCore>

#include "qmeta/exif.h"

namespace qmeta {

// Constructs a file from the given QByteArray data.
File::File(QByteArray *data) {
  set_exif(NULL);
  set_iptc(NULL);
  set_xmp(NULL);
  QBuffer *file = new QBuffer(data, this);
  if (file->open(QIODevice::ReadOnly))
    set_file(file);
  else
    set_file(NULL);
}

// Constructs a file from the given QIODevice file.
File::File(QIODevice *file) {
  set_exif(NULL);
  set_iptc(NULL);
  set_xmp(NULL);
  set_file(file);
}

// Constructs a file and tries to load the file with the given file_name.
File::File(const QString &file_name) {
  set_exif(NULL);
  set_iptc(NULL);
  set_xmp(NULL);
  QFile *file = new QFile(file_name, this);
  if (file->open(QIODevice::ReadOnly))
    set_file(file);
  else
    set_file(NULL);
}

// Returns the thumbnail from supported metadata. Currently Exif is the only
// supported metadata.
QByteArray File::Thumbnail() {
  QByteArray thumbnail;
  if (exif())
    thumbnail = exif()->Thumbnail();
  return thumbnail;
}

// Initializes metadata objects for the tracked file.
void File::InitMetadata() {
  if (!IsValid())
    return;

  InitExif();
  InitIptc();
  InitXmp();
}

}  // namespace qmeta
