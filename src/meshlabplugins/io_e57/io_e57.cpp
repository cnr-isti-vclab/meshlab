/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#include <external/e57/include/E57SimpleReader.h>
#include <external/e57/include/E57SimpleWriter.h>

#include "io_e57.h"

#define E57_FILE_EXTENSION "E57"
#define E57_FILE_DESCRIPTION "E57 (E57 point cloud)"

#define START_LOADING "Loading E57 File..."
#define LOADING_POINTS "Loading points..."
#define DONE_LOADING  "Done!"

#define BUFF_SIZE 1024

/**
 * Pre-processor macro used to handle failure in E57 functions.
 */
#define E57_WRAPPER(e57f, exceptionMessage) if (!(e57f)) throw MLException(QString{exceptionMessage})

/***
 * Load the cloud points read from the E57 file, inside the mesh to display.
 * @param m The mesh to display
 * @param mask
 * @param scanIndex Data block index given by the NewData3D
 * @param buffSize Dimension for buffer size
 * @param numberPointSize How many points are contained inside the cloud
 * @param fileReader The file reader object used to scan the file
 */
static void loadMesh(MeshModel &m, int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
                     const e57::Reader &fileReader, e57::Data3D& scanHeader, vcg::CallBackPos& positionCallback);

/**
 * Update progress of the progress bar inside MeshLab GUI
 * @param positionCallback Callback function to call to update the progress bar
 * @param percentage The completion percentage to set
 * @param description The description to show near the progress bar
 */
static void updateProgress(vcg::CallBackPos &positionCallback, int percentage, const char* description) noexcept;

/**
 * Convert a QT string filename to a std::string
 * @param fileName String to convert
 * @return The string converted into std::string type
 */
static inline std::string filenameToString(const QString& fileName) noexcept;

static void writeVerticies(e57::CompressedVectorWriter &dataWriter, E57Data3DPoints& data3DPoints, int count, int remaining,
                           vcgTriMesh::VertContainer &verticies);


void E57IOPlugin::initPreOpenParameter(const QString &format, RichParameterList & parlst) {}

void E57IOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,
                       const RichParameterList &parlst, vcg::CallBackPos* cb)
{

    // TODO: format exception messages
    if (formatName.toUpper() != tr(E57_FILE_EXTENSION)) {
        wrongOpenFormat(formatName);
        return;
    }

    int scanIndex = 0;
    bool columnIndex = false;

    std::string filePath = filenameToString(fileName);

    std::int64_t cols = 0;
    std::int64_t rows = 0;
    std::int64_t numberPointSize = 0;
    std::int64_t numberGroupSize = 0;
    std::int64_t numberCountSize = 0;

    e57::Reader fileReader{filePath};
    e57::E57Root e57FileInfo{};
    e57::Data3D scanHeader{};

    // check if the file is opened
    E57_WRAPPER(fileReader.IsOpen(), "Error while opening E57 file!");

    updateProgress(*cb, 0, START_LOADING);

    // read E57 root to explore the tree
    E57_WRAPPER(fileReader.GetE57Root(e57FileInfo), "Error while reading E57 root info!");
    // read 3D data
    E57_WRAPPER(fileReader.ReadData3D(scanIndex, scanHeader), "Error while reading 3D from file!");
    // read scan's size information
    E57_WRAPPER(fileReader.GetData3DSizes(scanIndex, rows, cols, numberPointSize, numberGroupSize, numberCountSize, columnIndex),
                "Error while reading scan information!");

    try {
        loadMesh(m, mask, scanIndex, ((rows > 0) ? rows : BUFF_SIZE), numberPointSize, fileReader, scanHeader, *cb);
    }
    catch (const e57::E57Exception& e) {
        E57_WRAPPER(fileReader.Close(), "Error while closing the E57 file!");
        throw MLException{e.what()};
    }

    updateProgress(*cb, 100, DONE_LOADING);

    E57_WRAPPER(fileReader.Close(), "Error while closing the E57 file!");
}

void E57IOPlugin::save(const QString& formatName, const QString& fileName, MeshModel& m, const int mask,
                       const RichParameterList&, vcg::CallBackPos* cb)
{

    using Mask = vcg::tri::io::Mask;

    if (formatName.toUpper() != tr(E57_FILE_EXTENSION)) {
        wrongSaveFormat(formatName);
        return;
    }

    vcgTriMesh::VertContainer& verticies = m.cm.vert;

    std::int64_t scanIndex;
    std::size_t totalPoints = verticies.size();
    std::string filePath = filenameToString(fileName);

    // create a new uuid for the file that will be saved
    QUuid uuid = QUuid::createUuid();
    e57::Data3D scanHeader{};
    e57::Writer fileWriter{filePath};

    scanHeader.guid = std::string{uuid.toString(QUuid::WithBraces).toStdString()};
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
    }
    if ((mask & Mask::IOM_VERTQUALITY) != 0) {
        scanHeader.pointFields.intensityField = true;
    }

    scanIndex = fileWriter.NewData3D(scanHeader);

    E57Data3DPoints data3DPoints{BUFF_SIZE, scanHeader};
    auto dataWriter = fileWriter.SetUpData3DPointsData(scanIndex, BUFF_SIZE, data3DPoints.points());

    try {

        int count = 0;
        std::size_t remainingVerticies = totalPoints;

        while (remainingVerticies > BUFF_SIZE) {
            writeVerticies(dataWriter, data3DPoints, count, BUFF_SIZE, verticies);
            count += BUFF_SIZE;
            remainingVerticies -= BUFF_SIZE;
        }

        writeVerticies(dataWriter, data3DPoints, count, remainingVerticies, verticies);
    }
    catch (const e57::E57Exception& e) {
        dataWriter.close();
        E57_WRAPPER(fileWriter.Close(), "Error while closing the E57 file during save process!");
        throw MLException{e.what()};
    }

    dataWriter.close();
    E57_WRAPPER(fileWriter.Close(), "Error while closing the E57 file during save process!");
}

/*
	returns the list of the file's type which can be imported
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
void E57IOPlugin::exportMaskCapability(const QString & /*format*/, int &capability, int &defaultBits) const
{
    capability = defaultBits = 0;
}

static void loadMesh(MeshModel &m, int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
              const e57::Reader &fileReader, e57::Data3D& scanHeader, vcg::CallBackPos& positionCallback) {

    using Mask = vcg::tri::io::Mask;

    // object holding data read from E57 file
    E57Data3DPoints data3DPoints{buffSize, scanHeader};

    auto currentLoadingPercentage = 0;
    auto loadingScale = (100 / numberPointSize);

    ulong size = 0;
    auto vertexIterator = vcg::tri::Allocator<CMeshO>::AddVertices(m.cm, static_cast<size_t>(numberPointSize));
    auto dataReader = fileReader.SetUpData3DPointsData(scanIndex, buffSize, data3DPoints.points());

    // to enable colors, quality and normals inside the mesh
    if (data3DPoints.areColorsAvailable()) mask |= Mask::IOM_VERTCOLOR;
    if (data3DPoints.areNormalsAvailable()) mask |= Mask::IOM_VERTNORMAL;
    if (data3DPoints.isQualityAvailable()) mask |= Mask::IOM_VERTQUALITY;

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
                updateProgress(positionCallback, currentLoadingPercentage, LOADING_POINTS);
            }
        }
    }
    catch (const e57::E57Exception& e) {
        dataReader.close();
        throw e;
    }

    dataReader.close();
}

static void updateProgress(vcg::CallBackPos& positionCallback, int percentage, const char* description) noexcept {

    if (positionCallback != nullptr) {
        positionCallback(percentage, description);
    }
}

static inline std::string filenameToString(const QString& fileName) noexcept {
    return std::string{QFile::encodeName(fileName).constData()};
}

static void writeVerticies(e57::CompressedVectorWriter &dataWriter, E57Data3DPoints& data3DPoints, int count, int remaining,
                           vcgTriMesh::VertContainer &verticies) {

    int bound = count + remaining;
    e57::Data3DPointsData& pointsData = data3DPoints.points();

    for (int i = count, buffIndex = 0; i < bound; i++, buffIndex++) {

        auto& cartesianPoints = verticies[i].P();
        if (data3DPoints.areCoordinatesAvailable()) {
            pointsData.cartesianX[buffIndex] = cartesianPoints[0];
            pointsData.cartesianY[buffIndex] = cartesianPoints[1];
            pointsData.cartesianZ[buffIndex] = cartesianPoints[2];
        }

        auto& colors = verticies[i].C();
        if (data3DPoints.areColorsAvailable()) {
            pointsData.colorRed[buffIndex] = colors[0];
            pointsData.colorGreen[buffIndex] = colors[1];
            pointsData.colorBlue[buffIndex] = colors[2];
        }

        auto& normals = verticies[i].N();
        if (data3DPoints.areNormalsAvailable()) {
            pointsData.normalX[buffIndex] = normals[0];
            pointsData.normalY[buffIndex] = normals[1];
            pointsData.normalZ[buffIndex] = normals[2];
        }

        auto& quality = verticies[i].Q();
        if (data3DPoints.isQualityAvailable()) {
            pointsData.intensity[buffIndex] = quality;
        }
    }

    // read data from the mesh and write them
    dataWriter.write(remaining);
}


MESHLAB_PLUGIN_NAME_EXPORTER(E57IOPlugin)
