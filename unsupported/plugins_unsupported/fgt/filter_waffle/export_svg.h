/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2006                                                \/)\/    *
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

#ifndef __VCG_LIB_EXPORTER_SVG
#define __VCG_LIB_EXPORTER_SVG

namespace vcg 
{
  namespace tri
	{
		namespace io 
		{

/**
 * SVG Properties.
 *
 * Support class to set the properties of the SVG exporter.
 */
class SVGProperties
{
	// definitions
public:
	
	// When multiple meshes are passed, they are arranged in a grid according these two values.
	// the default is two column and enough row. If numRow is not sufficient it is automatically enlarged.
	int numCol;
        int numRow;

	Point2f sizeCm; // The size, in the drawing, of each ViewBox (in cm)
	
 	Point2f marginCm; // how much space between each slice box (in cm)
  
	Point2f pageSizeCm() // This is automatically computed from the above values 
	{ 
		float xSize = numCol*sizeCm[0] + numCol*marginCm[0] + marginCm[0];
		float ySize = numRow*sizeCm[1] + numRow*marginCm[1] + marginCm[1];
		return Point2f(xSize,ySize);
	}
	
	
	Point3f projDir;	  // Direction of the Projection
	Point3f projUp;
	Point3f projCenter; // the 3d point that after projection will fall exactly in the center of the ViewBox.
	
	// How the mesh will be scaled. 
	// if this value is 0 the bounding box of all the passed meshes will be used to compute the scale and center
	// otherwise it is a scaling factor that is used to place the mesh in a unit cube (-1..1) 
	// usually it is 2/bbox.Diag
	float scale;
  
	// SVG Style Parameters
	int lineWidthPt;	// Line width.
	std::string strokeColor; 	// Stroke color (see StrokeColor).
	std::string strokeLineCap;// Stroke linecap (see StrokeLineCap).
		
	//Text details 
	bool showTextDetails;

        bool enumeration;

public:

		SVGProperties() 	
	{
		lineWidthPt = 1;
		strokeColor = "black";
		strokeLineCap = "round";

		// default projection (XZ plane with the z up)
		projDir= Point3f(0.0, 1.0, 0.0);
    projUp = Point3f(0.0, 0.0, 1.0);
		scale=0; 		
		//viewBox=Point2f(10, 10); 
		projCenter=Point3f(0, 0, 0);
		sizeCm=Point2f(10,10);
		marginCm=Point2f(1,1);
		showTextDetails=true;
		numCol=2;
                numRow=10;

                enumeration=false;
	}

};


/**
 * SVG exporter.
 *
 * This exporter save a mesh of EdgeMesh type in the SVG format.
 * Most of the features of the SVG format are not supported. 
 * The given EdgeMesh is saved as a set lines. The properties
 * of the SVG export can be set through the SVGProp class.
 */
template <class EdgeMeshType>
class ExporterSVG
{

public:
        size_t i;

	// Standard saving Function
	// just a wrapper to the below
	static bool Save(EdgeMeshType &m, const char *filename, SVGProperties & pro)
	{
		std::vector<EdgeMeshType*> MeshVec;
		MeshVec.push_back(&m);
		return Save(MeshVec,filename,pro);
	}	
	
	
	// Main saving function
	// save a Multiple Set of Edge Meshes on a single SVG files
	static bool Save(std::vector<EdgeMeshType*> &meshVec, const char *filename, SVGProperties & pro)
	{ 
		FILE * fpo = fopen(filename,"w");  
		if (fpo==NULL)			return false;
		
		WriteXmlHead(fpo, pro);
                for(size_t i=0;i<meshVec.size();++i)
                {
                        WriteXmlBody(fpo, *meshVec[i], pro, i);
		}
		fprintf(fpo, "</svg>");
		fclose(fpo);
		return true;
	}


static void WriteXmlHead(FILE *o, SVGProperties & pro)
{
	fprintf(o, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n");
	fprintf(o, "<!-- Created with vcg library -->\n");
	fprintf(o, "<svg width=\"%fcm\" height=\"%fcm\" \n",pro.pageSizeCm()[0], pro.pageSizeCm()[1]);
	fprintf(o, "	xmlns=\"http://www.w3.org/2000/svg\" \n");
	fprintf(o, "	xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n");
	fprintf(o, "	xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \n");
	fprintf(o, "	xmlns:cc=\"http://web.resource.org/cc/\" \n");
	fprintf(o, "	xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" \n");
	fprintf(o, "	xmlns:svg=\"http://www.w3.org/2000/svg\" \n \n");
	fprintf(o, "id=\"svg2\"> \n");
	fprintf(o, "	<defs id=\"defs4\"/> \n");
	fprintf(o, "	<metadata id=\"metadata7\"> \n");
	fprintf(o, "	<rdf:RDF> \n");
	fprintf(o, "	<cc:Work rdf:about=\"\"> \n");
	fprintf(o, "	<dc:format>image/svg+xml</dc:format> \n");
	fprintf(o, "	<dc:type rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" /> \n");
	fprintf(o, "	</cc:Work> \n");
	fprintf(o, "	</rdf:RDF> \n");
	fprintf(o, "</metadata> \n \n");
}

static void WriteXmlBody(FILE* fpo, EdgeMeshType &mp, SVGProperties &pro, int meshIndex)
{ 
	int rowInd = meshIndex / pro.numCol;
        int colInd = meshIndex % pro.numCol;

	fprintf(fpo, "  <rect width= \" %fcm \" height= \" %fcm \" x=\"%fcm \" y=\"%fcm \" "
					     "        style= \" stroke-width:1pt; fill-opacity:0.0; stroke:rgb(0,0,0)\" /> \n",
					     pro.sizeCm[0], pro.sizeCm[1], pro.marginCm[0]+colInd*(pro.sizeCm[0]+pro.marginCm[0]), pro.marginCm[1]+rowInd*(pro.sizeCm[1]+pro.marginCm[1]));
	fprintf(fpo, "<g stroke=\"%s\" stroke-linecap=\"%s\" stroke-width = \"%fpt\" > \n",  pro.strokeColor.c_str(), pro.strokeLineCap.c_str(),pro.lineWidthPt/100.0f);
  fprintf(fpo, "  <svg id = \"SliceNum%d\" viewBox=\"-1000 -1000 2000 2000\" width=\"%fcm\" height=\"%fcm\" x=\"%fcm\" y=\"%fcm\" >\n", meshIndex,pro.sizeCm[0],pro.sizeCm[1],
					pro.marginCm[0]+colInd*(pro.sizeCm[0]+pro.marginCm[0]), pro.marginCm[1]+rowInd*(pro.sizeCm[1]+pro.marginCm[1]) );

	
	// Main loop of edge printing
		typename EdgeMeshType::EdgeIterator i;

		// XY projection. 
		// It is a classcial ortho projection 
		// eg it resolves to a rotation Matrix such that 
		// - the passed projDir become the z axis
		// - the passed projUp lie on the upper YZ plane.
		
		// First Step align projDir to Z
		Matrix33f rotM = RotationMatrix(pro.projDir,Point3f(0,0,1),false);
		Point3f rotatedUp = rotM * pro.projUp;
		Point3f rotCenter = rotM * pro.projCenter;
		float scale = pro.scale;
		if(scale==0) scale = 2.0/mp.bbox.Diag();

                for (i = mp.edge.begin(); i != mp.edge.end(); ++i) if(!(*i).IsD())
		{
			Point3f p0 = (-rotCenter + rotM * ((*i).V(0)->P()))*scale*1000;
			Point3f p1 = (-rotCenter + rotM * ((*i).V(1)->P()))*scale*1000;
                        fprintf(fpo, "        <line x1=\"%f\" y1=\"%f\" x2=\"%f\" y2=\"%f\" />\n", p0[0],p0[1],p1[0],p1[1]);
		}
		
		fprintf(fpo, "  </svg>\n");
                fprintf(fpo, "</g>\n");


                fprintf(fpo, " <text x=\"%fcm\" y=\"%fcm\" style=\"font-family:Verdana;font-size:24\" > PLane number %i </text> \n", pro.marginCm[0]+colInd*(pro.sizeCm[0]+pro.marginCm[0]), pro.marginCm[1]+rowInd*(pro.sizeCm[1]+pro.marginCm[1]), meshIndex );

}
		
};


		};  // namespace io
	};  // namespace edge
};  // namespace vcg
#endif  // __VCG_LIB_EXPORTER_SVG
