/****************************************************************************
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

#include "filter_autoalign.h"
#include <vcg/complex/algorithms/autoalign_4pcs.h>
#include "../../meshlabplugins/edit_align/align/Guess.h"

using namespace vcg;
using namespace std;

// Constructor usually performs only two simple tasks of filling the two lists
//  - typeList: with all the possible id of the filtering actions
//  - actionList with the corresponding actions. If you want to add icons to your filtering actions you can do here by construction the QActions accordingly

FilterAutoalign::FilterAutoalign()
{
  typeList << FP_ALIGN_4PCS
           << FP_BEST_ROTATION
           ;

  foreach(FilterIDType tt , types())
    actionList << new QAction(filterName(tt), this);
}

// ST() must return the very short string describing each filtering action
// (this string is used also to define the menu entry)
QString FilterAutoalign::filterName(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_ALIGN_4PCS    :  return QString("Automatic Rough Alignement (4PCS)");
    case FP_BEST_ROTATION :  return QString("Automatic Rough Alignement (Brute Force)");
    default : assert(0);
  }
}

// Info() must return the longer string describing each filtering action
// (this string is used in the About plugin dialog)
QString FilterAutoalign::filterInfo(FilterIDType filterId) const
{
  switch(filterId) {
    case FP_ALIGN_4PCS     :  return QString("Automatic Rough Alignment of two meshes. Based on the paper <b> 4-Points Congruent Sets for Robust Pairwise Surface Registration</b>, by Aiger,Mitra, Cohen-Or. Siggraph 2008  ");
    case FP_BEST_ROTATION :  return QString("Automatic Rough Alignment of two meshes. Brute Force Approach");
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
void FilterAutoalign::initParameterSet(QAction *action,MeshDocument & md/*m*/, RichParameterSet & parlst)
{
   MeshModel *target;
   switch(ID(action))	 {
    case FP_ALIGN_4PCS :
        target= md.mm();
        foreach (target, md.meshList)
            if (target != md.mm())  break;

        parlst.addParam(new RichMesh ("fixMesh", md.mm(),&md, "Fixed Mesh",
                        "The mesh were the coplanar bases are sampled (it will contain the trasformation)"));
        parlst.addParam(new RichMesh ("movMesh", target,&md, "Moving Mesh",
                        "The mesh were similar coplanar based are searched."));
        parlst.addParam(new RichFloat("overlap",0.5f,"Overlap %","Estimated fraction of the\n first mesh overlapped by the second"));
        parlst.addParam(new RichInt("sampleNum",1000,"Sample Num"));
        parlst.addParam(new RichFloat("tolerance",0.3f,"Error tolerance",""));
        parlst.addParam(new RichBool("showSample",true,"show subsamples",""));
        parlst.addParam(new RichInt("randSeed",0,"Random Seed","0 means the random generator it is intializised with internal clock to guarantee different result every time"));
     break;

       case FP_BEST_ROTATION :
       target= md.mm();
       foreach (target, md.meshList)
       if (target != md.mm())  break;

       parlst.addParam(new RichMesh ("fixMesh", md.mm(),&md, "Fix Mesh",
                       "The mesh that will be moved"));
       parlst.addParam(new RichMesh ("movMesh", target,&md, "Mov Mesh",
                       "The mesh that will be kept fixed."));
       parlst.addParam(new RichInt("searchRange",6,"Search Range", "The size of the uniform grid that is used for searching the best translation for a given rotation"));
       parlst.addParam(new RichInt("sampleSize",5000,"Sample Size", "The size of the uniform grid that is used for searching the best translation for a given rotation"));
       parlst.addParam(new RichInt("gridSize",100000,"Grid Size", "The size of the uniform grid that is used for searching the best translation for a given rotation"));
       parlst.addParam(new RichInt("RotationNumber",64,"Rotation Number", "sss"));
       break;

   default: break; // do not add any parameter for the other filters
  }
}

// The Real Core Function doing the actual mesh processing.
bool FilterAutoalign::applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
  switch(ID(filter)) {
    case FP_ALIGN_4PCS :
    {
      MeshModel *fixMesh= par.getMesh("fixMesh");
      MeshModel *movMesh= par.getMesh("movMesh");
      MeshModel *sampleMesh= 0;
      bool showSample = par.getBool("showSample");
      if(showSample) sampleMesh = md.addOrGetMesh("sample","sample",false);
      tri::UpdateNormal<CMeshO>::NormalizePerVertex(fixMesh->cm);
      tri::UpdateNormal<CMeshO>::NormalizePerVertex(movMesh->cm);
      tri::Clean<CMeshO>::RemoveUnreferencedVertex(fixMesh->cm);
      tri::Clean<CMeshO>::RemoveUnreferencedVertex(movMesh->cm);
      tri::Allocator<CMeshO>::CompactEveryVector(fixMesh->cm);
      tri::Allocator<CMeshO>::CompactEveryVector(movMesh->cm);
      fixMesh->updateDataMask(MeshModel::MM_VERTMARK);
      movMesh->updateDataMask(MeshModel::MM_VERTMARK);

      vcg::tri::FourPCS<CMeshO> fpcs;
      fpcs.par.Default();
      fpcs.par.overlap    =  par.getFloat("overlap");
      fpcs.par.sampleNumP =  par.getInt("sampleNum");
      fpcs.par.deltaPerc  =  par.getFloat("tolerance");
      fpcs.par.seed       = par.getInt("randSeed");
      fpcs.Init(movMesh->cm,fixMesh->cm);
      Matrix44m Tr;
      bool res = fpcs.Align(Tr,cb);

      if(res)
      {
        Log("4PCS Completed, radius %f",fpcs.par.samplingRadius);
        Log("Tested %i candidate, best score was %i\n",fpcs.U.size(),fpcs.U[fpcs.iwinner].score);

        Log("Estimated overlap is now %f \n",fpcs.par.overlap);
        Log("Init %5.0f Coplanar Search %5.0f",fpcs.stat.init(),fpcs.stat.select());
        Log("findCongruent %5.0f testAlignment %5.0f",fpcs.stat.findCongruent(),fpcs.stat.testAlignment());
        movMesh->cm.Tr = Tr;
        if(showSample)
        {
          sampleMesh->cm.Clear();
          for(size_t i=0;i<fpcs.subsetQ.size();++i)
            tri::Allocator<CMeshO>::AddVertex(sampleMesh->cm, fpcs.subsetQ[i]->P(), fpcs.subsetQ[i]->N());
          tri::UpdateBounding<CMeshO>::Box(sampleMesh->cm);
        }
      }
      else Log("4PCS Failed");
    } break;



    case FP_BEST_ROTATION :
    {
      MeshModel *fixMesh= par.getMesh("fixMesh");
      MeshModel *movMesh= par.getMesh("movMesh");
      int searchRange = par.getInt("searchRange");
      int rotNum = par.getInt("RotationNumber");
      int gridSize = par.getInt("gridSize");
      int sampleSize = par.getInt("sampleSize");
      MeshModel *sample=md.addOrGetMesh("sample", "sample",false);
      MeshModel *occ=md.addOrGetMesh("occ", "occ",false);

      tri::Guess<Scalarm> GG;
      std::vector<tri::Guess<Scalarm>::Result> ResultVec;
      GG.pp.MatrixNum = rotNum;
      GG.pp.GridSize =gridSize;
      GG.pp.SampleNum = sampleSize;
      GG.Init<CMeshO>(fixMesh->cm, movMesh->cm);

      for(size_t i=0;i<GG.RotMVec.size();++i)
      {
        Point3m baseTran =  GG.ComputeBaseTranslation(GG.RotMVec[i]);
        Point3m bestTran;
        int res = GG.SearchBestTranslation(GG.u[0],GG.RotMVec[i],searchRange,baseTran,bestTran);
        ResultVec.push_back(tri::Guess<Scalarm>::Result(GG.BuildResult(GG.RotMVec[i],baseTran,bestTran), res, i, bestTran));
      }
      sort(ResultVec.begin(),ResultVec.end());
      movMesh->cm.Tr.Import(ResultVec.back().m);

      tri::BuildMeshFromCoordVector(sample->cm,GG.movVertBase);
      sample->cm.Tr.Import(ResultVec.back().m);

      qDebug("Result %i",ResultVec.back().score);
      GG.GenerateOccupancyMesh(occ->cm,0,ResultVec.back().m);
      occ->UpdateBoxAndNormals();

      Log("Automatic Rough Alignment Tested %i rotations, best err was %i",GG.RotMVec.size(), ResultVec.back().score);
    } break;
    default: assert (0);
  }
  return true;
}

FilterAutoalign::FilterClass FilterAutoalign::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_ALIGN_4PCS :
    case FP_BEST_ROTATION :
      return FilterClass(MeshFilterInterface::Layer+MeshFilterInterface::RangeMap);
    default :
      return MeshFilterInterface::Generic;
  }
}

MESHLAB_PLUGIN_NAME_EXPORTER(FilterAutoalign)
