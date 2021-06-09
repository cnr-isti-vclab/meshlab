/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
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
#ifndef E57IOPLUGIN_H
#define E57IOPLUGIN_H

#include <QObject>

#include <common/plugins/interfaces/io_plugin.h>
#include <common/ml_document/mesh_model.h>
#include <external/e57/include/E57Format.h>
#include <E57SimpleReader.h>

typedef typename CMeshO::VertexIterator VertexIterator;

namespace vcg {
    namespace tri {
        namespace io {
            class E57Data3DPoints {

            private:
                e57::Data3DPointsData data3DPointsData{};

            public:

                E57Data3DPoints(const E57Data3DPoints&) = delete;

                explicit E57Data3DPoints(size_t buffSize, e57::Data3D& scanHeader) {

                    if (scanHeader.pointFields.cartesianXField && scanHeader.pointFields.cartesianYField &&
                        scanHeader.pointFields.cartesianZField) {
                        data3DPointsData.cartesianX = new float[buffSize];
                        data3DPointsData.cartesianY = new float[buffSize];
                        data3DPointsData.cartesianZ = new float[buffSize];
                    }

                    if (scanHeader.pointFields.intensityField) {
                        data3DPointsData.intensity = new float[buffSize];
                    }

                    if (scanHeader.pointFields.colorRedField && scanHeader.pointFields.colorGreenField &&
                        scanHeader.pointFields.colorBlueField) {

                        data3DPointsData.colorRed = new uint8_t[buffSize];
                        data3DPointsData.colorGreen = new uint8_t[buffSize];
                        data3DPointsData.colorBlue = new uint8_t[buffSize];
                    }

                    if (scanHeader.pointFields.normalX && scanHeader.pointFields.normalY &&
                        scanHeader.pointFields.normalZ) {

                        data3DPointsData.normalX = new float[buffSize];
                        data3DPointsData.normalY = new float[buffSize];
                        data3DPointsData.normalZ = new float[buffSize];
                    }
                }

                ~E57Data3DPoints() {

                    if (areCoordinatesAvailable()) {
                        delete[] this->data3DPointsData.cartesianX;
                        delete[] this->data3DPointsData.cartesianY;
                        delete[] this->data3DPointsData.cartesianZ;
                    }

                    if (isQualityAvailable()) {
                        delete[] this->data3DPointsData.intensity;
                    }

                    if (areNormalsAvailable()) {
                        delete[] this->data3DPointsData.normalX;
                        delete[] this->data3DPointsData.normalY;
                        delete[] this->data3DPointsData.normalZ;
                    }

                    if (areColorsAvailable()) {
                        delete[] this->data3DPointsData.colorRed;
                        delete[] this->data3DPointsData.colorGreen;
                        delete[] this->data3DPointsData.colorBlue;
                    }
                }

                inline bool areCoordinatesAvailable() const {
                    return this->data3DPointsData.cartesianX && this->data3DPointsData.cartesianY && this->data3DPointsData.cartesianZ;
                }

                inline bool areColorsAvailable() const {
                    return this->data3DPointsData.colorRed && this->data3DPointsData.colorGreen && this->data3DPointsData.colorBlue;
                }

                inline bool areNormalsAvailable() const {
                    return this->data3DPointsData.normalX && this->data3DPointsData.normalY && this->data3DPointsData.normalZ;
                }

                inline bool isQualityAvailable() const {
                    return this->data3DPointsData.intensity;
                }

                inline e57::Data3DPointsData& points() noexcept {
                    return data3DPointsData;
                }
            };
        }
    }
}

class E57IOPlugin : public QObject, public IOPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(IO_PLUGIN_IID)
	Q_INTERFACES(IOPlugin)
  
public:

    QString pluginName() const;
	std::list<FileFormat> importFormats() const;
	std::list<FileFormat> exportFormats() const;

	virtual void exportMaskCapability(const QString &format, int &capability, int &defaultBits) const;

	unsigned int numberMeshesContainedInFile(const QString& format, const QString& fileName, const RichParameterList& preParams) const;

	void open(const QString &formatName, const QString &fileName, MeshModel &m,
           int& mask, const RichParameterList &, vcg::CallBackPos *cb = 0);
	void open(const QString &formatName, const QString &fileName, const std::list<MeshModel*>& meshModelList,
           std::list<int>& maskList, const RichParameterList& par, vcg::CallBackPos* cb = 0) override;

	void save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask,
           const RichParameterList&, vcg::CallBackPos *cb);


private:

    /***
     * Extract images contained inside the read E57 file and write them to
     * the same read file location.
     * @param fileReader The current file reader of the opened file
     * @param cb Callback to update the progressbar contained in MeshLab
     */
    static void extractImages(const e57::Reader &fileReader, vcg::CallBackPos* cb);

    /***
     * Load the cloud points read from the E57 file, inside the mesh to display.
     * @param m The mesh to display
     * @param mask
     * @param scanIndex Data block index given by the NewData3D
     * @param buffSize Dimension for buffer size
     * @param numberPointSize How many points are contained inside the cloud
     * @param fileReader The file reader object used to scan the file
     * @param cb Callback to update the progressbar contained in MeshLab
     */
    static void loadMesh(MeshModel &m, int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
                         const e57::Reader &fileReader, e57::Data3D& scanHeader, vcg::CallBackPos* positionCallback);

    // TODO: add function documentation
    static void writeVertices(e57::CompressedVectorWriter &dataWriter, vcg::tri::io::E57Data3DPoints& data3DPoints,
                              int count, int remaining, vcgTriMesh::VertContainer &vertices);

};

#endif
