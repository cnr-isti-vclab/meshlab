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

#include "io_e57.h"
#include <external/e57/include/E57SimpleReader.h>

#define START_LOADING "Loading E57 File..."
#define LOADING_POINTS "Loading points..."
#define DONE_LOADING  "Done!"

#define E57_DEBUG 1
#define E57_WRAPPER(e57f, exceptionMessage) if (!e57f) throw MLException(QString{exceptionMessage})

/***
 * Load the cloud points read from the E57 file, inside the mesh to display.
 * @param m The mesh to display
 * @param mask
 * @param scanIndex Data block index given by the NewData3D
 * @param buffSize Dimension for buffer size
 * @param numberPointSize How many points are contained inside the cloud
 * @param fileReader The file reader object used to scan the file
 */
static void loadMesh(MeshModel &m, const int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
                     const e57::Reader &fileReader, vcg::CallBackPos* positionCallback);

static void updateProgress(vcg::CallBackPos &positionCallback, int percentage, const char* description) noexcept;

void E57IOPlugin::initPreOpenParameter(const QString &format, RichParameterList & parlst)
{
}

void E57IOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,
                       const RichParameterList &parlst, vcg::CallBackPos* cb)
{

    // TODO: format exception messages

    int scanIndex = 0;
    bool columnIndex = false;

    std::int64_t cols = 0;
    std::int64_t rows = 0;
    std::int64_t numberPointSize = 0;
    std::int64_t numberGroupSize = 0;
    std::int64_t numberCountSize = 0;

    auto stdFilename = std::string{QFile::encodeName(fileName).constData()};

    mask = 0;

    if (formatName.toUpper() != tr("E57")) {
        wrongOpenFormat(formatName);
        return;
    }

    e57::Reader fileReader{stdFilename};
    e57::E57Root e57FileInfo{};
    e57::Data3D scanHeader{};

    // check if the file is opened
    E57_WRAPPER(fileReader.IsOpen(), "Error while opening E57 file!");

    if (cb)
        updateProgress(*cb, 0, START_LOADING);

    // read E57 root to explore the tree
    E57_WRAPPER(fileReader.GetE57Root(e57FileInfo), "Error while reading E57 root info!");
    // read 3D data
    E57_WRAPPER(fileReader.ReadData3D(scanIndex, scanHeader), "Error while reading 3D from file!");
    // read scan's size information
    E57_WRAPPER(fileReader.GetData3DSizes(scanIndex, rows, cols, numberPointSize, numberGroupSize, numberCountSize, columnIndex),
                "Error while reading scan information!");

    loadMesh(m, mask, scanIndex, ((rows > 0) ? rows : 1024), numberPointSize, fileReader, cb);

    if (cb)
        updateProgress(*cb, 100, DONE_LOADING);

    fileReader.Close();
}

void E57IOPlugin::save(const QString & formatName, const QString & /*fileName*/, MeshModel & /*m*/, const int /*mask*/,
                       const RichParameterList &, vcg::CallBackPos * /*cb*/)
{
    wrongSaveFormat(formatName);
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
    return {
            FileFormat("E57 (E57 point cloud)", tr("E57"))
    };
}

/*
	Returns the list of the file's type which can be exported
*/
std::list<FileFormat> E57IOPlugin::exportFormats() const
{
    return {};
}

/*
	Returns the mask on the basis of the file's type.
	otherwise it returns 0 if the file format is unknown
*/
void E57IOPlugin::exportMaskCapability(const QString & /*format*/, int &capability, int &defaultBits) const
{
    capability = defaultBits=0;
    return;
}

void loadMesh(MeshModel &m, const int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
              const e57::Reader &fileReader, vcg::CallBackPos* positionCallback) {

    e57::Data3DPointsData data3DPointsData{};
    // allocate the buffers to store points position
    data3DPointsData.cartesianX = new float[buffSize];
    data3DPointsData.cartesianY = new float[buffSize];
    data3DPointsData.cartesianZ = new float[buffSize];

    auto currentLoadingPercentage = 0;
    auto loadingScale = (100 / numberPointSize);

    auto size = 0UL;
    auto vertexIterator = vcg::tri::Allocator<CMeshO>::AddVertices(m.cm, static_cast<size_t>(numberPointSize));
    auto dataReader = fileReader.SetUpData3DPointsData(scanIndex, buffSize, data3DPointsData);

    m.Enable(mask);

    // read the data from the E57 file
    try {

        while ((size = dataReader.read()) > 0) {

            for (auto i = 0UL; i < size; i++, vertexIterator++) {

                // read the x, y, z coordinates from the Data3DPoints
                auto& currentPoint = (*vertexIterator).P();
                auto x = data3DPointsData.cartesianX[i], y = data3DPointsData.cartesianY[i],
                        z = data3DPointsData.cartesianZ[i];

                currentPoint[0] = x;
                currentPoint[1] = y;
                currentPoint[2] = z;

                currentLoadingPercentage += loadingScale;

                if (positionCallback)
                    updateProgress(*positionCallback, currentLoadingPercentage, LOADING_POINTS);
            }
        }
    }
    catch (const e57::E57Exception& e) {

        // free the used memory
        delete[] data3DPointsData.cartesianX;
        delete[] data3DPointsData.cartesianY;
        delete[] data3DPointsData.cartesianZ;

        throw e;
    }

    // free the used memory
    delete[] data3DPointsData.cartesianX;
    delete[] data3DPointsData.cartesianY;
    delete[] data3DPointsData.cartesianZ;
}

void updateProgress(vcg::CallBackPos& positionCallback, int percentage, const char* description) noexcept {

    positionCallback(percentage, description);
}

MESHLAB_PLUGIN_NAME_EXPORTER(E57IOPlugin)
