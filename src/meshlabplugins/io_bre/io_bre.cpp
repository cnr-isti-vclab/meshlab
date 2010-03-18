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
#include <QtGui>
#include <QFile>

#include "io_bre.h"
#include <stdlib.h>
#include <wrap/io_trimesh/export.h>

#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/clean.h>

#include <vcg/complex/trimesh/allocate.h>

using namespace std;
using namespace vcg;

///////////////////////////////////////////////////////////////////////////////////////////
// class ImporterBRE
///////////////////////////////////////////////////////////////////////////////////////////
template <class OpenMeshType>
int vcg::tri::io::ImporterBRE<OpenMeshType>::Open( MeshModel &meshModel, OpenMeshType &m, int& mask, const QString &filename, bool pointsonly, CallBackPos *cb)
{
  QFile file(filename, NULL);
  m.Clear();  
  if( false == file.open(QFile::ReadOnly) )
  {
    return E_CANTOPEN;
  }
  // read Bre header
  BreHeader header;
  if( false == header.Read(file))
  {
    return E_UNABLEREADHEADER;
  }
  //test if DataType is supported
  int test_type = header.DataType();
  if ((test_type != 0) && (test_type != -1))
  {
    return E_NOTSUPPORTED;
  }

  //create VertexGrid
  VertexGrid grid(header.ExtentX(), header.ExtentY());

  qint64 breElementSize = 20;
  qint64 TestSize = (file.size() - header.Size()) % breElementSize; //test if file is valid
  if (TestSize != 0)
  {
    return E_INVALIDFILE;
  }

  int numberElements = (file.size() - header.Size()) / breElementSize; //get number of Bre elements

  if (( header.Version() == 0x101 ) || ( header.Version() == 0x201 ))
  {
    //enable colors and quality
    mask = vcg::tri::io::Mask::IOM_VERTCOLOR | vcg::tri::io::Mask::IOM_VERTQUALITY | vcg::tri::io::Mask::IOM_VERTTEXCOORD; 
    meshModel.Enable(mask);
    
    //Add camera position and image width and height
    m.shot.Extrinsics.Tra() = header.CameraPosition();
    m.shot.Intrinsics.ViewportPx[0] = header.ExtentX();
    m.shot.Intrinsics.ViewportPx[1] = header.ExtentY();
    //Add projector position as mesh attribute
    CMeshO::PerMeshAttributeHandle<Point3f> proPos = vcg::tri::Allocator<CMeshO>::AddPerMeshAttribute<Point3f>  (m,std::string("Projector position"));
    proPos() = header.ProjectorPosition();

    int result;
    if (pointsonly == true) 
    {
      //in the case of points only the number of added vertices is known and Allocator can be called only once before reading the elements
      CMeshO::VertexIterator vertexItr=vcg::tri::Allocator<CMeshO>::AddVertices(m, numberElements);
      //read in the Bre elements (vertices, colors and quality)
      result = vcg::tri::io::BreElement::ReadBreElementsRaw( file, vertexItr, numberElements, cb); 
    }
    else
    {
      //read in the Bre elements (vertices, colors and quality) and triangulate everything
      result = vcg::tri::io::ReadBreElementsInGrid(file, grid,m, test_type,numberElements, cb);
    }
    if ((result == 0) && (header.Transformed() == true))
    {
      //if transformed before, undo transformation (will be changed soon)
      Matrix44f inverse = vcg::Inverse(header.Matrix());
      m.Tr = inverse;
      return result;
    }
    else
    {
      return result;
    }
    
  }
  else
  {
    return E_NOTSUPPORTED; //format not supported
  }
  return E_NOERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////
// class BreMeshIOPlugin
///////////////////////////////////////////////////////////////////////////////////////////

// initialize importing parameters
void BreMeshIOPlugin::initPreOpenParameter(const QString &formatName, const QString &/*filename*/, RichParameterSet &parlst)
{
  
	if (formatName.toUpper() == tr("BRE"))
	{
		parlst.addParam(new RichBool("pointsonly",false,"only import points","Just import points, without triangulation"));
	}
  
}

bool BreMeshIOPlugin::open(const QString &/*formatName*/, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &parlst, CallBackPos *cb, QWidget * /*parent*/)
{
  // initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");
  mask = 0;
  QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";
  bool points = parlst.getBool("pointsonly");
  int result = vcg::tri::io::ImporterBRE<CMeshO>::Open(m, m.cm, mask, fileName,points, cb);
  if (result != 0) // all the importers return 0 on success
	{
	  errorMessage = errorMsgFormat.arg(fileName, ErrorMsg(result));
	  return false;
	}
  return true;
}

bool BreMeshIOPlugin::save(const QString &/*formatName*/,const QString &/*fileName*/, MeshModel &, const int /*mask*/, const RichParameterSet & /*par*/, CallBackPos *, QWidget */*parent*/)
{
  return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> BreMeshIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Breuckmann File Format"	, tr("BRE"));

	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> BreMeshIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	//formatList << Format("Breuckmann File Format"	, tr("BRE"));
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void BreMeshIOPlugin::GetExportMaskCapability(QString &/*format*/, int &/*capability*/, int &/*defaultBits*/) const
{
	/*if(format.toUpper() == tr("BRE"))
  {
    capability = 0;
    defaultBits = 0;
  }*/
}

void BreMeshIOPlugin::initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par) 
{
	if(format.toUpper() == tr("BRE"))
		par.addParam(new RichBool("Unify",true, "Unify Duplicated Vertices",
								"The STL format is not an vertex-indexed format. Each triangle is composed by independent vertices, so, usually, duplicated vertices should be unified"));		
  
}
void BreMeshIOPlugin::initSaveParameter(const QString &/*format*/, MeshModel &/*m*/, RichParameterSet &/*par*/)
{
  /*
	if(format.toUpper() == tr("STL") || format.toUpper() == tr("PLY"))
		par.addParam(new RichBool("Binary",true, "Binary encoding",
								"Save the mesh using a binary encoding. If false the mesh is saved in a plain, readable ascii format"));		
  */
}

void BreMeshIOPlugin::applyOpenParameter(const QString &format, MeshModel &m, const RichParameterSet &par) 
{
	if(format.toUpper() == tr("BRE"))
  {
		if(par.findParameter("Unify")->val->getBool())
    {
			tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////////
//
//  BreHeader
//  class to work with the header of .bre files.
//  Not everything in use yet.
//
/////////////////////////////////////////////////////////////////////////////////

vcg::tri::io::BreHeader::BreHeader() 
:m_data(1024, 0)
{
}

vcg::tri::io::BreHeader::~BreHeader() 
{
}

bool vcg::tri::io::BreHeader::Read(QFile &file) 
{
  if(m_data.size() != 1024)
  {
    m_data = m_data.fill(0, 1024);
  }
  //if reading is not successful, m_data will be refilled with 0 
  bool success = ( 1 == file.read(m_data.data(), 256));
  
  const QString testBR = "BR";
  QString testStr = QString::fromAscii(m_data.data()+6, 2);
  success = (QString::compare(testBR, testStr) == 0);

  if ( success && Size() > 256 ) 
  {
    success = (file.read(m_data.data()+256, (Size()-256)) == (Size()-256));
  }
  if ( !success ) 
  {
    m_data = m_data.fill(0, 1024);
  }
  return success;
}

int vcg::tri::io::BreHeader::Version() const 
{
  return *((quint16 *) (m_data.data() + 2));
}

float vcg::tri::io::BreHeader::SpacingX() const 
{
  return *((float*) (m_data.data() + 30));
}

float vcg::tri::io::BreHeader::SpacingY() const 
{
  return *((float*) (m_data.data() + 34));
}

Point3f vcg::tri::io::BreHeader::CameraPosition() const
{
  Point3f result;
  result[0] = *((float*) (m_data.data() + 38));
  result[1] = *((float*) (m_data.data() + 42));
  result[2] = *((float*) (m_data.data() + 46));
  return result;
}

Point3f vcg::tri::io::BreHeader::ProjectorPosition() const
{
  Point3f result;
  result[0] = *((float*) (m_data.data() + 50));
  result[1] = *((float*) (m_data.data() + 54));
  result[2] = *((float*) (m_data.data() + 58));
  return result;
}


  int vcg::tri::io::BreHeader::DataType() const {
    if ( Version() != 0x201 ) {
      return -1;
    }

    return (*(qint32*) (m_data.data() + 620)) != 0;
  }


int vcg::tri::io::BreHeader::ExtentX() const 
{
  return *((quint16*) (m_data.data() + 14));
}

int vcg::tri::io::BreHeader::ExtentY() const 
{
  return *((quint16*) (m_data.data() + 16));
}

int vcg::tri::io::BreHeader::Size() const 
{
  return *((quint16*) (m_data.data() + 4));
}


Matrix44f vcg::tri::io::BreHeader::Matrix() const 
{
  Matrix44f matrix;
  float *ptr = (float*) (m_data.data() + 128);

  for ( int i=0; i<4; i++) 
  {
    for ( int j=0; j<4; j++) 
    {
      matrix.ElementAt(i,j) = *(ptr + i*4 + j);
    }
  }
  return matrix;
}


bool vcg::tri::io::BreHeader::Transformed() const 
{
  if ( Version() != 0x101 && Version() != 0x201 ) 
  {
    return false;
  }

  return (*(quint16*) (m_data.data() + 62)) != 0;
}

/////////////////////////////////////////////////////////////////////////////////
//
//  BreElement
//  class to work with the elements in a .bre file
//
/////////////////////////////////////////////////////////////////////////////////

vcg::tri::io::BreElement::BreElement()
: m_data(20, 0)
{
}

bool vcg::tri::io::BreElement::Read(QFile &file) 
{
  if(m_data.size() != 20)
  {
    m_data.fill(0, 20);
  }

  //if reading is not successful, m_data will be refilled with 0 
  if ( 20 != file.read(m_data.data(), 20) ) 
  {
    m_data.fill(0, 20);
    return false;
  }
  return true;
}

Point3f vcg::tri::io::BreElement::Coord() const 
{
  float *p = (float*)m_data.data();
  return Point3f( p[0], p[1], p[2] );
}

uchar vcg::tri::io::BreElement::Quality() const
{
  uchar quality = *((uchar*)(m_data.data()+12));  
  return quality;
}

QPoint vcg::tri::io::BreElement::Pixel() const 
{
  QPoint pnt;
  pnt.setX(static_cast<float>(*((quint16*)(m_data.data()+14))));
  pnt.setY(static_cast<float>(*((quint16*)(m_data.data()+16))));
  return pnt;
}

uchar vcg::tri::io::BreElement::Red() const 
{
  quint16 rgb = *((quint16*)(m_data.data()+18));
  return (rgb  >> 7) & 0xf8;
}

uchar vcg::tri::io::BreElement::Green() const 
{
  quint16 rgb = *((quint16*)(m_data.data()+18));
  return (rgb  >> 2) & 0xf8;
}

uchar vcg::tri::io::BreElement::Blue() const 
{
  quint16 rgb = *((quint16*)(m_data.data()+18));
  return (rgb  << 3) & 0xf8;
}

int vcg::tri::io::BreElement::ReadBreElementsRaw( QFile &file, CMeshO::VertexIterator &it, int numberElements, CallBackPos *cb) 
{
  Point3f curPoint;
  int num = 0;
  vcg::tri::io::BreElement elem;

  while ( false == file.atEnd() ) 
  {
    if ( !elem.Read(file) ) 
    {
      return num;
    }
    num++;
    curPoint = elem.Coord();
    (*it).P().Import(curPoint);
    (*it).C()[0] = elem.Red();
    (*it).C()[1] = elem.Green();
    (*it).C()[2] = elem.Blue();
    (*it).C()[3] = 255;
    (*it).Q() = elem.Quality();
    (*cb)(100*(num/numberElements), "Reading Elements...");
    ++it;
  }

  if (num <= 1)
  {
    return E_EMPTYFILEPOINTS;
  }
  return E_NOERROR;
}

/////////////////////////////////////////////////////////////////////////////////
//
//  VertexGrid
//  class to sort the Vertices in a Grid (to triangulate them later)
//
/////////////////////////////////////////////////////////////////////////////////

vcg::tri::io::VertexGrid::VertexGrid(int width, int height) 
:m_width(width)
,m_height(height)
{
  m_grid.resize(m_width*m_height*sizeof(vcg::tri::io::VertexGrid::Vertex));
  m_grid.fill('0');
}

vcg::tri::io::VertexGrid::~VertexGrid() 
{
}


void  vcg::tri::io::VertexGrid::SetValue( int col,  int row , Point3f curPoint, GLbyte red, GLbyte green, GLbyte blue, uchar quality) 
{
  if ((col > m_width) || (row > m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return; //out of grid range
  }
  vcg::tri::io::VertexGrid::Vertex curVertex;
  curVertex.X = curPoint.X();
  curVertex.Y = curPoint.Y();
  curVertex.Z = curPoint.Z();
  curVertex.Red = red;
  curVertex.Green = green;
  curVertex.Blue = blue;
  curVertex.Valid = 1;
  curVertex.Quality = quality;

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  *position = curVertex;
}

Point3f vcg::tri::io::VertexGrid::GetValue( int col,  int row) 
{
  if ((col > m_width) || (row > m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return 0; //out of grid range
  }

  Point3f result;

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  result.X() = (*position).X;
  result.Y() = (*position).Y;
  result.Z() = (*position).Z;

  return result;
}

GLbyte vcg::tri::io::VertexGrid::Red( int col,  int row) 
{
  if ((col > m_width) || (row > m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return E_RANGERED; //out of grid range (red)
  }

  GLbyte result;

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  result = static_cast<GLbyte>((*position).Red);
  
  return result;
}

GLbyte vcg::tri::io::VertexGrid::Green( int col,  int row) 
{
  if ((col > m_width) || (row > m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return E_RANGEGREEN; //out of grid range (green)
  }

  GLbyte result;

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  result = static_cast<GLbyte>((*position).Green);
  
  return result;
}

GLbyte vcg::tri::io::VertexGrid::Blue( int col,  int row) 
{
  if ((col > m_width) || (row > m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return E_RANGEBLUE;//out of grid range (blue)
  }

  GLbyte result;

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  result = static_cast<GLbyte>((*position).Blue);
  
  return result;
}

uchar vcg::tri::io::VertexGrid::Quality( int col,  int row) 
{
  if ((col > m_width) || (row > m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return E_RANGEQUALITY;//out of grid range (blue)
  }

  GLbyte result;

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  result = static_cast<uchar>((*position).Quality);
  
  return result;
}


bool vcg::tri::io::VertexGrid::IsValid( int col, int row)
{
  if ((col >= m_width) || (row >= m_height) || ((int)(col*row*sizeof(vcg::tri::io::VertexGrid::Vertex)) > m_grid.size()))
  {
    return E_RANGEVAL; //out of grid range (val)
  }

  vcg::tri::io::VertexGrid::Vertex *position = ((vcg::tri::io::VertexGrid::Vertex*)(m_grid.data())) + ((row * m_width) + col);
  
  return (((*position).Valid == 1) ? true : false);
}



//function reads in the BreElements, writes them in a VertexGrid and creates a mesh
int vcg::tri::io::ReadBreElementsInGrid( QFile &file, VertexGrid &grid, CMeshO &m, int dataType, int numberElements, vcg::CallBackPos *cb) 
{
  CMeshO::PerMeshAttributeHandle<Point3f> test_index = tri::Allocator<CMeshO>::GetPerMeshAttribute<Point3f>(m, "Camera Position");
  Point3f curPoint;
  QPoint curPixel;
  GLbyte curRed, curGreen, curBlue;
  uchar curQuality;

  int num = 0;
  vcg::tri::io::BreElement elem;
  while ( false == file.atEnd() ) //read in all BreElements
  {
    if ( !elem.Read(file) ) 
    {
      return num;
    }
    num++;
    curPoint = elem.Coord();
    curPixel = elem.Pixel();
    curRed = elem.Red();
    curGreen = elem.Green();
    curBlue = elem.Blue();
    curQuality = elem.Quality();
    //write value in VertexGrid
    grid.SetValue( curPixel.x(), curPixel.y(), curPoint, curRed, curGreen, curBlue, curQuality);
    (*cb)(20*(num/numberElements), "Reading Elements...");
    
  }
  
  //creating mesh
  //going through the whole grid, testing if Valid. 
  //Only Points that are valid and have enough valid neigbours to form a triangle will be added.
  
  float cbstep = ((float)(80)/(float)(num));//for the progress bar
  float cbvalue = 0.f;//for the progress bar
  bool wasAdded = false;//for the progress bar
  unsigned int pointsAdded = 0;//for the progress bar
  

  for (int i=0; i<(grid.m_height-1); ++i)
  {
    for (int j=0; j<(grid.m_width-1); ++j)
    {
      if (grid.IsValid(j,i) == false)
      {
        continue;
      }
      if (grid.IsValid(j+1,i+1) == false)
      {
        continue;
      }
      if (grid.IsValid(j+1,i) == true)
      {
        CMeshO::FaceIterator faceItr=vcg::tri::Allocator<CMeshO>::AddFaces(m,1);
        CMeshO::VertexIterator vertexItr=vcg::tri::Allocator<CMeshO>::AddVertices(m, 3);
        curPoint = grid.GetValue(j,i);
        (*vertexItr).P().Import(curPoint);
        (*vertexItr).T().U() = j;
        (*vertexItr).T().V() = i;
        (*vertexItr).C()[0] = grid.Red(j,i);
        (*vertexItr).C()[1] = grid.Green(j,i);
        (*vertexItr).C()[2] = grid.Blue(j,i);
        (*vertexItr).C()[3] = 255;
        (*vertexItr).Q() = grid.Quality(j,i);
        (*faceItr).V(0)=&*vertexItr; 
        vertexItr++;
        curPoint = grid.GetValue(j+1,i + ((-1)*dataType));
        (*vertexItr).T().U() = j + 1;
        (*vertexItr).T().V() = i + ((-1)*dataType);
        (*vertexItr).P().Import(curPoint);
        (*vertexItr).C()[0] = grid.Red(j+1,i + ((-1)*dataType));
        (*vertexItr).C()[1] = grid.Green(j+1,i + ((-1)*dataType));
        (*vertexItr).C()[2] = grid.Blue(j+1,i + ((-1)*dataType));
        (*vertexItr).C()[3] = 255;
        (*vertexItr).Q() = grid.Quality(j+1,i + ((-1)*dataType));
        (*faceItr).V(1)=&*vertexItr; 
        vertexItr++;
        curPoint = grid.GetValue(j+1,i + 1 + dataType);
        (*vertexItr).P().Import(curPoint);
        (*vertexItr).T().U() = j + 1;
        (*vertexItr).T().V() = i + 1 + dataType;
        (*vertexItr).C()[0] = grid.Red(j+1,i + 1 + dataType);
        (*vertexItr).C()[1] = grid.Green(j+1,i + 1 + dataType);
        (*vertexItr).C()[2] = grid.Blue(j+1,i + 1 + dataType);
        (*vertexItr).C()[3] = 255;
        (*vertexItr).Q() = grid.Quality(j+1,i + 1 + dataType);
        (*faceItr).V(2)=&*vertexItr; 
        vertexItr++;
        wasAdded = true;
      }
      if (grid.IsValid(j,i+1) == true)
      {
        CMeshO::FaceIterator faceItr=vcg::tri::Allocator<CMeshO>::AddFaces(m,1);
        CMeshO::VertexIterator vertexItr=vcg::tri::Allocator<CMeshO>::AddVertices(m, 3);
        curPoint = grid.GetValue(j,i);
        (*vertexItr).P().Import(curPoint);
        (*vertexItr).T().U() = j;
        (*vertexItr).T().V() = i;
        (*vertexItr).C()[0] = grid.Red(j,i);
        (*vertexItr).C()[1] = grid.Green(j,i);
        (*vertexItr).C()[2] = grid.Blue(j,i);
        (*vertexItr).C()[3] = 255;
        (*vertexItr).Q() = grid.Quality(j,i);
        (*faceItr).V(0)=&*vertexItr;
        vertexItr++;
        curPoint = grid.GetValue(j + 1 + dataType,i+1);
        (*vertexItr).P().Import(curPoint);
        (*vertexItr).T().U() = j + 1 + dataType;
        (*vertexItr).T().V() = i + 1;
        (*vertexItr).C()[0] = grid.Red(j + 1 + dataType,i+1);
        (*vertexItr).C()[1] = grid.Green(j + 1 + dataType,i+1);
        (*vertexItr).C()[2] = grid.Blue(j + 1 + dataType,i+1);
        (*vertexItr).C()[3] = 255;
        (*vertexItr).Q() = grid.Quality(j + 1 + dataType,i+1);
        (*faceItr).V(1)=&*vertexItr; 
        vertexItr++;
        curPoint = grid.GetValue(j + ((-1)*dataType),i+1);
        (*vertexItr).P().Import(curPoint);
        (*vertexItr).T().U() = j + ((-1)*dataType);
        (*vertexItr).T().V() = i + 1;
        (*vertexItr).C()[0] = grid.Red(j + ((-1)*dataType),i+1);
        (*vertexItr).C()[1] = grid.Green(j + ((-1)*dataType),i+1);
        (*vertexItr).C()[2] = grid.Blue(j + ((-1)*dataType),i+1);
        (*vertexItr).C()[3] = 255;
        (*vertexItr).Q() = grid.Quality(j + ((-1)*dataType),i+1);
        (*faceItr).V(2)=&*vertexItr; 
        vertexItr++;
        wasAdded = true;
      }

      if(true == wasAdded)
      {
        pointsAdded++;
        cbvalue += cbstep;
      }
      wasAdded = false;
    }
    if(pointsAdded > 100) //for the progress bar. Calling cb too often results in no update for the progress bar
    {                     //because of the timer in cb
      (*cb)(20+(int)cbvalue, "Triangulation");
      pointsAdded = 0;
    }
  }

  if (m.vert.size() == 0)
  {
    return E_EMPTYFILEFACES;
  }
  return E_NOERROR;
}

/*

bool BGrid<T>::TriangleContainZ( const OpenObjects::TCoord<T>& a, const OpenObjects::TCoord<T>& b, const OpenObjects::TCoord<T>& c, const OpenObjects::TCoord<T>& point ) const 
{
  const static T tolerance = std::numeric_limits<T>::epsilon() * static_cast<T>( -100.0 );

  double det = (b.X() - a.X()) * (point.Y() - a.Y()) - (b.Y() - a.Y()) * (point.X() - a.X());
  if ( det < tolerance ) 
  {
    return false;
  }
  det = (c.X() - b.X()) * (point.Y() - b.Y()) - (c.Y() - b.Y()) * (point.X() - b.X());
  if ( det < tolerance ) 
  {
    return false;
  }
  det = (a.X() - c.X()) * (point.Y() - c.Y()) - (a.Y() - c.Y()) * (point.X() - c.X());
  
  return ! (det < tolerance );
}*/









Q_EXPORT_PLUGIN(BreMeshIOPlugin)
