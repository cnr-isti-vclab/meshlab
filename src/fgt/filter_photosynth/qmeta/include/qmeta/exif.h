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
// This file defines the Exif class.

#ifndef QMETA_EXIF_H_
#define QMETA_EXIF_H_

#include <QHash>

#include "qmeta/exif_data.h"
#include "qmeta/identifiers.h"
#include "qmeta/standard.h"

class QIODevice;

namespace qmeta {

class TiffHeader;

class Exif : public Standard {
  Q_OBJECT

 public:
  enum Tag {
    // Exif-specific IFD.
    kExifIfdPointer = 34665,
    kGpsInfoIfdPointer = 34853,
    kInteroperabilityIfdPointer = 40965,

    // TIFF Rev. 6.0 attribute information used in Exif, represented in decimal.
    // Tags relating to image data structure.
    kImageWidth = 256,  // Image width
    kImageLength = 257,  // Image height
    kBitsPerSample  = 258,  // Number of bits per component
    kCompression = 259,  // Compression scheme
    kPhotometricInterpretation = 262,  // Pixel composition
    kOrientation = 274,  // Orientation of image
    kSamplesPerPixel = 277,  // Number of components
    kPlanarConfiguration = 284,  // Image data arrangement
    kYCbCrSubSampling = 530,  // Subsampling ratio of Y to C
    kYCbCrPositioning = 531,  // Y and C positioning
    kXResolution = 282,  // Image resolution in width direction
    kYResolution = 283,  // Image resolution in height direction
    kResolutionUnit = 296,  // Unit of X and Y resolution
    // Tags relating to recording offset.
    kStripOffsets = 273,  // Image data location
    kRowsPerStrip = 278,  // Number of rows per strip
    kStripByteCounts = 279,  // Bytes per compressed strip
    kJPEGInterchangeFormat = 513,  // Offset to JPEG SOI
    kJPEGInterchangeFormatLength = 514,  // Bytes of JPEG data
    // Tags relating to image data characteristics.
    kTransferFunction = 301,  // Transfer function
    kWhitePoint = 318,  // White point chromaticity
    kPrimaryChromaticities = 319,  // Chromaticities of primaries
    kYCbCrCoefficients = 529,  // Color space transformation matrix coefficients
    kReferenceBlackWhite = 532,  // Pair of black and white reference values
    // Other tags.
    kDateTime = 306,  // File change date and time
    kImageDescription = 270,  // Image title
    kMake = 271,  // Image input equipment manufacturer
    kModel = 272,  // Image input equipment model
    kSoftware = 305,  // Software used
    kArtist = 315,  // Person who created the image
    kCopyright = 33432,  // Copyright holder

    // The attribute information recorded in the Exif IFD.
    // Tags relating to version.
    kExifVersion = 36864,  // Exif version
    kFlashpixVersion = 40960,  // Supported Flashpix version
    // Tag relating to image data characteristics.
    kColorSpace = 40961,  // Color space information
    // Tags relating to image configuration.
    kComponentsConfiguration = 37121,  // Meaning of each component
    kCompressedBitsPerPixel = 37122,  // Image compression mode
    kPixelXDimension = 40962,  // Valid image width
    kPixelYDimension = 40963,  // Valid image height
    // Tags relating to user information.
    kMakerNote = 37500,  // Manufacturer notes
    kUserComment = 37510,  // User comments
    // Tags relating to related file information.
    kRelatedSoundFile = 40964,
    // Tags relating to date and time.
    kDateTimeOriginal = 36867,  // Date and time of original data generation
    kDateTimeDigitized = 36868,  // Date and time of digital data generation
    kSubSecTime = 37520,  // Date Time subseconds
    kSubSecTimeOriginal = 37521,  // Date Time Original subseconds
    kSubSecTimeDigitized = 37522,  // Date Time Digitized subseconds
    // Tags relating to picture-taking conditions.
    kExposureTime = 33434,  // Exposure time
    kFNumber = 33437,  // F number
    kExposureProgram = 34850,  // Exposure program
    kSpectralSensitivity = 34852,  // Spectral sensitivity
    kISOSpeedRatings = 34855,  // ISO speed rating
    kOECF = 34856,  // Optoelectric conversion factor
    kShutterSpeedValue = 37377,  // Shutter speed
    kApertureValue = 37378,  // Aperture
    kBrightnessValue = 37379,  // Brightness
    kExposureBiasValue = 37380,  // Exposure bias
    kMaxApertureValue = 37381,  // Maximum lens aperture
    kSubjectDistance = 37382,  // Subject distance
    kMeteringMode = 37383,  // Metering mode
    kLightSource = 37384,  // Light source
    kFlash = 37385,  // Flash
    kFocalLength = 37386,  //Lens focal length
    kSubjectArea = 37396,  // Subject area
    kFlashEnergy = 41483,  // Flash energy
    kSpatialFrequencyResponse = 41484,  // Spatial frequency response
    kFocalPlaneXResolution = 41486,  // Focal plane X resolution
    kFocalPlaneYResolution = 41487,  // Focal plane Y resolution
    kFocalPlaneResolutionUnit = 41488,  // Focal plane resolution unit
    kSubjectLocation = 41492,  // Subject location
    kExposureIndex = 41493,  // Exposure index
    kSensingMethod = 41495,  // Sensing method
    kFileSource = 41728,  // File source
    kSceneType = 41729,  // Scene type
    kCFAPattern = 41730,  // CFA pattern
    kCustomRendered = 41985,  // Custom image processing
    kExposureMode = 41986,  // Exposure mode
    kWhiteBalance = 41987,  // White balance
    kDigitalZoomRatio = 41988,  //Digital zoom ratio
    kFocalLengthIn35mmFilm = 41989,  // Focal length in 35 mm film
    kSceneCaptureType = 41990,  // Scene capture type
    kGainControl = 41991,  // Gain control
    kContrast = 41992,  // Contrast
    kSaturation = 41993,  // Saturation
    kSharpness = 41994,  // Sharpness
    kDeviceSettingDescription = 41995,  // Device settings description
    kSubjectDistanceRange = 41996,  // Subject distance range
    // Other tags.
    kImageUniqueID = 42016,  // Unique image ID

    // The attribute information recorded in the GPS Info IFD.
    kGPSVersionID = 0,  // GPS tag version
    kGPSLatitudeRef = 1,  // North or South Latitude
    kGPSLatitude = 2,  // Latitude
    kGPSLongitudeRef = 3,  // East or West Longitude
    kGPSLongitude = 4,  // Longitude
    kGPSAltitudeRef = 5,  // Altitude reference
    kGPSAltitude = 6,  // Altitude
    kGPSTimeStamp = 7,  // GPS time (atomic clock)
    kGPSSatellites = 8,  // GPS satellites used for measurement
    kGPSStatus = 9,  // GPS receiver status
    kGPSMeasureMode = 10,  // GPS measurement mode
    kGPSDOP = 11,  // Measurement precision
    kGPSSpeedRef = 12,  // Speed unit
    kGPSSpeed = 13,  // Speed of GPS receiver
    kGPSTrackRef = 14,  // Reference for direction of movement
    kGPSTrack = 15,  // Direction of movement
    kGPSImgDirectionRef = 16,  // Reference for direction of image
    kGPSImgDirection = 17,  // Direction of image
    kGPSMapDatum = 18,  // Geodetic survey data used
    kGPSDestLatitudeRef = 19,  // Reference for latitude of destination
    kGPSDestLatitude = 20,  // Latitude of destination
    kGPSDestLongitudeRef = 21,  // Reference for longitude of destination
    kGPSDestLongitude = 22,  // Longitude of destination
    kGPSDestBearingRef = 23,  // Reference for longitude of destination
    kGPSDestBearing = 24,  // Bearing of destination
    kGPSDestDistanceRef = 25,  // Reference for distance to destination
    kGPSDestDistance = 26,  // Distance to destination
    kGPSProcessingMethod = 27,  // Name of GPS processing method
    kGPSAreaInformation = 28,  // Name of GPS area
    kGPSDateStamp = 29,  // GPS date
    kGPSDifferential = 30,  // GPS differential correction
  };

  explicit Exif(QObject *parent = NULL);
  bool Init(QIODevice *file, TiffHeader *tiff_header);
  QByteArray Thumbnail();
  ExifData Value(Tag tag);

  QHash<Tag, QString> tag_names() const { return tag_names_; }

 private:
  void InitTagNames();
  void ReadIfds(int ifd_offset);

  void set_tag_names(QHash<Tag, QString> names) { tag_names_ = names; }
  QHash<Tag, qint64> tag_offsets() const { return tag_offsets_; }
  void set_tag_offsets(QHash<Tag, qint64> offsets) { tag_offsets_ = offsets; }
  TiffHeader* tiff_header() const { return tiff_header_; }
  void set_tiff_header(TiffHeader *tiff_header) { tiff_header_ = tiff_header; }

  // The tag names to read for human.
  QHash<Tag, QString> tag_names_;
  // Records offsets of tags used in Exif.
  QHash<Tag, qint64> tag_offsets_;
  // Tracks the TiffHeader object.
  TiffHeader *tiff_header_;
};

}  // namespace qmeta

#endif  // QMETA_EXIF_H_
