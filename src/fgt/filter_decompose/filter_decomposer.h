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
$Log: sampleplugins.h,v $
Revision 1.2  2006/11/29 00:59:21  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.1  2006/09/25 09:24:39  e_cerisoli
add sampleplugins

****************************************************************************/

#ifndef SAMPLEFILTERSPLUGIN_H
#define SAMPLEFILTERSPLUGIN_H

#include <QObject>

#include <common/interfaces.h>


template<class MeshType>
class FunctorType{
   typedef typename MeshType::FaceType FaceType;
public:
    double operator() (vcg::face::Pos<FaceType>);
};

//template<class MeshType>
//class WeightFunctor{
//    typedef typename MeshType::FacePointer FacePointer;
//    typedef typename MeshType::FaceType FaceType;
//    typedef typename MeshType::ScalarType ScalarType;
//    typename MeshType::template PerFaceAttributeHandle<WeightFunctor::ScalarType>;

//private: float _ambient_weight, _dihedral_weight, _elength_weight, _geodesic_weight, _max1, _max2, _maxq, _geomin, _geomax;
//         FacePointer _start,_end;
//         MeshType::template PerFaceAttributeHandle<WeightFunctor::ScalarType> _nfH1, _nfH2;
//         double _totmax;
//public:
//      WeightFunctor(float aow, float dw, float geodesic, float elength, FaceType start, FaceType end,  typename MeshType::template PerFaceAttributeHandle<float> nfH1,  typename MeshType::template PerFaceAttributeHandle<float> nfH2, float max1, float max2, float geomin, float geomax, float maxq)
//      {
//          if(max1<0 || max2<0 || maxq<0 || elength<0 || start==NULL ||
//                  end==NULL ||aow<0 || dw<0 || geodesic <0 || geomin<0 || geomax<0)
//              assert(0);

//          _max1 = max1;
//          _max2 = max2;
//          _maxq = maxq;
//          _geomin = geomin;
//          _geomax = geomax;
//          _totmax = std::pow(_geomax*max1, 1.2f) + std::pow(_geomax*max2, 1.2f);
//          _nfH1 = nfH1;
//          _nfH2 = nfH2;
//          _start = start;
//          _end = end;
//          _elength_weight = elength;
//          _ambient_weight = aow;
//          _dihedral_weight = dw;
//          _geodesic_weight = geodesic;
//      }

//    /* Computes the weight of the edge f1->f2 using the float parameters as coefficients for the expression:
//     * f1= pos.f
//     * f2= pos.FFlip

//     * alpha * f1->Q() + beta * f1\/f2 + delta * f1<-->start/end
//     * where:

//     * f1->Q() = ambient occlusion value for the face f1
//     ** The final weight of the edge should be lower for faces with higher ambient occlusion value.

//     * f1\/f2 = dihedral angle between face f1 and f2: concave edges are priviledged.
//     ** The final weight of the edge should be higher for convex edges and lower for concave ones.

//     * f1<-->start/end = geodesic distance from the current face to the represantative faces
//                         chosen by the user.
//     **
//     ** weighted also on the length of the edge shared by f1 and f2.
//    */
//    double operator() (vcg::face::Pos<FaceType> pos)
//    {
//        return ((_elength_weight * vcg::Norm(pos.V()->P() - pos.VFlip()->P())) + ( _geodesic_weight * GeodesicDistance(pos)) + (_ambient_weight * (pos.f->Q()+pos.FFlip()->Q())/2) + (_dihedral_weight * AngleDistance(pos)));
//    }

//    double GeodesicDistance(vcg::face::Pos<FaceType> pos)
//    {
//        //if the current face is too close to either the startFace or the endFace
//        //the value _totmax is returned, which makes sure the final weight will be high
//        //and the edge won't be considered for a minimumcut.
//        //this is useful to avoid trivial cuts.
//        if(_nfH1[pos.f] <= _max1*_geomin || _nfH1[pos.f] >= _max1*_geomax)
//            return _totmax;
//        if(_nfH2[pos.f] <= _max2*_geomin || _nfH2[pos.f] >= _max2*_geomax)
//            return _totmax;
//        return (std::pow(_nfH1[pos.f], 1.2f) + std::pow(_nfH2[pos.f], 1.2f))/_totmax;
//    }

//    double AngleDistance(vcg::face::Pos<FaceType> pos){
//        return (M_PI + vcg::face::DihedralAngleRad<FaceType>(*(pos.f), pos.E()))/2*M_PI;
//    }
//};


class QScriptEngine;

class ExtraSamplePlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshFilterInterface)

public:
	enum {
	  FP_MOVE_VERTEX,
      FP_TOPOLOGICAL_CUT,
      FP_DECOMPOSER
		 } ;

	ExtraSamplePlugin();

	virtual QString pluginName(void) const { return "ExtraSamplePlugin"; }

	QString filterName(FilterIDType filter) const;
	QString filterInfo(FilterIDType filter) const;
	void initParameterSet(QAction *,MeshModel &/*m*/, RichParameterSet & /*parent*/);
    bool applyFilter(QAction *filter, MeshDocument &md, RichParameterSet & /*parent*/, vcg::CallBackPos * cb) ;
    int postCondition( QAction* ) const {return MeshModel::MM_VERTCOORD | MeshModel::MM_FACENORMAL | MeshModel::MM_VERTNORMAL | MeshModel::MM_FACECOLOR;}
    FilterClass getClass(QAction *a);
	QString filterScriptFunctionName(FilterIDType filterID);

};


#endif
