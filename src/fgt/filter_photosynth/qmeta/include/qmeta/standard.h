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
// This file defines the Standard class, which is the base class for all
// metadata standard classes such as Exif and Iptc.

#ifndef QMETA_STANDARD_H_
#define QMETA_STANDARD_H_

#include <QObject>

class QIODevice;

namespace qmeta {

class Standard : public QObject {
  Q_OBJECT

 public:
  explicit Standard(QObject *parent = NULL);

 protected:
  QIODevice* file() const { return file_; }
  void set_file(QIODevice *file) { file_ = file; }
  qint64 file_start_offset() const { return file_start_offset_; }
  void set_file_start_offset(qint64 offset) { file_start_offset_ = offset; }

  // Tracks the file containing the metadata.
  QIODevice *file_;
  // The offset of the beginning of the metadata standard in the tracked file.
  qint64 file_start_offset_;
};

}  // namespace qmeta

#endif  // QMETA_STANDARD_H_
