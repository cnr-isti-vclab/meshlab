/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
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
#ifndef IOBREPLUGIN_H
#define IOBREPLUGIN_H

#include <common/interfaces.h>

namespace vcg {
namespace tri {
namespace io  {

  template <class OpenMeshType>
  class ImporterBRE
  {
   
  public:
    typedef typename OpenMeshType::VertexPointer VertexPointer;
    typedef typename OpenMeshType::ScalarType ScalarType;
    typedef typename OpenMeshType::VertexType VertexType;
    typedef typename OpenMeshType::FaceType FaceType;
    typedef typename OpenMeshType::VertexIterator VertexIterator;
    typedef typename OpenMeshType::FaceIterator FaceIterator;

    static int Open( MeshModel &meshModel, OpenMeshType &m, int& mask, const QString &filename, bool pointsonly, CallBackPos *cb);

  };


  /////////////////////////////////////////////////////////////////////////////////
  //
  //  BreHeader
  //
  /////////////////////////////////////////////////////////////////////////////////
  class BreHeader 
  {
  public:
  	BreHeader();
  	virtual ~BreHeader();
  	bool Read(QFile &file);
  	int   Version() const;
  	// 0 = undef, 0x101 = erweitertes Format, 0x201 = erweitertes Raster Format
  	int   Size() const;
  	int   ExtentX() const;
  	int   ExtentY() const;
    Point3f CameraPosition() const;
    Point3f ProjectorPosition() const;
  	// Ausdehnung in X und Y Richtung
	  float SpacingX() const;
	  float SpacingY() const;
  	bool  Transformed() const;
  	// Abfrage, ob Matrix() schon auf die 3D Daten angewendet wurde.
	  Matrix44f Matrix() const;


    int DataType() const;
      // 0 : RASTER_DATA     
      // 1 : SECTION_DATA
      // 2 : CLOUD_DATA
      // -1: UNDEF , wenn Version() != 0x201


  protected:
  	QByteArray  m_data;
  };
  /////////////////////////////////////////////////////////////////////////////////
  //
  //  BreElement 
  //
  /////////////////////////////////////////////////////////////////////////////////
  class BreElement 
  {
  public:
    BreElement();
    bool Read(QFile & );
    Point3f Coord() const;
    uchar Quality() const;
    QPoint Pixel() const;
    uchar Red() const;
    uchar Green() const;
    uchar Blue() const;

    static int ReadBreElementsRaw( QFile &file, CMeshO::VertexIterator &it, int numberElements, CallBackPos *cb);

  private:
    QByteArray  m_data;
  };

  /////////////////////////////////////////////////////////////////////////////////
  //
  //  VertexGrid
  //
  /////////////////////////////////////////////////////////////////////////////////
  class VertexGrid
  {
  public:
    //VertexGrid();
    VertexGrid(int width, int height);
    void SetValue( int col,  int row , Point3f curPointt, GLbyte red, GLbyte green, GLbyte blue, uchar quality);
    Point3f GetValue(int col,  int row);
    GLbyte Red( int col,  int row);
    GLbyte Green( int col,  int row);
    GLbyte Blue( int col,  int row);
    uchar Quality( int col,  int row);
    bool IsValid(int col, int row);
    ~VertexGrid();
    int m_width;
    int m_height;

  private:
    QByteArray m_grid;

  public:
    struct Vertex
    {
      unsigned char Valid;
      float X;
      float Y;
      float Z;
      uchar Quality;
      GLbyte Red;
      GLbyte Green;
      GLbyte Blue;
    };
  };

  int ReadBreElementsInGrid( QFile &file, vcg::tri::io::VertexGrid &grid, CMeshO &m, int dataType,int numberElements, vcg::CallBackPos *cb);
  void UndoTransformation( CMeshO& mesh, const Matrix44f& matrix );
}//namespace io
}//namespace tri
}//namespace vcg

class BreMeshIOPlugin : public QObject, public MeshIOInterface
{
  Q_OBJECT
  Q_INTERFACES(MeshIOInterface)
  
public:
	
  BreMeshIOPlugin() : MeshIOInterface() {}

  QList<Format> importFormats() const;
  QList<Format> exportFormats() const;

  void GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const;

  bool open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask,const RichParameterSet & par, vcg::CallBackPos *cb=0, QWidget *parent=0);
  bool save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet & par, vcg::CallBackPos *cb=0, QWidget *parent= 0);
  virtual void initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterSet & par);
  virtual void applyOpenParameter(const QString &format, MeshModel &m, const RichParameterSet &par);
  void initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterSet &parlst);
  virtual void initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterSet & par);

};


static char* errorStr;

enum BreError
{
	E_NOERROR,				  //  0
	E_CANTOPEN,				  //  1
	E_UNABLEREADHEADER, //  2
  E_INVALIDFILE,			//  3
	E_NOTSUPPORTED,     //  4
  E_RANGESET,         //  5
  E_RANGEGET,         //  6
  E_RANGEVAL,         //  7
  E_RANGERED,         //  8
  E_RANGEGREEN,       //  9
  E_RANGEBLUE,        // 10
  E_RANGEQUALITY,     // 11
  E_EMPTYFILEFACES,   // 12
  E_EMPTYFILEPOINTS   // 13
};
		
static const char *ErrorMsg(int error)
{
	static const char * bre_error_msg[] =
	{
		"No errors",
		"Can't open file",
    "Unable to read header",
		"Invalid file",
    "File format not supported",
    "Out of grid range, unable to set value",
    "Out of grid range, unable to get value",
    "Out of grid range, unable to get validation",
    "Out of grid range, unable to get color (red)",
    "Out of grid range, unable to get color (green)",
    "Out of grid range, unable to get color (blue)",
    "Out of grid range, unable to get quality",
    "File does not include any valid triangles",
    "File does not include any points"
  };

	if(error > 13 || error < 0) return "Unknown error";
	else if (error == 11) return errorStr;
	else return bre_error_msg[error];
};



#endif
