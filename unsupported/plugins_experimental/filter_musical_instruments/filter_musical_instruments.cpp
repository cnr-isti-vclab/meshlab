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

#include "filter_musical_instruments.h"
#include <vcg/complex/algorithms/create/platonic.h>

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterMusicalInstruments::FilterMusicalInstruments()
{
	typeList <<CR_HELMHOLTZ;

  foreach(FilterIDType tt , types())
	  actionList << new QAction(filterName(tt), this);
}

QString FilterMusicalInstruments::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case CR_HELMHOLTZ : return QString("Helmholtz Resonator");
		default : assert(0);
	}
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
 QString FilterMusicalInstruments::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
    case CR_HELMHOLTZ : return QString("Create a Helmholtz Resonator");
		default : assert(0);
	}
}

// This function define the needed parameters for each filter. Return true if the filter has some parameters
// it is called every time, so you can set the default value of parameters according to the mesh
// For each parmeter you need to define,
// - the name of the parameter,
// - the string shown in the dialog
// - the default value
// - a possibly long string describing the meaning of that parameter (shown as a popup help in the dialog)
void FilterMusicalInstruments::initParameterSet(QAction *action, MeshModel & /*m*/, RichParameterSet & parlst)
{
	 switch(ID(action))	 {

     case CR_HELMHOLTZ :
       parlst.addParam(new RichFloat("hole_radius",3,"Hole Radius","Radius of the blowing hole"));
       parlst.addParam(new RichFloat("neck_length",15,"Neck Lenght","Lenght of the neck"));
       parlst.addParam(new RichFloat("sphere_radius",10,"Sphere Radius","Radius of the sphere"));
       parlst.addParam(new RichFloat("wall_thickness",3,"Thickness","wall thickness"));
       parlst.addParam(new RichInt("subdivide",32,"# of slices","how many slices are in the sphere and neck"));
       break;
		default : return;
	}
}

// The Real Core Function doing the actual mesh processing.
bool FilterMusicalInstruments::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
  QString newmeshname;

  switch(ID(filter))	 
  {
    case CR_HELMHOLTZ:
    {
      float hole_radius = par.getFloat("hole_radius");
		  float neck_length = par.getFloat("neck_length");
		  float sphere_radius = par.getFloat("sphere_radius");
		  float wall_thickness = par.getFloat("wall_thickness");

      // subdivision param
      int sub = par.getInt("subdivide");

      //check measure consistency
      if(hole_radius > sphere_radius)
        return false;

      // creating mesh
      newmeshname = QString("Helmholtz_rh%1_nl%2_sr%3").arg(hole_radius).arg(neck_length).arg(sphere_radius);
      MeshModel* m=md.addNewMesh("",newmeshname);
      m->setFileName(newmeshname+".ply");

		  m->cm.face.EnableFFAdjacency();
		  m->updateDataMask(MeshModel::MM_FACEFACETOPO);

      // calculating top of the inner sphere, where neck join in
      // the distance is from the sphere center

      float neck_join = sqrt(sphere_radius*sphere_radius - hole_radius*hole_radius);
      float neck_join_angle = asin(hole_radius/sphere_radius); 
      float neck_join2 = sqrt((sphere_radius + wall_thickness)*(sphere_radius + wall_thickness) - (hole_radius + wall_thickness)*(hole_radius + wall_thickness));
      float neck_join_angle2 = asin((hole_radius + wall_thickness)/(sphere_radius + wall_thickness));

      vcg::tri::Allocator<CMeshO>::AddVertices(m->cm, sub*(4*((sub/2.0))) + 2);
      CMeshO::VertexIterator vi = m->cm.vert.begin();

      int Vstep=0; int Rstep=0;

      for(Rstep = 0; Rstep<sub; Rstep++)
      {
        // inner cavity
        for(Vstep = 1; Vstep<=(sub/2.0); Vstep++)
        {
         float Vangle = (3.1415 - neck_join_angle) / float(sub/2.0);
         float Rangle = (2.0 * 3.1415) / float(sub);
         float px = sphere_radius * cos(Rangle * float(Rstep)) * sin(Vangle * float(Vstep));
         float py = sphere_radius * cos(3.1415 - (Vangle * float(Vstep)));
         float pz = sphere_radius * sin(Rangle * float(Rstep)) * sin(Vangle * float(Vstep));

         (*vi).P()= vcg::Point3f(px, py, pz);
         vi++;
        }

        // inner neck
        for(Vstep = 1; Vstep<=(sub/2.0); Vstep++)
        {
         float Vangle = (3.1415 - neck_join_angle) / float(sub/2.0);
         float Rangle = (2.0 * 3.1415) / float(sub);
         float neck_inc = neck_length / float(sub/2.0);

         float px = hole_radius * cos(Rangle * float(Rstep));
         float py = neck_join + (neck_inc * float(Vstep));
         float pz = hole_radius * sin(Rangle * float(Rstep));

         (*vi).P()= vcg::Point3f(px, py, pz);
         vi++;
        }

        // outer neck
        for(Vstep = (sub/2.0); Vstep>=1; Vstep--)
        {
         float Vangle = (3.1415 - neck_join_angle) / float(sub/2.0);
         float Rangle = (2.0 * 3.1415) / float(sub);
         float neck_inc = (neck_length-(neck_join2 - neck_join)) / float(sub/2.0);

         float px = (hole_radius + wall_thickness) * cos(Rangle * float(Rstep));
         float py = neck_join2 + (neck_inc * float(Vstep));
         float pz = (hole_radius + wall_thickness) * sin(Rangle * float(Rstep));

         (*vi).P()= vcg::Point3f(px, py, pz);
         vi++;
        }

        // outer cavity
        for(Vstep = (sub/2.0); Vstep>=1; Vstep--)
        {
         float Vangle = (3.1415 - neck_join_angle2) / float(sub/2.0);
         float Rangle = (2.0 * 3.1415) / float(sub);
         float px = (sphere_radius + wall_thickness) * cos(Rangle * float(Rstep)) * sin(Vangle * float(Vstep));
         float py = (sphere_radius + wall_thickness) * cos(3.1415 - (Vangle * float(Vstep)));
         float pz = (sphere_radius + wall_thickness) * sin(Rangle * float(Rstep)) * sin(Vangle * float(Vstep));

         (*vi).P()= vcg::Point3f(px, py, pz);
         vi++;
        }
      }

      // inner and outer bottom 
      (*vi).P()= vcg::Point3f(0.0, -sphere_radius, 0.0);
      CMeshO::VertexPointer inner_cap_vert = &*vi;
      vi++;
      (*vi).P()= vcg::Point3f(0.0, -(sphere_radius+wall_thickness), 0.0);
      CMeshO::VertexPointer outer_cap_vert = &*vi;
      vi++;


      int slicelen = (4*((sub/2.0)));

      vcg::tri::Allocator<CMeshO>::AddFaces(m->cm, (2*(sub)*(slicelen-1)) + (2*sub));
      CMeshO::FaceIterator fi = m->cm.face.begin();


      Vstep=0; Rstep=0;

      for(Rstep = 0; Rstep<sub-1; Rstep++)
      {
        for(Vstep = 0; Vstep<slicelen-1; Vstep++)
        {
          (*fi).V(0)=&m->cm.vert[(Rstep*slicelen)+Vstep]; 
          (*fi).V(1)=&m->cm.vert[(Rstep*slicelen)+1+Vstep+(slicelen)];
          (*fi).V(2)=&m->cm.vert[(Rstep*slicelen)+1+Vstep];
          ++fi;

          (*fi).V(0)=&m->cm.vert[(Rstep*slicelen)+Vstep]; 
          (*fi).V(1)=&m->cm.vert[(Rstep*slicelen)+Vstep+(slicelen)];
          (*fi).V(2)=&m->cm.vert[(Rstep*slicelen)+1+Vstep+(slicelen)];
          ++fi;
        }
      }
      for(Vstep = 0; Vstep<slicelen-1; Vstep++)
      {
        (*fi).V(0)=&m->cm.vert[(Rstep*slicelen)+Vstep]; 
        (*fi).V(1)=&m->cm.vert[1+Vstep];
        (*fi).V(2)=&m->cm.vert[(Rstep*slicelen)+1+Vstep];
        ++fi;

        (*fi).V(0)=&m->cm.vert[(Rstep*slicelen)+Vstep]; 
        (*fi).V(1)=&m->cm.vert[Vstep];
        (*fi).V(2)=&m->cm.vert[1+Vstep];
        ++fi;
      }
      // bottom caps
      for(Rstep = 0; Rstep<sub; Rstep++)
      {
        (*fi).V(0) = inner_cap_vert; 
        (*fi).V(1) = &m->cm.vert[Rstep!=(sub-1)?((Rstep*slicelen)+slicelen):0];
        (*fi).V(2) = &m->cm.vert[(Rstep*slicelen)];
        ++fi;
      }  
      for(Rstep = 0; Rstep<sub; Rstep++)
      {
        (*fi).V(0) = outer_cap_vert; 
        (*fi).V(1) = &m->cm.vert[(Rstep*slicelen)+(slicelen-1)];
        (*fi).V(2) = &m->cm.vert[Rstep!=(sub-1)?(Rstep*slicelen)+slicelen+(slicelen-1):(slicelen-1)];
        ++fi;
      }

      vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);
      vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m->cm);

		  break;
    }
  }


	return true;
}

 MeshFilterInterface::FilterClass FilterMusicalInstruments::getClass(QAction *a)
{
  switch(ID(a))
  {

    case CR_HELMHOLTZ:
      return MeshFilterInterface::MeshCreation;
      break;
    default: assert(0);
      return MeshFilterInterface::Generic;
  }
}

QString FilterMusicalInstruments::filterScriptFunctionName( FilterIDType filterID )
 {
	switch(filterID) 
	{
		case CR_HELMHOLTZ : return QString("Helmholtz Resonator");
		default : assert(0);
	}
 }


Q_EXPORT_PLUGIN(FilterMusicalInstruments)
