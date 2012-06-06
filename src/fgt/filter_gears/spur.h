#ifndef SPUR_H
#define SPUR_H

// topology computation
#include <vcg/complex/algorithms/update/topology.h>

// append
#include <vcg/complex/append.h>

// clean
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/update/position.h>

// just for writing log
#include <QString>
#include <common/interfaces.h>

// temporary
#include <GL/glew.h>
#include <wrap/gl/glu_tessellator_cap.h>

using namespace vcg;

// class that creates only the gear profile
template <typename MeshType> class Spur {

private:
    static const int ppinv = 20;
    static const int ppc = 10;
    static const int pph = 60;

    // involute tooth profile
    static void tooth(MeshType &tth, float r, float m, float rtop, float pangle) {

        float rb = r * cos(pangle*M_PI/180);
        float angle = (M_PI * m / r)/2;
        float maxt = math::Sqrt((rtop)*(rtop)/(rb*rb)-1);
        float maxl = rb*maxt*maxt/2.0;
        float theta = math::Asin((m/3)/(r-1.25*m+m/3));

        //ascending involute
        int i = 0;
        for (; i <= ppinv; ++i) {

            float t = sqrt(2*(maxl*i/ppinv)/rb);

            float x = rb * (cos(t) + t * sin(t));
            float y = -rb * (sin(t) - t * cos(t));

            // avoid strange tooth form
            if (y < tan(-angle/2)*x) {
                break;
            }

            typename tri::Allocator<MeshType>::VertexIterator vp = tri::Allocator<MeshType>::AddVertices(tth, 1);
            vp->P().X() = x*cos(angle) - y*sin(angle);
            vp->P().Y() = x*sin(angle) + y*cos(angle);
            vp->P().Z() = 0;

        }

        // descending involute
        --i;
        for (; i >= 0; --i) {

            float t = sqrt(2*(maxl*i/ppinv)/rb);

            float x = rb * (cos(t) + t * sin(t));
            float y = rb * (sin(t) - t * cos(t));

            typename tri::Allocator<MeshType>::VertexIterator vp = tri::Allocator<MeshType>::AddVertices(tth, 1);
            vp->P().X() = x;
            vp->P().Y() = y;
            vp->P().Z() = 0;

        }

        // descending arc circle
        float cx = (r-1.25*m+m/3) * cos(-theta);
        float cy = (r-1.25*m+m/3) * sin(-theta);
        for (float alpha = M_PI/2.0; alpha < M_PI - theta; alpha += (M_PI/2 - theta)/ppc) {
            float x = m/3 * cos(alpha) + cx;
            float y = m/3 * sin(alpha) + cy;
            typename tri::Allocator<MeshType>::VertexIterator vp = tri::Allocator<MeshType>::AddVertices(tth, 1);
            vp->P().X() = x;
            vp->P().Y() = y;
            vp->P().Z() = 0;
        }

        // ascending arc circle
        for (float alpha = M_PI - theta; alpha > M_PI/2.0; alpha -= (M_PI/2 - theta)/ppc) {
            float x = m/3 * cos(alpha) + cx;
            float y = - (m/3 * sin(alpha) + cy);
            typename tri::Allocator<MeshType>::VertexIterator vp = tri::Allocator<MeshType>::AddVertices(tth, 1);
            vp->P().X() = x*cos(-angle) - y*sin(-angle);
            vp->P().Y() = x*sin(-angle) + y*cos(-angle);
            vp->P().Z() = 0;
        }

    }

    static void rotateTooth(MeshType &tth, float angle) {

        for (typename MeshType::VertexIterator vi = tth.vert.begin(); vi != tth.vert.end(); ++vi) {
            float x = vi->P().X();
            float y = vi->P().Y();
            vi->P().X() = x * cos(angle) - y * sin(angle);
            vi->P().Y() = x * sin(angle) + y * cos(angle);
        }

    }

    // create gears, requires previously checked parameters
    static void create(MeshType &g, float r, float m, float rtop, int n, float angle, float h) {

        MeshType tth;

        tooth(tth, r, m, rtop, angle);
        tri::Append<MeshType, MeshType>::Mesh(g, tth);

        // add teeth
        for (int i = 1; i < n; i++) {
            rotateTooth(tth, -2*M_PI/n);
            tri::Append<MeshType, MeshType>::Mesh(g, tth);
        }

        // add edges
        for (int i = 0; i < g.vn; ++i) {
            typename tri::Allocator<MeshType>::EdgeIterator ep = tri::Allocator<MeshType>::AddEdges(g, 1);
            ep->V(0) = &g.vert[i];
            ep->V(1) = &g.vert[(i+1)%(g.vn)];
        }

        // add hole
        if (h > 0) {
            int count = g.vn;
            for (int i = 0; i < pph; ++i) {
                typename tri::Allocator<MeshType>::VertexIterator vp = tri::Allocator<MeshType>::AddVertices(g, 1);
                vp->P().X() = h*cos(math::ToRad(i*360.0/pph));
                vp->P().Y() = h*sin(math::ToRad(i*360.0/pph));
                vp->P().Z() = 0;
            }
            for (int i = 0; i < pph; ++i) {
                typename tri::Allocator<MeshType>::EdgeIterator ep = tri::Allocator<MeshType>::AddEdges(g, 1);
                ep->V(0) = &g.vert[i+count];
                ep->V(1) = &g.vert[(i+1)%pph+count];
            }
        }

    }


public:

    // create a couple of spur gears, checking for input parameters
    static bool createCouple(MeshFilterInterface &fi, QString &err, MeshType &sprocket, MeshType &gear, float interaxis, float transmission, float module, float pangle = 20.0, float hole = 0.0) {

        if ( (interaxis < 0) || (transmission <= 0) || (module <= 0) || (pangle <= 0) || (pangle >= 90.0) || (hole < 0)) {
            err = "Bad parameter.";
            return false;
        }

        float rs = interaxis / (transmission+1);
        int ns = 2 * rs / module;

        // teeth number
        if (2.0 * rs / module - ns != 0) {
            err = "The number of teeth of the sprocket is not an integer. Try adjusting the interaxis, the speed ratio or the module.";
            return false;
        }

        float rg = transmission*interaxis / (transmission+1);
        int ng = 2 * rg / module;

        if (2.0 * rg / module - ng != 0) {
            err = "The number of teeth of the gear is not an integer. Try adjusting the interaxis, the speed ratio or the module.";
            return false;
        }


        // base radius vs. foot radius
        float rbs = rs * cos(pangle*M_PI/180);
        if (rbs < rs - 1.25*module + module/3) {
            err = "Root radius bigger than base radius in sprocket. Try increasing the module or decreasing the pressure angle.";
            return false;
        }

        float rbg = rg * cos(pangle*M_PI/180);
        if (rbg < rg - 1.25*module + module/3) {
            err = "Root radius bigger than base radius in gear. Try increasing the module or decreasing the pressure angle.";
            return false;
        }

        // holes
        if (hole >= rs - 1.25*module) {
            err = "Hole radius bigger than root radius of sprocket.";
            return false;
        }
        if (hole >= rg - 1.25*module) {
            err = "Hole radius bigger than root radius of gear.";
            return false;
        }

        //interference
        float rtops = rs + module;
        float rmaxs = math::Sqrt(rbs*rbs + math::Sqr(interaxis*sin(pangle*M_PI/180)));
        if (rmaxs < rtops) {
            fi.Log("Sprocket interference, adopting corrections.\n");
            rtops = rmaxs;
        }

        float rtopg = rg + module;
        float rmaxg = math::Sqrt(rbg*rbg + math::Sqr(interaxis*sin(pangle*M_PI/180)));
        if (rmaxg < rtopg) {
            fi.Log("Gear interference, adopting corrections.\n");
            rtopg = rmaxg;
        }

        // contact ratio
        float deg = math::Sqrt(sqr(rtops) - sqr(rs - 1.25*module + module/3));
        deg += math::Sqrt(sqr(rtopg) - sqr(rg - 1.25*module + module/3));
        deg -= interaxis * sin(pangle*M_PI/180);
        deg /= M_PI * module;
        if (deg < 1)
            fi.Log("Contact ratio is less than 1.\n");

        // strange teeth form
        float maxt = math::Sqrt(sqr(rs+module)/(rbs*rbs)-1);
        float x = rbs * (cos(maxt) + maxt * sin(maxt));
        float y = -rbs * (sin(maxt) - maxt * cos(maxt));
        float angle = (M_PI * module / rs)/4;
        if (y < tan(-angle)*x) {
            fi.Log("Waring: top of the teeth of the sprocket must be cut, to avoid this try decreasing the pressure angle.\n");
        }

        maxt = math::Sqrt(sqr(rg+module)/(rbg*rbg)-1);
        x = rbg * (cos(maxt) + maxt * sin(maxt));
        y = -rbg * (sin(maxt) - maxt * cos(maxt));
        angle = (M_PI * module / rg)/4;
        if (y < tan(-angle)*x) {
            fi.Log("Waring: top of the teeth of the gear must be cut, to avoid this try decreasing the pressure angle.\n");
        }


        // finally create gears
        create(sprocket, rs, module, rtops, ns, pangle, hole);

        create(gear, rg, module, rtopg, ng, pangle, hole);

        return true;

    }

    // create a single spur gear, checking for input parameters
    static bool createSingle(MeshFilterInterface &fi, QString &err, MeshType &gear, float radius, float module, float pangle = 20.0, float hole = 0.0) {

        if ( (radius <= 0) || (module <= 0) || (pangle <= 0) || (pangle >= 90.0) || (hole < 0)) {
            err = "Bad parameter.";
            return false;
        }

        // teeth number
        int n = 2 * radius / module;
        if (2.0 * radius / module - n != 0) {
            err = "The number of teeth is not an integer. Make sure the module divides the diameter.";
            return false;
        }

        // base radius vs. foot radius
        float rb = radius * cos(pangle*M_PI/180);
        if (rb < radius - 1.25*module + module/3) {
            err = "Root radius bigger than base radius. Try increasing the module or decreasing the pressure angle.";
            return false;
        }

        // hole
        if (hole >= radius - 1.25*module) {
            err = "Hole radius bigger than root radius.";
            return false;
        }

        // strange teeth form
        float maxt = math::Sqrt(sqr(radius+module)/(rb*rb)-1);
        float x = rb * (cos(maxt) + maxt * sin(maxt));
        float y = -rb * (sin(maxt) - maxt * cos(maxt));
        float angle = (M_PI * module / radius)/4;
        if (y < tan(-angle)*x) {
            fi.Log("Waring: top of the teeth must be cut, to avoid this try decreasing the pressure angle.\n");
        }

        create(gear, radius, module, radius+module, n, pangle, hole);

        return true;

    }

};


#endif // SPUR_H
