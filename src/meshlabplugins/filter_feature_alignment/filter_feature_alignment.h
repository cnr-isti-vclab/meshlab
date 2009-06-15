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

//---solo per funzioni colore. toglile quando portate in color.h---
#include <math.h>
#include <vcg/math/perlin_noise.h>
#include <vcg/math/random_generator.h>
//------------------------------------------------------------------

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
        virtual const QString filterName(FilterIDType filter) const;
        virtual const QString filterInfo(FilterIDType filter) const;
        virtual const int getRequirements(QAction *);
        virtual bool autoDialog(QAction *);
        virtual void initParameterSet(QAction *,MeshDocument &/*m*/, FilterParameterSet & /*parent*/);
        virtual bool applyFilter(QAction *filter, MeshDocument &md, FilterParameterSet & /*parent*/, CallBackPos * cb) ;
        virtual bool applyFilter(QAction */*filter*/, MeshModel &, FilterParameterSet & /*parent*/, CallBackPos *) { assert(0); return false;}

    private:
        template<class MESH_TYPE, class ALIGNER_TYPE>
        static void setAlignmentParameters(MESH_TYPE& mFix, MESH_TYPE& mMov, FilterParameterSet& par, typename ALIGNER_TYPE::Parameters& param);

        template<class MESH_TYPE, class FEATURE_TYPE>
        static bool ComputeFeatureOperation(MeshModel& m, typename FEATURE_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
        static bool ExtractionOperation(MeshModel& m, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
        static bool MatchingOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
        static int RigidTransformationOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
        static int ConsensusOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
        static bool RansacOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, CallBackPos *cb=NULL);

        template<class MESH_TYPE, class FEATURE_TYPE, class ALIGNER_TYPE>
        static bool RansacDiagramOperation(MeshModel& mFix, MeshModel& mMov, typename ALIGNER_TYPE::Parameters& param, int trials,int from, int to, int step, CallBackPos *cb=NULL);

        template<class MESH_TYPE>
        static void PerlinColor(MESH_TYPE& m, Box3f bbox, float freq)
        {
            typedef MESH_TYPE MeshType;
            typedef typename MeshType::ScalarType ScalarType;
            typedef typename MeshType::VertexIterator VertexIterator;

            Point3<ScalarType> p;                                    
            VertexIterator vi;
            for(vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
            {
                if(!(*vi).IsD()){
                    p = bbox.GlobalToLocal(m.Tr * (*vi).P());           //actual vertex position
                    //create and assign color
                    (*vi).C() = Color4b( int(255*math::Perlin::Noise(p[0]*freq,p[1]*freq,p[2]*freq)),
                                         int(255*math::Perlin::Noise(64+p[0]*freq,64+p[1]*freq,64+p[2]*freq)),
                                         int(255*math::Perlin::Noise(128+p[0]*freq,128+p[1]*freq,128+p[2]*freq)), 255 );
                }
            }
        }

        template<class MESH_TYPE>
        static void ColorNoise(MESH_TYPE& m, int noiseBits)
        {
            typedef MESH_TYPE MeshType;
            typedef typename MeshType::VertexIterator VertexIterator;

            if(noiseBits>8) noiseBits = 8;
            if(noiseBits<1) return;

            math::SubtractiveRingRNG randomGen =  math::SubtractiveRingRNG(time(NULL));
            VertexIterator vi;
            for(vi = m.vert.begin(); vi!=m.vert.end(); ++vi)
            {
                if(!(*vi).IsD()){
                    (*vi).C()[0] = math::Clamp<int>((*vi).C()[0] + randomGen.generate(int(2*pow(2.0f,noiseBits))) - int(pow(2.0f,noiseBits)),0,255);
                    (*vi).C()[1] = math::Clamp<int>((*vi).C()[1] + randomGen.generate(int(2*pow(2.0f,noiseBits))) - int(pow(2.0f,noiseBits)),0,255);
                    (*vi).C()[2] = math::Clamp<int>((*vi).C()[2] + randomGen.generate(int(2*pow(2.0f,noiseBits))) - int(pow(2.0f,noiseBits)),0,255);
                }
            }
        }
};
#endif
