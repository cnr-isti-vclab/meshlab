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
// This file implements the detail of the Xmp class.

#include "qmeta/xmp.h"

#include <QtCore>

namespace qmeta {

Xmp::Xmp(QObject *parent) : Standard(parent) {}

// Initializes the Xmp object. Returns true if successful.
bool Xmp::Init(QIODevice *file, qint64 file_start_offset) {
  set_file(file);
  set_file_start_offset(file_start_offset);

  // Checks if wrapper exists. Return false if the header is invalid, or if
  // the header is valid but the valid trailer is not found.
  file->seek(file_start_offset);
  if (QString(file->read(17)) == "<?xpacket begin=\"" &&
      // Checks the Unicode "zero width non-breaking space charater" (U+FEFF)
      // used as a byte-order marker.
      file->read(3).toHex() == "efbbbf" &&
      // Checks the rest part of the wrapper header.
      file->read(31) == "\" id=\"W5M0MpCehiHzreSzNTczkc9d\"") {
    // Makes sure the closing notation of the wrapper header exists.
    // Note that header attributes other than "begin" and "id" are not
    // supported currently.
    bool header_is_valid = false;
    while (!file->atEnd()) {
      if (QString(file->read(1)) == "?" && QString(file->read(1)) == ">") {
        header_is_valid = true;
        break;
      }
    }
    // Returns false if the wrapper header is invalid.
    if (!header_is_valid)
      return false;
    // Found wrapper header, now checks if the wrapper trailer exists.
    bool found_trailer = false;
    while (!file->atEnd()) {
      if (QString(file->read(1)) == "<" && QString(file->read(1)) == "?" &&
          QString(file->read(17)) == "xpacket end=\"w\"?>") {
        found_trailer = true;
        break;
      }
    }
    if (!found_trailer)
      return false;
    // Found wrapper trailer. Now we can make sure the XMP wrapper is valid.
  }
  return true;
}

}  // namespace qmeta

