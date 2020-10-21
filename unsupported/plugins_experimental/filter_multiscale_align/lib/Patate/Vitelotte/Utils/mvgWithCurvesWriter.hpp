/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "mvgWithCurvesWriter.h"


namespace Vitelotte
{


template < typename _Mesh >
MVGWithCurvesWriter<_Mesh>::MVGWithCurvesWriter(Version version)
    : Base(version) {
}


template < typename _Mesh >
void
MVGWithCurvesWriter<_Mesh>::write(std::ostream& _out, const Mesh& mesh) {
    Base::write(_out, mesh);

    for(unsigned pci = 0; pci < mesh.nPointConstraints(); ++pci)
    {
        PointConstraint pc(pci);

        if(mesh.isValueConstraint(pc))
        {
            _out << "pvc " << vertexIndex(mesh.vertex(pc))
                 << " " << mesh.value(pc).transpose().format(m_format) << "\n";
        }
        if(mesh.isGradientConstraint(pc))
        {
            _out << "pgc " << vertexIndex(mesh.vertex(pc))
                 << " " << mesh.gradient(pc).transpose().format(m_format) << "\n";
        }
//        _out << "pc " << (mesh.isValueConstraint(pc)? "o": "x") << " "
//             << (mesh.isGradientConstraint(pc)? "o": "x") << ";";
//        if(mesh.isValueConstraint(pc))
//        {
//            _out << " " << mesh.value(pc).transpose().format(m_format) << ";";
//        }
//        if(mesh.isGradientConstraint(pc))
//        {
//            _out << " " << mesh.gradient(pc).transpose().format(m_format) << ";";
//        }
//        _out << " " << vertexIndex(mesh.vertex(pc)) << "\n";
    }

    for(unsigned ci = 0; ci < mesh.nCurves(); ++ci)
    {
        Curve curve(ci);

        Halfedge h = mesh.firstHalfedge(curve);
        _out << "c " << vertexIndex(mesh.fromVertex(h)) << " " << mesh.fromCurvePos(h);
        while(h.isValid())
        {
            _out << " " << vertexIndex(mesh.toVertex(h)) << " " << mesh.toCurvePos(h);
            h = mesh.nextCurveHalfedge(h);
        }
        _out << "\n";

        if(mesh.bezierPath(curve).nPoints())
        {
            typedef typename Mesh::BezierPath BezierPath;
            const BezierPath& path = mesh.bezierPath(curve);
            _out << "bp " << ci << " M " << path.point(0).transpose().format(m_format);
            for(unsigned si = 0; si < path.nSegments(); ++si)
            {
                switch(path.type(si))
                {
                case BEZIER_LINEAR:    _out << " L"; break;
                case BEZIER_QUADRATIC: _out << " Q"; break;
                case BEZIER_CUBIC:     _out << " C"; break;
                case BEZIER_EMPTY:     continue;
                }
                for(unsigned pi = 1; pi < path.nPoints(si); ++pi)
                {
                    _out << " " << path.point(si, pi).transpose().format(m_format);
                }
            }
            _out << "\n";
        }

        if(mesh.valueTear(curve))    _out << "dcvTear " << ci << "\n";
        if(mesh.gradientTear(curve)) _out << "dcgTear " << ci << "\n";

        if(!mesh.valueFunction(curve, Mesh::VALUE_LEFT).empty())
        {
            _out << (mesh.valueTear(curve)? "dcvLeft ": "dcv ") << ci;
            writeValueFunction(_out, mesh.valueFunction(curve, Mesh::VALUE_LEFT));
            _out << "\n";
        }
        if(mesh.valueTear(curve) && !mesh.valueFunction(curve, Mesh::VALUE_RIGHT).empty())
        {
            _out << "dcvRight " << ci;
            writeValueFunction(_out, mesh.valueFunction(curve, Mesh::VALUE_RIGHT));
            _out << "\n";
        }

        if(!mesh.valueFunction(curve, Mesh::GRADIENT_LEFT).empty())
        {
            _out << (mesh.gradientTear(curve)? "dcgLeft ": "dcg ") << ci;
            writeValueFunction(_out, mesh.valueFunction(curve, Mesh::GRADIENT_LEFT));
            _out << "\n";
        }
        if(mesh.gradientTear(curve) && !mesh.valueFunction(curve, Mesh::GRADIENT_RIGHT).empty())
        {
            _out << "dcgRight " << ci;
            writeValueFunction(_out, mesh.valueFunction(curve, Mesh::GRADIENT_RIGHT));
            _out << "\n";
        }
    }
}


template < typename _Mesh >
void
MVGWithCurvesWriter<_Mesh>::writeValueFunction(std::ostream& out, const ValueFunction& vg) const
{
    for(typename ValueFunction::ConstIterator stop = vg.begin();
        stop != vg.end(); ++stop)
    {
        out << "   " << stop->first << " " << stop->second.transpose().format(m_format);
    }
}


}
