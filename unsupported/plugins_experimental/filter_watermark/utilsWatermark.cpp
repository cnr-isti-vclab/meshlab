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

#include "utilsWatermark.h"
#include "qstring.h"
#include "filter_watermark.h"
#include "ERF.h"

using namespace std;

// Seed generator for the psedo random sequence in the watermark table 
// The seed is obtained by multiplying the ascii codes of each char of the chosen string
unsigned int utilsWatermark::ComputeSeed( QString string_code  )
{
	unsigned int code = (unsigned int)(string_code[0].digitValue());

	for (int i=1; i<string_code.length(); i++)
	{
		code *= (unsigned int)(string_code[i].digitValue());
	}

	return code;
}

vector< vector<double> > utilsWatermark::CreateWatermarkTable(unsigned int seed)
{
	
	
	double cellTablesize = 1.0;
	static vcg::math::MarsenneTwisterRNG rnd;
	rnd.initialize(seed);
	double p = 0; // I valori generati di 'p' vanno da -RANGE_MARCHIO a RANGE_MARCHIO

	int nsTheta = 360.0/cellTablesize;
	int nsPhi = 180.0/cellTablesize;

	vector<vector<double> > watermarkTable;
	watermarkTable.resize(nsPhi);
	for (int k = 0; k < nsPhi; ++k)
	    watermarkTable[k].resize(nsTheta);


	int i,j;
	for (i=0; i<nsPhi; i++)
	{
		for (j=0; j<nsTheta; j++)
		{
			p = 2.0*(rnd.generate01())*RANGE_MARCHIO - RANGE_MARCHIO;
			watermarkTable[i][j] = p;
		}
	}

	return watermarkTable;
}



// Spheric to Cartesian Coordinates System conversion
void utilsWatermark::sph2cartesian(double R, double theta, double phi, float &x, float &y, float &z)
{
	// Conversione Gradi --> Radianti
	phi *= DEG_TO_RAD;
	theta *= DEG_TO_RAD;

	x = float(R*cos(theta)*sin(phi));
	z = float(R*sin(theta)*sin(phi));
	y = float(R*cos(phi));
	
}

// Cartesian to Spheric Coordinates System conversion
void utilsWatermark::cartesian2sph(double x, double y, double z, double &R, double &theta, double &phi)
{
	R = sqrt( x*x + y*y + z*z );
	theta = atan2( z,x );
	phi = acos( y / R );
	
	// Conversione Radianti --> Gradi
	phi *= RAD_TO_DEG;
	theta *= RAD_TO_DEG;
}

// rounding
double utilsWatermark::round(double x)
{
	double app = floor(x);

	if (x - app >= 0.5)
		return (app + 1.0);
	else 
		return app;
}

// evaluate threshold
double utilsWatermark::thresholdRo(double muRhoH0, double varRhoH0, 
				   double muRhoH1, double varRhoH1,
				   double Pf, double &Pm)
{
	
	Erf erf;
	double Y;
	double Z;

	// Calcolo di T(ro)
	/////////////////////////////////////////////////


	Y = erf.inverfc( 2.0*Pf );

	double Trho = sqrt(2.0*varRhoH0)*Y + muRhoH0;

	// Calcolo di Pm
	///////////////////////////////////////////////////////

	//Z = gsl_sf_erfc( sqrt( ((muRhoH1 - Trho)*(muRhoH1 - Trho))/(2.0*varRhoH1)) );
	//Z = erfc( sqrt( ((muRhoH1 - Trho)*(muRhoH1 - Trho))/(2.0*varRhoH1)) );
	Z = erf.erfc( sqrt( ((muRhoH1 - Trho)*(muRhoH1 - Trho))/(2.0*varRhoH1)) );

	Pm = 0.5*Z;
	
	return Trho;
}
