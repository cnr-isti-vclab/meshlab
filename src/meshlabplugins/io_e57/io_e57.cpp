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
#include <Qt>
#include <QUuid>

#include <cmath>
#include <memory>

#include <external/e57/include/E57SimpleReader.h>
#include <external/e57/include/E57SimpleWriter.h>

#include "io_e57.h"

#define E57_FILE_EXTENSION      "E57"
#define E57_FILE_DESCRIPTION    "E57 (E57 points cloud)"

#define START_LOADING       "Loading E57 File..."
#define READING_IMAGES      "Reading images from E57 file..."
#define EXTRACTED_IMAGES    "Images from E57 file extracted to the file path..."
#define LOADING_POINTS      "Loading points..."
#define DONE_LOADING        "Done!"

#define BUFF_SIZE 1024

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
static inline QString formatImageFilename(const std::string& fileName) noexcept;

void E57IOPlugin::initPreOpenParameter(const QString &format, RichParameterList & parlst) {}

unsigned int E57IOPlugin::numberMeshesContainedInFile(const QString& format, const QString& fileName) const {

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

    e57::E57Root e57FileInfo{};
    e57::Reader fileReader{filenameToString(fileName)};

    // check if the file is opened
    E57_WRAPPER(fileReader.IsOpen(), "Error while opening E57 file!");
    // read E57 root to explore the tree
    E57_WRAPPER(fileReader.GetE57Root(e57FileInfo), "Error while reading E57 root info!");

    if (fileReader.GetData3DCount() == 0) {
        E57_WRAPPER(fileReader.Close(), "Error while closing the E57 file!");
        throw MLException{"No points cloud were found inside the E57 file!"};
    }

    UPDATE_PROGRESS(cb, 1, START_LOADING);

    // Read images...
    extractImages(fileReader, cb);

    // Read clouds...
    int scanIndex = 0;
    bool columnIndex = false;

    for (auto meshModel: meshModelList) {

        int mask = 0;
        e57::Data3D scanHeader{};

        int64_t rows = 0, cols = 0;
        int64_t numberPointSize = 0, numberGroupSize = 0, numberCountSize = 0;

        // read 3D data
        E57_WRAPPER(fileReader.ReadData3D(scanIndex, scanHeader), "Error while reading 3D from file!");

        // read scan's size information
        E57_WRAPPER(fileReader.GetData3DSizes(scanIndex, rows, cols,
                                              numberPointSize, numberGroupSize, numberCountSize, columnIndex),
                    "Error while reading scan information!");

        // set the mesh label if the mesh has a name
        if (!scanHeader.name.empty()) {
            meshModel->setLabel(QString::fromStdString(scanHeader.name));
        }

        // is useless to load a mesh with no points...
        if (numberPointSize == 0) {
            continue;
        }

        auto transformMatrix = vcg::Matrix44f::Identity();
        auto quaternion = vcg::Quaternion<float>{
            static_cast<float>(scanHeader.pose.rotation.x),
            static_cast<float>(scanHeader.pose.rotation.y),
            static_cast<float>(scanHeader.pose.rotation.z),
            static_cast<float>(scanHeader.pose.rotation.w),
        };

        quaternion.ToMatrix(transformMatrix);
        transformMatrix.ElementAt(0, 3) = static_cast<float>(scanHeader.pose.translation.x);
        transformMatrix.ElementAt(1, 3) = static_cast<float>(scanHeader.pose.translation.y);
        transformMatrix.ElementAt(2, 3) = static_cast<float>(scanHeader.pose.translation.z);

        meshModel->cm.Tr = transformMatrix;

        try {
            loadMesh(*meshModel, mask, scanIndex,
                     ((rows > 0) ? rows : BUFF_SIZE), numberPointSize, fileReader, scanHeader, cb);
            maskList.push_back(mask);
            scanIndex++;
        }
        catch (const std::exception& e) {
            E57_WRAPPER(fileReader.Close(), "Error while closing the E57 file!");
            throw MLException{e.what()};
        }
    }

    UPDATE_PROGRESS(cb, 100, DONE_LOADING);
    E57_WRAPPER(fileReader.Close(), "Error while closing the E57 file!");
}

void E57IOPlugin::extractImages(const e57::Reader &fileReader, vcg::CallBackPos* cb) {

    int imagesCount = fileReader.GetImage2DCount();

    UPDATE_PROGRESS(cb, 2, READING_IMAGES);

    for (int imageIndex = 0; imageIndex < imagesCount; imageIndex++) {

        QImage img;
        e57::Image2D imageHeader;
        e57::Image2DProjection imageProjection;
        e57::Image2DType imageType, imageMaskType, imageVisualType;

        int64_t width = 0, height = 0, size = 0;

        E57_WRAPPER(fileReader.ReadImage2D(imageIndex, imageHeader), "Error while reading E57 images!");

        E57_WRAPPER(fileReader.GetImage2DSizes(imageIndex, imageProjection, imageType, width, height, size,
                                               imageMaskType, imageVisualType), "Error while getting image.");

        auto imageBuffer = std::unique_ptr<char[]>(new char[size]);
        int64_t bytesRead = fileReader.ReadImage2DData(imageIndex, imageProjection, imageType, imageBuffer.get(), 0, size);

        const char* format = (imageType == e57::E57_JPEG_IMAGE) ? "jpeg" : "png";

        QString imageFilename = formatImageFilename(imageHeader.name);

        // load the data from the image and save it inside the file system
        img.loadFromData(QByteArray(imageBuffer.get(), bytesRead), format);
        img.save(imageFilename, "png", 100);
    }

    UPDATE_PROGRESS(cb, 20, EXTRACTED_IMAGES);
}

void E57IOPlugin::save(const QString& formatName, const QString& fileName, MeshModel& m, const int mask,
                       const RichParameterList&, vcg::CallBackPos* cb)
{

    using Mask = vcg::tri::io::Mask;

    if (formatName.toUpper() != tr(E57_FILE_EXTENSION)) {
        wrongSaveFormat(formatName);
    }

    std::int64_t scanIndex;
    const std::size_t totalPoints = m.cm.vert.size();
    const std::string filePath = filenameToString(fileName);

    // create a new uuid for the file that will be saved
    e57::Data3D scanHeader{};
    e57::Writer fileWriter{filePath};

    scanHeader.guid = QUuid::createUuid().toString(QUuid::WithBraces).toStdString();
    scanHeader.pointsSize = static_cast<int64_t>(totalPoints);

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

    const std::size_t buffSize = (totalPoints < BUFF_SIZE) ? totalPoints : BUFF_SIZE;

    vcg::tri::io::E57Data3DPoints data3DPoints{buffSize, scanHeader};
    e57::CompressedVectorWriter dataWriter = fileWriter.SetUpData3DPointsData(scanIndex, buffSize, data3DPoints.points());

    try {

        int count = 0;
        std::size_t remainingVertices = totalPoints;

        while (remainingVertices > buffSize) {
            writeVertices(dataWriter, data3DPoints, count, buffSize, m.cm.vert);
            count += buffSize;
            remainingVertices -= buffSize;
        }

        // write the remaining vertices
        if (remainingVertices > 0) {
            writeVertices(dataWriter, data3DPoints, count, remainingVertices, m.cm.vert);
        }
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

void E57IOPlugin::loadMesh(MeshModel &m, int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
              const e57::Reader &fileReader, e57::Data3D& scanHeader, vcg::CallBackPos* positionCallback) {

    using Mask = vcg::tri::io::Mask;

    // object holding data read from E57 file
    vcg::tri::io::E57Data3DPoints data3DPoints{buffSize, scanHeader};

    auto currentLoadingPercentage = 20;
    const auto loadingScale = ((100 - currentLoadingPercentage) / numberPointSize);

    ulong size = 0;
    auto vertexIterator = vcg::tri::Allocator<CMeshO>::AddVertices(m.cm, static_cast<size_t>(numberPointSize));
    auto dataReader = fileReader.SetUpData3DPointsData(scanIndex, buffSize, data3DPoints.points());

    // to enable colors, quality and normals inside the mesh
    if (data3DPoints.areColorsAvailable()) {
        mask |= Mask::IOM_VERTCOLOR;
    }
    if (data3DPoints.areNormalsAvailable()) {
        mask |= Mask::IOM_VERTNORMAL;
    }
    if (data3DPoints.isQualityAvailable()) {
        mask |= Mask::IOM_VERTQUALITY;
    }

    // set the mask
    m.Enable(mask);

    // read the data from the E57 file
    try {

        e57::Data3DPointsData& pointsData = data3DPoints.points();

        while ((size = dataReader.read()) > 0) {

            for (auto i = 0UL; i < size; i++, vertexIterator++) {

                auto& currentPoint = (*vertexIterator).P();
                if (data3DPoints.areCoordinatesAvailable()) {
                    currentPoint[0] = pointsData.cartesianX[i];
                    currentPoint[1] = pointsData.cartesianY[i];
                    currentPoint[2] = pointsData.cartesianZ[i];
                }

                auto& currentNormal = (*vertexIterator).N();
                if (data3DPoints.areNormalsAvailable()) {
                    currentNormal[0] = pointsData.normalX[i];
                    currentNormal[1] = pointsData.normalY[i];
                    currentNormal[2] = pointsData.normalZ[i];
                }

                auto& currentQuality = (*vertexIterator).Q();
                if (data3DPoints.isQualityAvailable()) {
                    currentQuality = pointsData.intensity[i];
                }

                vcg::Color4b& currentColor = (*vertexIterator).C();
                if (data3DPoints.areColorsAvailable()) {

                    currentColor[0] = pointsData.colorRed[i];
                    currentColor[1] = pointsData.colorGreen[i];
                    currentColor[2] = pointsData.colorBlue[i];
                    currentColor[3] = 0xFF;
                }

                currentLoadingPercentage += loadingScale;
                UPDATE_PROGRESS(positionCallback, currentLoadingPercentage, LOADING_POINTS);
            }
        }
    }
    catch (const e57::E57Exception& e) {
        dataReader.close();
        throw;
    }

    dataReader.close();
}

void E57IOPlugin::writeVertices(e57::CompressedVectorWriter &dataWriter, vcg::tri::io::E57Data3DPoints& data3DPoints,
                                int count, int remaining, vcgTriMesh::VertContainer &vertices) {

    const int bound = count + remaining;
    e57::Data3DPointsData& pointsData = data3DPoints.points();

    for (int i = count, buffIndex = 0; i < bound; i++, buffIndex++) {

        if (data3DPoints.areCoordinatesAvailable()) {
            pointsData.cartesianX[buffIndex] = vertices[i].P().X();
            pointsData.cartesianY[buffIndex] = vertices[i].P().Y();
            pointsData.cartesianZ[buffIndex] = vertices[i].P().Z();
        }

        if (data3DPoints.areColorsAvailable()) {
            pointsData.colorRed[buffIndex] = static_cast<uint8_t>(vertices[i].C().X());
            pointsData.colorGreen[buffIndex] = static_cast<uint8_t>(vertices[i].C().Y());
            pointsData.colorBlue[buffIndex] = static_cast<uint8_t>(vertices[i].C().Z());
        }

        if (data3DPoints.areNormalsAvailable()) {
            pointsData.normalX[buffIndex] = vertices[i].N().X();
            pointsData.normalY[buffIndex] = vertices[i].N().Y();
            pointsData.normalZ[buffIndex] = vertices[i].N().Z();
        }

        if (data3DPoints.isQualityAvailable()) {
            pointsData.intensity[buffIndex] = vertices[i].Q();
        }
    }

    // write the mesh data
    dataWriter.write(remaining);
}

static inline std::string filenameToString(const QString& fileName) noexcept {
    return QFile::encodeName(fileName).toStdString();
}

static inline QString formatImageFilename(const std::string &fileName) noexcept {
    return QString{"%1.png"}.arg(QString::fromStdString(fileName));
}

MESHLAB_PLUGIN_NAME_EXPORTER(E57IOPlugin)
