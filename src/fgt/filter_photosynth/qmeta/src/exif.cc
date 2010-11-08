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
// This file implements the detail of the Exif class.

#include "qmeta/exif.h"

#include <QtCore>
#include <qitty/byte_array.h>

#include "qmeta/tiff_header.h"

namespace qmeta {

Exif::Exif(QObject *parent) : Standard(parent) {
  InitTagNames();
  QHash<Tag, qint64> tag_offsets;
  set_tag_offsets(tag_offsets);
}

// Initializes the Exif object.
bool Exif::Init(QIODevice *file, TiffHeader *tiff_header) {
  set_file(file);
  set_tiff_header(tiff_header);
  tiff_header->ToFirstIfd();
  ReadIfds(tiff_header->current_ifd_offset());
  if (tag_offsets().count() == 0)
    return false;
  else
    return true;
}

// Initializes tag names used in Exif.
void Exif::InitTagNames() {
  QHash<Tag, QString> tag_names;
  // Exif-specific IFD.
  tag_names.insert(kExifIfdPointer, tr("Exif IFD Pointer"));
  tag_names.insert(kGpsInfoIfdPointer, tr("GPS Info IFD Pointer"));
  tag_names.insert(kInteroperabilityIfdPointer,
                   tr("Interoperability IFD Pointer"));
  // TIFF Rev. 6.0 attribute information used in Exif.
  tag_names.insert(kImageWidth, tr("Image Width"));
  tag_names.insert(kImageLength, tr("Image Height"));
  tag_names.insert(kBitsPerSample, tr("Bits Per Sample"));
  tag_names.insert(kCompression, tr("Compression"));
  tag_names.insert(kPhotometricInterpretation,
                   tr("Photometric Interpretation"));
  tag_names.insert(kOrientation, tr("Orientation"));
  tag_names.insert(kSamplesPerPixel, tr("Samples Per Pixel"));
  tag_names.insert(kPlanarConfiguration, tr("Planar Configuration"));
  tag_names.insert(kYCbCrSubSampling, tr("YCbCr Sub Sampling"));
  tag_names.insert(kYCbCrPositioning, tr("YCbCr Positioning"));
  tag_names.insert(kXResolution, tr("X-Resolution"));
  tag_names.insert(kYResolution, tr("Y-Resolution"));
  tag_names.insert(kResolutionUnit, tr("Resolution Unit"));
  tag_names.insert(kStripOffsets, tr("Strip Offsets"));
  tag_names.insert(kRowsPerStrip, tr("Rows Per Strip"));
  tag_names.insert(kStripByteCounts, tr("Strip Byte Counts"));
  tag_names.insert(kJPEGInterchangeFormat, tr("JPEG Interchange Format"));
  tag_names.insert(kJPEGInterchangeFormatLength,
                   tr("JPEG Interchange Format Length"));
  tag_names.insert(kTransferFunction, tr("Transfer Function"));
  tag_names.insert(kWhitePoint, tr("White Point"));
  tag_names.insert(kPrimaryChromaticities, tr("Primary Chromaticities"));
  tag_names.insert(kYCbCrCoefficients, tr("YCbCr Coefficients"));
  tag_names.insert(kReferenceBlackWhite, tr("Reference Black White"));
  tag_names.insert(kDateTime, tr("Date Time"));
  tag_names.insert(kImageDescription, tr("Image Description"));
  tag_names.insert(kMake, tr("Make"));
  tag_names.insert(kModel, tr("Model"));
  tag_names.insert(kSoftware, tr("Software"));
  tag_names.insert(kArtist, tr("Artist"));
  tag_names.insert(kCopyright, tr("Copyright"));
  // The attribute information recorded in the Exif IFD.
  tag_names.insert(kExifVersion, tr("Exif Version"));
  tag_names.insert(kFlashpixVersion, tr("Flashpix Version"));
  tag_names.insert(kColorSpace, tr("Color Space"));
  tag_names.insert(kComponentsConfiguration, tr("Components Configuration"));
  tag_names.insert(kCompressedBitsPerPixel, tr("Compressed Bits Per Pixel"));
  tag_names.insert(kPixelXDimension, tr("Pixel X Dimension"));
  tag_names.insert(kPixelYDimension, tr("Pixel Y Dimension"));
  tag_names.insert(kMakerNote, tr("Maker Note"));
  tag_names.insert(kUserComment, tr("User Comment"));
  tag_names.insert(kRelatedSoundFile, tr("Related Sound File"));
  tag_names.insert(kDateTimeOriginal, tr("Date Time Original"));
  tag_names.insert(kDateTimeDigitized, tr("Date Time Digitized"));
  tag_names.insert(kSubSecTime, tr("Sub Sec Time"));
  tag_names.insert(kSubSecTimeOriginal, tr("Sub Sec Time Original"));
  tag_names.insert(kSubSecTimeDigitized, tr("Sub Sec Time Digitized"));
  tag_names.insert(kExposureTime, tr("Exposure Time"));
  tag_names.insert(kFNumber, tr("F Number"));
  tag_names.insert(kExposureProgram, tr("Exposure Program"));
  tag_names.insert(kSpectralSensitivity, tr("Spectral Sensitivity"));
  tag_names.insert(kISOSpeedRatings, tr("ISO Speed Rating"));
  tag_names.insert(kOECF, tr("Optoelectric conversion factor"));
  tag_names.insert(kShutterSpeedValue, tr("Shutter Speed"));
  tag_names.insert(kApertureValue, tr("Aperture Value"));
  tag_names.insert(kBrightnessValue, tr("Brightness"));
  tag_names.insert(kExposureBiasValue, tr("Exposure Bias"));
  tag_names.insert(kMaxApertureValue, tr("Maximum Lens Aperture"));
  tag_names.insert(kSubjectDistance, tr("Subject Distance"));
  tag_names.insert(kMeteringMode, tr("Metering Mode"));
  tag_names.insert(kLightSource, tr("Light Source"));
  tag_names.insert(kFlash, tr("Flash"));
  tag_names.insert(kFocalLength, tr("Focal Length"));
  tag_names.insert(kSubjectArea, tr("Subject Area"));
  tag_names.insert(kFlashEnergy, tr("Flash Energy"));
  tag_names.insert(kSpatialFrequencyResponse,
                   tr("Spatial Frequency Response"));
  tag_names.insert(kFocalPlaneXResolution, tr("Focal Plane X Resolution"));
  tag_names.insert(kFocalPlaneYResolution, tr("Focal Plane Y Resolution"));
  tag_names.insert(kFocalPlaneResolutionUnit,
                   tr("Focal Plane Resolution Unit"));
  tag_names.insert(kSubjectLocation, tr("Subject Location"));
  tag_names.insert(kExposureIndex, tr("Exposure Index"));
  tag_names.insert(kSensingMethod, tr("Sensing Method"));
  tag_names.insert(kFileSource, tr("File Source"));
  tag_names.insert(kSceneType, tr("Scene Type"));
  tag_names.insert(kCFAPattern, tr("CFA Pattern"));
  tag_names.insert(kCustomRendered, tr("Custom Rendered"));
  tag_names.insert(kExposureMode, tr("Exposure Mode"));
  tag_names.insert(kWhiteBalance, tr("White Balance"));
  tag_names.insert(kDigitalZoomRatio, tr("Digital Zoom Ratio"));
  tag_names.insert(kFocalLengthIn35mmFilm, tr("Focal Length In 35mm Film"));
  tag_names.insert(kSceneCaptureType, tr("Scene Capture Type"));
  tag_names.insert(kGainControl, tr("Gain Control"));
  tag_names.insert(kContrast, tr("Contrast"));
  tag_names.insert(kSaturation, tr("Saturation"));
  tag_names.insert(kSharpness, tr("Sharpness"));
  tag_names.insert(kDeviceSettingDescription,
                   tr("Device Setting Description"));
  tag_names.insert(kSubjectDistanceRange, tr("Subject Distance Range"));
  tag_names.insert(kImageUniqueID, tr("Image Unique ID"));
  // The attribute information recorded in the GPS Info IFD.
  tag_names.insert(kGPSVersionID, tr("GPS Version ID"));
  tag_names.insert(kGPSLatitudeRef, tr("GPS Latitude Ref"));
  tag_names.insert(kGPSLatitude, tr("GPS Latitude"));
  tag_names.insert(kGPSLongitudeRef, tr("GPS Longitude Ref"));
  tag_names.insert(kGPSLongitude, tr("GPS Longitude"));
  tag_names.insert(kGPSAltitudeRef, tr("GPS Altitude Ref"));
  tag_names.insert(kGPSAltitude, tr("GPS Altitude"));
  tag_names.insert(kGPSTimeStamp, tr("GPS Time Stamp"));
  tag_names.insert(kGPSSatellites, tr("GPS Satellites"));
  tag_names.insert(kGPSStatus, tr("GPS Status"));
  tag_names.insert(kGPSMeasureMode, tr("GPS Measure Mode"));
  tag_names.insert(kGPSDOP, tr("GPS DOP"));
  tag_names.insert(kGPSSpeedRef, tr("GPS Speed Ref"));
  tag_names.insert(kGPSSpeed, tr("GPS Speed"));
  tag_names.insert(kGPSTrackRef, tr("GPS Track Ref"));
  tag_names.insert(kGPSTrack, tr("GPS Track"));
  tag_names.insert(kGPSImgDirectionRef, tr("GPS Img Direction Ref"));
  tag_names.insert(kGPSImgDirection, tr("GPS Img Direction"));
  tag_names.insert(kGPSMapDatum, tr("GPS Map Datum"));
  tag_names.insert(kGPSDestLatitudeRef, tr("GPS Dest Latitude Ref"));
  tag_names.insert(kGPSDestLatitude, tr("GPS Dest Latitude"));
  tag_names.insert(kGPSDestLongitudeRef, tr("GPS Dest Longitude Ref"));
  tag_names.insert(kGPSDestLongitude, tr("GPS Dest Longitude"));
  tag_names.insert(kGPSDestBearingRef, tr("GPS Dest Bearing Ref"));
  tag_names.insert(kGPSDestBearing, tr("GPS Dest Bearing"));
  tag_names.insert(kGPSDestDistanceRef, tr("GPS Dest Distance Ref"));
  tag_names.insert(kGPSDestDistance, tr("GPS Dest Distance"));
  tag_names.insert(kGPSProcessingMethod, tr("GPS Processing Method"));
  tag_names.insert(kGPSAreaInformation, tr("GPS Area Information"));
  tag_names.insert(kGPSDateStamp, tr("GPS Date Stamp"));
  tag_names.insert(kGPSDifferential, tr("GPS Differential"));
  set_tag_names(tag_names);
}

// Reads all IFDs from the specified ifd_offset and saved
void Exif::ReadIfds(int ifd_offset) {
  tiff_header()->ToIfd(ifd_offset);

  QList<qint64> ifd_offsets;
  // Reads a sequence of 12-byte field entries.
  while (tiff_header()->HasNextIfdEntry()) {
    qint64 ifd_entry_offset = tiff_header()->NextIfdEntryOffset();
    Tag tag = static_cast<Tag>(tiff_header()->IfdEntryTag(ifd_entry_offset));

    if (!tag_names().contains(tag))
      continue;

    QHash<Tag, qint64> offsets = tag_offsets();
    offsets.insert(tag, ifd_entry_offset);
    set_tag_offsets(offsets);

    if (tag == kExifIfdPointer || tag == kGpsInfoIfdPointer) {
      QByteArray entry_value = tiff_header()->IfdEntryValue(ifd_entry_offset);
      qint64 ifd_pointer_offset = entry_value.toHex().toUInt(NULL, 16) +
                                  tiff_header()->file_start_offset();
      ifd_offsets.append(ifd_pointer_offset);
    }
  }
  for (int i = 0; i < ifd_offsets.count(); ++i) {
    ReadIfds(ifd_offsets.at(i));
  }
}

// Returns the byte data of the thumbnail saved in Exif.
QByteArray Exif::Thumbnail() {
  QByteArray thumbnail;
  quint32 thumbnail_offset = Value(kJPEGInterchangeFormat).ToUInt();
  quint32 length = Value(kJPEGInterchangeFormatLength).ToUInt();
  if (thumbnail_offset && length) {
    thumbnail_offset += tiff_header()->file_start_offset();
    file()->seek(thumbnail_offset);
    thumbnail = file()->read(length);
  }
  return thumbnail;
}

// Returns the value of the specified tag as a ExifData.
ExifData Exif::Value(Tag tag) {
  QByteArray value;
  if (tag_offsets().contains(tag))
    value = tiff_header()->IfdEntryValue(tag_offsets().value(tag));
  ExifData exif_data(value);
  return exif_data;
}

}  // namespace qmeta
