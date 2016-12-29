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

#include "io_txt.h"

//#include <wrap/io_trimesh/export.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool parseTXT(QString filename, CMeshO &m, int rowToSkip, int dataSeparator, int dataFormat, int rgbMode, int onError);

void TxtIOPlugin::initPreOpenParameter(const QString &format, const QString &/*fileName*/, RichParameterSet & parlst)
{
	if(format.toUpper() == tr("TXT"))
	{
            QStringList separator = (QStringList() << ";" << "," << "SPACE");
            QStringList strformat = (QStringList()   << "X Y Z"
                                                     << "X Y Z Reflectance"
                                                     << "X Y Z Reflectance R G B"
                                                     << "X Y Z Reflectance Nx Ny Nz"
                                                     << "X Y Z Reflectance R G B Nx Ny Nz"
                                                     << "X Y Z Reflectance Nx Ny Nz R G B"
                                                     << "X Y Z R G B"
                                                     << "X Y Z R G B Reflectance"
                                                     << "X Y Z R G B Reflectance Nx Ny Nz"
                                                     << "X Y Z R G B Nx Ny Nz Reflectance"
                                                     << "X Y Z Nx Ny Nz"
                                                     << "X Y Z Nx Ny Nz R G B Reflectance"
                                                     << "X Y Z Nx Ny Nz Reflectance R G B");
            QStringList rgbmode = (QStringList() << "[0-255]" << "[0.0-1.0]");
			QStringList onerror = (QStringList() << "skip" << "stop");

            parlst.addParam(new RichInt("rowToSkip", 0, "Header Row to be skipped", "The number of lines that must be skipped at the beginning of the file. Generally, these files have one or more 'header' lines, before the point list"));
            parlst.addParam(new RichEnum("strformat", 0, strformat,"Point format","Which values are specified for each point, and in which order."));
            parlst.addParam(new RichEnum("separator", 0, separator,"Separator","The separator between individual values in the point(s) description."));
            parlst.addParam(new RichEnum("rgbmode", 0, rgbmode,"Color format","Colors may be specified in the [0-255] or [0.0-1.0] interval."));
			parlst.addParam(new RichEnum("onerror", 0, onerror, "On Parsing Error", "When a line is not properly parsed, it is possible to 'skip' it and continue with the following lines, or 'stop' importing at that point"));
    }
}

bool TxtIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &parlst, CallBackPos *cb, QWidget *parent)
{
    bool result=false;

    if(formatName.toUpper() == tr("TXT"))
		{
            int rowToSkip = parlst.getInt("rowToSkip");
            int dataSeparator = parlst.getEnum("separator");
            int dataFormat = parlst.getEnum("strformat");
            int rgbMode = parlst.getEnum("rgbmode");
			int onError = parlst.getEnum("onerror");

            if(!(dataFormat==0) && !(dataFormat==6) && !(dataFormat==10))
                mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
            if(!(dataFormat==0) && !(dataFormat==10))
                mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
            if((dataFormat==3) || (dataFormat==4) || (dataFormat==5) || (dataFormat>=8))
                mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;

            m.Enable(mask);

            return parseTXT(fileName, m.cm, rowToSkip, dataSeparator, dataFormat, rgbMode, onError);
		}

	return result;
}

bool TxtIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	assert(0);
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> TxtIOPlugin::importFormats() const
{
	QList<Format> formatList;
    formatList << Format("TXT (Generic ASCII point list)", tr("TXT"));

	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> TxtIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void TxtIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
  capability=defaultBits=0;
	return;
}
 

bool parseTXT(QString filename, CMeshO &m, int rowToSkip, int dataSeparator, int dataFormat, int rgbMode, int onError)
{
    QFile impFile(filename);
    int ii;

    if(impFile.open(QIODevice::ReadOnly))
    {
        QString freshLine;
        QStringList tokenizedLine;
        QString separator;
        float XX,YY,ZZ;
        float NX,NY,NZ;
        float RR,GG,BB;
        float Qual;
        bool parseOK[10];

        //skipping first rowToSkip lines,because it's the header
        for(ii=0; ii<rowToSkip; ii++)
        {
            if(!impFile.atEnd())
                freshLine = QString(impFile.readLine()).simplified();
            else
            {
                impFile.close();
                return false;
            }
        }

        switch(dataSeparator)
        {
            case 0: separator = ";"; break;
            case 1: separator = ","; break;
            case 2: separator = " "; break;
        }

        //now, the data
        while(!impFile.atEnd())
        {
            // reading and tokenizing
            freshLine = QString(impFile.readLine()).simplified();
            tokenizedLine = freshLine.split(separator, QString::SkipEmptyParts);

            switch(dataFormat)
            {
                case 0: // X Y Z
                    {
                        // number of token mismatch
						if (tokenizedLine.size()<3)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							if (parseOK[0] && parseOK[1] && parseOK[2])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 1: // X Y Z Reflectance
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<4)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							Qual = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 2: // X Y Z Reflectance R G B
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<7)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							Qual = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							RR = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							GG = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							BB = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4] && parseOK[5] && parseOK[6])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 3: // X Y Z Reflectance Nx Ny Nz
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<7)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							Qual = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							NX = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							NY = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							NZ = tokenizedLine.at(6).toFloat(&(parseOK[6]));

							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4] && parseOK[5] && parseOK[6])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 4: // X Y Z Reflectance R G B Nx Ny Nz
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<10)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							Qual = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							RR = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							GG = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							BB = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							NX = tokenizedLine.at(7).toFloat(&(parseOK[7]));
							NY = tokenizedLine.at(8).toFloat(&(parseOK[8]));
							NZ = tokenizedLine.at(9).toFloat(&(parseOK[9]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4]
								&& parseOK[5] && parseOK[6] && parseOK[7] && parseOK[8] && parseOK[9])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 5: // X Y Z Reflectance Nx Ny Nz R G B
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<10)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							Qual = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							NX = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							NY = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							NZ = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							RR = tokenizedLine.at(7).toFloat(&(parseOK[7]));
							GG = tokenizedLine.at(8).toFloat(&(parseOK[8]));
							BB = tokenizedLine.at(9).toFloat(&(parseOK[9]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4]
								&& parseOK[5] && parseOK[6] && parseOK[7] && parseOK[8] && parseOK[9])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 6: // X Y Z R G B
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<6)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							RR = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							GG = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							BB = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4] && parseOK[5])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).C() = Color4b(RR, GG, BB, 255);
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 7: // X Y Z R G B Reflectance
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<7)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							RR = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							GG = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							BB = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							Qual = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4] && parseOK[5] && parseOK[6])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 8: // X Y Z R G B Reflectance Nx Ny Nz
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<10)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							RR = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							GG = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							BB = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							Qual = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							NX = tokenizedLine.at(7).toFloat(&(parseOK[7]));
							NY = tokenizedLine.at(8).toFloat(&(parseOK[8]));
							NZ = tokenizedLine.at(9).toFloat(&(parseOK[9]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4]
								&& parseOK[5] && parseOK[6] && parseOK[7] && parseOK[8] && parseOK[9])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 9: // X Y Z R G B Nx Ny Nz Reflectance
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<10)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							RR = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							GG = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							BB = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							NX = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							NY = tokenizedLine.at(7).toFloat(&(parseOK[7]));
							NZ = tokenizedLine.at(8).toFloat(&(parseOK[8]));
							Qual = tokenizedLine.at(9).toFloat(&(parseOK[9]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4]
								&& parseOK[5] && parseOK[6] && parseOK[7] && parseOK[8] && parseOK[9])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 10: // X Y Z Nx Ny Nz
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<6)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							NX = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							NY = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							NZ = tokenizedLine.at(5).toFloat(&(parseOK[5]));

							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4] && parseOK[5])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 11: // X Y Z Nx Ny Nz R G B Reflectance
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<10)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							NX = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							NY = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							NZ = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							RR = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							GG = tokenizedLine.at(7).toFloat(&(parseOK[7]));
							BB = tokenizedLine.at(8).toFloat(&(parseOK[8]));
							Qual = tokenizedLine.at(9).toFloat(&(parseOK[9]));
							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4]
								&& parseOK[5] && parseOK[6] && parseOK[7] && parseOK[8] && parseOK[9])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

                case 12: // X Y Z Nx Ny Nz Reflectance R G B
                    {
                        // number of token mismatch
                        if(tokenizedLine.size()<10)
						{
							if (onError == 1){ impFile.close(); return true; }
						}
						else
						{
							XX = tokenizedLine.at(0).toFloat(&(parseOK[0]));
							YY = tokenizedLine.at(1).toFloat(&(parseOK[1]));
							ZZ = tokenizedLine.at(2).toFloat(&(parseOK[2]));
							NX = tokenizedLine.at(3).toFloat(&(parseOK[3]));
							NY = tokenizedLine.at(4).toFloat(&(parseOK[4]));
							NZ = tokenizedLine.at(5).toFloat(&(parseOK[5]));
							Qual = tokenizedLine.at(6).toFloat(&(parseOK[6]));
							RR = tokenizedLine.at(7).toFloat(&(parseOK[7]));
							GG = tokenizedLine.at(8).toFloat(&(parseOK[8]));
							BB = tokenizedLine.at(9).toFloat(&(parseOK[9]));

							if (rgbMode == 1) //[0.0-1.0]
							{
								RR *= 255; GG *= 255; BB *= 255;
							}
							if (parseOK[0] && parseOK[1] && parseOK[2] && parseOK[3] && parseOK[4]
								&& parseOK[5] && parseOK[6] && parseOK[7] && parseOK[8] && parseOK[9])    // no parsing error
							{
								CMeshO::VertexIterator vi = tri::Allocator<CMeshO>::AddVertices(m, 1);
								(*vi).P().Import(Point3f(XX, YY, ZZ));
								(*vi).Q() = Qual;
								(*vi).C() = Color4b(RR, GG, BB, 255);
								(*vi).N().Import(Point3f(NX, NY, NZ));
							}
							else
								if (onError == 1){ impFile.close(); return true; }
						}
                    }
                    break;

            }


        }

        impFile.close();
        return true;
    }
    else
        return false;

}


 
 
MESHLAB_PLUGIN_NAME_EXPORTER(TxtIOPlugin)
