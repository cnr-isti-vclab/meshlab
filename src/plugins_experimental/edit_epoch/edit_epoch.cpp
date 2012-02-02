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
$Log: meshedit.cpp,v $
****************************************************************************/
#include <Qt>
#include <QtGui>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNode>

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>

#include "edit_epoch.h"
#include <meshlab/stdpardialog.h>
#include <vcg/complex/append.h>
#include <vcg/math/histogram.h>
#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/clustering.h>
#include <vcg/complex/algorithms/hole.h>
#include <wrap/io_trimesh/export_ply.h>
#include <meshlab/alnParser.h>

using namespace std;
using namespace vcg;
//FILE *logFP=0; 
EpochReconstruction er;

EditEpochPlugin::EditEpochPlugin() {
  epochDialog = 0;
	
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(10); 
	
	
}

const QString EditEpochPlugin::Info() 
{
	return tr("This edit can be used to extract 3D models from Arc3D results");
}

bool EditEpochPlugin::StartEdit(MeshDocument &_md, GLArea *_gla )
{
	this->md=&_md;
	gla=_gla;
	///////
	epochDialog=new v3dImportDialog(gla->window(),this);
		
	QString fileName=epochDialog->fileName;

 	if (fileName.isEmpty()) return false;
		
	// this change of dir is needed for subsequent texture/material loading
	QString FileNameDir = fileName.left(fileName.lastIndexOf("/")); 
	QDir::setCurrent(FileNameDir);

	QString errorMsgFormat = "Error encountered while loading file %1:\n%2";
	string stdfilename = QFile::encodeName(fileName).constData ();
 
	QDomDocument doc;
	
	
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
    

	epochDialog->setEpochReconstruction( &er);

	epochDialog->exportToPLY=false;


	connect(epochDialog, SIGNAL(closing()),gla,SLOT(endEdit()) );
	connect(epochDialog->ui.plyButton, SIGNAL(clicked()),this,SLOT(ExportPly()) );
	connect(this,SIGNAL(resetTrackBall()),gla,SLOT(resetTrackBall()));

	return true;
}

void EditEpochPlugin::EndEdit(MeshModel &/*m*/, GLArea * /*parent*/)
{
	gla->update();
	assert(epochDialog);
    delete epochDialog;
    epochDialog=0;

}  
/* 
This is the main function, which generates the final mesh (and the rasters) based on the selection provided by the user
*/

void EditEpochPlugin::ExportPly()
{
	md->setBusy(true);
	md->addNewMesh("",er.name,true);
	MeshModel* m=md->mm();
	
	// Options collection
						
	int t0=clock();
	
	int subSampleVal = epochDialog->ui.subsampleSpinBox->value();
	int minCountVal= epochDialog->ui.minCountSpinBox->value();
	float maxCCDiagVal= epochDialog->ui.maxCCDiagSpinBox->value();
	int smoothSteps=epochDialog->ui.smoothSpinBox->value();
	bool closeHole = epochDialog->ui.holeCheckBox->isChecked();
	int maxHoleSize = epochDialog->ui.holeSpinBox->value();
	
	CMeshO mm;
	QTableWidget *qtw=epochDialog->ui.imageTableWidget;
	float MinAngleCos=cos(vcg::math::ToRad(epochDialog->ui.qualitySpinBox->value()));
	bool removeSmallCC=epochDialog->ui.removeSmallCCCheckBox->isChecked();
	//vcg::tri::Clustering<CMeshO, vcg::tri::AverageColorCell<CMeshO> > Grid;

	int selectedNum=0,selectedCount=0;
	int i;
	 for(i=0;i<qtw->rowCount();++i) if(qtw->isItemSelected(qtw->item(i,0))) ++selectedNum;
	
	bool dilationFlag = epochDialog->ui.dilationCheckBox->isChecked();
	int dilationN = epochDialog->ui.dilationNumPassSpinBox->value();
	int dilationSz = epochDialog->ui.dilationSizeSlider->value() * 2 + 1;
	bool erosionFlag = epochDialog->ui.erosionCheckBox->isChecked();
	int erosionN = epochDialog->ui.erosionNumPassSpinBox->value();
	int erosionSz = epochDialog->ui.erosionSizeSlider->value() * 2 + 1;
	float scalingFactor = epochDialog->ui.scaleLineEdit->text().toFloat();
	std::vector<string> savedMeshVector;

// Generating a mesh for each selected image

	bool firstTime=true;
	QList<EpochModel>::iterator li;
	for(li=er.modelList.begin(), i=0;li!=er.modelList.end();++li,++i)
	{
			if(qtw->isItemSelected(qtw->item(i,0)))
			{
				++selectedCount;
				mm.Clear();
				int tt0=clock();
				(*li).BuildMesh(mm,subSampleVal,minCountVal,MinAngleCos,smoothSteps, 
					dilationFlag, dilationN, dilationSz, erosionFlag, erosionN, erosionSz,scalingFactor);
				int tt1=clock();
				gla->log->Logf(GLLogStream::SYSTEM,"** Mesh %i : Build in %i\n",selectedCount,tt1-tt0);

				tri::Append<CMeshO,CMeshO>::Mesh(m->cm,mm); // append mesh mr to ml
					
				int tt2=clock();
				gla->log->Logf(GLLogStream::SYSTEM,"** Mesh %i : Append in %i\n",selectedCount,tt2-tt1);

			}
	}
	 
	int t1=clock();
	gla->log->Logf(GLLogStream::SYSTEM,"Extracted %i meshes in %i\n",selectedCount,t1-t0);

///// Removing connected components

	if(removeSmallCC)
	{
		m->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_FACEMARK);
		tri::Clean<CMeshO>::RemoveSmallConnectedComponentsDiameter(m->cm,m->cm.bbox.Diag()*maxCCDiagVal/100.0);
	}

	int t2=clock();
	gla->log->Logf(GLLogStream::SYSTEM,"Topology and removed CC in %i\n",t2-t1);

	vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);					// updates bounding box
	
// Hole filling

	if(closeHole)
	{
		m->updateDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER | MeshModel::MM_FACEMARK);
		tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);	    
		vcg::tri::Hole<CMeshO>::EarCuttingFill<vcg::tri::MinimumWeightEar< CMeshO> >(m->cm,maxHoleSize,false);
	}

	m->updateDataMask(MeshModel::MM_VERTCOLOR);

	int t3=clock();
	gla->log->Logf(GLLogStream::SYSTEM,"---------- Total Processing Time%i\n\n\n",t3-t0);
	
	vcg::tri::UpdateBounding<CMeshO>::Box(m->cm);					// updates bounding box
	tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(m->cm);
   
//// Importing rasters

	if (epochDialog->ui.shotExport->isChecked())
	{
		int saveSelected=epochDialog->ui.saveShotCombo->currentIndex();
		for(int i=0; i<er.modelList.size(); ++i)
		{
			if (saveSelected==0 || (qtw->isItemSelected(qtw->item(i,0))))
			{
				RasterModel* rm=md->addNewRaster();
				rm->addPlane(new Plane(md->rm(),er.modelList[i].textureName,QString("RGB")));
				rm->setLabel(er.modelList[i].textureName);
				rm->shot=er.modelList[i].shot;
				rm->shot.RescalingWorld(scalingFactor, false);
			}
		}
	}

// Final operations 

	md->mm()->visible=true;
	md->setBusy(false);
	gla->rm.colorMode=GLW::CMPerVert;
	emit this->resetTrackBall();
	gla->update();
	
}    

void EditEpochPlugin::mousePressEvent(QMouseEvent *e, MeshModel &, GLArea * )
{
	
}

void EditEpochPlugin::mouseMoveEvent(QMouseEvent *e, MeshModel &, GLArea * ) 
{
	
	
}

void EditEpochPlugin::mouseReleaseEvent(QMouseEvent * e, MeshModel &/*m*/, GLArea * gla)
{
	
}


// this function toggles on and off all the buttons (according to the "modal" states of the interface),
// do not confuse it with the updatebuttons function of the epochDialog class.
void EditEpochPlugin::toggleButtons()
{

}

void EpochModel::depthFilter(FloatImage &depthImgf, FloatImage &countImgf, float depthJumpThr, 
														 bool dilation, int dilationNumPasses, int dilationWinsize,
														 bool erosion, int erosionNumPasses, int erosionWinsize)
{
	FloatImage depth;
	FloatImage depth2;
	int w = depthImgf.w;
	int h = depthImgf.h;
	
	depth=depthImgf;

	if (dilation)
	{
		for (int k = 0; k < dilationNumPasses; k++)
		{
			depth.Dilate(depth2, dilationWinsize / 2);
			depth=depth2;
		}
	}

	if (erosion)
	{
		for (int k = 0; k < erosionNumPasses; k++)
		{
			depth.Erode(depth2, erosionWinsize / 2);
			depth=depth2;
		}
	}

  Histogramf HH;
  HH.Clear();
  HH.SetRange(0,depthImgf.MaxVal()-depthImgf.MinVal(),10000);
  for(int i=1; i < static_cast<int>(depthImgf.v.size()); ++i)
    HH.Add(fabs(depthImgf.v[i]-depth.v[i-1]));

  int deletedCnt=0;
  
  depthJumpThr = static_cast<float>(HH.Percentile(0.8));
	for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
				if ((depthImgf.Val(x, y) - depth.Val(x, y)) / depthImgf.Val(x, y) > 0.6)
        {
					countImgf.Val(x, y) = 0.0f;
          ++deletedCnt;
        }
		}

	countImgf.convertToQImage().save("tmp_filteredcount.jpg","jpg");
  
}

float EpochModel::ComputeDepthJumpThr(FloatImage &depthImgf, float percentile)
{
  Histogramf HH;
  HH.Clear();
  HH.SetRange(0,depthImgf.MaxVal()-depthImgf.MinVal(),10000);
  for(unsigned int i=1; i < static_cast<unsigned int>(depthImgf.v.size()); ++i)
    HH.Add(fabs(depthImgf.v[i]-depthImgf.v[i-1]));

  return HH.Percentile(percentile);
}



/// Apply the hand drawn mask image 
bool EpochModel::CombineHandMadeMaskAndCount(CharImage &CountImg, QString maskName )
{
	QImage maskImg(maskName);
  qDebug("Trying to read maskname %s",qPrintable(maskName));
	if(maskImg.isNull()) 
		return false;

	if( (maskImg.width()!= CountImg.w)  || (maskImg.height()!= CountImg.h) )
	{
		qDebug("Warning mask and images does not match! %i %i vs %i %i",maskImg.width(),CountImg.w,maskImg.height(),CountImg.h);
		return false;
	}
	
	for(int j=0;j<maskImg.height();++j)
		for(int i=0;i<maskImg.width();++i)
			if(qRed(maskImg.pixel(i,j))>128)
				CountImg.Val(i,j)=0;

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

/* 
This filter average apply a laplacian smoothing over a depth map averaging the samples with a weighting scheme that follows the Counting masks.
The result  of the laplacian is applied only on sample with low quality.
*/

void EpochModel::Laplacian2(FloatImage &depthImg, FloatImage &countImg, int minCount, CharImage &featureMask, float depthThr)
{
  FloatImage Sum;
  int w=depthImg.w,h=depthImg.h;
  Sum.resize(w,h);
  
 for(int y=1;y<h-1;++y)
  for(int x=1;x<w-1;++x)
    {
      float curDepth=depthImg.Val(x,y);
      int cnt=0;
      for(int j=-1;j<=1;++j)
        for(int i=-1;i<=1;++i)
         {
           int q=countImg.Val(x+i,y+j)-minCount+1;
           if(q>0 && fabs(depthImg.Val(x+i,y+j)-curDepth) < depthThr) {
             Sum.Val(x,y)+=q*depthImg.Val(x+i,y+j);
             cnt+=q;
           }
         }
         if(cnt>0) {
           Sum.Val(x,y)/=cnt;
         }
         else Sum.Val(x,y)=depthImg.Val(x,y);
    }

 for(int y=1;y<h-1;++y)
  for(int x=1;x<w-1;++x)
    {
      float q=(featureMask.Val(x,y)/255.0);
      depthImg.Val(x,y) = depthImg.Val(x,y)*q + Sum.Val(x,y)*(1-q);
    }
}

// It generate a feature mask that mark the featureless area of the original photo. 
// Featureless areas are usually affected by noise and have to be smoothed more

void EpochModel::GenerateGradientSmoothingMask(int subsampleFactor, QImage &OriginalTexture, CharImage &mask)
{
	CharImage gray(OriginalTexture);
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
			unsigned char maxGrad=0;
			for(int si=0;si<subsampleFactor;++si)
				for(int sj=0;sj<subsampleFactor;++sj)
					maxGrad = max(maxGrad, grad.Val(x*subsampleFactor+sj,y*subsampleFactor+si));

			mask.Val(x,y) = maxGrad;
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
  
  mask.convertToQImage().save("tmp_testmask.jpg","jpg");
  mask2.convertToQImage().save("tmp_testmaskSmooth.jpg","jpg");

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
  
	grad.convertToQImage().save("tmp_test.jpg","jpg");
	mask.convertToQImage().save("tmp_testmaskeroded.jpg","jpg");
}

/*
Main processing function;

it takes a depth map, a count map, 
- resample them to a (width/subsample,height/subsample) image
- leave only the faces that are within a given orientation range
- that have a count greater than minCount.
- and smooth them with a count/quality aware laplacian filter
*/ 

bool EpochModel::BuildMesh(CMeshO &m, int subsampleFactor, int minCount, float minAngleCos, int smoothSteps,
													 bool dilation, int dilationPasses, int dilationSize, 
													 bool erosion, int erosionPasses, int erosionSize,float scalingFactor)
{
  FloatImage depthImgf;
  CharImage countImgc;
  int ttt0=clock();
  depthImgf.Open(depthName.toAscii());
  countImgc.Open(countName.toAscii());
  
  QImage TextureImg;
  TextureImg.load(textureName);
  int ttt1=clock();

  CombineHandMadeMaskAndCount(countImgc,maskName);  // set count to zero for all masked points
  
  FloatImage depthSubf;  // the subsampled depth image 
  FloatImage countSubf;  // the subsampled quality image (quality == count)
  
  SmartSubSample(subsampleFactor,depthImgf,countImgc,depthSubf,countSubf,minCount);
  
  CharImage FeatureMask; // the subsampled image with (quality == features)
  GenerateGradientSmoothingMask(subsampleFactor, TextureImg, FeatureMask);

  depthSubf.convertToQImage().save("tmp_depth.jpg", "jpg");

  int ttt2=clock();

  float depthThr = ComputeDepthJumpThr(depthSubf,0.8f);
  for(int ii=0;ii<smoothSteps;++ii) 
    Laplacian2(depthSubf,countSubf,minCount,FeatureMask,depthThr);

  int ttt3=clock();

  vcg::tri::Grid<CMeshO>(m,depthSubf.w,depthSubf.h,depthImgf.w,depthImgf.h,&*depthSubf.v.begin());

  int ttt4=clock();


	// The depth is filtered and the minimum count mask is update accordingly.
	// To be more specific the border of the depth map are identified by erosion
	// and the relative vertex removed (by setting mincount equal to 0).
  float depthThr2 = ComputeDepthJumpThr(depthSubf,0.95f);
	depthFilter(depthSubf, countSubf, depthThr2, 
		dilation, dilationPasses, dilationSize, 
		erosion, erosionPasses, erosionSize);

	int vn = m.vn;
  for(int i=0;i<vn;++i)
    if(countSubf.v[i]<minCount) 
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
    QRgb c = TextureImg.pixel(int(in[0]), int(in[1]));
    (*vi).C().SetRGB(qRed(c),qGreen(c),qBlue(c));
    if(FeatureMask.Val(int(in[0]/subsampleFactor), int(in[1]/subsampleFactor))<200) (*vi).Q()=0; 
    else (*vi).Q()=1; 
    (*vi).Q()=float(FeatureMask.Val(in[0]/subsampleFactor, in[1]/subsampleFactor))/255.0;
  }
    
  int ttt5=clock();

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
       if(dir.dot(n) < minAngleCos)
                {
                  (*fi).SetD();
                  --m.fn;
                }
     }
   }

  tri::Clean<CMeshO>::RemoveUnreferencedVertex(m);
  int ttt6=clock();

	Matrix44f scaleMat;
	scaleMat.SetScale(scalingFactor,scalingFactor,scalingFactor);
	vcg::tri::UpdatePosition<CMeshO>::Matrix(m, scaleMat);

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

				// import leuven camera
					{
						double cam[9];
						float focus,scale;

						FILE* lvcam;

						lvcam = fopen(cameraName.toAscii(),"rb");

						// focus + image centers
						fscanf(lvcam,"%lf %lf %lf",&(cam[0]),&(cam[1]),&(cam[2]));
						fscanf(lvcam,"%lf %lf %lf",&(cam[3]),&(cam[4]),&(cam[5]));
						fscanf(lvcam,"%lf %lf %lf",&(cam[6]),&(cam[7]),&(cam[8]));

						shot.Intrinsics.CenterPx[0] = cam[2];
						shot.Intrinsics.CenterPx[1] = cam[5];
						focus = cam[0];
						scale = 1.0f;
						while(focus>10.0f)
						{
							focus /= 10.0f;
							scale /= 10.0f;
						}
						shot.Intrinsics.FocalMm = focus;
						shot.Intrinsics.PixelSizeMm[0] = scale;
						shot.Intrinsics.PixelSizeMm[1] = scale;

						// distortion
						fscanf(lvcam,"%lf %lf %lf",&(cam[0]),&(cam[1]),&(cam[2]));
						shot.Intrinsics.k[0] = cam[0];
						shot.Intrinsics.k[1] = cam[1];

						// orientation axis
						fscanf(lvcam,"%lf %lf %lf",&(cam[0]),&(cam[1]),&(cam[2]));
						fscanf(lvcam,"%lf %lf %lf",&(cam[3]),&(cam[4]),&(cam[5]));
						fscanf(lvcam,"%lf %lf %lf",&(cam[6]),&(cam[7]),&(cam[8]));

						Matrix44f myrot;

						myrot[0][0] = cam[0];		myrot[0][1] = cam[3];		myrot[0][2] = cam[6];		myrot[0][3] = 0.0f;
						myrot[1][0] = -cam[1];	myrot[1][1] = -cam[4];	myrot[1][2] = -cam[7];	myrot[1][3] = 0.0f;
						myrot[2][0] = -cam[2];	myrot[2][1] = -cam[5];	myrot[2][2] = -cam[8];	myrot[2][3] = 0.0f;
						myrot[3][0] = 0.0f;			myrot[3][1] = 0.0f;			myrot[3][2] = 0.0f;			myrot[3][3] = 1.0;

						shot.Extrinsics.SetRot(myrot);

						// camera position
						fscanf(lvcam,"%lf %lf %lf",&(cam[0]),&(cam[1]),&(cam[2]));
						shot.Extrinsics.SetTra(Point3f(cam[0], cam[1], cam[2]));
		//				shot.Extrinsics.sca = 1.0f;


						// image size
						fscanf(lvcam,"%lf %lf",&(cam[0]),&(cam[1]));
						shot.Intrinsics.ViewportPx.X() = (int)(cam[0]);
						shot.Intrinsics.ViewportPx.Y() = (int)(cam[1]);

						fclose(lvcam);			
					}
            }

  QString tmpName=textureName.left(textureName.length()-4);
  maskName = tmpName.append(".mask.png");
  return true;
}

QString EpochModel::ThumbName(QString &_imageName)
{
  QString tmpName=_imageName.left(_imageName.length()-4);
  return tmpName.append(".thumb.jpg");
}