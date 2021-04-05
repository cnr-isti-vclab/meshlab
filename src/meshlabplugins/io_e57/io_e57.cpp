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
#define DONE_LOADING  "Done!"

#define E57_DEBUG 1
#define E57_WRAPPER(e57f, exceptionMessage) if (!e57f) throw MLException(QString{exceptionMessage})

void debug(const char* message) noexcept;
void debug(std::string& message) noexcept;

void updateProgress(vcg::CallBackPos &positionCallback, int percentage, const char* description) noexcept;

void E57IOPlugin::initPreOpenParameter(const QString &format, RichParameterList & parlst)
{
	return;
}

void E57IOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &parlst, vcg::CallBackPos* cb)
{

    // TODO: format exception messages

    mask = 0;

    if (formatName.toUpper() != tr("E57")) {
        wrongOpenFormat(formatName);
        return;
	}

    int scanIndex = 0;
    bool columnIndex = false;

    std::size_t buffSize;

    std::int64_t cols = 0;
    std::int64_t rows = 0;
    std::int64_t numberPointSize = 0;
    std::int64_t numberGroupSize = 0;
    std::int64_t numberCountSize = 0;

    std::string stdFilename = std::string{QFile::encodeName(fileName).constData()};

    e57::Reader fileReader{stdFilename};
    e57::E57Root e57FileInfo{};
    e57::Data3D scanHeader{};
    e57::Data3DPointsData data3DPointsData{};

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

    buffSize = (rows > 0) ? rows : 1024;

    // TODO: colors?

    // allocate the buffers to store points position
    data3DPointsData.cartesianX = new float[buffSize];
    data3DPointsData.cartesianY = new float[buffSize];
    data3DPointsData.cartesianZ = new float[buffSize];

    try {

        unsigned long size;
        auto vertexIterator = vcg::tri::Allocator<CMeshO>::AddVertices(m.cm, static_cast<size_t>(numberPointSize));
        e57::CompressedVectorReader dataReader = fileReader.SetUpData3DPointsData(scanIndex, buffSize, data3DPointsData);

        m.Enable(mask);

        while ((size = dataReader.read()) > 0) {
            for (unsigned long i = 0; i < size; i++, vertexIterator++) {

                auto vertex = (*vertexIterator).P();
                auto x = data3DPointsData.cartesianX[i];
                auto y = data3DPointsData.cartesianY[i];
                auto z = data3DPointsData.cartesianZ[i];

                vertex[0] = x; vertex[1] = y; vertex[2] = z;

                std::fprintf(stderr, "Debug::E57(%s) :: {x=%.3ff,y=%.3ff,z=%.3f}\n", stdFilename.c_str(), x, y, z);
            }
        }

    }
    catch (const e57::E57Exception& exception) {

        if (E57_DEBUG)
            std::fprintf(stderr, "Debug::E57(%s) Exception(%s::%s)\n", stdFilename.c_str(), exception.what(), exception.context().c_str());

        delete[] data3DPointsData.cartesianX;
        delete[] data3DPointsData.cartesianY;
        delete[] data3DPointsData.cartesianZ;

        fileReader.Close();

        throw MLException(QString{exception.what()});
    }


    if (E57_DEBUG) {
        std::fprintf(stderr, "Debug::E57(%s) :: E57 Root(guid): %s\n", stdFilename.c_str(), e57FileInfo.guid.c_str());
        std::fprintf(stderr, "Debug::E57(%s) :: E57 Root(2D Count): %lld\n", stdFilename.c_str(), fileReader.GetImage2DCount());
    }

    updateProgress(*cb, 99, DONE_LOADING);

    delete[] data3DPointsData.cartesianX;
    delete[] data3DPointsData.cartesianY;
    delete[] data3DPointsData.cartesianZ;

    // close the file to free the memory
    fileReader.Close();
}

void E57IOPlugin::save(const QString & formatName, const QString & /*fileName*/, MeshModel & /*m*/, const int /*mask*/, const RichParameterList &, vcg::CallBackPos * /*cb*/)
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

void updateProgress(vcg::CallBackPos& positionCallback, int percentage, const char* description) noexcept {

    if (positionCallback != nullptr) {
        positionCallback(percentage, description);
    }
}

MESHLAB_PLUGIN_NAME_EXPORTER(E57IOPlugin)
