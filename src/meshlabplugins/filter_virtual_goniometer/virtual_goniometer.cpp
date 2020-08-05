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

#include <math.h>
#include <stdlib.h>
#include <vcg/space/colorspace.h>
#include <vcg/space/fitting3.h>
#include "virtual_goniometer.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/point_outlier.h>
#include <vcg/complex/algorithms/create/platonic.h>

#include <QOpenGLContext>
#include <vcg/complex/algorithms/update/color.h>
#include <vcg/complex/algorithms/update/normal.h>

#include "connectedComponent.h"

/*#include <eigenlib/Eigen/Eigenvalues>
#include <complex>*/


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <vector>
#include <algorithm>
#include <numeric>
#include <ctime>






using namespace std;
using namespace vcg;

static float SegParam = 2.0;

#define TRUE 1
#define FALSE 0
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SIGN(a) (((a)<0)?(-1):(1))
#define ABS(a) (((a)<0)?(-(a)):(a))

// ERROR CHECKING UTILITY
#define CheckError(x,y); if ((x)) {this->errorMessage = (y); return false;}
///////////////////////////////////////////////////////

VirtualGoniometerFilterPlugin::VirtualGoniometerFilterPlugin()
{
   typeList <<
      FP_QUALITY_VIRTUAL_GONIOMETER <<
      FP_QUALITY_VIRTUAL_GONIOMETER_NEXT <<
      FP_QUALITY_VIRTUAL_GONIOMETER_UNDO <<
      FP_QUALITY_VIRTUAL_GONIOMETER_RESET;

   FilterIDType tt;

   foreach(tt , types())
   {
      actionList << new QAction(filterName(tt), this);
      if (tt == FP_QUALITY_VIRTUAL_GONIOMETER){
         actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_V);
         actionList.last()->setIcon(QIcon(":/images/virtual_goniometer.png"));
         actionList.last()->setPriority(QAction::HighPriority);
	   }
      if (tt == FP_QUALITY_VIRTUAL_GONIOMETER_NEXT){
         actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_N);
         actionList.last()->setIcon(QIcon(":/images/virtual_goniometer_next.png"));
         actionList.last()->setPriority(QAction::HighPriority);
	   }
      if (tt == FP_QUALITY_VIRTUAL_GONIOMETER_UNDO){
         actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_Z);
         actionList.last()->setIcon(QIcon(":/images/virtual_goniometer_undo.png"));
         actionList.last()->setPriority(QAction::HighPriority);
	   }
      if (tt == FP_QUALITY_VIRTUAL_GONIOMETER_RESET){
         actionList.last()->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_U);
         actionList.last()->setIcon(QIcon(":/images/virtual_goniometer_reset.png"));
         actionList.last()->setPriority(QAction::HighPriority);
	   }

   }
}

QString VirtualGoniometerFilterPlugin::filterName(FilterIDType filter) const
{
 switch(filter)
 {
	   case FP_QUALITY_VIRTUAL_GONIOMETER:         return tr("Virtual Goniometer");
	   case FP_QUALITY_VIRTUAL_GONIOMETER_NEXT:    return tr("Virtual Goniometer: Next break curve");
	   case FP_QUALITY_VIRTUAL_GONIOMETER_UNDO:    return tr("Virtual Goniometer: Undo");
	   case FP_QUALITY_VIRTUAL_GONIOMETER_RESET:   return tr("Virtual Goniometer: Reset");
 }
 assert(0);
 return QString("Unknown filter");
}

QString VirtualGoniometerFilterPlugin::filterInfo(FilterIDType filterId) const
{
 switch(filterId)
 {
	//case FP_QUALITY_VIRTUAL_GONIOMETER:       return tr("Compute the angle across a break edge.");
   case FP_QUALITY_VIRTUAL_GONIOMETER:   return tr("Compute the angle across a break edge.<br><br>"
                                                "This plugin was developed by researchers at the University of Minnesota affiliated with the <b><a href='http://amaaze.umn.edu'>AMAAZE</a></b> consortium.<br>" 
                                                "Documentation is available here: <a href='https://amaaze.umn.edu/'>https://amaaze.umn.edu/</a>.<br>"
                                                "Questions or comments can be emailed to <a href= 'mailto:amaaze@umn.edu'>amaaze@umn.edu</a>.<br>");

	case FP_QUALITY_VIRTUAL_GONIOMETER_NEXT:  return tr("Advance to next break curve.");
	case FP_QUALITY_VIRTUAL_GONIOMETER_UNDO:  return tr("Undo a Virtual Goniometer measurement.");
	case FP_QUALITY_VIRTUAL_GONIOMETER_RESET:  return tr("Reset Virtual Goniometer.");
 }
 assert(0);
 return QString("Unknown filter");
}

int index_first_selected(MeshModel &m){

   int i=0;
   CMeshO::VertexIterator vi;
   for(vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); ++vi)
   {
      if(!(*vi).IsD() && (*vi).IsS()){
         (*vi).ClearS();
         break;
      }
      i++;
   }   
   return i;
}


void VirtualGoniometerFilterPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
 switch(ID(action))
 {
	case FP_QUALITY_VIRTUAL_GONIOMETER:
	{
      //Get number of selected vertices
      int num_selected_pts = tri::UpdateSelection<CMeshO>::VertexCount(m.cm);
      
      if(num_selected_pts <= 10){
         float x=0.0f, y=0.0f, z=0.0f;
         if(num_selected_pts >= 1){
            int i = index_first_selected(m);
            x = m.cm.vert[i].P()[0];
            y = m.cm.vert[i].P()[1];
            z = m.cm.vert[i].P()[2];
         }
         parlst.addParam(new RichPoint3f("Location", Point3f(x,y,z), "Location", "Location on mesh to run Virtual Goniometer."));
         parlst.addParam(new RichFloat("Radius", 3.0, "Radius", "Radius of patch to use."));
         parlst.addParam(new RichDynamicFloat("SegParam",SegParam,0.0,5.0,"Segmentation Parameter", "Parameter controlling how much influence the geometry has in segmentation."));
         parlst.addParam(new RichBool("Automatic Radius", FALSE,"Burst measurements", "Use automatic radius selection and take multiple measurements."));
         parlst.addParam(new RichBool("UpdateParam", FALSE, "Update Parameter Only", "Only update the segmentation parameter, and do not run the virtual goniometer."));
      }
      break;

	} 
	case FP_QUALITY_VIRTUAL_GONIOMETER_NEXT:
	{
      break;
	}	
   case FP_QUALITY_VIRTUAL_GONIOMETER_UNDO:
	{
      break;
	} 
	case FP_QUALITY_VIRTUAL_GONIOMETER_RESET:
	{
      parlst.addParam(new RichBool("Load", FALSE, "Load from CSV file.", "Load measurements from CSV file."));
	} break;

 }
}

void remove_last_lines(char *filename, int n)
{
	//char* home;
	//home = getenv("HOME");
	//string pathname = string(home) + "/VirtualGoniometer_Measurements.csv";

	ifstream myFile(filename);
	string str;
	vector<string> file_contents;

	if(!myFile.is_open()){
		printf("Cannot open file %s\n", filename);
   }
   else{

      while(getline(myFile, str))
         file_contents.push_back(str);
      myFile.close();

      ofstream fout;
      fout.open(filename);

      int num_lines = file_contents.size();

      if(n > num_lines-1){
         printf("File does not have enough lines to remove.\n");
         n = 0;
      }
      printf("Removing %d lines from %s\n",n,filename);
      for(int i=0; i<file_contents.size()-n; i++)
         fout << file_contents[i] << "\n";

      fout.close();
   }
   
}

void patch_statistics(vector<float> &vecx, vector<float> &vecy, vector<float> &vecz, float *meanx, float *meany, float *meanz, float *surf_meanx, float *surf_meany, float *surf_meanz, float *radius){

   int num_selected_pts = vecx.size();

   //Mean location of patch
   *meanx = accumulate(vecx.begin(), vecx.end(), 0.0)/num_selected_pts;
   *meany = accumulate(vecy.begin(), vecy.end(), 0.0)/num_selected_pts;
   *meanz = accumulate(vecz.begin(), vecz.end(), 0.0)/num_selected_pts;

   int ind = 0;
   float min_dist = (vecx[0] - *meanx)*(vecx[0] - *meanx) + (vecy[0] - *meany)*(vecy[0] - *meany) + (vecz[0] - *meanz)*(vecz[0] - *meanz);
   for(int j=0; j < num_selected_pts; j++){
      float dist = (vecx[j] - *meanx)*(vecx[j] - *meanx) + (vecy[j] - *meany)*(vecy[j] - *meany) + (vecz[j] - *meanz)*(vecz[j] - *meanz);
      if(dist < min_dist){
         min_dist = dist;
         ind = j;
      }
   }

   //Closest point on surface to mean
   *surf_meanx = vecx[ind];
   *surf_meany = vecy[ind];
   *surf_meanz = vecz[ind];

   //Compute radius of patch
   *radius = 0.0;
   for(int j=0; j < num_selected_pts; j++){
      float dist = (vecx[j] - *surf_meanx)*(vecx[j] - *surf_meanx) + (vecy[j] - *surf_meany)*(vecy[j] - *surf_meany) + (vecz[j] - *surf_meanz)*(vecz[j] - *surf_meanz);
      if(dist > *radius)
         *radius = dist;
   }
   *radius = sqrt(*radius);
}

void geodesic_patch_info(MeshModel &m, float *radius, int *center_ind)
{

   CMeshO::PerVertexAttributeHandle<float> distFromCenter = vcg::tri::Allocator<CMeshO>::GetPerVertexAttribute<float>(m.cm, std::string("DistParam"));

   //Get radius of patch
   *radius = 0.0;
   *center_ind = 0;
   float a = distFromCenter[0];
   for(int i=0; i < m.cm.vert.size(); i++){
      if(m.cm.vert[i].IsS()){
         if(distFromCenter[i] < a){
            *center_ind = i;
            a = distFromCenter[i];
         }
         *radius = MAX(*radius,distFromCenter[i]);
      }
   }

}

void withness(vector<float> x, float *w, float *m, float *mlow, float *mhigh)
{
  int sum = 0;
  int n = x.size();
  sort(x.begin(), x.end());
  vector<float> v(n-1,0);

  //Compute Variance
  float mean = 0.0;
  for(int i=0; i<n; i++)
  {
	mean += x[i];
  }
  mean = mean/n;
  float nvar = 0.0;
  for(int i = 0; i<n; i++)
  {
	nvar += (x[i] - mean)*(x[i] - mean);
  }

  int minind = 0;
  for(int i = 0; i<n-1; i++)
  {
     float m1 = 0.0;
     float m2 = 0.0;

     for(int j = 0; j<i+1; j++)
     {
        m1 += x[j];
     }
     for(int j = i+1; j<n; j++)
     {
        m2 += x[j];
     }

     m1 = m1/(i+1);
     m2 = m2/(n-i-1);

     v[i] = 0;
     for(int j = 0; j<i+1; j++)
     {
        v[i] += (x[j]-m1)*(x[j]-m1);
     }
     for(int j = i+1; j<n; j++)
     {
        v[i] += (x[j]-m2)*(x[j]-m2);
     }
     v[i] = v[i]/nvar;

     if(v[i] < v[minind])
     {
        minind = i;
     }
  }

  *mlow = x[MAX(minind-n/10,0)];
  *mhigh = x[MIN(minind+n/10,n-1)]; 
  *m = x[minind];
  *w = v[minind];
}

void withness(vector<float> x, float *w, float *m){

   float mlow, mhigh;
   withness(x,w,m,&mlow,&mhigh);
}


void power_method(float *C1, float *C2, float *C3, float tol, float *l, float *v)
{
	float w[3];
	float norm;
	v[0] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	v[1] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	v[2] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	float err = 1.0;
	int i = 0;
	while(err > tol && ++i < 10000)
	{
		w[0] = C1[0]*v[0] + C1[1]*v[1] + C1[2]*v[2];
		w[1] = C2[0]*v[0] + C2[1]*v[1] + C2[2]*v[2];
		w[2] = C3[0]*v[0] + C3[1]*v[1] + C3[2]*v[2];

		*l = w[0]*v[0] + w[1]*v[1] + w[2]*v[2];
		err = (w[0] - *l*v[0])*(w[0] - *l*v[0]) + (w[1] - *l*v[1])*(w[1] - *l*v[1]) + (w[2] - *l*v[2])*(w[2] - *l*v[2]);

		norm = sqrt(w[0]*w[0] + w[1]*w[1] + w[2]*w[2]);
		v[0] = w[0]/norm;
		v[1] = w[1]/norm;
		v[2] = w[2]/norm;
	}
}

float PCA_smallest_eig(vector<float> &vecx, vector<float> &vecy, vector<float> &vecz, float *eig, vector<int> &C, int c, bool center){
   
   //Rows of covariance matrix
   float C1[3] = {0,0,0};
   float C2[3] = {0,0,0};
   float C3[3] = {0,0,0};
   float m[3] = {0,0,0};
   float l;
   int n = vecx.size();
   
   //Compute mean
   int num = 0;
   for(int j = 0; j<n; j++)
   {
      if(C[j] == c)
      {
         m[0] += vecx[j];
         m[1] += vecy[j];
         m[2] += vecz[j];
         num++;
      }
   }
   m[0] = m[0]/num;
   m[1] = m[1]/num;
   m[2] = m[2]/num;

   //Compute covariance matrix
   for(int j = 0; j<n; j++)
   {
      if(C[j] == c)
      {
         float x1,x2,x3;
         if(center){
            x1 = vecx[j] - m[0];
            x2 = vecy[j] - m[1]; 
            x3 = vecz[j] - m[2]; 
         }else{
            x1 = vecx[j];
            x2 = vecy[j]; 
            x3 = vecz[j]; 
         }
         C1[0] += x1*x1;
         C1[1] += x1*x2;
         C1[2] += x1*x3;
         C2[1] += x2*x2;
         C2[2] += x2*x3;
         C3[2] += x3*x3;
      }
   }
   C1[0] = C1[0]/num;
   C1[1] = C1[1]/num;
   C1[2] = C1[2]/num;
   C2[1] = C2[1]/num;
   C2[2] = C2[2]/num;
   C3[2] = C3[2]/num;
   C2[0] = C1[1];
   C3[0] = C1[2];
   C3[1] = C2[2];

   /*typedef Eigen::Matrix<double, 3, 3> Matrix3d;
   Eigen::Matrix3d A;

   A(0,0) = C1[0]; A(0,1) = C1[1]; A(0,2) = C1[2];
   A(1,0) = C2[0]; A(1,1) = C2[1]; A(1,2) = C2[2];
   A(2,0) = C3[0]; A(2,1) = C3[1]; A(2,2) = C3[2];

   Eigen::EigenSolver<Matrix3d> es;*/
//   es.compute(A, /* computeEigenvectors = */ true);
   
   //Spectral shift
   power_method(C1, C2, C3, 1E-10, &l, eig);
   float lmax = l;
   //printf("lmax=%f\n",lmax);
   C1[0] = C1[0] - l - 1;
   C2[1] = C2[1] - l - 1;
   C3[2] = C3[2] - l - 1;

   //Find eigenvector
   power_method(C1, C2, C3, 1E-10, &l, eig);
   float lmin = l + lmax + 1;
   return lmin;
   //printf("lmin=%f\n",l+lmax+1);
   

/*   double l1 = es.eigenvalues()(0,0).real();
   double l2 = es.eigenvalues()(1,0).real();
   double l3 = es.eigenvalues()(2,0).real();

   int j = 0;
   if(l2 < l1 && l2 < l3)
      j = 1;
   if(l3 < l2 && l3 < l1)
      j = 2; 

   double e1 = es.eigenvectors()(0,j).real();
   double e2 = es.eigenvectors()(1,j).real();
   double e3 = es.eigenvectors()(2,j).real();
   printf("eigval=(%f,%f,%f)\n",l1,l2,l3);
   printf("eig=(%f,%f,%f)\n",e1,e2,e3);
   printf("myeig=(%f,%f,%f)\n",eig[0],eig[1],eig[2]);*/

}

float median(vector<float> x){

   int n = x.size();

   if(n>=2){
      sort(x.begin(),x.end());
      if(n%2==0)
         return (x[n/2 -1] + x[n/2])/2.0;
      else
         return x[n/2];
   }else if(n==1)
      return x[0];
   else
      return 0.0;
}

void RobustPCA(vector<float> &vecx, vector<float> &vecy, vector<float> &vecz, float *p, vector<int> &C, int c, int num_iter, bool rand_init){
  
   int i,j,k;

   //Subset data
   vector<vector<float>> X;
   vector<float> point(3,0.0), mean(3,0.0);
   for(i=0; i<vecx.size(); i++){
      if(C[i] == c){
         point[0] = vecx[i]; 
         point[1] = vecy[i]; 
         point[2] = vecz[i]; 
         X.push_back(point);
      }
   }
   
   //Number of points in cluster
   int n = X.size();

   //Compute mean
   for(i=0;i<n;i++) for(j=0;j<3;j++) mean[j]+=X[i][j];
   mean[0]/=n; mean[1]/=n; mean[2]/=n; 

   //Center data
   for(i=0;i<n;i++) for(j=0;j<3;j++) X[i][j]-=mean[j];

   //Normalize data
   float max_norm = 0.0;
   for(i=0;i<n;i++){
      float norm = 0.0;
      for(j=0;j<3;j++) 
         norm += X[i][j]*X[i][j];
      max_norm = max(norm,max_norm);
   }
   max_norm = sqrt(max_norm);
   for(i=0;i<n;i++) for(j=0;j<3;j++) X[i][j]/=max_norm;

   float dt = 0.1;
   float energy_prev = 1000;

   if(rand_init){
      p[0] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      p[1] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
      p[2] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
   }
   float normp = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
   for(j=0;j<3;j++) p[j]/=normp;

   for(k=0;k<num_iter;k++){

      //printf("p[%d]=(%f,%f,%f)\n",k,p[0],p[1],p[2]);

      vector<float> Xp(n,0.0);
      vector<float> grad(3,0.0);

      //Compute gradient
      for(i=0;i<n;i++) for(j=0;j<3;j++) Xp[i]+=X[i][j]*p[j];

      float b = -median(Xp);
      float energy = 0.0;
      for(i=0;i<n;i++) energy+=ABS(Xp[i] + b);
      energy = energy/n;
      energy_prev = energy;
      //printf("%d:Energy = %f, dt=%f\n",k,energy,dt);

      for(i=0;i<n;i++) for(j=0;j<3;j++) grad[j]+=SIGN(Xp[i] + b)*X[i][j];

      for(j=0;j<3;j++) p[j]-=dt*grad[j]/n;

      normp = sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);
      for(j=0;j<3;j++) p[j]/=normp;
   }
}

vector<int> ClusterPatch(vector<float> &vecx, vector<float> &vecy, vector<float> &vecz, vector<float> &normalx, vector<float> &normaly, vector<float> &normalz, float surf_meanx, float surf_meany, float surf_meanz, float radius){

   int n = normalx.size();
   float N1[3] = {0,0,0};
   float N2[3] = {0,0,0};
   float v[3] = {0,0,0};
   vector<int> C(n,0);
   float w,m,mlow,mhigh;
   vector<float> x(n, 0);
   vector<int> C1(n,1);

   PCA_smallest_eig(normalx, normaly, normalz, N1, C1, 1, FALSE);
   
   //Compute mean
   for(int j = 0; j<n; j++)
   {
      N2[0] += normalx[j];
      N2[1] += normaly[j];
      N2[2] += normalz[j];
   }

   //Cross product
   v[0] = N1[1]*N2[2] - N1[2]*N2[1];
   v[1] = N1[2]*N2[0] - N1[0]*N2[2];
   v[2] = N1[0]*N2[1] - N1[1]*N2[0];

   float normv = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
   v[0] = v[0]/normv; v[1] = v[1]/normv; v[2] = v[2]/normv;
   
   for(int k=0 ;k<n ;k++){
      x[k] = normalx[k]*v[0] + normaly[k]*v[1] + normalz[k]*v[2];
      x[k]+= SegParam*((vecx[k] - surf_meanx)*v[0] + (vecy[k] - surf_meany)*v[1] + (vecz[k] - surf_meanz)*v[2])/radius;
   }
   withness(x,&w,&m,&mlow,&mhigh);

/*   for(int i=0; i<n; i++)
   {
      if(x[i] >= mhigh)
         C[i] = 1;
      if(x[i] <= mlow)
         C[i] = 2;
   }*/

   for(int i=0; i<n; i++)
   {
      if(x[i] > m)
         C[i] = 1;
      else
         C[i] = 2;
   }

   return C;
}

vector<int> VirtualGoniometer(vector<float> &vecx, vector<float> &vecy, vector<float> &vecz, vector<float> &normalx, vector<float> &normaly, vector<float> &normalz, float surf_meanx, float surf_meany, float surf_meanz, float radius, float *theta, float *fit)
{
   int T = 100;
   int n = normalx.size();
   float m1[3], m2[3]; //PCA surface normals
   float n1[3], n2[3]; //Average surface normals
   float o1[3], o2[3]; //Robust PCA normals

   vector<int> C = ClusterPatch(vecx, vecy, vecz, normalx, normaly, normalz, surf_meanx, surf_meany, surf_meanz, radius);

   //Compute normals by averaging normal vectors in each patch
   n1[0]=0; n1[1]=0; n1[2]=0;
   n2[0]=0; n2[1]=0; n2[2]=0;

   for(int j = 0; j<n; j++)
   {
     if(C[j] == 1)
     {
       n1[0] += normalx[j];
       n1[1] += normaly[j];
       n1[2] += normalz[j];
     }
     else if(C[j] == 2)
     {
       n2[0] += normalx[j];
       n2[1] += normaly[j];
       n2[2] += normalz[j];
     }
   }

   float n1_norm = sqrt(n1[0]*n1[0] + n1[1]*n1[1] + n1[2]*n1[2]);
   float n2_norm = sqrt(n2[0]*n2[0] + n2[1]*n2[1] + n2[2]*n2[2]);

   n1[0] = n1[0]/n1_norm;
   n1[1] = n1[1]/n1_norm;
   n1[2] = n1[2]/n1_norm;
   n2[0] = n2[0]/n2_norm;
   n2[1] = n2[1]/n2_norm;
   n2[2] = n2[2]/n2_norm;

   //Compute normals by PCA 
   float fit1 = PCA_smallest_eig(vecx, vecy, vecz, m1, C, 1, TRUE);
   float fit2 = PCA_smallest_eig(vecx, vecy, vecz, m2, C, 2, TRUE);
   *fit = fit1 + fit2;

   //Check signs of normals
   float dot = n1[0]*m1[0] + n1[1]*m1[1] + n1[2]*m1[2];
   if(dot < 0){
      m1[0] = -m1[0];
      m1[1] = -m1[1];
      m1[2] = -m1[2];
   }
   dot = n2[0]*m2[0] + n2[1]*m2[1] + n2[2]*m2[2];
   if(dot < 0){
      m2[0] = -m2[0];
      m2[1] = -m2[1];
      m2[2] = -m2[2];
   }

   theta[0] = 180-acos(m1[0]*m2[0] + m1[1]*m2[1] + m1[2]*m2[2])*(180/3.1415926);
   theta[1] = 180-acos(n1[0]*n2[0] + n1[1]*n2[1] + n1[2]*n2[2])*(180/3.1415926);
   
   /*o1[0] = m1[0]; o1[1] = m1[1]; o1[2] = m1[2];
   RobustPCA(vecx, vecy, vecz, o1, C, 1, 100, FALSE);

   o2[0] = m2[0]; o2[1] = m2[1]; o2[2] = m2[2];
   RobustPCA(vecx, vecy, vecz, o2, C, 2, 100, FALSE);
   
   theta[2] = 180-acos(o1[0]*o2[0] + o1[1]*o2[1] + o1[2]*o2[2])*(180/3.1415926);

   o1[0] = m1[0]; o1[1] = m1[1]; o1[2] = m1[2];
   RobustPCA(vecx, vecy, vecz, o1, C, 1, 1000, TRUE);

   o2[0] = m2[0]; o2[1] = m2[1]; o2[2] = m2[2];
   RobustPCA(vecx, vecy, vecz, o2, C, 2, 1000, TRUE);

   //Check signs of normals
   dot = n1[0]*o1[0] + n1[1]*o1[1] + n1[2]*o1[2];
   if(dot < 0){
      o1[0] = -o1[0];
      o1[1] = -o1[1];
      o1[2] = -o1[2];
   }
   dot = n2[0]*o2[0] + n2[1]*o2[1] + n2[2]*o2[2];
   if(dot < 0){
      o2[0] = -o2[0];
      o2[1] = -o2[1];
      o2[2] = -o2[2];
   }

   theta[3] = 180-acos(o1[0]*o2[0] + o1[1]*o2[1] + o1[2]*o2[2])*(180/3.1415926);*/

   return C;
}




void get_vertices(MeshModel &m, vector<unsigned int> &indices, vector<float> &vecx, vector<float> &vecy, vector<float> &vecz){

   int num_selected_pts = indices.size();
   vecx.resize(num_selected_pts);
   vecy.resize(num_selected_pts);
   vecz.resize(num_selected_pts);
   for(int k=0;k<num_selected_pts;k++){
      vecx[k] = m.cm.vert[indices[k]].P()[0];
      vecy[k] = m.cm.vert[indices[k]].P()[1];
      vecz[k] = m.cm.vert[indices[k]].P()[2];
   }
}
void get_normals(MeshModel &m, vector<unsigned int> &indices, vector<float> &normalx, vector<float> &normaly, vector<float> &normalz){

   int num_selected_pts = indices.size();
   normalx.resize(num_selected_pts);
   normaly.resize(num_selected_pts);
   normalz.resize(num_selected_pts);
   for(int k=0;k<num_selected_pts;k++){
      normalx[k] = m.cm.vert[indices[k]].N()[0];
      normaly[k] = m.cm.vert[indices[k]].N()[1];
      normalz[k] = m.cm.vert[indices[k]].N()[2];
   }
}

//Change contrast for a patch on the mesh
void change_contrast_patch(MeshModel &m, vector<unsigned int> indices, float factor){
   tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
   for(int k=0;k<indices.size();k++)
      m.cm.vert[indices[k]].SetS();
   tri::UpdateColor<CMeshO>::PerVertexContrast(m.cm, factor, TRUE);
   tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
}

//Color a patch on the mesh
void color_patch(MeshModel &m, vector<unsigned int> indices, Color4b color){
   tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
   for(int k=0;k<indices.size();k++)
      m.cm.vert[indices[k]].SetS();
   tri::UpdateColor<CMeshO>::PerVertexConstant(m.cm, color, TRUE);
   tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
}

//Return elements from indices where C=val
vector <unsigned int>  subset_indices(vector<unsigned int> &indices, vector<int> C, int val){
   vector<unsigned int> sub_indices;
   for(int k=0;k<indices.size();k++){
      if(C[k] == val)
         sub_indices.push_back(indices[k]); 
   }
   return sub_indices;
}
vector <unsigned int>  subset_indices(vector<int> C, int val){
   vector<unsigned int> sub_indices;
   for(int k=0;k<C.size();k++){
      if(C[k] == val)
         sub_indices.push_back(k); 
   }
   return sub_indices;
}
vector <unsigned int>  subset_indices(vector<int> C){
   vector<unsigned int> sub_indices;
   for(int k=0;k<C.size();k++){
      if(C[k]>0)
         sub_indices.push_back(k); 
   }
   return sub_indices;
}


//Updates indices for colors of patches
void update_colors(int *ci, int *cj, int num_colors){

   int color_i = *ci;
   int color_j = *cj;

   color_j++;
   if(color_j==color_i)
      color_j++;
   if(color_j == num_colors){
      color_i++;
      color_j = color_i+1;
   }
   if(color_i==num_colors){
      color_i=0;
      color_j=1;
   }

   *ci = color_i;
   *cj = color_j;
}

//Returns indices of vertices in mesh that were selected
vector<unsigned int> get_selected_indices(MeshModel &m){
   
   vector<unsigned int> indices;
   int i = 0;
   CMeshO::VertexIterator vi;
   for(vi=m.cm.vert.begin(); vi!=m.cm.vert.end(); ++vi)
   {
      if(!(*vi).IsD() && (*vi).IsS())
         indices.push_back(i);
      i++;
   }
   return indices;
}

//Returns C-string with mesh name (filename_Mesh.ply returns filename)
void mesh_name(MeshModel &m, char *plyfile){
   
   //Get mesh ply filename
   QString qs_plyfile = m.shortName();
   int len = qs_plyfile.length();
   strcpy(plyfile,(char *)qUtf8Printable(qs_plyfile));

   //Remove .ply extension
   plyfile[len-4] = '\0';

   //Remove _Mesh if found
   if(!strcmp(plyfile+len-9,"_Mesh"))
      plyfile[len-9] = '\0';
}

//Get date/time
void get_date_time(char *dt){
   time_t my_time = time(NULL);
   string date_time = ctime(&my_time);
   date_time.erase(date_time.end()-1);
   strcpy(dt,date_time.c_str());
}

//Main code here
bool VirtualGoniometerFilterPlugin::applyFilter(QAction *action, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos * /*cb*/)
{
   if (md.mm() == NULL)
      return false;

   MeshModel &m=*(md.mm());
   CMeshO::VertexIterator vi;

   //Requirements
   tri::RequirePerVertexNormal(m.cm);
   tri::UpdateNormal<CMeshO>::PerVertexNormalized(m.cm);

   //Get number of selected vertices
   int num_selected_pts = tri::UpdateSelection<CMeshO>::VertexCount(m.cm);

   //Number of vertices in whole mesh
   int num_verts = m.cm.vert.size();

   //Variables for virtual goniometer
   float fit;
   vector<float> vecx, vecy, vecz, normalx, normaly, normalz;
   float theta[4];
   float meanx, meany, meanz, surf_meanx, surf_meany, surf_meanz, radius;
   vector<unsigned int> indices;
   
   //General variables
   static bool first_VG = TRUE;
   char out_file[1000];
   FILE *pFile;
   int i,j,k;

   //Get *.ply mesh name
   static char plyfile[1000];
   char tmp[1000];
   mesh_name(m,tmp);
   //If user switched to a new mesh, reset the goniometer
   if(strcmp(plyfile,tmp)){
      strcpy(plyfile,tmp);
      first_VG = TRUE;
   }

   //Get date/time stamp
   char date_time[1000];
   get_date_time(date_time);

   //Format output filename
   sprintf(out_file,"%s/VirtualGoniometer_Measurements.csv",getenv("HOME"));
  
   //Check if file exists or not and write header if it does not exist
   pFile = fopen(out_file,"r");
   if(pFile == NULL){
      pFile = fopen(out_file,"w");
      fprintf(pFile,"Mesh Name,Date,Measurement #,Break #,Colors,User Data,Angle,Number of Vertices,Radius,x,y,z,fit,SegParam\n");
   }
   fclose(pFile);

   //Internal state of goniometer
   static int break_number = 1; 
   static int measurement_number = 1;
   static vector<int> num_lines(1000,0);
   static vector<int> past_break_numbers(1000,1);
   static vector<int> past_selection(num_verts,0); 
   static vector<int> past_selection_break(num_verts,0); 

   //Colors used in VG
   vector<Color4b> VGcolors{Color4b::Red,Color4b::Blue,Color4b::Green,Color4b::Magenta,Color4b::Yellow,Color4b::Cyan,Color4b::Black};
   vector<char*> ColorNames{"Red","Blue","Green","Magenta","Yellow","Cyan","Black"};
   int num_colors = VGcolors.size();
   static int num_color_combos;
   static vector<Color4b> Color1;
   static vector<Color4b> Color2;
   static vector<char*> Color1_name;
   static vector<char*> Color2_name;

   //Runs first time VG called
   if(first_VG){           
      //Set color of whole mesh to light gray
      tri::UpdateColor<CMeshO>::PerVertexConstant(m.cm, Color4b::LightGray, FALSE);

      //Set up colors for virtual goniometer
      for(i=0; i<num_colors-1; i++){
         for(j=i+1; j<num_colors; j++){
            Color1.push_back(VGcolors[i]);   
            Color2.push_back(VGcolors[j]);   
            Color1_name.push_back(ColorNames[i]);
            Color2_name.push_back(ColorNames[j]);
         }
      }
      num_color_combos = Color1.size();

      break_number = 1;
      measurement_number = 1;

      past_selection.resize(num_verts,0);
      fill(past_selection.begin(),past_selection.end(),0);

      past_selection_break.resize(num_verts,0);
      fill(past_selection_break.begin(),past_selection_break.end(),0);

      fill(num_lines.begin(),num_lines.end(),0);
      fill(past_break_numbers.begin(),past_break_numbers.end(),1);

      first_VG = FALSE;
   }

   switch(ID(action))
   {
      case FP_QUALITY_VIRTUAL_GONIOMETER_RESET:
      {
         Log("Reseting Virtual Goniometer.\n");
         break_number = 1;
         measurement_number = 1;

         past_selection.resize(num_verts,0);
         fill(past_selection.begin(),past_selection.end(),0);

         past_selection_break.resize(num_verts,0);
         fill(past_selection_break.begin(),past_selection_break.end(),0);

         fill(past_break_numbers.begin(),past_break_numbers.end(),1);
         fill(num_lines.begin(),num_lines.end(),0);

         tri::UpdateColor<CMeshO>::PerVertexConstant(m.cm, Color4b::LightGray, FALSE);

         bool loadcsv = par.getBool("Load");
         if(loadcsv){
            ifstream myFile(out_file);
            string str;
            vector<string> file_contents;
            vector<bool> matching;
            int num_matching = 0;
            int session_number = 0;

            if(!myFile.is_open()){
               printf("Cannot open CSV file %s\n", out_file);
            }
            else{
               while(getline(myFile, str)){
                  file_contents.push_back(str);
                  
                  stringstream ss(str);
                  vector<string> csv_line;
                  while(ss.good()){
                     string substr;
                     getline(ss, substr, ',');
                     csv_line.push_back(substr);
                  }
                  //If meshname matches
                  if(!strcmp(csv_line[0].c_str(),plyfile)){
                     //If start of new session
                     if(abs(stof(csv_line[2])-1.0) < 0.001)
                        session_number++;
                     //Only record from first session
                     if(session_number == 1){
                        num_matching++;
                        matching.push_back(TRUE);
                     }else
                        matching.push_back(FALSE);
                  }else
                     matching.push_back(FALSE);
               }
               myFile.close();

               if(num_matching == 0){
                  Log("Found no measurements for this mesh in the CSV file.\n");
                  break;
               }
               Log("Loading %d measurements from CSV file...\n",num_matching);

               //Reorganize csv file so matching measurements are at the end
               ofstream fout;
               fout.open(out_file);
               for(i=0; i<file_contents.size(); i++){
                  if(!matching[i])
                     fout << file_contents[i] << endl;
               }
               for(i=0; i<file_contents.size(); i++){
                  if(matching[i])
                     fout << file_contents[i] << endl;
               }
               fout.close();

               //Set up Virtual Goniometer based on found measurements
               for(i=0; i<file_contents.size(); i++){
                  if(matching[i]){
                     stringstream ss(file_contents[i]);
                     vector<string> csv_line;
                     while(ss.good()){
                        string substr;
                        getline(ss, substr, ',');
                        csv_line.push_back(substr);
                     }
                     float frac_measurement_number = stof(csv_line[2]);
                     measurement_number = stoi(csv_line[2]);
                     break_number = stoi(csv_line[3]);
                     float angle = stof(csv_line[6]);
                     float radius = stof(csv_line[8]);
                     float x = stof(csv_line[9]);
                     float y = stof(csv_line[10]);
                     float z = stof(csv_line[11]);

                     Log("Break #%d, Radius=%.1f, Angle = %.0f\n", break_number, radius, angle);

                     int ind = 0;
                     float min_dist = 0.0;
                     Point3m location(x,y,z);
                     for(j=0; j < num_verts; j++){
                        float dist = Distance(m.cm.vert[j].cP(),location);
                        if(dist < min_dist || j==0){
                           min_dist = dist;
                           ind = j;
                        }
                     }
                    
                     //Find neighborhood
                     /*VertexConstDataWrapper<CMeshO> wrapper(m.cm);
                     KdTree<typename CMeshO::ScalarType> tree(wrapper);

                     vector<float> dists;
                     vector<unsigned int> points;
                     tree.doQueryDist(m.cm.vert[ind].cP(),radius,points,dists);*/

                     std::vector<CMeshO::VertexPointer> NotReachableVector;
                     std::vector<CMeshO::VertexPointer> BorderVector;
                     std::vector<CMeshO::VertexPointer> ComponentVector;
                     CMeshO::VertexPointer StartingVertex = &(m.cm.vert[ind]);
                     NotReachableVector.clear();
                     ComponentVector.clear();
                     float maxHop = m.cm.bbox.Diag() / 100.0;
                     tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *StartingVertex, 6, maxHop, NotReachableVector);

                     //Get component as list of points
                     ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, radius, BorderVector, NotReachableVector);
                     tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
                     for(int ii=0;ii<ComponentVector.size();ii++)
                        ComponentVector[ii]->SetS();
                     vector<unsigned int> points = get_selected_indices(m);
                     tri::UpdateSelection<CMeshO>::VertexClear(m.cm);

                     get_vertices(m, points, vecx, vecy, vecz);
                     get_normals(m, points, normalx, normaly, normalz);
                     vector<int> C = ClusterPatch(vecx, vecy, vecz, normalx, normaly, normalz, m.cm.vert[ind].P()[0], m.cm.vert[ind].P()[1], m.cm.vert[ind].P()[2], radius);

                     for(k=0;k<points.size();k++){//Set past selection to record patch
                        past_selection[points[k]] = measurement_number;
                        past_selection_break[points[k]] = break_number;
                     }
                     color_patch(m, subset_indices(points,C,1), Color1[(break_number-1)%num_color_combos]);
                     color_patch(m, subset_indices(points,C,2), Color2[(break_number-1)%num_color_combos]);

                     num_lines[measurement_number]++;
                     past_break_numbers[measurement_number]=break_number;
                  }
               }
               measurement_number++;
               if(break_number >= 2){
                  for(i=1; i<break_number; i++)
                     change_contrast_patch(m, subset_indices(past_selection_break,i), 0.4);
               }
            }
         }
         break;
      }
      case FP_QUALITY_VIRTUAL_GONIOMETER_NEXT:
      {
         //Increment break number
         break_number++;

         //Print message to log
         Log("Moving to Break Edge #%d\n",break_number);
         this->RealTimeLog(QString("Virtual Goniometer"),m.shortName(),"Moving to Break Edge #%d\n",break_number);

         //Gray out old measurements
         change_contrast_patch(m, subset_indices(past_selection_break,break_number-1), 0.4);
         break;
      }
      case FP_QUALITY_VIRTUAL_GONIOMETER_UNDO:
      {
         if(measurement_number >=1 && break_number != past_break_numbers[measurement_number-1]){

            Log("Reverting back to break #%d\n",break_number-1);
            break_number--;
            change_contrast_patch(m, subset_indices(past_selection_break,break_number), 1.0/0.4);

         }else if(measurement_number > 1){

            Log("Undoing virtual goniometer measurement.\n");
            measurement_number--;
            indices = subset_indices(past_selection, measurement_number);
            color_patch(m, indices, Color4b::LightGray);
            for(i=0; i<indices.size(); i++){
               past_selection_break[indices[i]] = 0;
               past_selection[indices[i]] = 0;
            }
            remove_last_lines(out_file,num_lines[measurement_number]);

         }else{
            Log("No measurements to undo!\n");
         }

         break;
      }
      case FP_QUALITY_VIRTUAL_GONIOMETER:
      {
         //Action is based on how many vertices are selected (0,1,>1)
         //If no points are selected
         if (num_selected_pts <= 10) { 



		      Point3m location =  par.getPoint3m("Location");
            SegParam = par.getDynamicFloat("SegParam");
            bool NoSeg = par.getBool("UpdateParam");
            if(NoSeg)
               break;

            float radius = par.getFloat("Radius");
            bool automatic = par.getBool("Automatic Radius");
            float a = Distance(m.cm.vert[0].cP(),location);
            int ind = 0;
            for(j=0; j < num_verts; j++){
               float dist = Distance(m.cm.vert[j].cP(),location);
               if(dist < a){
                  a = dist;
                  ind = j;
               }
            }

            if(automatic){
               tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
               m.cm.vert[ind].SetS();
               num_selected_pts = 1;
            }else{

               //Dijkstra to find component of radius radius
               std::vector<CMeshO::VertexPointer> NotReachableVector;
               std::vector<CMeshO::VertexPointer> BorderVector;
               std::vector<CMeshO::VertexPointer> ComponentVector;
               CMeshO::VertexPointer StartingVertex = &(m.cm.vert[ind]);
               NotReachableVector.clear();
               ComponentVector.clear();
               float maxHop = m.cm.bbox.Diag() / 100.0;
               tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *StartingVertex, 6, maxHop, NotReachableVector);
               ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, radius, BorderVector, NotReachableVector);


               /*VertexConstDataWrapper<CMeshO> wrapper(m.cm);
               KdTree<typename CMeshO::ScalarType> tree(wrapper);*/
               
               //Find neighborhood
               //vector<float> dists;
               //vector<unsigned int> points;
               //tree.doQueryDist(m.cm.vert[ind].cP(),radius,points,dists);

               if(ComponentVector.size() <= 10){
                  Log("Radius too small.");
                  break;
               }else{
                  num_selected_pts = ComponentVector.size();

                  //Make selection
                  tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
                  /*for(i=0;i<points.size();i++)
                     m.cm.vert[points[i]].SetS();*/
                  for(i=0;i<ComponentVector.size();i++)
                     ComponentVector[i]->SetS();
               }
            }
         }

         if(num_selected_pts <= 10){ //Then compute for several different radii

            //Number of radii to use
            int num_radii = 20;

            //Build KdTree
            VertexConstDataWrapper<CMeshO> wrapper(m.cm);
            KdTree<typename CMeshO::ScalarType> tree(wrapper);
            KdTree<typename CMeshO::ScalarType>::PriorityQueue queue;

            //Find index of selected point
            i = index_first_selected(m);

            //Find minimum/maximum radius
            float rmin = 0.0, rmax = 0.0;
            tree.doQueryK(m.cm.vert[i].cP(),20,queue);
            for(j=0; j < queue.getNofElements(); j++)
               rmin = MAX(rmin,Distance(m.cm.vert[i].cP(),m.cm.vert[queue.getIndex(j)].cP()));
            tree.doQueryK(m.cm.vert[i].cP(),(int)(num_verts/200),queue);
            for(j=0; j < queue.getNofElements(); j++)
               rmax = MAX(rmax,Distance(m.cm.vert[i].cP(),m.cm.vert[queue.getIndex(j)].cP()));
            
            //Geodesic distance function
            std::vector<CMeshO::VertexPointer> NotReachableVector;
            std::vector<CMeshO::VertexPointer> BorderVector;
            std::vector<CMeshO::VertexPointer> ComponentVector;
            CMeshO::VertexPointer StartingVertex = &(m.cm.vert[i]);
            NotReachableVector.clear();
            float maxHop = m.cm.bbox.Diag() / 100.0;
            tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *StartingVertex, 6, maxHop, NotReachableVector);

            //Loop over radii
            float change = (rmax-rmin)/(num_radii-1), rad = rmin;
            for(j=0;j<num_radii;j++){

               //Find neighborhood
               /*vector<float> dists;
               vector<unsigned int> points;
               tree.doQueryDist(m.cm.vert[i].cP(),rad,points,dists);*/
               
               //Get component as list of points
               ComponentVector.clear();
               ComponentVector = tri::ComponentFinder<CMeshO>::FindComponent(m.cm, rad, BorderVector, NotReachableVector);
               tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
               for(int ii=0;ii<ComponentVector.size();ii++)
                  ComponentVector[ii]->SetS();
               vector<unsigned int> points = get_selected_indices(m);
               tri::UpdateSelection<CMeshO>::VertexClear(m.cm);

               //Run virtual goniometer
               get_vertices(m, points, vecx, vecy, vecz);
               get_normals(m, points, normalx, normaly, normalz);
               vector<int> C = VirtualGoniometer(vecx, vecy, vecz, normalx, normaly, normalz, m.cm.vert[i].P()[0], m.cm.vert[i].P()[1], m.cm.vert[i].P()[2], rad, theta, &fit);

               //Color mesh for largest
               //if(j==(int)(num_radii/2)){
               if(j==num_radii-1){
                  for(k=0;k<points.size();k++){//Set past selection to record patch
                     past_selection[points[k]] = measurement_number;
                     past_selection_break[points[k]] = break_number;
                  }
                  color_patch(m, subset_indices(points,C,1), Color1[(break_number-1)%num_color_combos]);
                  color_patch(m, subset_indices(points,C,2), Color2[(break_number-1)%num_color_combos]);
               }

               Log("Break #%d, Radius=%.1f, Angle = %.0f, Fit = %.4f\n", break_number, rad, theta[0], fit);
               //this->RealTimeLog(QString("Virtual Goniometer"),m.shortName(),"Break #%d, Radius=%.1f, Angle = %.0f\n", break_number, radius, theta[0]);

               float frac_measurement_number = measurement_number + (float)j/(float)num_radii;

               //Output to csv file 
               pFile = fopen(out_file,"a");
               fprintf(pFile,"%s,%s,%.2f,%d,%s/%s, ,%f,%d,%f,%f,%f,%f,%f,%f\n",plyfile,date_time,frac_measurement_number,break_number,Color1_name[(break_number-1)%num_color_combos],Color2_name[(break_number-1)%num_color_combos],theta[0],points.size(),rad,m.cm.vert[i].P()[0],m.cm.vert[i].P()[1],m.cm.vert[i].P()[2],fit,SegParam);
               fclose(pFile);

               rad+=change;
            }
            num_lines[measurement_number]=num_radii;
            past_break_numbers[measurement_number]=break_number;
            measurement_number++;
            break;
         }
         //Else if num_selected_pts > 1 then compute on the users selection
         

         //Get selected indices and clear selection  
         indices = get_selected_indices(m);

         //Run virtual goniometer
         get_vertices(m, indices, vecx, vecy, vecz);
         get_normals(m, indices, normalx, normaly, normalz);
         int center_ind = 0;
         geodesic_patch_info(m, &radius, &center_ind);
         tri::UpdateSelection<CMeshO>::VertexClear(m.cm);
         surf_meanx = m.cm.vert[center_ind].P()[0];
         surf_meany = m.cm.vert[center_ind].P()[1];
         surf_meanz = m.cm.vert[center_ind].P()[2];
         //patch_statistics(vecx, vecy, vecz, &meanx, &meany, &meanz, &surf_meanx, &surf_meany, &surf_meanz, &radius);
         vector<int> C = VirtualGoniometer(vecx, vecy, vecz, normalx, normaly, normalz,  surf_meanx, surf_meany, surf_meanz, radius, theta, &fit);



         //Color mesh
         for(k=0;k<indices.size();k++){//Set past selection to record patch
            past_selection[indices[k]] = measurement_number;
            past_selection_break[indices[k]] = break_number;
         }
         color_patch(m, subset_indices(indices,C,1), Color1[(break_number-1)%num_color_combos]);
         color_patch(m, subset_indices(indices,C,2), Color2[(break_number-1)%num_color_combos]);

         //Print angle to log
         Log("Break #%d, Radius=%.1f, Angle = %.0f, Fit = %.4f\n", break_number, radius, theta[0], fit);
         this->RealTimeLog(QString("Virtual Goniometer"),m.shortName(),"Break #%d, Radius=%.1f, Angle = %.0f, Fit = %.4f\n", break_number, radius, theta[0], fit);

         //Output to csv file 
         pFile = fopen(out_file,"a");
         fprintf(pFile,"%s,%s,%d,%d,%s/%s, ,%f,%d,%f,%f,%f,%f,%f,%f\n",plyfile,date_time,measurement_number,break_number,Color1_name[(break_number-1)%num_color_combos],Color2_name[(break_number-1)%num_color_combos],theta[0],num_selected_pts,radius,surf_meanx,surf_meany,surf_meanz,fit,SegParam);
         fclose(pFile);

         //Increment measurement number
         num_lines[measurement_number]=1;
         past_break_numbers[measurement_number]=break_number;
         measurement_number++;

      } break;

      default: assert(0);
   }
   return true;
}

MeshFilterInterface::FilterClass VirtualGoniometerFilterPlugin::getClass(QAction *action)
{
  switch(ID(action))
  {
	   case FP_QUALITY_VIRTUAL_GONIOMETER: return FilterClass(MeshFilterInterface::Quality);
	   case FP_QUALITY_VIRTUAL_GONIOMETER_NEXT: return FilterClass(MeshFilterInterface::Quality);
	   case FP_QUALITY_VIRTUAL_GONIOMETER_UNDO: return FilterClass(MeshFilterInterface::Quality);
	   case FP_QUALITY_VIRTUAL_GONIOMETER_RESET: return FilterClass(MeshFilterInterface::Quality);
  }
  return MeshFilterInterface::Selection;
}

 int VirtualGoniometerFilterPlugin::getRequirements(QAction *action)
{
 switch(ID(action))
  {
     case FP_QUALITY_VIRTUAL_GONIOMETER: return MeshModel::MM_VERTCOLOR;
     case FP_QUALITY_VIRTUAL_GONIOMETER_NEXT: return MeshModel::MM_VERTCOLOR;
     case FP_QUALITY_VIRTUAL_GONIOMETER_UNDO: return MeshModel::MM_VERTCOLOR;
     case FP_QUALITY_VIRTUAL_GONIOMETER_RESET: return MeshModel::MM_VERTCOLOR;

	  default: return MeshModel::MM_NONE;
  }
}

int VirtualGoniometerFilterPlugin::postCondition(QAction *action) const
{
	switch(ID(action))
	{
   }
  return MeshModel::MM_ALL;
}

int VirtualGoniometerFilterPlugin::getPreConditions( QAction * action) const
{
  switch(ID(action))
  {
  }
  return 0;
}
MESHLAB_PLUGIN_NAME_EXPORTER(VirtualGoniometerFilterPlugin)
