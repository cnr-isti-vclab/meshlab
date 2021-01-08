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
#include "filter_create.h"
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/point_sampling.h>
#include <vcg/complex/algorithms/smooth.h>
#include <vcg/math/gen_normal.h>

using namespace vcg;
using namespace tri;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterCreate::FilterCreate()
{
    typeList << CR_BOX<< CR_ANNULUS << CR_SPHERE<< CR_SPHERE_CAP
             << CR_RANDOM_SPHERE<< CR_ICOSAHEDRON<< CR_DODECAHEDRON
             << CR_TETRAHEDRON<<CR_OCTAHEDRON<<CR_CONE<<CR_TORUS
			 << CR_FITPLANE;

  foreach(FilterIDType tt , types())
      actionList << new QAction(filterName(tt), this);
}

QString FilterCreate::pluginName() const
{
    return "FilterCreate";
}

QString FilterCreate::filterName(FilterIDType filterId) const
{
  switch(filterId) {
  case CR_BOX : return QString("Box/Cube");
  case CR_ANNULUS : return QString("Annulus");
  case CR_SPHERE: return QString("Sphere");
  case CR_SPHERE_CAP: return QString("Sphere Cap");
  case CR_RANDOM_SPHERE: return QString("Points on a Sphere");
  case CR_ICOSAHEDRON: return QString("Icosahedron");
  case CR_DODECAHEDRON: return QString("Dodecahedron");
  case CR_OCTAHEDRON: return QString("Octahedron");
  case CR_TETRAHEDRON: return QString("Tetrahedron");
  case CR_CONE: return QString("Cone");
  case CR_TORUS: return QString("Torus");
  case CR_FITPLANE: return QString("Fit a plane to selection");
  default : assert(0);
  }
  return NULL;
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterCreate::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
  case CR_BOX : return QString("Create a Box, Cube, Hexahedron. You can specify the side length.");
  case CR_ANNULUS : return QString("Create an Annulus e.g. a flat region bounded by two concentric circles, or a holed disk.");
  case CR_SPHERE: return QString("Create a Sphere, whose topology is obtained as regular subdivision of an icosahedron.");
  case CR_SPHERE_CAP: return QString("Create a Sphere Cap, or spherical dome, subtended by a cone of given angle");
  case CR_RANDOM_SPHERE: return QString("Create a spherical point cloud, it can be random or regularly distributed.");
  case CR_ICOSAHEDRON: return QString("Create an Icosahedron");
  case CR_DODECAHEDRON: return QString("Create an Dodecahedron");
  case CR_OCTAHEDRON: return QString("Create an Octahedron");
  case CR_TETRAHEDRON: return QString("Create a Tetrahedron");
  case CR_CONE: return QString("Create a Cone");
  case CR_TORUS: return QString("Create a Torus");
  case CR_FITPLANE: return QString("Create a quad on the plane fitting the selection");
  default : assert(0);
  }
  return NULL;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterCreate::initParameterList(const QAction *action, MeshModel & /*m*/, RichParameterList & parlst)
{
  switch(ID(action))	 {

  case CR_SPHERE :
    parlst.addParam(RichFloat("radius",1,"Radius","Radius of the sphere"));
    parlst.addParam(RichInt("subdiv",3,"Subdiv. Level","Number of the recursive subdivision of the surface. Default is 3 (a sphere approximation composed by 1280 faces).<br>"
                                "Admitted values are in the range 0 (an icosahedron) to 8 (a 1.3 MegaTris approximation of a sphere)"));
    break;

  case CR_SPHERE_CAP :
    parlst.addParam(RichFloat("angle",60,"Angle","Angle of the cone subtending the cap. It must be < 180"));
    parlst.addParam(RichInt("subdiv",3,"Subdiv. Level","Number of the recursive subdivision of the surface. Default is 3 (a sphere approximation composed by 1280 faces).<br>"
                                "Admitted values are in the range 0 (an icosahedron) to 8 (a 1.3 MegaTris approximation of a sphere)"));
    break;
  case CR_ANNULUS :
    parlst.addParam(RichFloat("internalRadius",0.5f,"Internal Radius","Internal Radius of the annulus"));
    parlst.addParam(RichFloat("externalRadius",1.0f,"External Radius","Externale Radius of the annulus"));
    parlst.addParam(RichInt("sides",32,"Sides","Number of the sides of the poligonal approximation of the annulus "));
    break;
  case CR_RANDOM_SPHERE :
    parlst.addParam(RichInt("pointNum",100,"Point Num","Number of points (approximate)."));
    parlst.addParam(RichEnum("sphereGenTech", 3,
                                 QStringList() << "Montecarlo" << "Poisson Sampling" << "DiscoBall" << "Octahedron" << "Fibonacci",
                                 tr("Generation Technique:"),
                                 tr("Generation Technique:"
                                        "<b>Montecarlo</b>: The points are randomly generated with an uniform distribution.<br>"
                                        "<b>Poisson Disk</b>: The points are to follow a poisson disk distribution.<br>"
                                        "<b>Disco Ball</b> Dave Rusin's disco ball algorithm for the regular placement of points on a sphere is used. <br>"
                                        "<b>Recursive Octahedron</b> Points are generated on the vertex of a recursively subdivided octahedron <br>"
                                        "<b>Fibonacci</b> . "
                                        )));

    break;
  case CR_BOX :
    parlst.addParam(RichFloat("size",1,"Scale factor","Scales the new mesh"));
    break;
  case CR_CONE:
    parlst.addParam(RichFloat("r0",1,"Radius 1","Radius of the bottom circumference"));
    parlst.addParam(RichFloat("r1",2,"Radius 2","Radius of the top circumference"));
    parlst.addParam(RichFloat("h",3,"Height","Height of the Cone"));
    parlst.addParam(RichInt("subdiv",36,"Side","Number of sides of the polygonal approximation of the cone"));
    break;

  case CR_TORUS:
    parlst.addParam(RichFloat("hRadius",3,"Horizontal Radius","Radius of the whole horizontal ring of the torus"));
    parlst.addParam(RichFloat("vRadius",1,"Vertical Radius","Radius of the vertical section of the ring"));
    parlst.addParam(RichInt("hSubdiv",24,"Horizontal Subdivision","Subdivision step of the ring"));
    parlst.addParam(RichInt("vSubdiv",12,"Vertical Subdivision","Number of sides of the polygonal approximation of the torus section"));
    break;

  case CR_FITPLANE:
      parlst.addParam(RichFloat("extent", 1.0, "Extent (with respect to selection)", "How large is the plane, with respect to the size of the selection: 1.0 means as large as the selection, 1.1 means 10% larger thena the selection"));
      parlst.addParam(RichInt("subdiv", 3, "Plane XY subivisions", "Subdivision steps of plane borders"));
      parlst.addParam(RichBool("hasuv", false, "UV parametrized", "The created plane has an UV parametrization"));
      parlst.addParam(RichEnum("orientation", 0,
		  QStringList() << "quasi-Straight Fit" << "Best Fit" << "XZ Parallel" << "YZ Parallel" << "YX Parallel",
		  tr("Plane orientation"),
		  tr("Orientation:"
		  "<b>quasi-Straight Fit</b>: The fitting plane will be oriented (as much as possible) straight with the axeses.<br>"
		  "<b>Best Fit</b>: The fitting plane will be oriented and sized trying to best fit to the selected area.<br>"
		  "<b>-- Parallel</b>: The fitting plane will be oriented with a side parallel with the chosen plane. WARNING: do not use if the selection is exactly parallel to a plane.<br>"
		  )));
	  break;
  default : return;
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterCreate::applyFilter(const QAction *filter, MeshDocument &md, std::map<std::string, QVariant>&, unsigned int& /*postConditionMask*/, const RichParameterList & par, CallBackPos * /*cb*/)
{
	MeshModel *currM = md.mm();
	MeshModel *m = nullptr;

	switch(ID(filter))	 
	{
	case CR_TETRAHEDRON :
		m = md.addNewMesh("", this->filterName(ID(filter)));
		tri::Tetrahedron<CMeshO>(m->cm);
		break;
	case CR_ICOSAHEDRON:
		m = md.addNewMesh("", this->filterName(ID(filter)));
		tri::Icosahedron<CMeshO>(m->cm);
		break;
	case CR_DODECAHEDRON:
		m = md.addNewMesh("", this->filterName(ID(filter)));
		tri::Dodecahedron<CMeshO>(m->cm);
		m->updateDataMask(MeshModel::MM_POLYGONAL);
		break;
	case CR_OCTAHEDRON:
		m = md.addNewMesh("", this->filterName(ID(filter)));
		tri::Octahedron<CMeshO>(m->cm);
		break;
	case CR_ANNULUS:
		m = md.addNewMesh("", this->filterName(ID(filter)));
		tri::Annulus<CMeshO>(m->cm,par.getFloat("internalRadius"), par.getFloat("externalRadius"), par.getInt("sides"));
		break;
	case CR_TORUS:
		{
			m = md.addNewMesh("", this->filterName(ID(filter)));
			Scalarm hRadius=par.getFloat("hRadius");
			Scalarm vRadius=par.getFloat("vRadius");
			int hSubdiv=par.getInt("hSubdiv");
			int vSubdiv=par.getInt("vSubdiv");
			tri::Torus(m->cm,hRadius,vRadius,hSubdiv,vSubdiv);
		}
		break;

	case CR_FITPLANE:
	{
		Box3m selBox; //boundingbox of the selected vertices
		std::vector< Point3m > selected_pts; //copy of selected vertices, for plane fitting

		if (currM == NULL)
		{
			errorMessage = "No mesh layer selected"; 
			return false;
		}

		if (currM->cm.svn == 0 && currM->cm.sfn == 0) // if no selection, fail
		{
			errorMessage = "No selection";
			return false;
		}

		m = md.addNewMesh("", "Fitted Plane");

		if (currM->cm.svn == 0 || currM->cm.sfn != 0)
		{
			tri::UpdateSelection<CMeshO>::VertexClear(currM->cm);
			tri::UpdateSelection<CMeshO>::VertexFromFaceLoose(currM->cm);
		}

		Point3m Naccum = Point3m(0.0, 0.0, 0.0);
		for (CMeshO::VertexIterator vi = currM->cm.vert.begin(); vi != currM->cm.vert.end(); ++vi)
		if (!(*vi).IsD() && (*vi).IsS())
		{
			Point3m p = (*vi).P();
			selBox.Add(p);
			selected_pts.push_back(p);
			Naccum = Naccum + (*vi).N();
		}
		log("Using %i vertices to build a fitting plane", int(selected_pts.size()));
		Plane3m plane;
		FitPlaneToPointSet(selected_pts, plane);
		plane.Normalize();
		// check if normal of the interpolated plane is coherent with average normal of the used points, otherwise, flip
		// i do this because plane fitter does not take in account source noramls, and a fliped fit is terrible to see
		Naccum = (Naccum / (CMeshO::ScalarType)selected_pts.size()).Normalize();
		if ((plane.Direction() * Naccum) < 0.0)
			plane.Set(-plane.Direction(), -plane.Offset());

		float errorSum = 0;
		for (size_t i = 0; i < selected_pts.size(); ++i)
			errorSum += fabs(SignedDistancePlanePoint(plane, selected_pts[i]));
		log("Fitting Plane avg error is %f", errorSum / float(selected_pts.size()));
		log("Fitting Plane normal is [%f, %f, %f]", plane.Direction().X(), plane.Direction().Y(), plane.Direction().Z());
		log("Fitting Plane offset is %f", plane.Offset());

		// find center of selection on plane
		Point3m centerP;
		for (size_t i = 0; i < selected_pts.size(); ++i)
		{
			centerP += plane.Projection(selected_pts[i]);
		}
		centerP /= selected_pts.size();
		log("center [%f, %f, %f]", centerP.X(), centerP.Y(), centerP.Z());

		// find horizontal and vertical axis
		Point3m dirH, dirV;

		int orientation = par.getEnum("orientation");

		if (orientation == 0)
		{
			if ((plane.Direction().X() <= plane.Direction().Y()) && (plane.Direction().X() <= plane.Direction().Z()))
				dirH = Point3m(1.0, 0.0, 0.0) ^ plane.Direction();
			else if ((plane.Direction().Y() <= plane.Direction().X()) && (plane.Direction().Y() <= plane.Direction().Z()))
				dirH = Point3m(0.0, 1.0, 0.0) ^ plane.Direction();
			else
				dirH = Point3m(0.0, 0.0, 1.0) ^ plane.Direction();

			dirH.Normalize();
			dirV = dirH ^ plane.Direction();
			dirV.Normalize();
		}
		else if (orientation == 1)
		{
			Matrix33m cov;
			vector<Point3m> PtVec;
			for (size_t i = 0; i < selected_pts.size(); ++i)
				PtVec.push_back(plane.Projection(selected_pts[i]));

			cov.Covariance(PtVec, centerP);
			Matrix33f eigenvecMatrix;
			Point3f eigenvecVector;
			Eigen::Matrix3d em;
			cov.ToEigenMatrix(em);
			Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eig(em);
			Eigen::Vector3d c_val = eig.eigenvalues();
			Eigen::Matrix3d c_vec = eig.eigenvectors();

			eigenvecMatrix.FromEigenMatrix(c_vec);
			eigenvecVector.FromEigenVector(c_val);

			// max eigenvector is best horizontal axis, but is not guarantee is orthogonal to plane normal, so
			// I use eigenvector ^ plane direction and assign it to vertical plane axis
			if ((eigenvecVector[0]<=eigenvecVector[1]) && (eigenvecVector[0]<=eigenvecVector[2]))
				dirV = Point3m(eigenvecMatrix[0][0], eigenvecMatrix[0][1], eigenvecMatrix[0][2]) ^ plane.Direction();
			if ((eigenvecVector[1]<=eigenvecVector[0]) && (eigenvecVector[1]<=eigenvecVector[2]))
				dirV = Point3m(eigenvecMatrix[1][0], eigenvecMatrix[1][1], eigenvecMatrix[1][2]) ^ plane.Direction();
			else
				dirV = Point3m(eigenvecMatrix[2][0], eigenvecMatrix[2][1], eigenvecMatrix[2][2]) ^ plane.Direction();

			dirV.Normalize();
			dirH = plane.Direction() ^ dirV;
			dirH.Normalize();
		}
		else if (orientation == 2)
		{
				dirH = Point3m(0.0, 1.0, 0.0) ^ plane.Direction();
				dirH.Normalize();
				dirV = dirH ^ plane.Direction();
				dirV.Normalize();

		}
		else if (orientation == 3)
		{
				dirH = Point3m(1.0, 0.0, 0.0) ^ plane.Direction();
				dirH.Normalize();
				dirV = dirH ^ plane.Direction();
				dirV.Normalize();
		}
		else if (orientation == 4)
		{

				dirH = Point3m(0.0, 0.0, 1.0) ^ plane.Direction();
				dirH.Normalize();
				dirV = dirH ^ plane.Direction();
				dirV.Normalize();
		}

		// hotfix for unlikely case where the fitting is perfecrly parallel to a plane
		if (orientation >= 2 )
		{
			if (Point3m(0.0, 1.0, 0.0) * plane.Direction() == 1.0)
			{
				dirH = Point3m(1.0, 0.0, 0.0);
				dirV = Point3m(0.0, 0.0, 1.0);
			}
			if (Point3m(0.0, 0.0, 1.0) * plane.Direction() == 1.0)
			{
				dirH = Point3m(1.0, 0.0, 0.0);
				dirV = Point3m(0.0, 1.0, 0.0);
			}
			if (Point3m(1.0, 0.0, 0.0) * plane.Direction() == 1.0)
			{
				dirH = Point3m(0.0, 1.0, 0.0);
				dirV = Point3m(0.0, 0.0, 1.0);
			}
		}

		log("H [%f, %f, %f]", dirH.X(), dirH.Y(), dirH.Z());
		log("V [%f, %f, %f]", dirV.X(), dirV.Y(), dirV.Z());


		// find extent
		Scalarm dimH = -1000000;
		Scalarm dimV = -1000000;
		for (size_t i = 0; i < selected_pts.size(); ++i)
		{
			Point3m pp = plane.Projection(selected_pts[i]);
			Scalarm distH = fabs(((pp - centerP) * dirH));
			Scalarm distV = fabs(((pp - centerP) * dirV));

			if (distH > dimH)
				dimH = distH;
			if (distV > dimV)
				dimV = distV;
		}
		Scalarm exScale = par.getFloat("extent");
		dimV = dimV * exScale;
		dimH = dimH * exScale;
		log("extent on plane [%f, %f]", dimV, dimH);

		int vertNum = par.getInt("subdiv") + 1;
		if (vertNum <= 1) vertNum = 2;
		int numV, numH;
		numV = numH = vertNum;

		// UV vector, just in case
		float *UUs, *VVs;
		UUs = new float[numH*numV];
		VVs = new float[numH*numV];

		int vind = 0;
		for (int ir = 0; ir < numV; ir++)
		for (int ic = 0; ic < numH; ic++)
			{
				Point3m newP = (centerP + (dirV * -dimV) + (dirH * -dimH)); 
				newP = newP + (dirH * ic * (2.0 * dimH / (numH-1))) + (dirV * ir * (2.0 * dimV / (numV-1)));
				tri::Allocator<CMeshO>::AddVertex(m->cm, newP, plane.Direction());
				UUs[vind] = ic * (1.0 / (numH - 1));
				VVs[vind] = ir * (1.0 / (numV - 1));
				vind++;
			}
		
		FaceGrid(m->cm, numH, numV);

		bool hasUV = par.getBool("hasuv");
		if (hasUV)
		{
			m->updateDataMask(MeshModel::MM_WEDGTEXCOORD);

			CMeshO::FaceIterator fi;
			for (fi = m->cm.face.begin(); fi != m->cm.face.end(); ++fi)
			{
				for (int i = 0; i<3; ++i)
				{
					int vind = (*fi).V(i)->Index();
					(*fi).WT(i).U() = UUs[vind];
					(*fi).WT(i).V() = VVs[vind];
				}
			}
		}
		delete[] UUs;	// delete temporary UV storage
		delete[] VVs;

	} break;

	case CR_RANDOM_SPHERE:
	{
		int pointNum = par.getInt("pointNum");
		int sphereGenTech = par.getEnum("sphereGenTech");
		math::MarsenneTwisterRNG rng;
		m = md.addNewMesh("", this->filterName(ID(filter)));
		m->cm.Clear();
		std::vector<Point3m> sampleVec;


		switch(sphereGenTech)
		{
			case 0: // Montecarlo
			{
				for(int i=0;i<pointNum;++i)
					sampleVec.push_back(math::GeneratePointOnUnitSphereUniform<CMeshO::ScalarType>(rng));
			} break;
			case 1: // Poisson Disk
			{
				int oversamplingFactor =100;
				if(pointNum <= 100) oversamplingFactor = 1000;
				if(pointNum >= 10000) oversamplingFactor = 50;
				if(pointNum >= 100000) oversamplingFactor = 20;
				CMeshO tt;
				tri::Allocator<CMeshO>::AddVertices(tt,pointNum*oversamplingFactor);
				for(CMeshO::VertexIterator vi=tt.vert.begin();vi!=tt.vert.end();++vi)
					vi->P()=math::GeneratePointOnUnitSphereUniform<CMeshO::ScalarType>(rng);
				tri::UpdateBounding<CMeshO>::Box(tt);

				const float SphereArea = float(4 * M_PI);
				float poissonRadius = 2.0*sqrt((SphereArea / float(pointNum*2))/M_PI);

				std::vector<Point3m> sampleVec;
				tri::TrivialSampler<CMeshO> pdSampler(sampleVec);
				tri::SurfaceSampling<CMeshO, tri::TrivialSampler<CMeshO> >::PoissonDiskParam pp;
				tri::SurfaceSampling<CMeshO,tri::TrivialSampler<CMeshO> >::PoissonDiskPruning(pdSampler, tt, poissonRadius, pp);
			} break;
			case 2: // Disco Ball
				GenNormal<CMeshO::ScalarType>::DiscoBall(pointNum,sampleVec);
				break;
			case 3: // Recursive Oct
				GenNormal<CMeshO::ScalarType>::RecursiveOctahedron(pointNum,sampleVec);
				break;
			case 4: // Fibonacci
				GenNormal<CMeshO::ScalarType>::Fibonacci(pointNum,sampleVec);
				break;
		}
		for(size_t i=0;i<sampleVec.size();++i)
			tri::Allocator<CMeshO>::AddVertex(m->cm,sampleVec[i],sampleVec[i]);
	} break;

	case CR_SPHERE_CAP:
	{
		int rec = par.getInt("subdiv");
		const Scalarm angleDeg = par.getFloat("angle");
		m = md.addNewMesh("", this->filterName(ID(filter)));
		m->updateDataMask(MeshModel::MM_FACEFACETOPO);
		tri::UpdateTopology<CMeshO>::FaceFace(m->cm);
		tri::SphericalCap(m->cm,math::ToRad(angleDeg),rec);
	} break;

	case CR_SPHERE:
    {
		int rec = par.getInt("subdiv");
		Scalarm radius = par.getFloat("radius");
		m = md.addNewMesh("", this->filterName(ID(filter)));
		m->cm.face.EnableFFAdjacency();
		m->updateDataMask(MeshModel::MM_FACEFACETOPO);
		assert(tri::HasPerVertexTexCoord(m->cm) == false);
		tri::Sphere<CMeshO>(m->cm,rec);
		tri::UpdatePosition<CMeshO>::Scale(m->cm,radius);
	} break;

    case CR_BOX:
    {
      Scalarm sz=par.getFloat("size");
      Box3m b(Point3m(1,1,1)*(-sz/2),Point3m(1,1,1)*(sz/2));
	  m = md.addNewMesh("", this->filterName(ID(filter)));
      tri::Box<CMeshO>(m->cm,b);
            m->updateDataMask(MeshModel::MM_POLYGONAL);
	} break;

	case CR_CONE:
	{
		Scalarm r0 = par.getFloat("r0");
		Scalarm r1 = par.getFloat("r1");
		Scalarm h = par.getFloat("h");
		int subdiv = par.getInt("subdiv");
		m = md.addNewMesh("", this->filterName(ID(filter)));
		tri::Cone<CMeshO>(m->cm, r0, r1, h, subdiv);
	} break;

	}//CASE FILTER

	tri::UpdateBounding<CMeshO>::Box(m->cm);
	tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFaceNormalized(m->cm);
    return true;
}

 FilterPluginInterface::FilterClass FilterCreate::getClass(const QAction *a) const
{
	switch(ID(a))
	{
		case CR_BOX:
		case CR_TETRAHEDRON:
		case CR_ICOSAHEDRON:
		case CR_DODECAHEDRON:
		case CR_SPHERE:
		case CR_SPHERE_CAP:
		case CR_ANNULUS:
		case CR_RANDOM_SPHERE:
		case CR_OCTAHEDRON:
		case CR_CONE:
		case CR_TORUS:
		case CR_FITPLANE:
			return FilterPluginInterface::MeshCreation;
			break;
		default: 
			assert(0);
			return FilterPluginInterface::Generic;
  }
}

QString FilterCreate::filterScriptFunctionName( FilterIDType filterID )
 {
	switch(filterID)
	{
		case CR_BOX : return QString("box");
		case CR_ANNULUS : return QString("annulus");
		case CR_SPHERE: return QString("sphere");
		case CR_SPHERE_CAP: return QString("spherecap");
		case CR_RANDOM_SPHERE: return QString("randomsphere");
		case CR_ICOSAHEDRON: return QString("icosahedron");
		case CR_DODECAHEDRON: return QString("dodecahedron");
		case CR_OCTAHEDRON: return QString("octahedron");
		case CR_TETRAHEDRON: return QString("tetrahedron");
		case CR_CONE: return QString("cone");
		case CR_TORUS: return QString("torus");
		case CR_FITPLANE:  return QString("fitplane");
		default : assert(0);
    }
    return NULL;
}


MESHLAB_PLUGIN_NAME_EXPORTER(FilterCreate)
