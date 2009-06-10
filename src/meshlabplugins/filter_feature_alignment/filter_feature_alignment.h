/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2007                                                \/)\/    *
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

#ifndef FILTER_FEATUREALIGNMENTPLUGIN_H
#define FILTER_FEATUREALIGNMENTPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QString>

#include <meshlab/meshmodel.h>
#include <meshlab/interfaces.h>

using namespace std;
using namespace vcg;

class FilterFeatureAlignment : public QObject, public MeshFilterInterface
{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

    public:
        enum { AF_COMPUTE_FEATURE,
               AF_EXTRACTION,
               AF_MATCHING,
               AF_RIGID_TRANSFORMATION,
               AF_CONSENSUS,
               AF_RANSAC,
               AF_RANSAC_DIAGRAM,                          
               AF_DELETE_FEATURE,
               AF_PERLIN_COLOR,
               AF_COLOR_NOISE
            };

        FilterFeatureAlignment();
        ~FilterFeatureAlignment();

        virtual const FilterClass getClass(QAction *);
        virtual const QString filterName(FilterIDType filter);
        virtual const QString filterInfo(FilterIDType filter);
        virtual const int getRequirements(QAction *);
        virtual bool autoDialog(QAction *);
        virtual void initParameterSet(QAction *,MeshDocument &/*m*/, FilterParameterSet & /*parent*/);
        virtual bool applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & /*parent*/, CallBackPos * cb) ;
        virtual bool applyFilter(QAction */*filter*/, MeshModel &, FilterParameterSet & /*parent*/, CallBackPos *) { assert(0); return false;}

    private:
        template<class MESH_TYPE, class FEATURE_TYPE>
        static bool ComputeFeatureOperation(MeshModel& m, typename FEATURE_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static vector<FEATURE_TYPE*>* ExtractionOperation(int k, MeshModel& m, int samplingStrategy, bool pickPoints, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static bool MatchingOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, int k, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static int RigidTransformationOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, int k, CallBackPos *cb=NULL);

        template<class MESH_TYPE>
        static int ConsensusOperation(MeshModel& mFix, MeshModel& mMov, float consensusDist, int fullConsensusSamples, bool normEq, bool paint, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static bool RansacOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, int ransacIter, float consensusDist, int fullConsensusSamples, float overlap, float shortConsOffset, float consOffset, float succOffset, int k, int g, int samplingStrategy, bool pickPoints, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static bool RansacDiagramOperation(MeshModel& mFix, MeshModel& mMov, int numFixFeatureSelected, int numMovFeatureSelected, int nBase, float consensusDist, int fullConsensusSamples, float overlap, float shortConsOffset, float consOffset, float succOffset, int k, int g, int samplingStrategy, int trials,int from, int to, int step, CallBackPos *cb=NULL);
};
#endif
