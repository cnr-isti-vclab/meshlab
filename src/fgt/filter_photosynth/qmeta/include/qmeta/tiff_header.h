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
// This file defines the TiffHeader class which is used for TIFF files and
// Exif metadata embeded in JPEG files. Both the Tiff and the Jpeg classes
// should instantiate the TiffHeader object and pass it to the corresponded
// Exif object.

#ifndef QMETA_TIFF_HEADER_H_
#define QMETA_TIFF_HEADER_H_

#include <QHash>
#include <QObject>

#include "qmeta/identifiers.h"

class QIODevice;

namespace qmeta {

class TiffHeader : public QObject {
  Q_OBJECT

 public:
  // The value data types used in TIFF header.
  enum Type {
    // An 8-bit unsigned integer.
    kByteType = 1,
    // An 8-bit byte containing one 7-bit ASCII code. The final byte is
    // terminated with NULL.
    kAsciiType = 2,
    // A 16-bit (2-byte) unsigned integer.
    kShortType = 3,
    // A 32-bit (4-byte) unsigned integer.
    kLongType = 4,
    // Two LONGs. The first LONG is the numerator and the second LONG expresses
    // the denominator.
    kRationalType = 5,
    // An 8-bit signed integer.
    kSByte = 6,
    // An 8-bit byte that can take any value depending ont he field definition.
    kUndefinedType = 7,
    // A 16-bit signed integer.
    kSShort = 8,
    // A 32-bit (4-byte) signed integer (2's complement notation)
    kSLongType = 9,
    // Two SLONGs. The first SLONG is the numerator and the second SLONG is the
    // denominator.
    kSRationalType = 10,
    // A 4-byte IEEE floating point value.
    kFloat = 11,
    // An 8-byte IEEE floating point value.
    kDouble = 12,
  };

  explicit TiffHeader(QObject *parent = NULL);
  bool HasNextIfdEntry();
  int IfdEntryTag(qint64 ifd_entry_offset);
  Type IfdEntryType(qint64 ifd_entry_offset);
  QByteArray IfdEntryValue(qint64 ifd_entry_offset);
  qint64 IfdEntryOffset(qint64 ifd_entry_offset);
  bool Init(QIODevice *file, qint64 file_start_offset);
  qint64 NextIfdEntryOffset();
  void ToFirstIfd();
  void ToIfd(qint64 offset);

  qint64 current_ifd_offset() const { return current_ifd_offset_; }
  qint64 file_start_offset() const { return file_start_offset_; }

 private:
  void InitTypeByteUnit();
  QByteArray ReadFromFile(const int max_size);

  int current_entry_count() const { return current_entry_count_; }
  void set_current_entry_count(int count) { current_entry_count_ = count; }
  int current_entry_number() const { return current_entry_number_; }
  void set_current_entry_number(int number) { current_entry_number_ = number; }
  void set_current_ifd_offset(qint64 offset) { current_ifd_offset_ = offset; }
  Endianness endianness() const { return endianness_; }
  void set_endianness(Endianness endian) { endianness_ = endian; }
  QIODevice* file() const { return file_; }
  void set_file(QIODevice *file) { file_ = file; }
  void set_file_start_offset(qint64 offset) { file_start_offset_ = offset; }
  int first_ifd_offset() const { return first_ifd_offset_; }
  void set_first_ifd_offset(int offset) { first_ifd_offset_ = offset; }
  QHash<Type, int> type_byte_unit() const { return type_byte_unit_; }
  void set_type_byte_unit(QHash<Type, int> unit) { type_byte_unit_ = unit; }

  // The count of directory entries of the current IFD.
  int current_entry_count_;
  // The number of current directory entry in the IFD.
  int current_entry_number_;
  // Tracks the beginning offset of current IFD.
  int current_ifd_offset_;
  // The byte order of the TIFF file.
  Endianness endianness_;
  // The tracked file.
  QIODevice *file_;
  // The beginning offset of the TIFF header in the tracked file.
  qint64 file_start_offset_;
  // The offset of the first IFD.
  int first_ifd_offset_;
  // The byte unit for each entry type.
  QHash<Type, int> type_byte_unit_;
};

}  // namespace qmeta

#endif  // QMETA_TIFF_HEADER_H_
