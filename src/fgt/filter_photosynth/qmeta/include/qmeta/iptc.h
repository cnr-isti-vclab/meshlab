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
// This file defines the Iptc class.

#ifndef QMETA_IPTC_H_
#define QMETA_IPTC_H_

#include <QHash>

#include "standard.h"

class QIODevice;

namespace qmeta {

class Iptc : public Standard {
  Q_OBJECT

 public:
  // Only contains the list of valid editorial IPTC tags (2:xx).
  enum Tag {
    kRecordVersion = 0,
    kObjectTypeReference = 3,
    kObjectAttributeReference = 4,
    kObjectName = 5,
    kEditStatus = 7,
    kEditorialUpdate = 8,
    kUrgency = 10,
    kSubjectReference = 12,
    kCategory = 15,
    kSupplementalCategory = 20,
    kFixtureIdentifier = 22,
    kKeywords = 25,
    kContentLocationCode = 26,
    kContentLocationName = 27,
    kReleaseDate = 30,
    kReleaseTime = 35,
    kExpirationDate = 37,
    kExpirationTime = 38,
    kSpecialInstructions = 40,
    kActionAdvised = 42,
    kReferenceService = 45,
    kReferenceDate = 47,
    kReferenceNumber = 50,
    kDateCreated = 55,
    kTimeCreated = 60,
    kDigitalCreationDate = 62,
    kDigitalCreationTime = 63,
    kOriginatingProgram = 65,
    kProgramVersion = 70,
    kObjectCycle = 75,
    kByLine = 80,
    kByLineTitle = 85,
    kCity = 90,
    kSubLocation = 92,
    kProvinceState = 95,
    kCountryPrimaryLocationCode = 100,
    kCountryPrimaryLocationName = 101,
    kOriginalTransmissionReference = 103,
    kHeadline = 105,
    kCredit = 110,
    kSource = 115,
    kCopyrightNotice = 116,
    kContact = 118,
    kCaptionAbstract = 120,
    kWriterEditor = 122,
    kRasterizedCaption = 125,
    kImageType = 130,
    kImageOrientation = 131,
    kLanguageIdentifier = 135,
    kAudioType = 150,
    kAudioSamplingRate = 151,
    kAudioSamplingResolution = 152,
    kAudioDuration = 153,
    kAudioOutcue = 154,
    kObjDataPreviewFileFormat = 200,
    kObjDataPreviewFileFormatVer = 201,
    kObjDataPreviewData = 202,
  };

  explicit Iptc(QObject *parent = NULL);
  bool Init(QIODevice *file, const qint64 file_start_offset);
  QByteArray Value(Tag tag);
  QList<QByteArray> Values(Tag tag);

  QHash<Tag, QString> tag_names() const { return tag_names_; }

 private:
  void InitRepeatableTags();
  void InitTagNames();
  QByteArray ReadDataSet(int offset);
  bool ReadRecord();

  QList<Tag> repeatable_tags() const { return repeatable_tags_; }
  void set_repeatable_tags(QList<Tag> tags) { repeatable_tags_ = tags; }
  void set_tag_names(QHash<Tag, QString> names) { tag_names_ = names; }
  QHash<Tag, qint64> tag_offsets() const { return tag_offsets_; }
  void set_tag_offsets(QHash<Tag, qint64> offsets) { tag_offsets_ = offsets; }

  // Records the repeatable tags.
  QList<Tag> repeatable_tags_;
  // The tag names to read for human.
  QHash<Tag, QString> tag_names_;
  // Records offsets of tags used in IPTC.
  QHash<Tag, qint64> tag_offsets_;
};

}  // namespace qmeta

#endif  // QMETA_IPTC_H_
