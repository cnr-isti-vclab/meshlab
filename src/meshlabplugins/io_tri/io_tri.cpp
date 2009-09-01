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
/****************************************************************************
 History
 $Log: meshio.cpp,v $
*****************************************************************************/
#include <Qt>
#include <QtGui>

#include "io_tri.h"

#include <wrap/io_trimesh/import_asc.h>
#include <wrap/io_trimesh/export.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace vcg;

bool parseTRI(const std::string &filename, CMeshO &m);

void TriIOPlugin::initPreOpenParameter(const QString &format, const QString &/*fileName*/, RichParameterSet & parlst)
{
	if(format.toUpper() == tr("ASC"))
	{
			parlst.addParam(new RichInt("rowToSkip",0,"Header Row to be skipped","The number of lines that must be skipped at the beginning of the file."));
			parlst.addParam(new RichBool("triangulate", true, "Grid triangulation", "if true it assumes that the points are arranged in a complete xy grid and it tries to perform a naive height field triangulation of the input data.  Lenght of the lines is detected automatically by searching x jumps. If the input point cloud data is not arranged as a xy regular height field, no triangles are created."));
	}
}

bool TriIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &parlst, CallBackPos *cb, QWidget *parent)
{
	bool result;
	if(formatName.toUpper() == tr("TRI"))
		{
			mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
			m.Enable(mask);			
			return parseTRI(qPrintable(fileName), m.cm);
		}
	if(formatName.toUpper() == tr("ASC"))
		{
			mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
			m.Enable(mask);			
			bool triangulate = parlst.getBool("triangulate");
			int rowToSkip = parlst.getInt("rowToSkip");
			int result = tri::io::ImporterASC<CMeshO>::Open(m.cm, qPrintable(fileName),cb,triangulate,rowToSkip);
			if (result != 0) // all the importers return 0 on success
			{
				errorMessage = QString("Failed to open:")+fileName;
				return false;
			}
			
		return true;
		}
	return result;
}

bool TriIOPlugin::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
{
	assert(0);
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> TriIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList 
		<< Format("TRI (photogrammetric reconstructions)", tr("TRI")) 
	  << Format("ASC (ascii triplets of points)", tr("ASC"));
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> TriIOPlugin::exportFormats() const
{
	QList<Format> formatList;
	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void TriIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
  capability=defaultBits=0;
	return;
}
 
static inline void readbeOne(void * ptr,unsigned int sz,FILE* f) {
  unsigned char* array = new unsigned char[sz];
  fread(array,sz,1,f);
  for(unsigned int i = 0; i < sz; ++i) {
    memcpy((unsigned char *)(ptr)+i,&array[sz-1-i],1);
  }
  delete [] array;
}

size_t readOtherE(void * ptr,unsigned int sz,unsigned int cpt,FILE* f) {
  for (unsigned int i = 0; i < cpt; ++i) {
    readbeOne((unsigned char *)(ptr)+i*sz, sz, f);
  }
  return cpt*sz;
}

int readHeader(FILE* fp, bool &TRIInverseBytes, int &numPoints, int &numFaces) {
  // Determine byte ordering
  unsigned short byteOrder;
  fread(&byteOrder,sizeof(short),1,fp);
  if (byteOrder==0x4c45 || byteOrder==0x4542) {
    TRIInverseBytes=true;
  } else if (byteOrder==0x454c || byteOrder==0x4245) {
    TRIInverseBytes=false;
  } else {
    return 1;
  }

  // Get number of points and faces
  if (TRIInverseBytes) {
    readOtherE(&numPoints,sizeof(int),1,fp);
    readOtherE(&numFaces,sizeof(int),1,fp);
  } else {
    fread(&numPoints,sizeof(int),1,fp);
    fread(&numFaces,sizeof(int),1,fp);
  }
  // advance pointer ??
  char s[4];
  fread(s,sizeof(char),4,fp);
	printf("extara is %s (%d)\n", s, *(int*)(&s[0]));
  return 0;
}

static int readPoint(FILE* fp, bool TRIInverseBytes, float &x,float &y, float &z) {
  if (TRIInverseBytes) {
    //printf("!!! Warning : not implemented\n");
    readOtherE(&x,sizeof(float),1,fp);
    readOtherE(&y,sizeof(float),1,fp);
    readOtherE(&z,sizeof(float),1,fp);
  } else {
    fread(&x,sizeof(float),1,fp);
    fread(&y,sizeof(float),1,fp);
    fread(&z,sizeof(float),1,fp);
  }
  return 0;
}
static int readTexel(FILE* fp, bool TRIInverseBytes, float &s, float &t) {
  if (TRIInverseBytes) {
    //printf("!!! Warning : not implemented\n");
    readOtherE(&s,sizeof(float),1,fp);
    readOtherE(&t,sizeof(float),1,fp);
  } else {
    fread(&s,sizeof(float),1,fp);
    fread(&t,sizeof(float),1,fp);
  }
  return 0;
}

static int readFace(FILE *fp, bool TRIInverseBytes, int &p1, int &p2, int &p3) {
  if (TRIInverseBytes) {
    //printf("!!! Warning : not implemented\n");
    readOtherE(&p1,sizeof(int),1,fp);
    readOtherE(&p2,sizeof(int),1,fp);
    readOtherE(&p3,sizeof(int),1,fp);
  } else {
    fread(&p1,sizeof(int),1,fp);
    fread(&p2,sizeof(int),1,fp);
    fread(&p3,sizeof(int),1,fp);
  }
  return 0;
}

bool parseTRI(const std::string &filename, CMeshO &m) {
  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp) {
		return false;
  }

  int err = 0;

  // Read header info
  int numPoints, numFaces;
  bool TRIInverseBytes;
  err |= readHeader(fp, TRIInverseBytes, numPoints, numFaces);

  if (err) {
   // Error::setError("Error parsing .tri\n");
    fclose(fp);
    return 1;
  }
	qDebug("Reading a mesh of %i vert and %i faces",numPoints,numFaces);
	
	tri::Allocator<CMeshO>::AddVertices(m,numPoints);
	tri::Allocator<CMeshO>::AddFaces(m,numFaces);

  // Read points
  float x, y, z;
  for (int i = 0; i < numPoints; ++i) {
    err = readPoint(fp, TRIInverseBytes, x, y, z);
		m.vert[i].P()=Point3f(x, y, z);
  }

  if (err) {
    //Error::setError("Error parsing .tri\n");
    fclose(fp);		
		return false;
  }

  // Read faces
  int p1, p2, p3;
  for (int i = 0; i < numFaces; ++i) {
    err |= readFace(fp, TRIInverseBytes, p1, p2, p3);
		assert(p1>=0 && p1<numPoints);
		assert(p2>=0 && p2<numPoints);
		assert(p3>=0 && p3<numPoints);
		
    m.face[i].V(0)= &m.vert[p1];
    m.face[i].V(1)= &m.vert[p2];
    m.face[i].V(2)= &m.vert[p3];
  }
 	
	QString texPNG(filename.c_str());
	texPNG = texPNG.left(texPNG.length()-3)+"PNG";
	QImage texturePNG;
	texturePNG.load(texPNG);
	if(!texturePNG.isNull())
	{
		qDebug("Image Loaded %s has %i keys",qPrintable(texPNG),texturePNG.textKeys().size());
		QString infoPNG=texturePNG.text("uv");
		if(!infoPNG.isNull())
		{
			m.textures.push_back(qPrintable(texPNG));
			qDebug("Loading texture %s",qPrintable(texPNG));
			QStringList numList = infoPNG.split(" ", QString::SkipEmptyParts);
			qDebug("Found %i numbers for %i faces",numList.size(),numFaces);
			for (int i = 0; i < numFaces ; ++i) 
						{
							for(int j=0;j<3;++j)
									{
										float s=(numList.at(i*6+j*2+0).toInt())/float(texturePNG.width());
										float t=(numList.at(i*6+j*2+1).toInt())/float(texturePNG.height());
										m.face[i].WT(j).U()=s;				
										m.face[i].WT(j).V()=1.0-t;				
										m.face[i].WT(j).N()=0;
									}
						}
			return true;
		}
	}

	QString texJPG(filename.c_str());
	texJPG = texJPG.left(texJPG.length()-3)+"JPG";
	QImage textureJPG;
	textureJPG.load(texJPG);
	if(textureJPG.isNull())
				return false;

  if (!feof(fp) ) {
			char texCode[4];
			bool floatFlag=false;
			
			int ret=fread(texCode,sizeof(char),4,fp);
			qDebug("TexString code is '%s' (int:%d) (float:%f)\n", texCode, *(int*)(&texCode[0]),  *(float*)(&texCode[0]));

			if(feof(fp)){
					qDebug("Premature end of file");
					return false;
				}

			if(texCode==QString("TC00")) floatFlag=false;
			
			m.textures.push_back(qPrintable(texJPG));
			qDebug("Loading texture %s",qPrintable(texJPG));
			
			for (int i = 0; i < numFaces ; ++i) 
			{
				if(feof(fp)){
					qDebug("Premature end of file after reading %i tex faces",i);
					return false;
				}
				float s,t;
				short ss,ts;
				for(int j=0;j<3;++j)
					{
						if(floatFlag){
							fread(&s,sizeof(float),1,fp);
							fread(&t,sizeof(float),1,fp);
						} else {
							fread(&ss,sizeof(short),1,fp);
							fread(&ts,sizeof(short),1,fp);
							s=ss/float(textureJPG.width());
							t=ts/float(textureJPG.height());
						}
						
						m.face[i].WT(j).U()=s;				
						m.face[i].WT(j).V()=1.0-t;				
						m.face[i].WT(j).N()=0;
					}
			} 
  } // if ! eof
  fclose(fp);

  if (err) return false;
  

  return true;
}


 
 
Q_EXPORT_PLUGIN(TriIOPlugin)
