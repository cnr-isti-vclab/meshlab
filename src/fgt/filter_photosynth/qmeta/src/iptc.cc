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
// This file implements the detail of the Iptc class.

#include "qmeta/iptc.h"

#include <QtCore>

namespace qmeta {

Iptc::Iptc(QObject *parent) : Standard(parent) {
  InitRepeatableTags();
  InitTagNames();
}

// Initializes the IPTC object. Returns false if no valid tag is found.
bool Iptc::Init(QIODevice *file, const qint64 file_start_offset) {
  set_file(file);
  set_file_start_offset(file_start_offset);
  return ReadRecord();
}

// Adds all repeatable tags to the repeatable_tags_ property.
void Iptc::InitRepeatableTags() {
  QList<Tag> repeatable_tags;
  repeatable_tags.append(kObjectAttributeReference);
  repeatable_tags.append(kSubjectReference);
  repeatable_tags.append(kSupplementalCategory);
  repeatable_tags.append(kKeywords);
  repeatable_tags.append(kContentLocationCode);
  repeatable_tags.append(kContentLocationName);
  repeatable_tags.append(kReferenceService);
  repeatable_tags.append(kReferenceDate);
  repeatable_tags.append(kReferenceNumber);
  repeatable_tags.append(kByLine);
  repeatable_tags.append(kByLineTitle);
  repeatable_tags.append(kContact);
  repeatable_tags.append(kWriterEditor);
  set_repeatable_tags(repeatable_tags);
}

// Initializes tag names used in IPTC.
void Iptc::InitTagNames() {
  QHash<Tag, QString> tag_names;
  tag_names.insert(kRecordVersion, tr("Record Version"));
  tag_names.insert(kObjectTypeReference, tr("Object Type Reference"));
  tag_names.insert(kObjectAttributeReference, tr("Object Attribute Reference"));
  tag_names.insert(kObjectName, tr("Object Name"));
  tag_names.insert(kEditStatus, tr("Edit Status"));
  tag_names.insert(kEditorialUpdate, tr("Editorial Update"));
  tag_names.insert(kUrgency, tr("Urgency"));
  tag_names.insert(kSubjectReference, tr("Subject Reference"));
  tag_names.insert(kCategory, tr("Category"));
  tag_names.insert(kSupplementalCategory, tr("Supplemental Category"));
  tag_names.insert(kFixtureIdentifier, tr("Fixture Identifier"));
  tag_names.insert(kKeywords, tr("Keywords"));
  tag_names.insert(kContentLocationCode, tr("Content Location Code"));
  tag_names.insert(kContentLocationName, tr("Content Location Name"));
  tag_names.insert(kReleaseDate, tr("Release Date"));
  tag_names.insert(kReleaseTime, tr("Release Time"));
  tag_names.insert(kExpirationDate, tr("Expiration Date"));
  tag_names.insert(kExpirationTime, tr("Expiration Time"));
  tag_names.insert(kSpecialInstructions, tr("Special Instructions"));
  tag_names.insert(kActionAdvised, tr("Action Advised"));
  tag_names.insert(kReferenceService, tr("Reference Service"));
  tag_names.insert(kReferenceDate, tr("Reference Date"));
  tag_names.insert(kReferenceNumber, tr("Reference Number"));
  tag_names.insert(kDateCreated, tr("Date Created"));
  tag_names.insert(kTimeCreated, tr("Time Created"));
  tag_names.insert(kDigitalCreationDate, tr("Digital Creation Date"));
  tag_names.insert(kDigitalCreationTime, tr("Digital Creation Time"));
  tag_names.insert(kOriginatingProgram, tr("Originating Program"));
  tag_names.insert(kProgramVersion, tr("Program Version"));
  tag_names.insert(kObjectCycle, tr("Object Cycle"));
  tag_names.insert(kByLine, tr("By Line"));
  tag_names.insert(kByLineTitle, tr("By Line Title"));
  tag_names.insert(kCity, tr("City"));
  tag_names.insert(kSubLocation, tr("Sub Location"));
  tag_names.insert(kProvinceState, tr("Province State"));
  tag_names.insert(kCountryPrimaryLocationCode,
                   tr("Country Primary Location Code"));
  tag_names.insert(kCountryPrimaryLocationName,
                   tr("Country Primary Location Name"));
  tag_names.insert(kOriginalTransmissionReference,
                   tr("Original Transmission Reference"));
  tag_names.insert(kHeadline, tr("Headline"));
  tag_names.insert(kCredit, tr("Credit"));
  tag_names.insert(kSource, tr("Source"));
  tag_names.insert(kCopyrightNotice, tr("Copyright Notice"));
  tag_names.insert(kContact, tr("Contact"));
  tag_names.insert(kCaptionAbstract, tr("Caption Abstract"));
  tag_names.insert(kWriterEditor, tr("Writer Editor"));
  tag_names.insert(kRasterizedCaption, tr("Rasterized Caption"));
  tag_names.insert(kImageType, tr("Image Type"));
  tag_names.insert(kImageOrientation, tr("Image Orientation"));
  tag_names.insert(kLanguageIdentifier, tr("Language Identifier"));
  tag_names.insert(kAudioType, tr("Audio Type"));
  tag_names.insert(kAudioSamplingRate, tr("Audio Sampling Rate"));
  tag_names.insert(kAudioSamplingResolution, tr("Audio Sampling Resolution"));
  tag_names.insert(kAudioDuration, tr("Audio Duration"));
  tag_names.insert(kAudioOutcue, tr("Audio Outcue"));
  tag_names.insert(kObjDataPreviewFileFormat,
                   tr("Object Data Preview File Format"));
  tag_names.insert(kObjDataPreviewFileFormatVer,
                   tr("Object Data Preview File Format Version"));
  tag_names.insert(kObjDataPreviewData, tr("Object Data Preview Data"));
  set_tag_names(tag_names);
}

// Reads the DataSet started from the specified offset.
QByteArray Iptc::ReadDataSet(int offset) {
  QByteArray data;
  file()->seek(offset);
  Tag tag = static_cast<Tag>(file()->read(1).toHex().toInt(NULL, 16));
  if (!tag_names().contains(tag))
    return data;
  int size = file()->read(2).toHex().toInt(NULL, 16);
  data = file()->read(size);
  return data;
}

// Reads the Record started from the specified offset. Also saves all found
// tags and their offsets in the tag_offsets_ property. Returns false if
// no tag is found.
bool Iptc::ReadRecord() {
  file()->seek(file_start_offset());
  QHash<Tag, qint64> tag_offsets;
  while (file()->read(2).toHex() == "1c02") {
    qint64 tag_offset = file()->pos();
    Tag tag = static_cast<Tag>(file()->read(1).toHex().toInt(NULL, 16));
    QByteArray data = ReadDataSet(tag_offset);

    if (repeatable_tags().contains(tag))
      tag_offsets.insertMulti(tag, tag_offset);
    else
      tag_offsets.insert(tag, tag_offset);
  }
  // Returns false if no valid tag is found.
  if (tag_offsets.count() == 0)
    return false;

  set_tag_offsets(tag_offsets);
  return true;
}

// Returns the value associated with the specified tag.
QByteArray Iptc::Value(Tag tag) {
  QByteArray value;
  if (tag_offsets().contains(tag)) {
    int offset = tag_offsets().value(tag);
    value = ReadDataSet(offset);
  }
  return value;
}

// Returns a lit containing all the values associated with the specified tag.
QList<QByteArray> Iptc::Values(Tag tag) {
  QList<QByteArray> values;
  QList<qint64> offsets = tag_offsets().values(tag);
  for (int i = 0; i < offsets.count(); ++i) {
    qint64 offset = offsets.at(i);
    QByteArray value = ReadDataSet(offset);
    values.append(value);
  }
  qSort(values);
  return values;
}

}  // namespace qmeta
