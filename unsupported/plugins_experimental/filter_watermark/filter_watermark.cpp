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

#include "filter_watermark.h"
#include "utilsWatermark.h"
#include <QtScript>


using namespace vcg;
using namespace std;


// Constructor usually performs only two simple tasks of filling the two lists 
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

WatermarkPlugin::WatermarkPlugin() 
{ 
	typeList << FP_EMBED_WATERMARK << FP_DECODE_WATERMARK;
  
	  foreach(FilterIDType tt , types())
		  actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action 
// (this string is used also to define the menu entry)
QString WatermarkPlugin::filterName(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_EMBED_WATERMARK :  return QString("Watermark embedding on 3D points"); 
		case FP_DECODE_WATERMARK :  return QString("Watermark decoding on 3D points"); 
		default : assert(0); 
	}
  return QString();
}

// Info() must return the longer string describing each filtering action 
// (this string is used in the About plugin dialog)
 QString WatermarkPlugin::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
		case FP_EMBED_WATERMARK :  return QString("Embed a watermark on the model with a given strength depending on a string code. The string code can be a mix of letters, numbers and special characters. A missing alarm probability is guaranteed according to a prefixed false alarm probability.<br>  See: <br />   <i>F. Uccheddu, M. Corsini and M. Barni </i><br/>  <b>Wavelet-Based Blind Watermarking of 3D Models</b><br/>ACM Multimedia and Security Workshop, 2004, pp. 143-154.<br/><br>"); 
		case FP_DECODE_WATERMARK :  return QString("Decode a watermark potentially embedded on the model and depending on a string code. The string code is a mix of letters, numbers and special characters."); 
		default : assert(0); 
	}
	return QString("Unknown Filter");
}

// The FilterClass describes in which generic class of filters it fits. 
// This choice affect the submenu in which each filter will be placed 
// More than a single class can be choosen.
WatermarkPlugin::FilterClass WatermarkPlugin::getClass(QAction *a)
{
  switch(ID(a))
	{
    case FP_EMBED_WATERMARK :  return MeshFilterInterface::Smoothing;
		case FP_DECODE_WATERMARK :  return MeshFilterInterface::Smoothing; 
		default : assert(0); 
	}
	return MeshFilterInterface::Generic;
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parameter you need to define, 
// - the name of the parameter, 
// - the string shown in the dialog 
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void WatermarkPlugin::initParameterSet(QAction *action,MeshModel &m, RichParameterSet & parlst) 
{
	 switch(ID(action))	 {
		case FP_EMBED_WATERMARK :  
    case FP_DECODE_WATERMARK :
      parlst.addParam(new RichFloat("Power_gamma",(float)0.001,"Power value","Power value for the watermark; trade-off between visual quality and robustness. The default is 0.001"));
			parlst.addParam(new RichString("String_code","STRING_CODE", "Embedding code","The code to embed into the model. It can be an alphanumeric string"));
			break;											
		default : assert(0); 
	}
}




// The Real Core Function doing the actual mesh processing.
// Move Vertex of a random quantity
bool WatermarkPlugin::applyFilter(QAction * filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
	
	MeshModel &m=*md.mm();
	const float max_gamma = par.getFloat("Power_gamma");
	const QString string_code = par.getString("String_code");

	utilsWatermark utilw;
	double cellTablesize = 1.0;
	int nsTheta = 360.0/cellTablesize;
	int nsPhi = 180.0/cellTablesize;

	unsigned int seed = utilw.ComputeSeed(string_code);
	
  vector<vector<double> > watermarkTable = utilw.CreateWatermarkTable(seed);

	switch(ID(filter))
	{
		case  FP_EMBED_WATERMARK:
			{
				int indexThetaRef,indexPhiRef;
		
				double DELTAR;

				//for(unsigned int i = 0; i< m.cm.vert.size(); i++)
				for(CMeshO::VertexIterator vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
				{
					double sphR,sphTheta,sphPhi;
					
						double dx = (vi)->P()[0];
						double dy = (vi)->P()[1];
						double dz = (vi)->P()[2];

					// Passo 1 : Trasformazione in coordinate sferiche della wavelet
					 utilw.cartesian2sph( vi->P()[0], vi->P()[1],vi->P()[2], sphR, sphTheta, sphPhi );
					
					// Può accadere, basta pensare al vertice (-1.0,0.0,0.0)
					if ( (sphTheta+180.0) >= 360.0)
						sphTheta -= 360.0;
			                    
					double dd = PHIEXCLUSION_DEGREE;
					if ((sphR > 0.000000000001)&&(sphPhi < 180.0-PHIEXCLUSION_DEGREE)&&(sphPhi > PHIEXCLUSION_DEGREE))
					{
						// Passo 2 : Mappatura sul Tabella

 						nsPhi = 180.0/cellTablesize;
						nsTheta = 360.0/cellTablesize;

						indexThetaRef = (int)utilw.round( (sphTheta+180.0) );
						indexPhiRef = (int)utilw.round( (179.0/180.0)*sphPhi );

						if (indexThetaRef == nsTheta)
							indexThetaRef = 0;

						DELTAR = watermarkTable[indexPhiRef][indexThetaRef];

						//watermark on vertex module
						sphR = sphR + DELTAR*max_gamma;

						if (sphR < 0.00000000001)
							sphR = 0.00000000001;

						utilw.sph2cartesian( sphR,sphTheta,sphPhi, (vi)->P()[0], (vi)->P()[1], (vi)->P()[2] );
						double dx = (vi)->P()[0];
						double dy = (vi)->P()[1];
						double dz = (vi)->P()[2];

						int dd=0;
					}
					
				}
				vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
			

			} break;
		case  FP_DECODE_WATERMARK:
			{
				double Pf = 0.01;
				double Pm;
			
				double sphR,sphTheta,sphPhi;

				int indexThetaRef,indexPhiRef;
				
				double DELTAR;

				vector<double> corr;
				vector<double> sphRi;

				int n=0;

				for(CMeshO::VertexIterator vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
				{

					// Passo 1 : Trasformazione in coordinate sferiche della wavelet
					 utilw.cartesian2sph( vi->P()[0], vi->P()[1],vi->P()[2], sphR, sphTheta, sphPhi );
					
					// Può accadere, basta pensare al vertice (-1.0,0.0,0.0)
					if ( (sphTheta+180.0) >= 360.0)
						sphTheta -= 360.0;
			                    
					if ((sphR > 0.000000000001)&&(sphPhi < 180.0-PHIEXCLUSION_DEGREE)&&(sphPhi > PHIEXCLUSION_DEGREE))
					{
						nsPhi = 180.0/cellTablesize;
						nsTheta = 360.0/cellTablesize;

						indexThetaRef = (int)utilw.round( (sphTheta+180.0) );
						indexPhiRef = (int)utilw.round( (179.0/180.0)*sphPhi );

						if (indexThetaRef == nsTheta)
							indexThetaRef = 0;

						DELTAR = watermarkTable[indexPhiRef][indexThetaRef];
				 				
						sphRi.push_back( sphR );
						corr.push_back(sphR*DELTAR);
						n++;
								
					}
				}

				double mean_corr = 0;
				double var_corr = 0;

				// mean
				for(int i = 0; i < n; i++)
					mean_corr += corr[i];
				mean_corr /= (double)n;
		       
		        
				// variance
				for(int i = 0; i < n; i++)
					 var_corr+= pow(corr[i] - mean_corr, 2);
				var_corr /= (double)n;
				

				//VALORI TEORICI
				double muRhoH0,muRhoH1;
				double varRhoH0, varRhoH1;
				double var_sphR = 0.0;
 				for (int i=0; i<n; i++)
 					var_sphR += pow(sphRi[i],2);
 				var_sphR /= (double)n;

				double b = RANGE_MARCHIO;

				muRhoH0 = 0.0;
				varRhoH0 = (((b*b)/3.0)*var_sphR)/(double)(n-1);

				muRhoH1 = max_gamma*((b*b)/3.0)/(double)(n);
				varRhoH1 = ((b*b)/3.0)/(double)(n-1)*(var_sphR+pow(max_gamma,2)*b*b/(double)(3*n));

				double threshold = utilw.thresholdRo( muRhoH0, varRhoH0, muRhoH1, varRhoH1, Pf, Pm );

				if (mean_corr <= threshold){
					errorMessage = "The mesh is NOT watermarked"; // 
					return false;
				}
				vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
			} break;
			default : assert(0); 
		}
      
	
}




QString WatermarkPlugin::filterScriptFunctionName( FilterIDType filterID )
{
	switch(filterID) {
		case FP_EMBED_WATERMARK :  return QString("embedWatermarking"); 
		case FP_DECODE_WATERMARK :  return QString("decodeWatermarking");
		default : assert(0); 
	}
	return QString();
}

Q_EXPORT_PLUGIN(WatermarkPlugin)
