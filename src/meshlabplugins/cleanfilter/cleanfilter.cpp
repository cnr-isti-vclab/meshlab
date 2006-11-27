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

 $Log$
 Revision 1.4  2006/11/27 06:57:19  cignoni
 Wrong way of using the __DATE__ preprocessor symbol

 Revision 1.3  2006/11/07 17:26:01  cignoni
 small gcc compiling issues

 Revision 1.2  2006/11/07 14:56:23  zifnab1974
 Changes for compilation with gcc 3.4.6 on linux AMD64

 Revision 1.1  2006/11/07 09:09:27  cignoni
 First Working release, moved in from epoch svn

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

*****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>


// temporaneamente prendo la versione corrente dalla cartella test
#include<vcg/complex/trimesh/update/bounding.h>
#include <wrap/io_trimesh/io_mask.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/clean.h>
#include <vcg/math/matrix33.h>
#include <vcg/complex/trimesh/create/ball_pivoting.h>

#include <QMessageBox>
#include <QFileDialog>

#include "cleanfilter.h"
#include "remove_small_cc.h"
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/space/normal_extrapolation.h>

/////////////////////// Accessory functions

bool askFloat(float &val, QString Title, QString Label, QWidget *parent)
{
  QDialog *dialog=new QDialog(parent);
  dialog->setModal(true);
  dialog->setWindowTitle(Title);

  QPushButton *okButton = new QPushButton("OK", dialog);
  QPushButton *cancButton = new QPushButton("cancel", dialog);
  QGridLayout *gridLayout  = new QGridLayout(dialog);
  dialog->setLayout(gridLayout);

  gridLayout->addWidget(okButton,1,1);
  gridLayout->addWidget(cancButton,1,0);

  QLineEdit *floatEdit = new QLineEdit(dialog);
  floatEdit->setText(QString::number(val));
  QLabel *label = new QLabel(Label,dialog);
  gridLayout->addWidget(label,0,0 );
  gridLayout->addWidget(floatEdit,0,1);
  
  QObject::connect(floatEdit, SIGNAL(returnPressed ()),  dialog, SLOT(accept()));  
  QObject::connect(okButton, SIGNAL(clicked()),  dialog, SLOT(accept()));  
  QObject::connect(cancButton, SIGNAL(clicked()), dialog, SLOT(reject()));  
 
  dialog->exec();
  if(dialog->result()== QDialog::Accepted )
  {
    val=floatEdit->text().toFloat();
    return true;
  }
  if(dialog->result()== QDialog::Rejected ) return false;
  assert(0); 
  return true;
}


bool askInt(int &val, QString Title, QString Label, QWidget *parent)
{
  QDialog *dialog=new QDialog(parent);
  dialog->setModal(true);
  dialog->setWindowTitle(Title);

  QPushButton *okButton = new QPushButton("OK", dialog);
  QPushButton *cancButton = new QPushButton("cancel", dialog);
  QGridLayout *gridLayout  = new QGridLayout(dialog);
  dialog->setLayout(gridLayout);

  gridLayout->addWidget(okButton,1,1);
  gridLayout->addWidget(cancButton,1,0);

  QLineEdit *floatEdit = new QLineEdit(dialog);
  floatEdit->setText(QString::number(val));
  QLabel *label = new QLabel(Label,dialog);
  gridLayout->addWidget(label,0,0 );
  gridLayout->addWidget(floatEdit,0,1);
  
  QObject::connect(floatEdit, SIGNAL(returnPressed ()),  dialog, SLOT(accept()));  
  QObject::connect(okButton, SIGNAL(clicked()),  dialog, SLOT(accept()));  
  QObject::connect(cancButton, SIGNAL(clicked()), dialog, SLOT(reject()));  
 
  dialog->exec();
  if(dialog->result()== QDialog::Accepted )
  {
    val=floatEdit->text().toInt();
    return true;
  }
  if(dialog->result()== QDialog::Rejected ) return false;
  assert(0); 
  return true;
}

/////////////////////////////////////////////////////////////////////


using namespace vcg;

CleanFilter::CleanFilter() 
{
  typeList << FP_REBUILD_SURFACE << FP_REMOVE_WRT_Q << FP_REMOVE_ISOLATED_COMPLEXITY << FP_REMOVE_ISOLATED_DIAMETER;
 
  FilterType tt;
  foreach(tt , types())
	    actionList << new QAction(ST(tt), this);
}

CleanFilter::~CleanFilter() {
	for (int i = 0; i < actionList.count() ; i++ ) 
		delete actionList.at(i);
}

const QString CleanFilter::ST(FilterType filter) 
{
 switch(filter)
  {
	  case FP_REBUILD_SURFACE :		return QString("Build surface from points");
	  case FP_REMOVE_WRT_Q :		return QString("Remove Faces wrt quality");
	  case FP_REMOVE_ISOLATED_DIAMETER   :		return QString("Remove isolated pieces (wrt diameter)");
	  case FP_REMOVE_ISOLATED_COMPLEXITY :		return QString("Remove isolated pieces (wrt face num)");
  	default: assert(0);
  }
  return QString("error!");
}

const CleanFilter::FilterClass CleanFilter::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_REMOVE_WRT_Q :
    case FP_REMOVE_ISOLATED_DIAMETER :
    case FP_REMOVE_ISOLATED_COMPLEXITY :
      return MeshFilterInterface::Cleaning;     
    default : return MeshFilterInterface::Generic;
  }
}

const ActionInfo &CleanFilter::Info(QAction *action)
{
	static ActionInfo ai;
  switch(ID(action))
  {
  case FP_REBUILD_SURFACE :	 ai.Help = tr("Merge"); break;
	case FP_REMOVE_ISOLATED_COMPLEXITY:	 ai.Help = tr("Remove Isolated"); break;
	case FP_REMOVE_ISOLATED_DIAMETER:	 ai.Help = tr("Remove Isolated"); break;
	case FP_REMOVE_WRT_Q:	     ai.Help = tr("Remove all the faces with quality lower than..."); break;
  }
  return ai;
}

const PluginInfo &CleanFilter::Info()
{
   static PluginInfo ai;
   ai.Date=tr( __DATE__ );
	 ai.Version = tr("0.1");
	 ai.Author = ("Paolo Cignoni");
   return ai;
}

const int CleanFilter::getRequirements(QAction *action)
{
  switch(ID(action))
  {
    case FP_REMOVE_WRT_Q:
    case FP_REBUILD_SURFACE :	return 0;
	  case FP_REMOVE_ISOLATED_COMPLEXITY:
    case FP_REMOVE_ISOLATED_DIAMETER:
        return MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_FACEMARK;
    default: assert(0);
  }
  return 0;
}

bool CleanFilter::getParameters(QAction *action, QWidget *parent, MeshModel &m,FilterParameter &par)
{
 par.clear();
 switch(ID(action))
  {
    case FP_REBUILD_SURFACE :	
      {
        static float maxDiag=0;
        if(askFloat(maxDiag, "Ballpivoting Surface reconstruction",
          "Enter ball size as a diag perc. (0 autoguess))",parent))
        {
          par.addFloat("BallRadius",m.cm.bbox.Diag()*maxDiag/100.0);
          return true;
        } 
        else return false;
      }
      return true;
	  case FP_REMOVE_ISOLATED_DIAMETER:	 
      {
        static float maxDiag=10;
        if(askFloat(maxDiag, "Remove Small Connected Components under a given size","Enter size (as a diag perc 0..100)",parent))
        {
          par.addFloat("MinComponentDiag",m.cm.bbox.Diag()*maxDiag/100.0);
          return true;
        } 
        else return false;
      }
	  case FP_REMOVE_ISOLATED_COMPLEXITY:	 
      {
        static int minCC=25;
        if(askInt(minCC, "Remove Small Connected Components","Enter minimum conn. comp size:",parent))
        {
          par.addInt("MinComponentSize",minCC);
          return true;
        } 
        else return false;
      }
    case FP_REMOVE_WRT_Q:
      {
        static float val=1.0;
        if(askFloat(val, "Quality Filter","Delete all Vertices with quality under:",parent))
        {
          par.addFloat("MaxQualityThr",val);
          return true;
        } 
        else return false; 
      }
  }
}


bool CleanFilter::applyFilter(QAction *filter, MeshModel &m, FilterParameter & par, vcg::CallBackPos * cb) 
{
	if(filter->text() == ST(FP_REBUILD_SURFACE) )
	  {
      float radius = par.getFloat("BallRadius");		
      float clustering = 0.1;
      float crease=0;
      m.cm.fn=0;
      m.cm.face.resize(0);
      NormalExtrapolation<vector<CVertexO> >::ExtrapolateNormals(m.cm.vert.begin(), m.cm.vert.end(), 10,-1,NormalExtrapolation<vector<CVertexO> >::IsCorrect,  cb);
      tri::Pivot<CMeshO> pivot(m.cm, radius, clustering, crease); 
      // the main processing
      pivot.buildMesh(cb);
	  }
    if(filter->text() == ST(FP_REMOVE_ISOLATED_DIAMETER) )
	  {
      float minCC= par.getFloat("MinComponentDiag");		
      RemoveSmallConnectedComponentsDiameter<CMeshO>(m.cm,minCC);
    }  	

    if(filter->text() == ST(FP_REMOVE_ISOLATED_COMPLEXITY) )
	  {
      float minCC= par.getInt("MinComponentSize");		
      RemoveSmallConnectedComponentsSize<CMeshO>(m.cm,minCC);
	  }
	if(filter->text() == ST(FP_REMOVE_WRT_Q) )
	  {
      float val=par.getFloat("MaxQualityThr");		
      CMeshO::VertexIterator vi;
      for(vi=m.cm.vert.begin();vi!=m.cm.vert.end();++vi)
        if(!(*vi).IsD() && (*vi).Q()<val)
        {
          (*vi).SetD();
          m.cm.vn--;
        } 
      CMeshO::FaceIterator fi;
      for(fi=m.cm.face.begin();fi!=m.cm.face.end();++fi) if(!(*fi).IsD())
               if((*fi).V(0)->IsD() ||(*fi).V(1)->IsD() ||(*fi).V(2)->IsD() ) 
       {
        (*fi).SetD();
        --m.cm.fn;
       }

	  }
	return true;
}


Q_EXPORT_PLUGIN(CleanFilter)
