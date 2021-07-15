/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005-2021                                           \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
#include <QUuid>

#include <cmath>
#include <memory>

#include <external/e57/include/E57SimpleReader.h>
#include <external/e57/include/E57SimpleWriter.h>

#include "io_e57.h"

#define E57_FILE_EXTENSION      "E57"
#define E57_FILE_DESCRIPTION    "E57 (E57 points cloud)"

#define START_LOADING       "Loading E57 File..."
#define EXTRACTED_IMAGES    "Images from E57 file extracted to the file path..."
#define LOADING_MESH        "Loading mesh..."
#define DONE_LOADING        "Done!"

/**
 * [Macro] Throw MLException in case of failure using E57 functions.
 */
#define E57_WRAPPER(e57f, exceptionMessage) if (!(e57f)) throw MLException(QString{exceptionMessage})

/**
 * [Macro] Update progress of the progress bar inside MeshLab GUI
 * @param positionCallback Callback function to call to update the progress bar
 * @param percentage The completion percentage to set
 * @param description The description to show near the progress bar
 */
#define UPDATE_PROGRESS(positionCallback, percentage, description) \
    if (((positionCallback) != nullptr)) positionCallback(percentage, description)

/**
 * Convert a QT string filename to a std::string
 * @param fileName String to convert
 * @return The string converted into std::string type
 */
static inline std::string filenameToString(const QString& fileName) noexcept;

/**
 * Give a image filename format it to "${fileName}.png" QString
 * @param fileName The filename to format
 * @return A QString formatted as "${fileName}.png"
 */
static inline QString formatImageFilename(const std::string& fileName, const char* format) noexcept;

unsigned int E57IOPlugin::numberMeshesContainedInFile(const QString& format, const QString& fileName, const RichParameterList&) const {

    unsigned int count;

    if (format.toUpper() != tr(E57_FILE_EXTENSION)) {
        wrongOpenFormat(format);
    }

    e57::Reader fileReader{filenameToString(fileName)};

    // check if the file is opened
    E57_WRAPPER(fileReader.IsOpen(), "Error while opening E57 file!");
    // read how many meshes are contained inside the file
    count = fileReader.GetData3DCount();
    // close the file to free the resources
    E57_WRAPPER(fileReader.Close(), "Error while closing the E57 file!");

    return count;
}

void E57IOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,
                       const RichParameterList &parlst, vcg::CallBackPos* cb)
{
}

void E57IOPlugin::open(const QString &formatName, const QString &fileName, const std::list<MeshModel*>& meshModelList,
                       std::list<int>& maskList, const RichParameterList& par, vcg::CallBackPos* cb) {

    if (formatName.toUpper() != tr(E57_FILE_EXTENSION)) {
        wrongOpenFormat(formatName);
    }

    e57::E57Root e57FileInfo{ };
    e57::Reader  e57FileReader{filenameToString(fileName) };

    // check if the file is opened
    E57_WRAPPER(e57FileReader.IsOpen(), "Error while opening E57 file!");
    // read E57 root to explore the tree
    E57_WRAPPER(e57FileReader.GetE57Root(e57FileInfo), "Error while reading E57 root info!");

    int64_t data3DCount = e57FileReader.GetData3DCount();

    // If there are no meshes inside the file warn the user!
    if (data3DCount == 0) {
        E57_WRAPPER(e57FileReader.Close(), "Error while closing the E57 file!");
        throw MLException{"No points cloud were found inside the E57 file!"};
    }

    UPDATE_PROGRESS(cb, 1, START_LOADING);

    // Read clouds...
    int scanIndex = 0;
    bool columnIndex = false;

    for (auto meshModel: meshModelList) {

        int mask = 0;
        e57::Data3D scanHeader{};

        int64_t rows = 0, cols = 0;
        int64_t numberPointSize = 0, numberGroupSize = 0, numberCountSize = 0;

        UPDATE_PROGRESS(cb, (scanIndex * data3DCount) / 100, LOADING_MESH);

        // read 3D data
        E57_WRAPPER(e57FileReader.ReadData3D(scanIndex, scanHeader), "Error while reading 3D from file!");

        // read scan's size information
        E57_WRAPPER(e57FileReader.GetData3DSizes(
                scanIndex, rows, cols, numberPointSize, numberGroupSize, numberCountSize, columnIndex
        ), "Error while reading scan information!");

        // If the name is not empty then set a name for the mesh.
        if (!scanHeader.name.empty()) {
            meshModel->setLabel(QString::fromStdString(scanHeader.name));
        }

        try {

            if (numberPointSize != 0) {

                // Does the mesh have an imageMetaAndImage from which to extract colors?
                std::pair<e57::Image2D, QImage> imageMetaAndImage = extractMeshImage(e57FileReader, scanIndex, false);

                // Read points from file and load them inside the MeshLab's mesh.
                loadMesh(*meshModel, mask, scanIndex, numberPointSize, e57FileReader, scanHeader, imageMetaAndImage, par);

                // Once the mesh is loaded apply a transformation matrix to translate and rotate the points.
                translatedAndRotateMesh(meshModel, scanHeader);
            }

            // Put the modified mask into the mask list.
            maskList.push_back(mask);

            // Increase the scanIndex to get info about the next mesh to parse from the file.
            scanIndex++;
        }
        catch (const std::exception& e) {
            E57_WRAPPER(e57FileReader.Close(), "Error while closing the E57 file!");
            throw MLException{e.what()};
        }
    }

    UPDATE_PROGRESS(cb, 100, DONE_LOADING);
    E57_WRAPPER(e57FileReader.Close(), "Error while closing the E57 file!");
}

void E57IOPlugin::translatedAndRotateMesh(MeshModel *meshModel, const e57::Data3D &scanHeader) const {

    auto rotationMatrix = Matrix44m::Identity();
    auto translateMatrix = Matrix44m::Identity();

    auto quaternion = vcg::Quaternion<Scalarm>{
        static_cast<Scalarm>(scanHeader.pose.rotation.w),
        static_cast<Scalarm>(scanHeader.pose.rotation.x),
        static_cast<Scalarm>(scanHeader.pose.rotation.y),
        static_cast<Scalarm>(scanHeader.pose.rotation.z),
    };

    quaternion.ToMatrix(rotationMatrix);

    translateMatrix.ElementAt(0, 3) = static_cast<Scalarm>(scanHeader.pose.translation.x);
    translateMatrix.ElementAt(1, 3) = static_cast<Scalarm>(scanHeader.pose.translation.y);
    translateMatrix.ElementAt(2, 3) = static_cast<Scalarm>(scanHeader.pose.translation.z);

    meshModel->cm.Tr = translateMatrix * rotationMatrix;
}

std::pair<e57::Image2D, QImage> E57IOPlugin::extractMeshImage(const e57::Reader &fileReader, int scanIndex, bool saveToDisk) {

    QImage img;
    e57::Image2D imageHeader;
    e57::Image2DProjection imageProjection;
    e57::Image2DType imageType, imageMaskType, imageVisualType;

    int64_t width = 0, height = 0, size = 0;

    // If the image is not present then return an empty image
    if (!fileReader.ReadImage2D(scanIndex, imageHeader)) {
        return std::pair<e57::Image2D, QImage>{imageHeader, QImage{}};
    }

    // Get sizes specs for the image
    fileReader.GetImage2DSizes(scanIndex, imageProjection, imageType, width, height, size,
                                           imageMaskType, imageVisualType);

    // If no image is present...
    if (imageType == e57::Image2DType::E57_NO_IMAGE) {
        return std::pair<e57::Image2D, QImage>{imageHeader, QImage{}};
    }

    // Read the image data inside a byte buffer to create a QImage object
    std::unique_ptr<char[]> imageBuffer = std::unique_ptr<char[]>(new char[size]);
    int64_t bytesRead = fileReader.ReadImage2DData(scanIndex, imageProjection, imageType, imageBuffer.get(), 0, size);

    // An image contained in a e57 file can be a JPEG or a PNG
    const char* format = (imageType == e57::E57_JPEG_IMAGE) ? "jpeg" : "png";

    // load the data from the image and save it inside the file system
    img.loadFromData(QByteArray(imageBuffer.get(), bytesRead), format);

    // Do we need to store the image inside the disk?
    if (saveToDisk) {
        img.save(formatImageFilename(imageHeader.name, format), format, 100);
    }

    return std::pair<e57::Image2D, QImage>{imageHeader, img.copy()};
}

void E57IOPlugin::save(const QString& formatName, const QString& fileName, MeshModel& m, const int mask,
                       const RichParameterList&, vcg::CallBackPos* cb)
{

    using Mask = vcg::tri::io::Mask;

    vcg::tri::Allocator<CMeshO>::CompactEveryVector(m.cm);

    if (formatName.toUpper() != tr(E57_FILE_EXTENSION)) {
        wrongSaveFormat(formatName);
    }

    std::int64_t scanIndex;
    const std::size_t totalPoints = m.cm.vert.size();
    const std::string filePath = filenameToString(fileName);

    // create a new uuid for the file that will be saved
    e57::Data3D scanHeader{};
    e57::Writer fileWriter{filePath};

    E57_WRAPPER(fileWriter.IsOpen(), "Error while opening E57 file for writing!");

    scanHeader.guid = QUuid::createUuid().toString(QUuid::WithBraces).toStdString();
    scanHeader.pointsSize = static_cast<int64_t>(totalPoints);

    e57::Translation translation;
    e57::Quaternion quaternion;

    Point4m translationColumn = m.cm.Tr.GetColumn4(3);
    translation.x = translationColumn.X();
    translation.y = translationColumn.Y();
    translation.z = translationColumn.Z();
    scanHeader.pose.translation = translation;

    vcg::Quaternion<Scalarm> q;

    Matrix44m transformMatrixCopy = m.cm.Tr;
    transformMatrixCopy[3][0] = 0;
    transformMatrixCopy[3][1] = 0;
    transformMatrixCopy[3][2] = 0;

    q.FromMatrix(transformMatrixCopy);
    quaternion.w = q[0];
    quaternion.x = q[1];
    quaternion.y = q[2];
    quaternion.z = q[3];

    scanHeader.pose.rotation = quaternion;

    scanHeader.pointFields.cartesianInvalidStateField = true;
    scanHeader.pointFields.cartesianXField = true;
    scanHeader.pointFields.cartesianYField = true;
    scanHeader.pointFields.cartesianZField = true;

    if ((mask & Mask::IOM_VERTNORMAL) != 0) {
        scanHeader.pointFields.normalX = true;
        scanHeader.pointFields.normalY = true;
        scanHeader.pointFields.normalZ = true;
    }
    if ((mask & Mask::IOM_VERTCOLOR) != 0) {
        scanHeader.pointFields.colorRedField = true;
        scanHeader.pointFields.colorGreenField = true;
        scanHeader.pointFields.colorBlueField = true;
        scanHeader.colorLimits.colorRedMinimum = e57::E57_UINT8_MIN;
        scanHeader.colorLimits.colorRedMaximum = e57::E57_UINT8_MAX;
        scanHeader.colorLimits.colorGreenMinimum = e57::E57_UINT8_MIN;
        scanHeader.colorLimits.colorGreenMaximum = e57::E57_UINT8_MAX;
        scanHeader.colorLimits.colorBlueMinimum = e57::E57_UINT8_MIN;
        scanHeader.colorLimits.colorBlueMaximum = e57::E57_UINT8_MAX;
    }
    if ((mask & Mask::IOM_VERTQUALITY) != 0) {
        scanHeader.pointFields.intensityField = true;
    }

    scanIndex = fileWriter.NewData3D(scanHeader);

    vcg::tri::io::E57Data3DPoints data3DPoints{totalPoints, scanHeader};
    e57::Data3DPointsData_t<Scalarm>& pointsData = data3DPoints.points();

    e57::CompressedVectorWriter dataWriter = fileWriter.SetUpData3DPointsData(scanIndex, totalPoints, pointsData);

    try {

        CMeshO::VertContainer& vertices = m.cm.vert;

        for (std::size_t i = 0; i < totalPoints; i++) {

            pointsData.cartesianX[i] = vertices[i].P().X();
            pointsData.cartesianY[i] = vertices[i].P().Y();
            pointsData.cartesianZ[i] = vertices[i].P().Z();

            pointsData.cartesianInvalidState[i] = 0;

            if (data3DPoints.areColorsAvailable()) {
                pointsData.colorRed[i] = static_cast<uint8_t>(vertices[i].C().X());
                pointsData.colorGreen[i] = static_cast<uint8_t>(vertices[i].C().Y());
                pointsData.colorBlue[i] = static_cast<uint8_t>(vertices[i].C().Z());
                pointsData.isColorInvalid[i] = 0;
            }

            if (data3DPoints.areNormalsAvailable()) {
                pointsData.normalX[i] = vertices[i].N().X();
                pointsData.normalY[i] = vertices[i].N().Y();
                pointsData.normalZ[i] = vertices[i].N().Z();
            }

            if (data3DPoints.isQualityAvailable()) {
                pointsData.intensity[i] = vertices[i].Q();
                pointsData.isIntensityInvalid[i] = 0;
            }
        }

        // write the mesh data
        dataWriter.write(totalPoints);

    }
    catch (const e57::E57Exception& e) {
        dataWriter.close();
        E57_WRAPPER(fileWriter.Close(), "Error while closing the E57 file during save process!");
        throw MLException{QString{"E57 Exception: %1.\nError Code: %2"}.arg(QString::fromStdString(e.context()), e.errorCode())};
    }

    dataWriter.close();
    E57_WRAPPER(fileWriter.Close(), "Error while closing the E57 file during save process!");
}

/*
	Returns the list of the file's type which can be imported
*/
QString E57IOPlugin::pluginName() const
{
    return QString{"IOE57"};
}

std::list<FileFormat> E57IOPlugin::importFormats() const
{
    return {FileFormat(E57_FILE_DESCRIPTION, tr(E57_FILE_EXTENSION))};
}

/*
	Returns the list of the file's type which can be exported
*/
std::list<FileFormat> E57IOPlugin::exportFormats() const
{
    return {FileFormat(E57_FILE_DESCRIPTION, tr(E57_FILE_EXTENSION))};
}

/*
	Returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void E57IOPlugin::exportMaskCapability(const QString& format, int &capability, int &defaultBits) const
{

    using Mask = vcg::tri::io::Mask;
    int mask = 0;

    if (format.toUpper() != tr(E57_FILE_EXTENSION)) return;

    mask |= Mask::IOM_VERTNORMAL;
    mask |= Mask::IOM_VERTCOLOR;
    mask |= Mask::IOM_VERTQUALITY;

    capability = defaultBits = mask;
}

void E57IOPlugin::loadMesh(MeshModel &m, int &mask, int scanIndex, size_t buffSize,
                           const e57::Reader &fileReader, e57::Data3D &scanHeader,
                           std::pair<e57::Image2D, QImage> image, const RichParameterList &par) {

    using Mask = vcg::tri::io::Mask;

    e57::Image2D meshImageHeader = image.first;
    QImage meshImage = image.second;

    // object holding data read from E57 file
    vcg::tri::io::E57Data3DPoints data3DPoints{buffSize, scanHeader};

    size_t size = 0;
    auto dataReader = fileReader.SetUpData3DPointsData(scanIndex, buffSize, data3DPoints.points());

    // to enable colors, quality and normals inside the mesh
    mask |= Mask::IOM_VERTCOLOR;

    if (data3DPoints.areNormalsAvailable()) {
        mask |= Mask::IOM_VERTNORMAL;
    }
    if (data3DPoints.isQualityAvailable()) {
        mask |= Mask::IOM_VERTQUALITY;
    }

    // set the mask
    m.enable(mask);

    // read the data from the E57 file
    try {

        e57::Data3DPointsData_t<Scalarm>& pointsData = data3DPoints.points();

        while ((size = dataReader.read()) > 0) {

            for (std::size_t i = 0; i < size; i++) {

                Point3m coordinates;

                if (data3DPoints.areCoordinatesAvailable()) {

                    if (pointsData.cartesianInvalidState == nullptr || pointsData.cartesianInvalidState[i] == 0) {
                        coordinates[0] = pointsData.cartesianX[i];
                        coordinates[1] = pointsData.cartesianY[i];
                        coordinates[2] = pointsData.cartesianZ[i];
                    }
                    else {
                        continue;
                    }
                }
                else if (data3DPoints.areSphericalCoordinatesAvailable()) {
                    if (pointsData.sphericalInvalidState == nullptr || pointsData.sphericalInvalidState[i] == 0) {

                        auto range = pointsData.sphericalRange[i];
                        auto phi = pointsData.sphericalElevation[i];
                        auto theta = pointsData.sphericalAzimuth[i];

                        coordinates[0] = range * std::cos(phi) * std::cos(theta);
                        coordinates[1] = range * std::cos(phi) * std::sin(theta);
                        coordinates[2] = range * std::sin(phi);

                    }
                    else {
                        continue;
                    }
                }
                else {
                    continue;
                }

                auto vertex = vcg::tri::Allocator<CMeshO>::AddVertex(m.cm, coordinates);

                // Set the normals.
                if (data3DPoints.areNormalsAvailable()) {
                    vertex->N()[0] = pointsData.normalX[i];
                    vertex->N()[1] = pointsData.normalY[i];
                    vertex->N()[2] = pointsData.normalZ[i];
                }

                // Set the quality.
                if (data3DPoints.isQualityAvailable()) {
                    vertex->Q() = pointsData.intensity[i];
                }

                // Set the point color.
                if (data3DPoints.areColorsAvailable()) {
                    vertex->C()[0] = pointsData.colorRed[i];
                    vertex->C()[1] = pointsData.colorGreen[i];
                    vertex->C()[2] = pointsData.colorBlue[i];
                    vertex->C()[3] = 0xFF;
                }
                else {
                    // TODO: extract colors from the image?
                }

            }
        }

        /* If the colors are not available for the mesh use a gray scale */
        if (!data3DPoints.areColorsAvailable()) {

            const float percentile = 5.0f;
            vcg::Histogram<Scalarm> histogram{};
            vcg::tri::Stat<CMeshO>::ComputePerVertexQualityHistogram(m.cm, histogram);

            const Scalarm minPercentile = histogram.Percentile(percentile / 100.0);
            const Scalarm maxPercentile = histogram.Percentile(1.0 - (percentile / 100));

            vcg::tri::UpdateColor<CMeshO>::PerVertexQualityGray(m.cm, minPercentile, maxPercentile);

        }

    }
    catch (const e57::E57Exception& e) {
        dataReader.close();
        throw MLException{QString{"E57 Exception: %1.\nError Code: %2"}.arg(QString::fromStdString(e.context()), e.errorCode())};
    }

    dataReader.close();
}

static inline std::string filenameToString(const QString& fileName) noexcept {
    return QFile::encodeName(fileName).toStdString();
}

static inline QString formatImageFilename(const std::string &fileName, const char* format) noexcept {
    return QString{"%1.%s"}.arg(QString::fromStdString(fileName), QString::fromStdString(format));
}

MESHLAB_PLUGIN_NAME_EXPORTER(E57IOPlugin)
