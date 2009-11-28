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
               AF_RANSAC_DIAGRAM
            };

        FilterFeatureAlignment();
        ~FilterFeatureAlignment();

        virtual FilterClass getClass(QAction *);
        virtual QString filterName(FilterIDType filter) const;
        virtual QString filterInfo(FilterIDType filter) const;
        virtual int getRequirements(QAction *);
        virtual bool autoDialog(QAction *);
        virtual void initParameterSet(QAction *,MeshDocument &/*m*/, FilterParameterSet & /*parent*/);
        virtual bool applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & /*parent*/, CallBackPos * cb) ;
        virtual bool applyFilter(QAction */*filter*/, MeshModel &, FilterParameterSet & /*parent*/, CallBackPos *) { assert(0); return false;}

    private:
        template<class ALIGNER_TYPE>
        static void setAlignmentParameters(typename ALIGNER_TYPE::MeshType& mFix, typename ALIGNER_TYPE::MeshType& mMov, FilterParameterSet& par, typename ALIGNER_TYPE::Parameters& param);

        template<class ALIGNER_TYPE>
        static bool logResult(FilterIDType filter, typename ALIGNER_TYPE::Result& res, QString& errorMsg);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static bool ComputeFeatureOperation(MeshModel& m, typename FEATURE_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class ALIGNER_TYPE>
        static bool ExtractionOperation(MeshModel& m, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class ALIGNER_TYPE>
        static typename ALIGNER_TYPE::Result MatchingOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class ALIGNER_TYPE>
        static typename ALIGNER_TYPE::Result RigidTransformationOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class CONSENSUS_TYPE>
        static float ConsensusOperation(MeshModel& mFix, MeshModel& mMov, typename CONSENSUS_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class ALIGNER_TYPE>
        static typename ALIGNER_TYPE::Result RansacOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class ALIGNER_TYPE>
        static typename ALIGNER_TYPE::Result RansacDiagramOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, int trials,int from, int to, int step, CallBackPos *cb=NULL);

};
#endif
