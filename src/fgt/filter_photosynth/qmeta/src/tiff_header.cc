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
// This file implements the detail of the TiffHeader class.

#include "qmeta/tiff_header.h"

#include <QtCore>
#include <qitty/byte_array.h>

namespace qmeta {

TiffHeader::TiffHeader(QObject *parent) : QObject(parent) {
  InitTypeByteUnit();
}

// Returns true if next IFD entry exists.
bool TiffHeader::HasNextIfdEntry() {
  if (current_entry_number() < current_entry_count())
    return true;
  else
    return false;
}

// Returns the Tag of the entry at the specified entry_offset in decimal.
int TiffHeader::IfdEntryTag(qint64 ifd_entry_offset) {
  file()->seek(ifd_entry_offset);
  return ReadFromFile(2).toHex().toInt(NULL, 16);
}

// Returns the Type of the entry at the specified entry_offset.
TiffHeader::Type TiffHeader::IfdEntryType(qint64 ifd_entry_offset) {
  file()->seek(ifd_entry_offset + 2);
  return static_cast<Type>(ReadFromFile(2).toHex().toInt(NULL, 16));
}

// Returns the value of the entry at the specified entry_offset. Note that
// the returned value is always in the big-endian byte order.
QByteArray TiffHeader::IfdEntryValue(qint64 ifd_entry_offset) {
  Type type = IfdEntryType(ifd_entry_offset);
  int count = ReadFromFile(4).toHex().toInt(NULL, 16);
  // Retrieves the byte unit of the specified type.
  int current_type_byte_unit = type_byte_unit().value(type);
  // Calculates the number of bytes used for the entry value.
  int value_byte_count = current_type_byte_unit * count;
  // Jumps to the offset containing the entry value if the byte count > 4.
  if (value_byte_count > 4) {
    qint64 offset = ReadFromFile(4).toHex().toInt(NULL, 16) +
                    file_start_offset();
    file()->seek(offset);
  }

  QByteArray value;
  if (current_type_byte_unit == 1) {
    value = file()->read(value_byte_count);
  } else {
    for (int i = 0; i < count; ++i) {
      // If the specified type is RATIONAL or SRATIONAL, read 4 bytes twice
      // for double LONG or double SLONG, respectively.
      if (current_type_byte_unit == 8) {
        value.append(ReadFromFile(4));
        value.append(ReadFromFile(4));
      } else {
        value.append(ReadFromFile(current_type_byte_unit));
      }
    }
  }
  return value;
}

// Returns the value offset for the IFD entry at the specified ifd_entry_offset.
// Returns -1 if the if the value is not an offset.
qint64 TiffHeader::IfdEntryOffset(qint64 ifd_entry_offset) {
  Type type = IfdEntryType(ifd_entry_offset);
  int count = ReadFromFile(4).toHex().toInt(NULL, 16);
  // Retrieves the byte unit of the specified type.
  int current_type_byte_unit = type_byte_unit().value(type);
  // Calculates the number of bytes used for the entry value.
  int value_byte_count = current_type_byte_unit * count;
  // Jumps to the offset containing the entry value if the byte count > 4.
  if (value_byte_count > 4) {
    qint64 offset = ReadFromFile(4).toHex().toInt(NULL, 16) +
                    file_start_offset();
    return offset;
  } else {
    return -1;
  }
}

// Initializes the TiffHeader object. Returns true if successful.
bool TiffHeader::Init(QIODevice *file, qint64 file_start_offset) {
  set_file(file);

  file->seek(file_start_offset);
  // Determines the byte order in the specified file.
  QByteArray byte_order = file->read(2);
  if (byte_order == "II")
    set_endianness(kLittleEndians);
  else if (byte_order == "MM")
    set_endianness(kBigEndians);
  else
    return false;

  // Further identifies whether the specified file has a valid TIFF header.
  // Reads the next two bytes which should have the value of 42 in decimal.
  if (ReadFromFile(2).toHex().toInt(NULL, 16) != 42)
    return false;

  // Reads the next four bytes to determine the offset of the first IFD.
  int first_ifd_offset = ReadFromFile(4).toHex().toInt(NULL, 16);
  // Sets the offset to the current position if the read value equals to 8.
  // The reason is for JPEG files, if the TIFF header is followed immediately
  // by the first IFD, it is written as 00000008 in hexidecimal.
  if (first_ifd_offset == 8)
    first_ifd_offset = file->pos();

  // Sets properties.
  set_file_start_offset(file_start_offset);
  set_first_ifd_offset(first_ifd_offset);
  // Jumps to the first IFD by default.
  ToFirstIfd();
  return true;
}

// Initializes the type_byte_unit_ property.
void TiffHeader::InitTypeByteUnit() {
  QHash<Type, int> type_byte_unit;
  type_byte_unit.insert(kByteType, 1);
  type_byte_unit.insert(kAsciiType, 1);
  type_byte_unit.insert(kShortType, 2);
  type_byte_unit.insert(kLongType, 4);
  type_byte_unit.insert(kRationalType, 8);
  type_byte_unit.insert(kSByte, 1);
  type_byte_unit.insert(kUndefinedType, 1);
  type_byte_unit.insert(kSShort, 2);
  type_byte_unit.insert(kSLongType, 4);
  type_byte_unit.insert(kSRationalType, 8);
  type_byte_unit.insert(kFloat, 4);
  type_byte_unit.insert(kDouble, 8);
  set_type_byte_unit(type_byte_unit);
}

// Returns the offset of the next IFD entry and increases the entry offset.
// Returns -1 if there is no further IFD entry.
qint64 TiffHeader::NextIfdEntryOffset() {
  if (current_entry_number() == current_entry_count())
    return -1;

  qint64 entry_offset = current_ifd_offset() + 2 + current_entry_number() * 12;
  // Increases the current entry number by 1.
  set_current_entry_number(current_entry_number() + 1);
  // If already reaches the end of the current IFD. Jumps to the next IFD if
  // available.
  if (current_entry_number() == current_entry_count()) {
    file()->seek(current_ifd_offset() + 2 + current_entry_count() * 12);
    qint64 next_ifd_offset = ReadFromFile(4).toHex().toInt(NULL, 16);
    if (next_ifd_offset != 0)
      ToIfd(next_ifd_offset + file_start_offset());
  }
  return entry_offset;
}

// Reads at most max_size bytes from the tracked file object, and returns the
// data read as a QByteArray in big-endian byte order.
QByteArray TiffHeader::ReadFromFile(const int max_size) {
  QByteArray data = file()->read(max_size);
  if (endianness() == kLittleEndians)
    data = qitty_utils::ReverseByteArray(data);
  return data;
}

// Jumps to the offset of the first IFD and sets the current_entry_number_ and
// the entry_count_ properties.
void TiffHeader::ToFirstIfd() {
  ToIfd(first_ifd_offset());
}

// Jumps to the IFD at specified offset and sets the current_entry_number_ and
// the entry_count_ properties. The specified offset must point to the beginning
// of a valid IFD.
void TiffHeader::ToIfd(qint64 offset) {
  file()->seek(offset);
  set_current_ifd_offset(offset);
  set_current_entry_count(ReadFromFile(2).toHex().toInt(NULL, 16));
  set_current_entry_number(0);
}

}  // namespace qmeta
