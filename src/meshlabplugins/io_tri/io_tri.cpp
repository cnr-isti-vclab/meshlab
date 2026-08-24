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

#include <cstring>

#include "io_tri.h"

#include <wrap/io_trimesh/import_asc.h>
#include <wrap/io_trimesh/export.h>

using namespace vcg;

bool parseTRI(const std::string &filename, CMeshO &m);

RichParameterList TriIOPlugin::initPreOpenParameter(const QString &format) const
{
	RichParameterList parlst;
	if(format.toUpper() == tr("ASC"))
	{
			parlst.addParam(RichInt("rowToSkip",0,"Header Row to be skipped","The number of lines that must be skipped at the beginning of the file."));
			parlst.addParam(RichBool("triangulate", true, "Grid triangulation", "if true it assumes that the points are arranged in a complete xy grid and it tries to perform a naive height field triangulation of the input data.  Length of the lines is detected automatically by searching x jumps. If the input point cloud data is not arranged as a xy regular height field, no triangles are created."));
	}
	return parlst;
}

void TriIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterList &parlst, CallBackPos *cb)
{
	if(formatName.toUpper() == tr("TRI"))
	{
		mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
		m.enable(mask);
		if (!parseTRI(qUtf8Printable(fileName), m.cm))
			throw MLException("Error while opening TRI file");
	}
	else if(formatName.toUpper() == tr("ASC"))
	{
		mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
		m.enable(mask);
		bool triangulate = parlst.getBool("triangulate");
		int rowToSkip = parlst.getInt("rowToSkip");
		int result = tri::io::ImporterASC<CMeshO>::Open(m.cm, qUtf8Printable(fileName),cb,triangulate,rowToSkip);
		if (result != 0) // all the importers return 0 on success
		{
			throw MLException("Error while opening ASC file");
		}
	}
	else {
		wrongOpenFormat(formatName);
	}
}

void TriIOPlugin::save(const QString& formatName, const QString &, MeshModel &, const int, const RichParameterList &, vcg::CallBackPos *)
{
	wrongSaveFormat(formatName);
}

/*
	returns the list of the file's type which can be imported
*/
QString TriIOPlugin::pluginName() const
{
	return "IOTRI";
}

std::list<FileFormat> TriIOPlugin::importFormats() const
{
	std::list<FileFormat> formatList = {
		FileFormat("TRI (photogrammetric reconstructions)", tr("TRI")) ,
		FileFormat("ASC (ascii triplets of points)", tr("ASC"))
	};
	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
std::list<FileFormat> TriIOPlugin::exportFormats() const
{
	return {};
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void TriIOPlugin::exportMaskCapability(const QString &, int &capability, int &defaultBits) const
{
  capability=defaultBits=0;
	return;
}
 
static inline bool readbeOne(void * ptr,unsigned int sz,FILE* f) {
  unsigned char* array = new unsigned char[sz];
  if (fread(array,sz,1,f) != 1) {
    delete [] array;
    return false;
  }
  for(unsigned int i = 0; i < sz; ++i) {
    memcpy((unsigned char *)(ptr)+i,&array[sz-1-i],1);
  }
  delete [] array;
  return true;
}

bool readOtherE(void * ptr,unsigned int sz,unsigned int cpt,FILE* f) {
  for (unsigned int i = 0; i < cpt; ++i) {
    if (!readbeOne((unsigned char *)(ptr)+i*sz, sz, f))
      return false;
  }
  return true;
}

int readHeader(FILE* fp, bool &TRIInverseBytes, int &numPoints, int &numFaces) {
  // Determine byte ordering
  unsigned short byteOrder;
  if (fread(&byteOrder,sizeof(short),1,fp) != 1)
    return 1;
  if (byteOrder==0x4c45 || byteOrder==0x4542) {
    TRIInverseBytes=true;
  } else if (byteOrder==0x454c || byteOrder==0x4245) {
    TRIInverseBytes=false;
  } else {
    return 1;
  }

  // Get number of points and faces
  if (TRIInverseBytes) {
    if (!readOtherE(&numPoints,sizeof(int),1,fp) ||
        !readOtherE(&numFaces,sizeof(int),1,fp))
      return 1;
  } else {
    if (fread(&numPoints,sizeof(int),1,fp) != 1 ||
        fread(&numFaces,sizeof(int),1,fp) != 1)
      return 1;
  }
  // advance pointer ??
  char s[4];
  if (fread(s,sizeof(char),4,fp) != 4)
    return 1;

  if (numPoints < 0 || numFaces < 0)
    return 1;

  const long dataBegin = ftell(fp);
  if (dataBegin < 0 || fseek(fp, 0, SEEK_END) != 0)
    return 1;
  const long dataEnd = ftell(fp);
  if (dataEnd < dataBegin || fseek(fp, dataBegin, SEEK_SET) != 0)
    return 1;

  const unsigned long long remainingBytes = static_cast<unsigned long long>(dataEnd - dataBegin);
  const unsigned long long requiredBytes =
      static_cast<unsigned long long>(numPoints) * 3 * sizeof(float) +
      static_cast<unsigned long long>(numFaces) * 3 * sizeof(int);
  if (requiredBytes > remainingBytes)
    return 1;
  return 0;
}

static int readPoint(FILE* fp, bool TRIInverseBytes, float &x,float &y, float &z) {
  if (TRIInverseBytes) {
    if (!readOtherE(&x,sizeof(float),1,fp) ||
        !readOtherE(&y,sizeof(float),1,fp) ||
        !readOtherE(&z,sizeof(float),1,fp))
      return 1;
  } else {
    if (fread(&x,sizeof(float),1,fp) != 1 ||
        fread(&y,sizeof(float),1,fp) != 1 ||
        fread(&z,sizeof(float),1,fp) != 1)
      return 1;
  }
  return 0;
}

//static int readTexel(FILE* fp, bool TRIInverseBytes, float &s, float &t) {
//  if (TRIInverseBytes) {
//    //printf("!!! Warning : not implemented\n");
//    readOtherE(&s,sizeof(float),1,fp);
//    readOtherE(&t,sizeof(float),1,fp);
//  } else {
//    fread(&s,sizeof(float),1,fp);
//    fread(&t,sizeof(float),1,fp);
//  }
//  return 0;
//}

static int readFace(FILE *fp, bool TRIInverseBytes, int &p1, int &p2, int &p3) {
  if (TRIInverseBytes) {
    if (!readOtherE(&p1,sizeof(int),1,fp) ||
        !readOtherE(&p2,sizeof(int),1,fp) ||
        !readOtherE(&p3,sizeof(int),1,fp))
      return 1;
  } else {
    if (fread(&p1,sizeof(int),1,fp) != 1 ||
        fread(&p2,sizeof(int),1,fp) != 1 ||
        fread(&p3,sizeof(int),1,fp) != 1)
      return 1;
  }
  return 0;
}

bool parseTRI(const std::string &filename, CMeshO &m) {
  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp) {
		return false;
  }
  struct FileCloser {
    FILE *fp;
    ~FileCloser() { fclose(fp); }
  } fileCloser = {fp};

  int err = 0;

  // Read header info
  int numPoints, numFaces;
  bool TRIInverseBytes;
  err |= readHeader(fp, TRIInverseBytes, numPoints, numFaces);

  if (err) {
   // Error::setError("Error parsing .tri\n");
    return false;
  }
	qDebug("Reading a mesh of %i vert and %i faces",numPoints,numFaces);
	
	tri::Allocator<CMeshO>::AddVertices(m,numPoints);
	tri::Allocator<CMeshO>::AddFaces(m,numFaces);

  // Read points
  float x, y, z;
  for (int i = 0; i < numPoints; ++i) {
    err |= readPoint(fp, TRIInverseBytes, x, y, z);
		if (err) return false;
		m.vert[i].P()=Point3m(x, y, z);
  }

  if (err) {
    //Error::setError("Error parsing .tri\n");
		return false;
  }

  // Read faces
  int p1, p2, p3;
  for (int i = 0; i < numFaces; ++i) {
    err |= readFace(fp, TRIInverseBytes, p1, p2, p3);
		if (err || p1 < 0 || p1 >= numPoints ||
		    p2 < 0 || p2 >= numPoints ||
		    p3 < 0 || p3 >= numPoints)
			return false;
		
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
		qDebug("Image Loaded %s has %i keys",qUtf8Printable(texPNG),texturePNG.textKeys().size());
		QString infoPNG=texturePNG.text("uv");
		if(!infoPNG.isNull())
		{
			m.textures.push_back(qUtf8Printable(texPNG));
			qDebug("Loading texture %s",qUtf8Printable(texPNG));
			QStringList numList = infoPNG.split(" ", QString::SkipEmptyParts);
			qDebug("Found %i numbers for %i faces",numList.size(),numFaces);
			if (numFaces > numList.size() / 6)
				return false;
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
			char texCode[5] = {};
			bool floatFlag=false;
			
			if (fread(texCode,sizeof(char),4,fp) != 4) {
					qDebug("Premature end of file");
					return false;
				}

			qDebug("TexString code is '%s'\n", texCode);
			if(strcmp(texCode, "TC00") == 0) floatFlag=false;
			
			m.textures.push_back(qUtf8Printable(texJPG));
			qDebug("Loading texture %s",qUtf8Printable(texJPG));
			
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
							if (fread(&s,sizeof(float),1,fp) != 1 ||
							    fread(&t,sizeof(float),1,fp) != 1)
								return false;
						} else {
							if (fread(&ss,sizeof(short),1,fp) != 1 ||
							    fread(&ts,sizeof(short),1,fp) != 1)
								return false;
							s=ss/float(textureJPG.width());
							t=ts/float(textureJPG.height());
						}
						
						m.face[i].WT(j).U()=s;				
						m.face[i].WT(j).V()=1.0-t;				
						m.face[i].WT(j).N()=0;
					}
			} 
  } // if ! eof
  if (err) return false;
  

  return true;
}


 
 
MESHLAB_PLUGIN_NAME_EXPORTER(TriIOPlugin)
