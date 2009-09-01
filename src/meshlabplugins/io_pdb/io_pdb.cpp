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

#include "io_pdb.h"

#include <wrap/io_trimesh/import_ply.h>

#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/export.h>

#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/update/selection.h>
#include <vcg/complex/trimesh/update/position.h>
#include <vcg/complex/trimesh/update/color.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/complex/trimesh/append.h>

// debug, to be removed
#include <wrap/io_trimesh/export_ply.h>

#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/create/marching_cubes.h>
#include <vcg/complex/trimesh/create/mc_trivial_walker.h>

#include <QMessageBox>
#include <QFileDialog>

using namespace std;
using namespace vcg;

// initialize importing parameters
void PDBIOPlugin::initPreOpenParameter(const QString &formatName, const QString &/*filename*/, RichParameterSet &parlst)
{
	if (formatName.toUpper() == tr("PDB"))
	{
		parlst.addParam(new RichBool("usecolors",true,"Use Atoms colors","Atoms are colored according to atomic type"));
		parlst.addParam(new RichBool("justpoints",false,"SURFACE: Atoms as Points","Atoms are created as points, no surface is built. Overrides all subsequential surface parameters"));
		parlst.addParam(new RichBool("justspheres",true,"SURFACE: Atoms as Spheres","Atoms are created as intersecting spheres, no interpolation surface is built. Overrides all subsequential surface parameters"));
		parlst.addParam(new RichBool("interpspheres",false,"SURFACE: Atoms as Jointed Spheres","Atoms are created as spheres, joining surface is built. Overrides all subsequential surface parameters"));
		parlst.addParam(new RichBool("metaballs",false,"SURFACE: Atoms as Metaballs","Atoms are created as blobby interpolation surface, refer to BLINN Metaballs article. Overrides all subsequential surface parameters"));
		parlst.addParam(new RichFloat("voxelsize",0.25,"Surface Resolution","is used by Jointed Spheres and Metaball"));
		parlst.addParam(new RichFloat("blobby",2.0,"Blobbyness factor","is used by Metaball"));
		/*
		parlst.addParam(new RichInt("meshindex",0,"Index of Range Map to be Imported","PTX files may contain more than one range map. 0 is the first range map. If the number if higher than the actual mesh number, the import will fail");
		parlst.addParam(new RichBool("anglecull",true,"Cull faces by angle","short");
		parlst.addParam(new RichFloat("angle",85.0,"Angle limit for face culling","short");
		parlst.addParam(new RichBool("usecolor",true,"import color","Read color from PTX, if color is not present, uses reflectance instead");
		parlst.addParam(new RichBool("pointcull",true,"delete unsampled points","Deletes unsampled points in the grid that are normally located in [0,0,0]");
		parlst.addParam(new RichBool("pointsonly",false,"Keep only points","Just import points, without triangulation");
		parlst.addParam(new RichBool("switchside",false,"Swap rows/columns","On some PTX, the rows and columns number are switched over");		
		parlst.addParam(new RichBool("flipfaces",false,"Flip all faces","Flip the orientation of all the triangles");
		*/
	}
}

bool PDBIOPlugin::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &parlst, CallBackPos *cb, QWidget * /*parent*/)
{
	bool normalsUpdated = false;

	// initializing mask
	mask = 0;
	
	// initializing progress bar status
	if (cb != NULL)		(*cb)(0, "Loading...");

	QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: %2";

	//string filename = fileName.toUtf8().data();
	string filename = QFile::encodeName(fileName).constData ();
  
  if (formatName.toUpper() == tr("PDB"))
	{
		
		mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
		m.Enable(mask);

		return parsePDB(qPrintable(fileName), m.cm, parlst, cb);
 

		/*
		tri::io::ImporterPTX<CMeshO>::Info importparams;

		importparams.meshnum = parlst.getInt("meshindex");
		importparams.anglecull =parlst.getBool("anglecull");
		importparams.angle = parlst.getFloat("angle");
		importparams.savecolor = parlst.getBool("usecolor");
		importparams.pointcull = parlst.getBool("pointcull");
		importparams.pointsonly = parlst.getBool("pointsonly");
		importparams.switchside = parlst.getBool("switchside");
		importparams.flipfaces = parlst.getBool("flipfaces");

		// if color, add to mesh
		if(importparams.savecolor)
			importparams.mask |= tri::io::Mask::IOM_VERTCOLOR;

		// reflectance is stored in quality
		importparams.mask |= tri::io::Mask::IOM_VERTQUALITY;

		m.Enable(importparams.mask);

		int result = tri::io::ImporterPTX<CMeshO>::Open(m.cm, filename.c_str(), importparams, cb);
		if (result == 1)
		{
			errorMessage = errorMsgFormat.arg(fileName, tri::io::ImporterPTX<CMeshO>::ErrorMsg(result));
			return false;
		}

		// update mask
		mask = importparams.mask;
		*/
	}
	else 
	{
		assert(0); // Unknown File type
		return false;
	}

	if (cb != NULL)	(*cb)(99, "Done");

	return true;
}

bool PDBIOPlugin::save(const QString &formatName,const QString &fileName, MeshModel &m, const int mask, const RichParameterSet & par, CallBackPos *cb, QWidget */*parent*/)
{
  assert(0); 
	return false;
}

/*
	returns the list of the file's type which can be imported
*/
QList<MeshIOInterface::Format> PDBIOPlugin::importFormats() const
{
	QList<Format> formatList;
	formatList << Format("Protein Data Bank"	, tr("PDB"));

	return formatList;
}

/*
	returns the list of the file's type which can be exported
*/
QList<MeshIOInterface::Format> PDBIOPlugin::exportFormats() const
{
	QList<Format> formatList;
//	formatList << Format("Stanford Polygon File Format"	, tr("PLY"));

	return formatList;
}

/*
	returns the mask on the basis of the file's type. 
	otherwise it returns 0 if the file format is unknown
*/
void PDBIOPlugin::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const
{
  capability=defaultBits=0;
	return;
}

void PDBIOPlugin::initOpenParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par) 
{
	/*
	if(format.toUpper() == tr("STL"))
		par.addBool("Unify",true, "Unify Duplicated Vertices",
								"The STL format is not an vertex-indexed format. Each triangle is composed by independent vertices, so, usually, duplicated vertices should be unified");		
	*/
}
void PDBIOPlugin::initSaveParameter(const QString &format, MeshModel &/*m*/, RichParameterSet &par) 
{
	/*
	if(format.toUpper() == tr("STL") || format.toUpper() == tr("PLY"))
		par.addBool("Binary",true, "Binary encoding",
								"Save the mesh using a binary encoding. If false the mesh is saved in a plain, readable ascii format");		
  */
}
void PDBIOPlugin::applyOpenParameter(const QString &format, MeshModel &m, const RichParameterSet &par) 
{
  /*
	if(format.toUpper() == tr("STL"))
		if(par.getBool("Unify"))
			tri::Clean<CMeshO>::RemoveDuplicateVertex(m.cm);
	*/
}

Q_EXPORT_PLUGIN(PDBIOPlugin)


//---------- PDB READER -----------//
bool PDBIOPlugin::parsePDB(const std::string &filename, CMeshO &m, const RichParameterSet &parlst, CallBackPos *cb) 
{
	int atomNumber=0;
	int atomIndex;
	bool surfacecreated = false;

  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp) {
		return false;
  }

	//-- clear all molecule data
	atomDetails.clear();
	atomPos.clear();
	atomCol.clear();
	atomRad.clear();

	//-- read all lines, if the line contains  ATOM, then store the details for parsing
	char buf[82];
  buf[81]=0;

	while(1)
	{
		if(! fgets(buf,81,fp)) break;
		string st(buf);
		if (strcmp( st.substr(0,6).c_str(), "ATOM  ") == 0 ) 
		{
			atomDetails.push_back(st);
			atomNumber++;
		}
  }

	// updating progress bar status
	char msgbuf[256];
	sprintf(msgbuf,"Read %i atoms...",atomNumber);
	if (cb != NULL)		(*cb)(10, "Loading...");

	//-- atoms parsing
	for(atomIndex=0; atomIndex<atomDetails.size(); atomIndex++)
	{
		Point3f currAtomPos;
		Color4b currAtomCol;
		float   currAtomRad;

		// position
		mysscanf(atomDetails[atomIndex].substr( 31, 38).c_str(),"%f", &(currAtomPos.X()));
		mysscanf(atomDetails[atomIndex].substr( 39, 46).c_str(),"%f", &(currAtomPos.Y()));
		mysscanf(atomDetails[atomIndex].substr( 47, 54).c_str(),"%f", &(currAtomPos.Z()));
		atomPos.push_back(currAtomPos);

		// color
		currAtomCol=getAtomColor(atomDetails[atomIndex].substr(13, 4).c_str());
		atomCol.push_back(currAtomCol);

		// radius
		currAtomRad=getAtomRadius(atomDetails[atomIndex].substr(13, 4).c_str()); //  Van der Waals radii
		atomRad.push_back(currAtomRad);
	}

	// build mesh

	if(parlst.getBool("justpoints") && !surfacecreated)  	// pointcloud
	{
		tri::Allocator<CMeshO>::AddVertices(m,atomNumber);

		for (atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
		{
			m.vert[atomIndex].P()=atomPos[atomIndex];
			m.vert[atomIndex].C()=atomCol[atomIndex];
		}

		surfacecreated = true;
	}

	if(parlst.getBool("justspheres") && !surfacecreated)  	// spheres
	{
		CMeshO tmpmesh;
		tmpmesh.face.EnableFFAdjacency();
		vcg::tri::UpdateTopology<CMeshO>::FaceFace(tmpmesh);

		for ( atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
		{
				tmpmesh.Clear();
				vcg::tri::Sphere<CMeshO>(tmpmesh,1);

				// scale, move and apply color
				for(int vi=0; vi<tmpmesh.vn; vi++)
				{
					tmpmesh.vert[vi].P().X() = (tmpmesh.vert[vi].P().X() * atomRad[atomIndex]) + atomPos[atomIndex].X();
					tmpmesh.vert[vi].P().Y() = (tmpmesh.vert[vi].P().Y() * atomRad[atomIndex]) + atomPos[atomIndex].Y();
					tmpmesh.vert[vi].P().Z() = (tmpmesh.vert[vi].P().Z() * atomRad[atomIndex]) + atomPos[atomIndex].Z();

					tmpmesh.vert[vi].C() = atomCol[atomIndex];
				}

				tri::Append<CMeshO,CMeshO>::Mesh(m,tmpmesh);
		}
		tmpmesh.Clear();

		surfacecreated = true;
	}

	if(parlst.getBool("interpspheres") && !surfacecreated)  	// jointed spheres marching cube 
	{
		SimpleVolume<SimpleVoxel> 	volume;
	
		typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel> >	MyWalker;
		typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
		MyWalker walker;
		
		Box3d rbb;
		// calculating an enlarged bbox
		rbb.min[0]=rbb.min[1]=rbb.min[2]= 100000;
		rbb.max[0]=rbb.max[1]=rbb.max[2]=-100000;
		for (atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
		{
			if(atomPos[atomIndex].X() < rbb.min[0])			rbb.min[0]=atomPos[atomIndex].X();
			if(atomPos[atomIndex].X() > rbb.max[0])			rbb.max[0]=atomPos[atomIndex].X();
			if(atomPos[atomIndex].Y() < rbb.min[1])			rbb.min[1]=atomPos[atomIndex].Y();
			if(atomPos[atomIndex].Y() > rbb.max[1])			rbb.max[1]=atomPos[atomIndex].Y();
			if(atomPos[atomIndex].Z() < rbb.min[2])			rbb.min[2]=atomPos[atomIndex].Z();
			if(atomPos[atomIndex].Z() > rbb.max[2])			rbb.max[2]=atomPos[atomIndex].Z();
		}
		rbb.min[0]-=5;		rbb.min[1]-=5;		rbb.min[2]-=5;
		rbb.max[0]+=5;		rbb.max[1]+=5;		rbb.max[2]+=5;

		// defining resolution
		double step = parlst.getFloat("voxelsize");
		Point3i siz= Point3i::Construct((rbb.max-rbb.min)*(1.0/step));
					
		volume.Init(siz);
		float xpos,ypos,zpos;
		for(double i=0;i<siz[0];i++)
			for(double j=0;j<siz[1];j++)
				for(double k=0;k<siz[2];k++)
							{
								xpos = rbb.min[0]+step*i;
								ypos = rbb.min[1]+step*j;
								zpos = rbb.min[2]+step*k;
							 
								volume.Val(i,j,k)=10000;
								for (atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
								{
									if(! (fabs(xpos-atomPos[atomIndex].X())>3.0f) )
										if(! (fabs(ypos-atomPos[atomIndex].Y())>3.0f) )
											if(! (fabs(zpos-atomPos[atomIndex].Z())>3.0f) )
											{
												float val = pow((double)(xpos-atomPos[atomIndex].X()),2.0) + 
																		pow((double)(ypos-atomPos[atomIndex].Y()),2.0) + 
													    			pow((double)(zpos-atomPos[atomIndex].Z()),2.0) - atomRad[atomIndex];
												if(val < volume.Val(i,j,k))
													volume.Val(i,j,k) = val;
											}
								}
							}
		
		// MARCHING CUBES
		MyMarchingCubes	mc(m, walker);
		walker.BuildMesh<MyMarchingCubes>(m, volume, mc, 0);
		Matrix44f tr; tr.SetIdentity(); tr.SetTranslate(rbb.min[0],rbb.min[1],rbb.min[2]);
		Matrix44f sc; sc.SetIdentity(); sc.SetScale(step,step,step);
		tr=tr*sc;
		
		tri::UpdatePosition<CMeshO>::Matrix(m,tr);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m);
		tri::UpdateBounding<CMeshO>::Box(m);					// updates bounding box		
	
		surfacecreated = true;
	}


	if(parlst.getBool("metaballs") && !surfacecreated)  	// metaballs marching cube 
	{
		SimpleVolume<SimpleVoxel> 	volume;
	
		typedef vcg::tri::TrivialWalker<CMeshO, SimpleVolume<SimpleVoxel> >	MyWalker;
		typedef vcg::tri::MarchingCubes<CMeshO, MyWalker>	MyMarchingCubes;
		MyWalker walker;
		
		Box3d rbb;
		// calculating an enlarged bbox
		rbb.min[0]=rbb.min[1]=rbb.min[2]= 100000;
		rbb.max[0]=rbb.max[1]=rbb.max[2]=-100000;
		for (atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
		{
			if(atomPos[atomIndex].X() < rbb.min[0])			rbb.min[0]=atomPos[atomIndex].X();
			if(atomPos[atomIndex].X() > rbb.max[0])			rbb.max[0]=atomPos[atomIndex].X();
			if(atomPos[atomIndex].Y() < rbb.min[1])			rbb.min[1]=atomPos[atomIndex].Y();
			if(atomPos[atomIndex].Y() > rbb.max[1])			rbb.max[1]=atomPos[atomIndex].Y();
			if(atomPos[atomIndex].Z() < rbb.min[2])			rbb.min[2]=atomPos[atomIndex].Z();
			if(atomPos[atomIndex].Z() > rbb.max[2])			rbb.max[2]=atomPos[atomIndex].Z();
		}
		rbb.min[0]-=5;		rbb.min[1]-=5;		rbb.min[2]-=5;
		rbb.max[0]+=5;		rbb.max[1]+=5;		rbb.max[2]+=5;

		// defining resolution
		double step = parlst.getFloat("voxelsize");
		float blobby = -parlst.getFloat("blobby");
		Point3i siz= Point3i::Construct((rbb.max-rbb.min)*(1.0/step));
					

//	Log("Filling a Volume of %i %i %i",siz[0],siz[1],siz[2]);
		volume.Init(siz);
		float xpos,ypos,zpos;
		for(double i=0;i<siz[0];i++)
			for(double j=0;j<siz[1];j++)
				for(double k=0;k<siz[2];k++)
							{
								xpos = rbb.min[0]+step*i;
								ypos = rbb.min[1]+step*j;
								zpos = rbb.min[2]+step*k;
							 
								volume.Val(i,j,k)=0.0;
								for (atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
								{
									if(! (fabs(xpos-atomPos[atomIndex].X())>5.0f) )
										if(! (fabs(ypos-atomPos[atomIndex].Y())>5.0f) )
											if(! (fabs(zpos-atomPos[atomIndex].Z())>5.0f) )
											{
												float r2 = (pow((double)(xpos-atomPos[atomIndex].X()),2.0) + 
																	  pow((double)(ypos-atomPos[atomIndex].Y()),2.0) + 
													    		  pow((double)(zpos-atomPos[atomIndex].Z()),2.0));
												float val = exp((blobby/atomRad[atomIndex])*r2 - blobby);

												volume.Val(i,j,k) += val;
											}
								}
							}
		
		// MARCHING CUBES
		MyMarchingCubes	mc(m, walker);
		walker.BuildMesh<MyMarchingCubes>(m, volume, mc, 1);
		Matrix44f tr; tr.SetIdentity(); tr.SetTranslate(rbb.min[0],rbb.min[1],rbb.min[2]);
		Matrix44f sc; sc.SetIdentity(); sc.SetScale(step,step,step);
		tr=tr*sc;
		
		//tri::io::ExporterPLY<CMeshO>::Save(m,"./pippo.ply");

		tri::UpdatePosition<CMeshO>::Matrix(m,tr);
	  tri::Clean<CMeshO>::FlipMesh(m);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m);
		tri::UpdateBounding<CMeshO>::Box(m);					// updates bounding box		


		//------------------------------------------------

		double ww;
		double rr;
		double gg;
		double bb;
		for(int vind=0; vind<m.vn ; vind++)
		{
			xpos = m.vert[vind].P().X();
			ypos = m.vert[vind].P().Y(); 
			zpos = m.vert[vind].P().Z(); 				
			ww=rr=gg=bb=0;
			
			for (atomIndex = 0; atomIndex < atomNumber; ++atomIndex) 
			{
				if(! (fabs(xpos-atomPos[atomIndex].X())>5.0f) )
					if(! (fabs(ypos-atomPos[atomIndex].Y())>5.0f) )
						if(! (fabs(zpos-atomPos[atomIndex].Z())>5.0f) )
						{
							float r2 = (pow((double)(xpos-atomPos[atomIndex].X()),2.0) + 
												  pow((double)(ypos-atomPos[atomIndex].Y()),2.0) + 
								    		  pow((double)(zpos-atomPos[atomIndex].Z()),2.0));
							r2 = r2 / atomRad[atomIndex];
							r2 = min(2.0f,r2);

							ww += r2;
							rr += r2 * atomCol[atomIndex].X();
							gg += r2 * atomCol[atomIndex].Y();
							bb += r2 * atomCol[atomIndex].Z();

						}

						m.vert[vind].C().X() = rr/ww;
						m.vert[vind].C().Y() = gg/ww;
						m.vert[vind].C().Z() = bb/ww;
			}


		}

		//------------------------------------------------

		surfacecreated = true;
	}


	//-- ending all
	if (cb != NULL)	(*cb)(99, "Done");

	//-- finish by clearing all molecule data
	atomDetails.clear();
	atomPos.clear();
	atomCol.clear();
	atomRad.clear();

	// closing file
	fclose(fp);	
	
	return surfacecreated;
}


//-- helper function... parses + and - values with space
void PDBIOPlugin::mysscanf(const char* st, const char* format, float *f)
{
  if (!sscanf( st, "%f", f)) {
    if (sscanf( st, " - %f", f))
    *f=-*f; 
    else  *f=0.0;
  };
}


//-- returns the correct radius for each atom type
float PDBIOPlugin::getAtomRadius(const char* atomicElementCharP)
{
  static std::map<std::string,float> E2R;
  if(E2R.size()==0)
  {    
    // according to http://www.imb-jena.de/ImgLibDoc/glossary/IMAGE_VDWR.html 
    //E2R["H"]=  	1.20f;
    //E2R["C"]= 	1.70f;
    //E2R["N"]= 	1.55f;
    //E2R["O"]= 	1.52f;
    //E2R["F"]= 	1.47f;
    //E2R["P"]= 	1.80f;
    //E2R["S"]= 	1.80f;
    //E2R["CL"]= 	1.89f;

    /// according to http://www.umass.edu/microbio/rasmol/rasbonds.htm
    E2R["H"]=   1.100f; 
    E2R["C"]=   1.400f;//1.548f;
    E2R["N"]=   1.400f;
    E2R["O"]=   1.348f;
    E2R["P"]=   1.880f;
    E2R["S"]= 	1.808f;
    E2R["CA"]= 	1.948f;
    E2R["FE"]= 	1.948f;
    E2R["ZN"]= 	1.148f;
    E2R["CD"]= 	1.748f;
    E2R["I"]= 	1.748f;
  }
  std::string ss0,ss1;
  string atomicElement(atomicElementCharP);
  ss0=atomicElement.substr(0,1);
  ss1=atomicElement.substr(0,2);
  float rad=E2R[ss1];                                 
  if(rad==0) rad = E2R[ss0];
  if(rad==0) rad=1;
  return rad;
}

vcg::Color4b PDBIOPlugin::getAtomColor(const char* atomicElementCharP)
{
	string atomicElement(atomicElementCharP);
  static std::map<std::string,vcg::Color4b> E2C;

	if(E2C.size()==0)
	{
		E2C["H"] = vcg::Color4b(255,255,255,255)  	/* 0xFFFFFF			*/  	;
		E2C["HE"]= vcg::Color4b(217,255,255,255)   	/* 0xD9FFFF			*/  	;
		E2C["LI"]= vcg::Color4b(204,128,255,255)   	/* 0xCC80FF			*/  	;
		E2C["BE"]= vcg::Color4b(194,255,  0,255)   	/* 0xC2FF00			*/  	;
		E2C["B"] = vcg::Color4b(255,181,181,255)   	/* 0xFFB5B5			*/  	;
		E2C["C"] = vcg::Color4b(144,144,144,255)   	/* 0x909090			*/  	;
		E2C["N"] = vcg::Color4b( 48, 80,248,255)   	/* 0x3050F8			*/  	;
		E2C["O"] = vcg::Color4b(255, 13, 13,255)   	/* 0xFF0D0D			*/  	;
		E2C["F"] = vcg::Color4b(144,224, 80,255)   	/* 0x90E050			*/  	;
		E2C["NE"]= vcg::Color4b(179,227,245,255)   	/* 0xB3E3F5			*/  	;
		E2C["NA"]= vcg::Color4b(171, 92,242,255)   	/* 0xAB5CF2			*/  	;
		E2C["MG"]= vcg::Color4b(138,255,  0,255)   	/* 0x8AFF00			*/  	;
		E2C["AL"]= vcg::Color4b(191,166,166,255)   	/* 0xBFA6A6			*/  	;
		E2C["SI"]= vcg::Color4b(240,200,160,255)   	/* 0xF0C8A0			*/  	;
		E2C["P"] = vcg::Color4b(255,128,  0,255)   	/* 0xFF8000			*/  	;
		E2C["S"] = vcg::Color4b(255,255, 48,255)   	/* 0xFFFF30			*/  	;
		E2C["CL"]= vcg::Color4b( 31,240, 31,255)   	/* 0x1FF01F			*/  	;
		E2C["AR"]= vcg::Color4b(128,209,227,255)   	/* 0x80D1E3			*/  	;
		E2C["K"] = vcg::Color4b(143, 64,212,255)   	/* 0x8F40D4			*/  	;
		E2C["CA"]= vcg::Color4b( 61,255,  0,255)   	/* 0x3DFF00			*/  	;
		E2C["SC"]= vcg::Color4b(230,230,230,255)   	/* 0xE6E6E6			*/  	;
		E2C["TI"]= vcg::Color4b(191,194,199,255)   	/* 0xBFC2C7			*/  	;
		E2C["V"] = vcg::Color4b(166,166,171,255)   	/* 0xA6A6AB			*/  	;
		E2C["CR"]= vcg::Color4b(138,153,199,255)   	/* 0x8A99C7			*/  	;
		E2C["MN"]= vcg::Color4b(156,122,199,255)   	/* 0x9C7AC7			*/  	;
		E2C["FE"]= vcg::Color4b(224,102, 51,255)   	/* 0xE06633			*/  	;
		E2C["CO"]= vcg::Color4b(240,144,160,255)   	/* 0xF090A0			*/  	;
		E2C["NI"]= vcg::Color4b( 80,208, 80,255)   	/* 0x50D050			*/  	;
		E2C["CU"]= vcg::Color4b(200,128, 51,255)   	/* 0xC88033			*/  	;
		E2C["ZN"]= vcg::Color4b(125,128,176,255)   	/* 0x7D80B0			*/  	;
		E2C["GA"]= vcg::Color4b(194,143,143,255)   	/* 0xC28F8F			*/  	;
		E2C["GE"]= vcg::Color4b(102,143,143,255)   	/* 0x668F8F			*/  	;
		E2C["AS"]= vcg::Color4b(189,128,227,255)   	/* 0xBD80E3			*/  	;
		E2C["SE"]= vcg::Color4b(255,161,  0,255)   	/* 0xFFA100			*/  	;
		E2C["BR"]= vcg::Color4b(166, 41, 41,255)   	/* 0xA62929			*/  	;
		E2C["KR"]= vcg::Color4b( 92,184,209,255)   	/* 0x5CB8D1			*/  	;
		E2C["RB"]= vcg::Color4b(112, 46,176,255)   	/* 0x702EB0			*/  	;
		E2C["SR"]= vcg::Color4b(  0,255,  0,255)   	/* 0x00FF00			*/  	;
		E2C["Y"] = vcg::Color4b(148,255,255,255)   	/* 0x94FFFF			*/  	;
		E2C["ZR"]= vcg::Color4b(148,224,224,255)   	/* 0x94E0E0			*/  	;
		E2C["NB"]= vcg::Color4b(115,194,201,255)   	/* 0x73C2C9			*/  	;
		E2C["MO"]= vcg::Color4b( 84,181,181,255)   	/* 0x54B5B5			*/  	;
		E2C["TC"]= vcg::Color4b( 59,158,158,255)   	/* 0x3B9E9E			*/  	;
		E2C["RU"]= vcg::Color4b( 36,143,143,255)   	/* 0x248F8F			*/  	;
		E2C["RH"]= vcg::Color4b( 10,125,140,255)   	/* 0x0A7D8C			*/  	;
		E2C["PD"]= vcg::Color4b(  0,105,133,255)   	/* 0x006985			*/  	;
		E2C["AG"]= vcg::Color4b(192,192,192,255)   	/* 0xC0C0C0			*/  	;
		E2C["CD"]= vcg::Color4b(255,217,143,255)   	/* 0xFFD98F			*/  	;
		E2C["IN"]= vcg::Color4b(166,117,115,255)   	/* 0xA67573			*/  	;
		E2C["SN"]= vcg::Color4b(102,128,128,255)   	/* 0x668080			*/  	;
		E2C["SB"]= vcg::Color4b(158, 99,181,255)   	/* 0x9E63B5			*/  	;
		E2C["TE"]= vcg::Color4b(212,122,  0,255)   	/* 0xD47A00			*/  	;
		E2C["I"] = vcg::Color4b(148,  0,148,255)   	/* 0x940094			*/  	;
		E2C["XE"]= vcg::Color4b( 66,158,176,255)   	/* 0x429EB0			*/  	;
		E2C["CS"]= vcg::Color4b( 87, 23,143,255)   	/* 0x57178F			*/  	;
		E2C["BA"]= vcg::Color4b(  0,201,  0,255)   	/* 0x00C900			*/  	;
		E2C["LA"]= vcg::Color4b(112,212,255,255)   	/* 0x70D4FF			*/  	;
		E2C["CE"]= vcg::Color4b(255,255,199,255)   	/* 0xFFFFC7			*/  	;
		E2C["PR"]= vcg::Color4b(217,255,199,255)   	/* 0xD9FFC7			*/  	;
		E2C["ND"]= vcg::Color4b(199,255,199,255)   	/* 0xC7FFC7			*/  	;
		E2C["PM"]= vcg::Color4b(163,255,199,255)   	/* 0xA3FFC7			*/  	;
		E2C["SM"]= vcg::Color4b(143,255,199,255)   	/* 0x8FFFC7			*/  	;
		E2C["EU"]= vcg::Color4b( 97,255,199,255)   	/* 0x61FFC7			*/  	;
		E2C["GD"]= vcg::Color4b( 69,255,199,255)   	/* 0x45FFC7			*/  	;
		E2C["TB"]= vcg::Color4b( 48,255,199,255)   	/* 0x30FFC7			*/  	;
		E2C["DY"]= vcg::Color4b( 31,255,199,255)   	/* 0x1FFFC7			*/  	;
		E2C["HO"]= vcg::Color4b(  0,255,156,255)   	/* 0x00FF9C			*/  	;
		E2C["ER"]= vcg::Color4b(  0,230,117,255)   	/* 0x00E675			*/  	;
		E2C["TM"]= vcg::Color4b(  0,212, 82,255)   	/* 0x00D452			*/  	;
		E2C["YB"]= vcg::Color4b(  0,191, 56,255)   	/* 0x00BF38			*/  	;
		E2C["LU"]= vcg::Color4b(  0,171, 36,255)   	/* 0x00AB24			*/  	;
		E2C["HF"]= vcg::Color4b( 77,194,255,255)   	/* 0x4DC2FF			*/  	;
		E2C["TA"]= vcg::Color4b( 77,166,255,255)   	/* 0x4DA6FF			*/  	;
		E2C["W"] = vcg::Color4b( 33,148,214,255)   	/* 0x2194D6			*/  	;
		E2C["RE"]= vcg::Color4b( 38,125,171,255)   	/* 0x267DAB			*/  	;
		E2C["OS"]= vcg::Color4b( 38,102,150,255)   	/* 0x266696			*/  	;
		E2C["IR"]= vcg::Color4b( 23, 84,135,255)   	/* 0x175487			*/  	;
		E2C["PT"]= vcg::Color4b(208,208,224,255)   	/* 0xD0D0E0			*/  	;
		E2C["AU"]= vcg::Color4b(255,209, 35,255)   	/* 0xFFD123			*/  	;
		E2C["HG"]= vcg::Color4b(184,184,208,255)   	/* 0xB8B8D0			*/  	;
		E2C["TL"]= vcg::Color4b(166, 84, 77,255)   	/* 0xA6544D			*/  	;
		E2C["PB"]= vcg::Color4b( 87, 89, 97,255)   	/* 0x575961			*/  	;
		E2C["BI"]= vcg::Color4b(158, 79,181,255)   	/* 0x9E4FB5			*/  	;
		E2C["PO"]= vcg::Color4b(171, 92,  0,255)   	/* 0xAB5C00			*/  	;
		E2C["AT"]= vcg::Color4b(117, 79, 69,255)   	/* 0x754F45			*/  	;
		E2C["RN"]= vcg::Color4b( 66,130,150,255)   	/* 0x428296			*/  	;
		E2C["FR"]= vcg::Color4b( 66,  0,102,255)   	/* 0x420066			*/  	;
		E2C["RA"]= vcg::Color4b(  0,125,  0,255)   	/* 0x007D00			*/  	;
		E2C["AC"]= vcg::Color4b(112,171,250,255)   	/* 0x70ABFA			*/  	;
		E2C["TH"]= vcg::Color4b(  0,186,255,255)   	/* 0x00BAFF			*/  	;
		E2C["PA"]= vcg::Color4b(  0,161,255,255)   	/* 0x00A1FF			*/  	;
		E2C["U"] = vcg::Color4b(  0,143,255,255)   	/* 0x008FFF			*/  	;
		E2C["NP"]= vcg::Color4b(  0,128,255,255)   	/* 0x0080FF			*/  	;
		E2C["PU"]= vcg::Color4b(  0,107,255,255)   	/* 0x006BFF			*/  	;
		E2C["AM"]= vcg::Color4b( 84, 92,242,255)   	/* 0x545CF2			*/  	;
		E2C["CM"]= vcg::Color4b(120, 92,227,255)   	/* 0x785CE3			*/  	;
		E2C["BK"]= vcg::Color4b(138, 79,227,255)   	/* 0x8A4FE3			*/  	;
		E2C["CF"]= vcg::Color4b(161, 54,212,255)   	/* 0xA136D4			*/  	;
		E2C["ES"]= vcg::Color4b(179, 31,212,255)   	/* 0xB31FD4			*/  	;
		E2C["FM"]= vcg::Color4b(179, 31,186,255)   	/* 0xB31FBA			*/  	;
		E2C["MD"]= vcg::Color4b(179, 13,166,255)   	/* 0xB30DA6			*/  	;
		E2C["NO"]= vcg::Color4b(189, 13,135,255)   	/* 0xBD0D87			*/  	;
		E2C["LR"]= vcg::Color4b(199,  0,102,255)   	/* 0xC70066			*/  	;
		E2C["RF"]= vcg::Color4b(204,  0, 89,255)   	/* 0xCC0059			*/  	;
		E2C["DB"]= vcg::Color4b(209,  0, 79,255)   	/* 0xD1004F			*/  	;
		E2C["SG"]= vcg::Color4b(217,  0, 69,255)   	/* 0xD90045			*/  	;
		E2C["BH"]= vcg::Color4b(224,  0, 56,255)   	/* 0xE00038			*/  	;
		E2C["HS"]= vcg::Color4b(230,  0, 46,255)   	/* 0xE6002E			*/  	;
		E2C["MT"]= vcg::Color4b(235,  0, 38,255)   	/* 0xEB0026			*/  	;
	}                                                          

  std::string ss0,ss1,ss2;
  size_t last=std::min(atomicElement.length(),atomicElement.find_first_of(' '));
  ss0=atomicElement.substr(0,1);
	vcg::Color4b color=vcg::Color4b::Black;                 
	color=E2C[ss0];                                 
	if((color.Red==0) && (color.Green==0) && (color.Blue==0))
  { 
    ss1=atomicElement.substr(0,2);
    color = E2C[ss1];
  }
  
  return color;
}