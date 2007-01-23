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
 Revision 1.7  2007/01/23 09:21:28  corsini
 add mean+erosion filter

 Revision 1.6  2007/01/11 11:48:04  cignoni
 Reordered include

 Revision 1.5  2006/12/06 21:25:00  cignoni
 small optimization and logging for profiling

 Revision 1.4  2006/11/30 11:40:33  cignoni
 Updated the calls to the hole filling functions to the new interface

 Revision 1.3  2006/11/29 00:59:16  cignoni
 Cleaned plugins interface; changed useless help class into a plain string

 Revision 1.2  2006/11/08 15:49:42  cignoni
 Added quality to the loaded masks

 Revision 1.1  2006/11/07 18:14:21  cignoni
 Moved from the epoch svn repository

 Revision 1.1  2006/01/20 13:03:27  cignoni
 *** empty log message ***

*****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>



#include <QMessageBox>
#include <QFileDialog>

#include "epoch_io.h"
#include "epoch_reconstruction.h"
#include <vcg/math/matrix33.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/create/platonic.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/clustering.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/hole.h>
#include <wrap/io_trimesh/io_mask.h>
#include "../cleanfilter/remove_small_cc.h"

FILE *logFP=0; 
using namespace vcg;

bool EpochModel::CombineMaskAndQuality(CharImage &qualityImg, QString maskName )
{
	QImage maskImg(maskName);

	if(maskImg.isNull()) 
		return false;

	if(maskImg.width()!= qualityImg.w) 
		return false;

	if(maskImg.height()!= qualityImg.h) 
		return false;

	for(int j=0;j<maskImg.height();++j)
		for(int i=0;i<maskImg.width();++i)
			if(qRed(maskImg.pixel(i,j))>128)
				qualityImg.Val(i,j)=0;

	return true;
}


void EpochModel::SmartSubSample(int factor, FloatImage &fli, CharImage &chi, FloatImage &subD, FloatImage &subQ, int minCount)
{
 assert(fli.w==chi.w && fli.h==chi.h);
 int w=fli.w/factor;
 int h=fli.h/factor;
 subQ.resize(w,h);
 subD.resize(w,h);

  for(int i=0;i<w;++i)
    for(int j=0;j<h;++j)
    {
      float maxcount=0;
      int cnt=0;
      float bestVal=0;
      for(int ki=0;ki<factor;++ki)
         for(int kj=0;kj<factor;++kj)
          {
            float q= chi.Val(i*factor+ki,j*factor+kj) - minCount+1 ;
            if(q>0)
            {
              maxcount+= q;
              bestVal +=q*fli.Val(i*factor+ki,j*factor+kj);
              cnt++;
            }
          }
      if(cnt>0)
      {
        subD.Val(i,j)=float(bestVal)/maxcount;
        subQ.Val(i,j)=minCount-1 + float(maxcount)/cnt  ;
      }
      else
      {
        subD.Val(i,j)=0;
        subQ.Val(i,j)=0;
      }
    }
}

void EpochModel::Laplacian(FloatImage &depth, CharImage &mask)
{
  FloatImage Sum;
  int w=depth.w,h=depth.h;
  Sum.resize(w,h);
  for(int x=1;x<w-1;++x)
    for(int y=1;y<h-1;++y)
      Sum.Val(x,y)=(depth.Val(x-1,y-1)+depth.Val(x  ,y-1)+depth.Val(x+1,y-1)+
                    depth.Val(x-1,y  )+depth.Val(x  ,y  )+depth.Val(x+1,y  )+
                    depth.Val(x-1,y+1)+depth.Val(x  ,y+1)+depth.Val(x+1,y+1))/9.0;

  for(int x=1;x<w-1;++x)
    for(int y=1;y<h-1;++y)
    {
      float q=(mask.Val(x,y)/255.0);
      depth.Val(x,y) =  depth.Val(x,y)*q + Sum.Val(x,y)*(1-q);
    }
}
/* 
This filter average apply a laplacian smoothing over a depth map averaging the samples with a weighting scheme that follows the Counting masks.
The result  of the laplacian is applied only on sample with low quality.
*/

void EpochModel::Laplacian2(FloatImage &depth, FloatImage &Q, int minCount, CharImage &mask)
{
  FloatImage Sum;
  int w=depth.w,h=depth.h;
  Sum.resize(w,h);
  
 for(int y=1;y<h-1;++y)
  for(int x=1;x<w-1;++x)
    {
      int cnt=0;
      for(int j=-1;j<=1;++j)
        for(int i=-1;i<=1;++i)
         {
           int q=Q.Val(x+i,y+j)-minCount+1;
           if(q>0) {
             Sum.Val(x,y)+=q*depth.Val(x+i,y+j);
             cnt+=q;
           }
         }
         if(cnt>0) {
           Sum.Val(x,y)/=cnt;
         }
      else Sum.Val(x,y)=depth.Val(x,y);
    }

 for(int y=1;y<h-1;++y)
  for(int x=1;x<w-1;++x)
    {
      float q=(mask.Val(x,y)/255.0);
      depth.Val(x,y) = depth.Val(x,y)*q + Sum.Val(x,y)*(1-q);
    }
}

void EpochModel::GenerateGradientSmoothingMask(int subsampleFactor, CharImage &mask)
{
	QImage base(textureName); 
	CharImage gray(base);
	CharImage grad;
	grad.resize(gray.w,gray.h);
	int w=gray.w,h=gray.h;
	for(int x=1;x<w-1;++x)
		for(int y=1;y<h-1;++y)
		{
			int dx=abs(int(gray.Val(x,y))-int(gray.Val(x-1,y))) + abs(int(gray.Val(x,y))-int(gray.Val(x+1,y)));
			int dy=abs(int(gray.Val(x,y))-int(gray.Val(x,y-1))) + abs(int(gray.Val(x,y))-int(gray.Val(x,y+1)));
			grad.Val(x,y)=min(255,16*dx+dy);
		}

	// create subsampled mask
	int ws=gray.w/subsampleFactor, hs=gray.h/subsampleFactor;
	mask.resize(ws,hs);

	for(int x=0;x<ws;++x)
		for(int y=0;y<hs;++y)
		{
			unsigned char sum=0;
			for(int si=0;si<subsampleFactor;++si)
				for(int sj=0;sj<subsampleFactor;++sj)
					sum = max(sum, grad.Val(x*subsampleFactor+sj,y*subsampleFactor+si));

			mask.Val(x,y) = min((unsigned char)255, sum);
		}

	CharImage mask2;
	mask2.resize(ws, hs);

	// average filter (11 x 11)
	int avg;
	int wsize = 5;
	for (int y = wsize; y < hs-wsize; y++)
		for (int x = wsize; x < ws-wsize; x++)
		{
			avg = 0;
			for (int yy = y - wsize; yy <= y + wsize; yy++)
				for (int xx = x - wsize; xx <= x + wsize; xx++)
					avg += mask.Val(xx, yy);

			mask2.Val(x, y) = min(255, avg / ((2 * wsize + 1)* (2 * wsize +1)));
		}

	// erosion filter (7 x 7)
	int minimum;
	wsize = 3;
	for (int y = wsize; y < hs-wsize; y++)
		for (int x = wsize; x < ws-wsize; x++)
		{
			minimum = mask2.Val(x, y);
			for (int yy = y - wsize; yy <= y + wsize; yy++)
				for (int xx = x - wsize; xx <= x + wsize; xx++)
					if (mask2.Val(xx, yy) < minimum)
						minimum = mask2.Val(xx, yy);

			mask.Val(x, y) = minimum;
		}
  
	grad.convertToQImage().save("C:/temp/test.jpg","jpg");
	mask.convertToQImage().save("C:/temp/testmask.jpg","jpg");
}

/*
Main processing function;

it takes a depth map, a count map, 
- resample them to a (width/subsample,height/subsample) image
- leave only the faces that are within a given orientation range
- that have a count greater than minCount.
- and smooth them with a count/quality aware laplacian filter
*/ 

bool EpochModel::BuildMesh(CMeshO &m, int subsample, int minCount, float minAngleCos, int smoothSteps)
{
  FloatImage fli;
  CharImage chi;
  int ttt0=clock();
  fli.Open(depthName.toAscii());
  chi.Open(countName.toAscii());
  
  QImage tx;
  tx.load(textureName);
  int ttt1=clock();
  if(logFP) fprintf(logFP,"**** Buildmesh: Opening files %i\n",ttt1-ttt0);

  CombineMaskAndQuality(chi,maskName);
  
  FloatImage flisub;  // the subsampled depth image 
  FloatImage flisubQ; // the subsampled quality image (quality == count)
  CharImage QualityMask; // the subsampled image with (quality == features)

  SmartSubSample(subsample,fli,chi,flisub,flisubQ,minCount);
  GenerateGradientSmoothingMask(subsample,QualityMask);

  flisub.convertToQImage().save("C:/temp/depth.jpg", "jpg");

  int ttt2=clock();
  if(logFP) fprintf(logFP,"**** Buildmesh: SubSample and Gradient %i\n",ttt2-ttt1);

  for(int ii=0;ii<smoothSteps;++ii) 
    // Laplacian(flisub,QualityMask);
    Laplacian2(flisub,flisubQ,minCount,QualityMask);

  int ttt3=clock();
  if(logFP) fprintf(logFP,"**** Buildmesh: Smoothing %i\n",ttt3-ttt2);

  vcg::tri::Grid<CMeshO>(m,flisub.w,flisub.h,fli.w,fli.h,&*flisub.v.begin());

  int ttt4=clock();
  if(logFP) fprintf(logFP,"**** Buildmesh: trimesh building %i\n",ttt4-ttt3);

  for(int i=0;i<m.vn;++i)
    if(flisubQ.v[i]<minCount) 
    {
      m.vert[i].SetD();
      m.vn--;
    }

  cam.Open(cameraName.toAscii());
  CMeshO::VertexIterator vi;
  Matrix33d Rinv= Inverse(cam.R);

  for(vi=m.vert.begin();vi!=m.vert.end();++vi)if(!(*vi).IsD())
  {
    Point3f in=(*vi).P();
    Point3d out;
    cam.DepthTo3DPoint(in[0], in[1], in[2], out);
    
    (*vi).P().Import(out);
    QRgb c = tx.pixel(int(in[0]), int(in[1]));
    (*vi).C().SetRGB(qRed(c),qGreen(c),qBlue(c));
    //(*vi).Q()=chi.Val(in[0], in[1]);
    //(*vi).Q()=flisubQ.Val(in[0]/subsample, in[1]/subsample);
    if(QualityMask.Val(int(in[0]/subsample), int(in[1]/subsample))<200) (*vi).Q()=0; 
    else (*vi).Q()=1; 
    (*vi).Q()=float(QualityMask.Val(in[0]/subsample, in[1]/subsample))/255.0;
  }
    
  int ttt5=clock();
  if(logFP) fprintf(logFP,"**** Buildmesh: Projecting and Coloring %i\n",ttt5-ttt4);

  CMeshO::FaceIterator fi;
  Point3f CameraPos=Point3f::Construct(cam.t);
   for(fi=m.face.begin();fi!=m.face.end();++fi)
   {

     if((*fi).V(0)->IsD() ||(*fi).V(1)->IsD() ||(*fi).V(2)->IsD() ) 
       {
        (*fi).SetD();
        --m.fn;
       }
     else
     {
       Point3f n=vcg::Normal(*fi);
       n.Normalize();
       Point3f dir=CameraPos-vcg::Barycenter(*fi);
       dir.Normalize();
       if(dir*n < minAngleCos)
                {
                  (*fi).SetD();
                  --m.fn;
                }
     }
   }

  int ttt6=clock();
  if(logFP) fprintf(logFP,"**** Buildmesh: Deleting skewed %i\n",ttt6-ttt5);

//  Matrix44d Rot;
//  Rot.SetRotate(M_PI,Point3d(1,0,0));
//  vcg::tri::UpdatePosition<CMeshO>::Matrix(m, Rot);

    return true;
}
void EpochModel::AddCameraIcon(CMeshO &m)
{
    tri::Allocator<CMeshO>::AddVertices(m,3);
    m.vert[m.vert.size()-3].P()=Point3f::Construct(cam.t+Point3d(0,0,0));
    m.vert[m.vert.size()-3].C()=Color4b::Green;
    m.vert[m.vert.size()-2].P()=Point3f::Construct(cam.t+Point3d(0,1,0));
    m.vert[m.vert.size()-2].C()=Color4b::Green;
    m.vert[m.vert.size()-1].P()=Point3f::Construct(cam.t+Point3d(1,0,0));
    m.vert[m.vert.size()-1].C()=Color4b::Green;

    tri::Allocator<CMeshO>::AddFaces(m,1);
    m.face[m.face.size()-1].V(0)= &m.vert[m.vert.size()-3];
    m.face[m.face.size()-1].V(1)= &m.vert[m.vert.size()-2];
    m.face[m.face.size()-1].V(2)= &m.vert[m.vert.size()-1];
  }






bool EpochModel::Init(QDomNode &node)
{
 if(!node.hasAttributes()) return false;
 QDomNamedNodeMap attr= node.attributes();
 QString indexString = (attr.namedItem("index")).nodeValue() ;
 qDebug("reading Model with index %i ",indexString.toInt());
        for(QDomNode n = node.firstChild(); !n.isNull(); n = n.nextSibling())
              {
                if(n.nodeName() == QString("camera"))  cameraName = n.attributes().namedItem("filename").nodeValue();
                if(n.nodeName() == QString("texture")) textureName= n.attributes().namedItem("filename").nodeValue();
                if(n.nodeName() == QString("depth"))   depthName  = n.attributes().namedItem("filename").nodeValue();
                if(n.nodeName() == QString("count"))   countName  = n.attributes().namedItem("filename").nodeValue();
            }

  QString tmpName=textureName.left(textureName.length()-4);
  maskName = tmpName.append(".mask.png");
  return true;
}

QString EpochModel::ThumbName(QString &imageName)
{
  QString tmpName=imageName.left(imageName.length()-4);
  return tmpName.append(".thumb.jpg");
}

EpochIO::EpochIO()
  {
    epochDialog = new v3dImportDialog();
	  epochDialog->hide();
  }

  EpochIO::~EpochIO()
  {
    delete epochDialog;
  }

bool EpochIO::open(const QString &formatName, QString &fileName, MeshModel &m, int& mask, CallBackPos *cb, QWidget *parent)
{
  EpochReconstruction er;
	
  mask = MeshModel::IOM_VERTCOLOR | MeshModel::IOM_VERTQUALITY; 
// just to be sure...
	
	if (fileName.isEmpty()) return false;
		// initializing progress bar status
	if (cb != NULL) (*cb)(0, "Loading...");

	// this change of dir is needed for subsequent texture/material loading
	QString FileNameDir = fileName.left(fileName.lastIndexOf("/")); 
	QDir::setCurrent(FileNameDir);

	QString errorMsgFormat = "Error encountered while loading file %1:\n%2";
	string filename = fileName.toUtf8().data();

	QDomDocument doc;
	
	if(formatName.toUpper() == tr("V3D") &&  fileName.endsWith(".v3d"))
	{
  		QFile file(fileName);
			if (file.open(QIODevice::ReadOnly) && doc.setContent(&file)) 
        {
					file.close();
					QDomElement root = doc.documentElement();
					if (root.nodeName() == tr("reconstruction")) 
          {
            QDomNode nhead = root.firstChildElement("head");
            for(QDomNode n = nhead.firstChildElement("meta"); !n.isNull(); n = n.nextSiblingElement("meta"))
              {
               if(!n.hasAttributes()) return false;
               QDomNamedNodeMap attr= n.attributes();
               if(attr.contains("name")) er.name = (attr.namedItem("name")).nodeValue() ;
               if(attr.contains("author")) er.author = (attr.namedItem("author")).nodeValue() ;
               if(attr.contains("created")) er.created = (attr.namedItem("created")).nodeValue() ;
              }    
             for(QDomNode n = root.firstChildElement("model"); !n.isNull(); n = n.nextSiblingElement("model"))
              {
                EpochModel em;
                em.Init(n);
                er.modelList.push_back(em);
              }
          }
        }
    }

	epochDialog->setEpochReconstruction( &er, cb);
  
  //Here we invoke the modal dialog and wait for its termination
  int continueValue = epochDialog->exec();

  // 
  int t0=clock();
  logFP=fopen("epoch.log","w");

  int subSampleVal = epochDialog->subsampleSpinBox->value();
  int minCountVal= epochDialog->minCountSpinBox->value();
  float maxCCDiagVal= epochDialog->maxCCDiagSpinBox->value();
  int mergeResolution=epochDialog->mergeResolutionSpinBox->value();
  int smoothSteps=epochDialog->smoothSpinBox->value();
  bool closeHole = epochDialog->holeCheckBox->isChecked();
  int maxHoleSize = epochDialog->holeSpinBox->value();
	if (continueValue == QDialog::Rejected)
  {
	    QMessageBox::warning(parent, "Open V3d format","Aborted");    
      return false;
  }
  CMeshO mm;
  QTableWidget *qtw=epochDialog->imageTableWidget;
  float MinAngleCos=cos(vcg::math::ToRad(epochDialog->qualitySpinBox->value()));
  bool clustering=epochDialog->fastMergeCheckBox->isChecked();
  bool removeSmallCC=epochDialog->removeSmallCCCheckBox->isChecked();
  vcg::tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > Grid;

  int selectedNum=0,selectedCount=0;
 	int i;
   for(i=0;i<qtw->rowCount();++i) if(qtw->isItemSelected(qtw->item(i,0))) ++selectedNum;
  if(selectedNum==0)
    {
	    QMessageBox::warning(parent, "Open V3d format","No range map selected. Nothing loaded");    
      return false;
  }


  bool firstTime=true;
  QList<EpochModel>::iterator li;
  for(li=er.modelList.begin(), i=0;li!=er.modelList.end();++li,++i)
  {
      if(qtw->isItemSelected(qtw->item(i,0)))
      {
        ++selectedCount;
        mm.Clear();
        int tt0=clock();
        (*li).BuildMesh(mm,subSampleVal,minCountVal,MinAngleCos,smoothSteps);
        int tt1=clock();
        if(logFP) fprintf(logFP,"** Mesh %i : Build in %i\n",selectedCount,tt1-tt0);

        if(clustering)
        {
          if (firstTime) 
            {
              //Grid.Init(mm.bbox,100000);
              vcg::tri::UpdateBounding<CMeshO>::Box(mm);	
              //Grid.Init(mm.bbox,1000.0*pow(10.0,mergeResolution),mm.bbox.Diag()/1000.0f);
              Grid.Init(mm.bbox,100000.0*pow(10.0,mergeResolution));
              firstTime=false;
            }
          Grid.Add(mm);
        }
        else  tri::Append<CMeshO,CMeshO>::Mesh(m.cm,mm); // append mesh mr to ml

        int tt2=clock();
        if(logFP) fprintf(logFP,"** Mesh %i : Append in %i\n",selectedCount,tt2-tt1);

      }
      if (cb)(*cb)(selectedCount*90/selectedNum, "Building meshes");
  }
   
  if (cb != NULL) (*cb)(90, "Final Processing: clustering");
  if(clustering)  
  {
    Grid.Extract(m.cm);
  }
 
  int t1=clock();
  if(logFP) fprintf(logFP,"Extracted %i meshes in %i\n",selectedCount,t1-t0);

  if (cb != NULL) (*cb)(95, "Final Processing: Removing Small Connected Components");
  if(removeSmallCC)
  {
    vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
	  m.updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_FACEMARK);
    RemoveSmallConnectedComponentsDiameter<CMeshO>(m.cm,m.cm.bbox.Diag()*maxCCDiagVal/100.0);
  }

  int t2=clock();
  if(logFP) fprintf(logFP,"Topology and removed CC in %i\n",t2-t1);

  vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
	
  if (cb != NULL) (*cb)(97, "Final Processing: Closing Holes");
  if(closeHole)
  {
	  m.updateDataMask(MeshModel::MM_FACETOPO | MeshModel::MM_BORDERFLAG | MeshModel::MM_FACEMARK);
    tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m.cm);	    
    vcg::tri::Hole<CMeshO>::EarCuttingFill<vcg::tri::MinimumWeightEar< CMeshO> >(m.cm,maxHoleSize,false);
  }

	if (cb != NULL) (*cb)(100, "Done");
//  vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals


  int t3=clock();
  if(logFP) fprintf(logFP,"---------- Total Processing Time%i\n\n\n",t3-t0);
  if(logFP) fclose(logFP);
  logFP=0;

	return true;
}


bool EpochIO::save(const QString &/*formatName*/,QString &/*fileName*/, MeshModel &/*m*/, const int &/*mask*/, vcg::CallBackPos * /*cb*/, QWidget *parent)
{
	QMessageBox::warning(parent, "Unknow type", "file's extension not supported!!!");
	return false;
}

QList<MeshIOInterface::Format> EpochIO::importFormats() const
{
	QList<Format> formatList;
  formatList << Format("Epoch Reconstructed mesh","V3D");
	return formatList;
};

const PluginInfo &EpochIO::Info()
{
	static PluginInfo ai;
	ai.Date=tr("March 2006");
	ai.Version = tr("0.1");
	ai.Author = ("Paolo Cignoni");
	return ai;
 }

QIcon *EpochModel::getIcon()
{
  QString iconName(textureName);
  iconName+=QString(".xbm");
  QIcon *ico=new QIcon();


  return ico;
} 

Q_EXPORT_PLUGIN(EpochIO)
  
