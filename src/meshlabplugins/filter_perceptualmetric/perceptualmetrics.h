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

/****************************************************************************
  History
$Log: perceptualmetrics.h,v $
****************************************************************************/

#ifndef PERCEPTUALMETRICS_H
#define PERCEPTUALMETRICS_H

#include <QObject>

#include <common/interfaces.h>
#include <vcg/complex/trimesh/stat.h>

template <class MeshType>
class PerceptualMetrics
{

// definitions
public:

	typedef typename MeshType::CoordType         CoordType;
	typedef typename MeshType::ScalarType        ScalarType;
	typedef typename MeshType::VertexType        VertexType;
	typedef typename MeshType::VertexPointer     VertexPointer;
	typedef typename MeshType::CoordType         CoordType;
	typedef typename MeshType::VertexIterator    VertexIterator;
	typedef typename MeshType::FacePointer       FacePointer;
	typedef typename MeshType::FaceIterator      FaceIterator;
	typedef typename MeshType::FaceType          FaceType;
	typedef typename MeshType::FaceContainer     FaceContainer;


// private methods
private:

	static void planeProjection(double a, double b, double c, double d, 
		const CoordType &axis1, const CoordType &axis2, const CoordType & O, 
		const CoordType &p, double &xcoor, double &ycoor)
	{
		double u = p[0];
		double v = p[1];
		double w = p[2];

		CoordType Pproj;

		double num = a*u + b*v + c*w + d;

		// den = a*a + b*b + c*c that is assumed one in this case (!)
		Pproj[0] = u - a * num; // num/den
		Pproj[1] = v - b * num; // num/den
		Pproj[2] = w - c * num; // num/den

		Pproj -= O;

		xcoor = axis1 * Pproj;
		ycoor = axis2 * Pproj;
	}

	static void computingDisplacementFunctionCoefficients(FacePointer f0, FacePointer f1, 
		double & a1u, double & a2u, double & a3u, double & a1v, double & a2v, double & a3v,
		double & area)
	{
		CoordType pD = f0->V(0)->P();
		CoordType pE = f0->V(1)->P();
		CoordType pF = f0->V(2)->P();

		CoordType pDp = f1->V(0)->P();
		CoordType pEp = f1->V(1)->P();
		CoordType pFp = f1->V(2)->P();

		CoordType axis1 = (pE-pD);
		CoordType axis2 = (pF-pD);
		CoordType N = axis1 ^ axis2;

		// axis adjustment
		axis2 = N ^ axis1;

		axis1.Normalize();
		axis2.Normalize();
		N.Normalize();

		double a = N[0];
		double b = N[1];
		double c = N[2];
		double d = -(a * pD[0] + b * pD[1] + c * pD[2]);

		// triangle in the local reference system
		double xD,yD,xE,yE,xF,yF;
		planeProjection(a,b,c,d, axis1, axis2, pD, pD, xD, yD);
		planeProjection(a,b,c,d, axis1, axis2, pD, pE, xE, yE);
		planeProjection(a,b,c,d, axis1, axis2, pD, pF, xF, yF);

		// triangle in the local reference system after deformations
		double xDp,yDp,xEp,yEp,xFp,yFp;
		planeProjection(a,b,c,d, axis1, axis2, pD, pDp, xDp, yDp);
		planeProjection(a,b,c,d, axis1, axis2, pD, pEp, xEp, yEp);
		planeProjection(a,b,c,d, axis1, axis2, pD, pFp, xFp, yFp);

		// deformation
		double uD = xDp - xD;
		double vD = yDp - yD;

		double uE = xEp - xE;
		double vE = yEp - yE;

		double uF = xFp - xF;
		double vF = yFp - yF;

		// parameterization
		double AA = (yE-yF) * xD + (yF-yD) * xE + (yD-yE) * xF;  // AA = 2A
		double factor = 1.0 / (AA);

		a1u = factor * ((xE*yF - xF*yE)*uD + (xF*yD - xD*yF)*uE + (xD*yE - xE*yD)*uF);
		a2u = factor * ((yE-yF)*uD + (yF-yD)*uE + (yD-yE)*uF);
		a3u = factor * ((xF-xE)*uD + (xD-xF)*uE + (xE-xD)*uF);

		a1v = factor * ((xE*yF - xF*yE)*vD + (xF*yD - xD*yF)*vE + (xD*yE - xE*yD)*vF);
		a2v = factor * ((yE-yF)*vD + (yF-yD)*vE + (yD-yE)*vF);
		a3v = factor * ((xF-xE)*vD + (xD-xF)*vE + (xE-xD)*vF);

		area = 0.5 * AA;
	}

// public methods
public:

	static double roughnessMultiscale(MeshType & refmesh, MeshType & mesh)
	{
		//...TODO...
		return 0.0;
	}

	static double roughnessSmoothing(MeshType & refmesh, MeshType & mesh)
	{
		//...TODO...
		return 0.0;
	}

	static double strainEnergy(MeshType & refmesh, MeshType & mesh)
	{
		double epsilonx, epsilony, epsilonz;
		double gammaxy, gammayz, gammazx;
		double epsilonx_prime, epsilony_prime, epsilonz_prime;
		double epsilon_ii_squared;
		double epsilon_ij_prime_epsilon_ij_prime;
		double Sdelta;    // Area of each triangle
		double ni = 0.0;  // Poisson's ratio
		double E = 1.0;   // Young's modulus
		double lambda;    // Lame's first parameter
		double G;
		double Warea, Wdistortion, W;
		double u,v;
		double a1u,a2u,a3u;
		double a1v,a2v,a3v;
		double x,y;
		double D;
		
		FacePointer f0;
		FacePointer f1;

		W = 0.0;
		for (int i = 0; i < refmesh.fn; i++)
		{
			f0 = &refmesh.face[i];
			f1 = &mesh.face[i];
			
			// displacement function are:
			//    u = a1u + x a2u + y a3u 
			//    v = a1v + x a2v + y a3v
			computingDisplacementFunctionCoefficients(f0, f1, a1u, a2u, a3u, a1v, a2v, a3v, Sdelta);

			// epsilonx = du / dx
			epsilonx = a2u;

			// epsilony = dv / dy
			epsilony = a3v;

			// epsilonz
			epsilonz = (ni / (ni - 1.0)) * (epsilonx + epsilony);
			
			// gammaxy = dv / dx + du / dy
			gammaxy = a2v + a3u;
			gammayz = 0.0;
			gammazx = 0.0;

			// strain energy for a single triangle
			///////////////////////////////////////////////////////////////////////////////////////
			
			epsilonx_prime = epsilonx - (epsilonx + epsilony + epsilonz) / 3.0;
			epsilony_prime = epsilony - (epsilonx + epsilony + epsilonz) / 3.0;
			epsilonz_prime = epsilonz - (epsilonx + epsilony + epsilonz) / 3.0;
			
			epsilon_ii_squared = (epsilonx + epsilony + epsilonz) * (epsilonx + epsilony + epsilonz);
			epsilon_ij_prime_epsilon_ij_prime = epsilonx_prime * epsilonx_prime + epsilony_prime * epsilony_prime + 
				epsilonz_prime * epsilonz_prime + 0.5 * (gammaxy*gammaxy + gammayz*gammayz + gammazx*gammazx);
			
			lambda = (E * ni) / ((1.0 + ni) * (1 - 2.0 * ni));
			G = E / (2.0 * (1.0 + ni));
			
			Warea = 0.5 * (lambda + 0.666666666666 * G) * epsilon_ii_squared * Sdelta;
			Wdistortion = G * epsilon_ij_prime_epsilon_ij_prime * Sdelta;
			W += Warea + Wdistortion;
		}

		// Average Strain Energy (ASE)
		//////////////////////////////////////////////////////////////////////////
		
		double area1 = vcg::tri::Stat<MeshType>::ComputeMeshArea(refmesh);
		//double area2 = vcg::tri::Stat<MeshType>::ComputeMeshArea(mesh);

		// the area if the deformed mesh is considered the same of the original one,
		// since the deformation is assumed to be small 

		return W / (area1);
	}

};

#endif  /* PERCEPTUALMETRICS_H */
