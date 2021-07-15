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

            /**
             * Wrapper RAII class for e57::Data3DPointsData. This wrapper class deallocates the used resources when
             * the destructor is called.
             */
            class E57Data3DPoints {

            private:
                e57::Data3DPointsData data3DPointsData{};

            public:

                E57Data3DPoints(const E57Data3DPoints&) = delete;

                explicit E57Data3DPoints(size_t buffSize, e57::Data3D& scanHeader) {

                    if (scanHeader.pointFields.cartesianXField && scanHeader.pointFields.cartesianYField &&
                        scanHeader.pointFields.cartesianZField) {

                        // XYZ coordinates
                        data3DPointsData.cartesianX = new float [buffSize];
                        data3DPointsData.cartesianY = new float [buffSize];
                        data3DPointsData.cartesianZ = new float [buffSize];
                        data3DPointsData.cartesianInvalidState = new int8_t [buffSize];
                    }

                    if (scanHeader.pointFields.intensityField) {
                        data3DPointsData.intensity = new float [buffSize];
                        data3DPointsData.isIntensityInvalid = new int8_t [buffSize];
                    }

                    if (scanHeader.pointFields.colorRedField && scanHeader.pointFields.colorGreenField &&
                        scanHeader.pointFields.colorBlueField) {

                        data3DPointsData.colorRed = new uint8_t [buffSize];
                        data3DPointsData.colorGreen = new uint8_t [buffSize];
                        data3DPointsData.colorBlue = new uint8_t [buffSize];
                        data3DPointsData.isColorInvalid = new int8_t [buffSize];
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
                        delete[] this->data3DPointsData.cartesianInvalidState;
                    }

                    if (isQualityAvailable()) {
                        delete[] this->data3DPointsData.intensity;
                        delete[] this->data3DPointsData.isIntensityInvalid;
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
                        delete[] this->data3DPointsData.isColorInvalid;
                    }
                }

                inline bool areCoordinatesAvailable() const {
                    return this->data3DPointsData.cartesianX || this->data3DPointsData.cartesianY || this->data3DPointsData.cartesianZ;
                }

                inline bool areColorsAvailable() const {
                    return this->data3DPointsData.colorRed || this->data3DPointsData.colorGreen || this->data3DPointsData.colorBlue;
                }

                inline bool areNormalsAvailable() const {
                    return this->data3DPointsData.normalX || this->data3DPointsData.normalY || this->data3DPointsData.normalZ;
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
	virtual void initPreOpenParameter(const QString &/*format*/, RichParameterList & /*par*/);

    unsigned int numberMeshesContainedInFile(const QString& format, const QString& fileName) const;

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
    std::pair<e57::Image2D, QImage> extractMeshImage(const e57::Reader &fileReader, int scanIndex, bool saveToDisk);

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
    void loadMesh(MeshModel &m, int &mask, int scanIndex, size_t buffSize, int64_t numberPointSize,
                  const e57::Reader &fileReader, e57::Data3D &scanHeader,
                  std::pair<e57::Image2D, QImage> image, const RichParameterList &par);

    /***
     * Write verticies, contained inside the mesh, into the dataWriter
     * @param dataWriter
     * @param data3DPoints
     * @param count How many verticies write
     * @param remaining How many verticies remains
     * @param vertices The verticies to write
     */
    void writeVertices(e57::CompressedVectorWriter &dataWriter, vcg::tri::io::E57Data3DPoints& data3DPoints,
                              int count, int remaining, vcgTriMesh::VertContainer &vertices);

    /***
     * Read the transform matrix inside the e57::Data3D and apply it to the mesh
     * @param meshModel The mesh to apply the transform matrix
     * @param scanHeader The meta information about the e57 mesh, from which extract the transformation matrix
     */
    void translatedAndRotateMesh(MeshModel *meshModel, const e57::Data3D &scanHeader) const;
};

#endif
