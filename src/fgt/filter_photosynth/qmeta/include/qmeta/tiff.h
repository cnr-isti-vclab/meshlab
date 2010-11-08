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
// This file defines the TIFF class.

#ifndef QMETA_TIFF_H_
#define QMETA_TIFF_H_

#include "qmeta/file.h"

class QString;

namespace qmeta {

class TiffHeader;

class Tiff : public File {
 public:
  explicit Tiff(QByteArray *data);
  explicit Tiff(QIODevice *file);
  explicit Tiff(const QString &file_name);
  void Init();
  bool IsValid();

 private:
  void InitExif();
  void InitIptc();
  void InitXmp();

  TiffHeader* tiff_header() const { return tiff_header_; }
  void set_tiff_header(TiffHeader *tiff_header) { tiff_header_ = tiff_header; }

  // Tracks the TiffHeader object of the tracked file.
  TiffHeader *tiff_header_;
};

}  // namespace qmeta

#endif  // QMETA_TIFF_H_
