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
// This file implements the detail of the TIFF class.

#include "qmeta/tiff.h"

#include <QtCore>
#include <qitty/byte_array.h>

#include "qmeta/exif.h"
#include "qmeta/iptc.h"
#include "qmeta/tiff_header.h"
#include "qmeta/xmp.h"

namespace qmeta {

Tiff::Tiff(QByteArray *data) : File(data) {
  Init();
}

Tiff::Tiff(QIODevice *file) : File(file) {
  Init();
}

Tiff::Tiff(const QString &file_name) : File(file_name) {
  Init();
}

// Initializes the Tiff object.
void Tiff::Init() {
  if (!file())
    return;

  TiffHeader *tiff_header = new TiffHeader(this);
  if (tiff_header->Init(file(), 0))
    set_tiff_header(tiff_header);
  else
    set_tiff_header(NULL);

  InitMetadata();
}

// Reimplements the File::IsValid().
bool Tiff::IsValid() {
  if (!file())
    return false;

  if (tiff_header())
    return true;
  else
    return false;
}

// Reimplements the File::InitExif().
void Tiff::InitExif() {
  if (tiff_header()) {
    // Creates the Exif object.
    Exif *exif = new Exif(this);
    if (exif->Init(file(), tiff_header()))
      set_exif(exif);
    else
      delete exif;
  }
}

// Reimplements the File::InitIptc().
void Tiff::InitIptc() {
  tiff_header()->ToFirstIfd();
  // Creates a variable used to save the offset of the IPTC data. This value
  // will be overwritten if the IPTC data is found.
  qint64 iptc_offset = -1;
  // Finds the IPTC data from the TIFF header. IPTC offset is recorded in
  // the "IPTC dataset" tag, and represented as 33723 in decimal.
  while (tiff_header()->HasNextIfdEntry()) {
    qint64 ifd_entry_offset = tiff_header()->NextIfdEntryOffset();
    int tag = tiff_header()->IfdEntryTag(ifd_entry_offset);
    if (tag == 33723) {
      iptc_offset = tiff_header()->IfdEntryOffset(ifd_entry_offset);
      break;
    }
  }
  if (iptc_offset != -1) {
    // Creates the Iptc object.
    Iptc *iptc = new Iptc(this);
    if (iptc->Init(file(), iptc_offset))
      set_iptc(iptc);
    else
      delete iptc;
  }
}

// Reimplements the File::InitXmp().
void Tiff::InitXmp() {
  tiff_header()->ToFirstIfd();
  // Creates a variable used to save the offset of the XMP packet. This value
  // will be overwritten if the XMP packet is found.
  qint64 xmp_offset = -1;
  // Finds the XMP packet from the TIFF header. XMP offset is recorded in
  // the "XMP packet" tag, and represented as 700 in decimal.
  while (tiff_header()->HasNextIfdEntry()) {
    qint64 ifd_entry_offset = tiff_header()->NextIfdEntryOffset();
    int tag = tiff_header()->IfdEntryTag(ifd_entry_offset);
    if (tag == 700) {
      xmp_offset = tiff_header()->IfdEntryOffset(ifd_entry_offset);
      break;
    }
  }
  if (xmp_offset != -1) {
    // Creates the Xmp object.
    Xmp *xmp = new Xmp(this);
    if (xmp->Init(file(), xmp_offset))
      set_xmp(xmp);
    else
      delete xmp;
  }
}

}  // namespace qmeta
