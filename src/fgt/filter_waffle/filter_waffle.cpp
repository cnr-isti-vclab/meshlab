
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

#include "filter_waffle.h"
#include <vcg/complex/algorithms/intersection.h>
#include <vcg/complex/algorithms/update/topology.h>

#include <wrap/gl/glu_tessellator_cap.h>



#include <vcg/space/intersection2.h>

#include <algorithm>

#include <wrap/gl/glu_tesselator.h>
#include <vcg/complex/allocate.h>
#include <vcg/complex/append.h>
//#include <vcg/complex/edgemesh/update/topology.h>

#include <math.h>

//#include "kdtree.h"

using namespace std;
using namespace vcg;


ExtraFilter_SlicePlugin::ExtraFilter_SlicePlugin ()
{
    typeList << FP_WAFFLE_SLICE;

    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);
}

QString ExtraFilter_SlicePlugin::filterName(FilterIDType filterId) const
{
    switch(filterId) {
        case FP_WAFFLE_SLICE    :  return QString("Cross section waffle");
        default : assert(0);
    }
    return QString("error!");
}

QString ExtraFilter_SlicePlugin::filterInfo(FilterIDType filterId) const
{
    switch(filterId) {
        case FP_WAFFLE_SLICE    :  return QString("Export one or more cross sections of the current mesh relative to one of the XY, YZ or ZX axes in svg format. By default, the cross-section goes through the middle of the object (Cross plane offset == 0).");
        default : assert(0);
    }
    return QString("error!");
}

void ExtraFilter_SlicePlugin::initParameterSet(QAction *filter, MeshModel &m, RichParameterSet &parlst)
{
    switch(ID(filter))
    {
        case FP_WAFFLE_SLICE :
        {
            QStringList axis;
            axis<<"X Axis"<<"Y Axis"<<"Z Axis";
            parlst.addParam(new RichEnum("planeAxis", 0, axis, tr("Plane perpendicular to"), tr("The Slicing plane will be done perpendicular to the axis")));
            parlst.addParam(new RichFloat  ("length",29,"Dimension on the longer axis (cm)","specify the dimension in cm of the longer axis of the current mesh, this will be the output dimension of the svg"));
            parlst.addParam(new RichFloat("planeOffset",0.0,"Cross plane offset","Specify an offset of the cross-plane. The offset corresponds to the distance from the point specified in the plane reference parameter. By default (Cross plane offset == 0)"));
            // BBox min=0, BBox center=1, Origin=2
            parlst.addParam(new RichFloat("eps",0.5,"Thickness of a planes","Thickness of the plans generated. Must be between 0 and 1. If > 1 the extrusion of the plans overlap."));
            parlst.addParam(new RichFloat("spacing",0.1,"Space between two planes", "Distance between planes. Must be between 0 and 1 and indicates how many parts to cut the figure."));
            parlst.addParam(new RichBool("hideBase",true,"Hide Original Mesh","Hide the Original Mesh"));
            parlst.addParam(new RichBool("hideEdge",false,"Hide Edge Mesh","Hide the Generated Edge Mesh"));
            parlst.addParam(new RichBool("hidePlanes",true,"Hide Planes","Hide the Generated Slicing Planes"));
            parlst.addParam(new RichBool("hideExtrudes",true,"Hide Extrusions","Hide the Generated Estrusions"));
//            parlst.addParam(new RichBool("singleFile", true, "Single SVG","Automatically generate a series of cross-sections along the whole length of the object and store each plane in a separate SVG file. The distance between each plane is given by the step value below"));
        }
        break;
    default : assert(0);
    }
}

void ExtraFilter_SlicePlugin::generateRectSeg(const float &dist, const float &epsTmp, const float &lengthDiag, Segment2f lati[])
{
    //the rectangle has the short side on the x, and the long sides on the y

    //Variable support
    float yMax = dist+epsTmp;
    float yMin = dist-epsTmp;

    //points for the rectangle
    Point2f a0,b0,a1,b1;    //a1 and a2 are the inner point
    a0.X() = 0;
    a0.Y() = yMax;

    a1.X() = 0;
    a1.Y() = yMin;

    b0.X() = lengthDiag;
    b0.Y() = yMax;

    b1.X() = lengthDiag;
    b1.Y() = yMin;

    //I keep saved the segments and I change only the Y in the main cicle
    //initialize the segments
    lati[L1].Set(a0,b0);    //side with Y greater
    lati[L2].Set(a1,b1);    //side with Y lower
    lati[l1].Set(a0,a1);    //is the smaller side, which passes through the center (0.0)
    //the other shorter side l2 is not far away

    //yMax == lati[l1].P0().Y();
    //yMin == lati[l1].P1().Y();
}

inline void flipOnX(Segment2f lati[])
{
    lati[L2].P1().X() = lati[L1].P1().X() = -lati[L1].P1().X();
}

inline void modifyOnY(Segment2f lati[], const float yMax, const float yMin)
{
    assert(yMax > yMin);

    lati[L1].P0().Y() = lati[L1].P1().Y() = lati[l1].P0().Y() = yMax;
    lati[L2].P0().Y() = lati[L2].P1().Y() = lati[l1].P1().Y() = yMin;

}

bool ExtraFilter_SlicePlugin::applyFilter(QAction *filter, MeshDocument &m, RichParameterSet &parlst, vcg::CallBackPos *cb)
{
    vcg::tri::UpdateBounding<CMeshO>::Box(m.mm()->cm);

    switch(ID(filter))
    {
        case FP_WAFFLE_SLICE :
        {
            MeshModel* base = m.mm();
            CMeshO &cmBase = base->cm;
            Box3f &bbox = m.mm()->cm.bbox;

            if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(cmBase)>0 || (tri::Clean<CMeshO>::CountNonManifoldVertexFF(cmBase,false) != 0))
            {
                Log("Mesh is not two manifold, cannot apply filter");
                return false;
            }
            if(parlst.getFloat("spacing") >= 1)
            {
                Log("the selected distance between the planes is greater than 1. The filter had no effect");
                return false;
            }

            Point3f planeAxis(0,0,0);
            Axis axis = (Axis) parlst.getEnum("planeAxis");
            planeAxis[axis] = 1.0f;

            float length = parlst.getFloat("length");

            bool hideBase = parlst.getBool("hideBase");
            bool hideEdge = parlst.getBool("hideEdge");
            bool hidePlanes = parlst.getBool("hidePlanes");
            bool hideExtrudes = parlst.getBool("hideExtrudes");

            // set common SVG Properties
            const float maxdim=m.mm()->cm.bbox.Dim()[m.mm()->cm.bbox.MaxDim()];

            Point3f sizeCm=m.mm()->cm.bbox.Dim()*(length/maxdim);
            // to check for dimensions with custom axis
            Axis axisOrthog, axisJoint;
            Point2f sizeCmOrth;
            switch(axis)
            {
            case X:
                {
                    axisOrthog = (Axis) Succ<X>::value;
                    axisJoint = (Axis) Succ<Succ<X>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[1],sizeCm[2]);
                    sizeCmOrth.X()=sizeCm[0];
                    sizeCmOrth.Y()=sizeCm[2];
                }
                break;
            case Y:
                {
                    axisOrthog = (Axis) Succ<Y>::value;
                    axisJoint = (Axis) Succ<Succ<Y>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[0],sizeCm[2]);
                    sizeCmOrth.X()=sizeCm[0];
                    sizeCmOrth.Y()=sizeCm[1];
                }
                break;
            case Z:
                {
                    axisOrthog = (Axis) Succ<Z>::value;
                    axisJoint = (Axis) Succ<Succ<Z>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[0],sizeCm[1]);
                    sizeCmOrth.X()=sizeCm[1];
                    sizeCmOrth.Y()=sizeCm[2];
                }
                break;
            }

            Log("sizecm %fx%f",pr.sizeCm[0],pr.sizeCm[1]);

            vector<CMeshO*> ev;

            const float planeDist = maxdim * parlst.getFloat("spacing");
            const int planeNum = (planeDist == 0) ? 1 : ( ((bbox.Dim()*planeAxis)/planeDist)+1 ); //evito la divisione per 0
            const float lengthDiag = bbox.Diag()/2.0;

            Segment2f lati[3]; //the rectangle is made up of three segments, the fourth side is ignored, so I never use it

            const float eps =planeDist*parlst.getFloat("eps");
            float epsTmp;
            float start;
            int rectNum;
            if(parlst.getFloat("eps") < 1)
            {
                start = - (planeNum/2) * planeDist;   //I have to go back from the center of half the length
                epsTmp = eps/2.0;
                generateRectSeg(0, epsTmp, bbox.Diag()*2, lati);
                rectNum = planeNum;
            }
            else
            {
                start = 0;
                epsTmp = bbox.Diag();
                generateRectSeg(0, bbox.Diag()*2, bbox.Diag()*2, lati);
                rectNum = 1;
                Log("thickness is greater than 1: the extrusions will overlap");
            }

            float planeOffset = parlst.getFloat("planeOffset");

            pr.lineWidthPt=200;
            pr.scale=2/maxdim;
            pr.numCol=(int)(max((int)sqrt(planeNum*1.0f),2)+1);
            pr.numRow=(int)(planeNum*1.0f/pr.numCol)+1;
            pr.projDir = planeAxis;
            pr.projCenter =  m.mm()->cm.bbox.Center();
            pr.enumeration = true;

            MeshModel* cap, *cap2, *extr;
            QString layername;


            for(int pl = 0; pl < 2; ++pl)
            {
//                Log("################## PLANE %i", pl);
                Plane3f slicingPlane;
                Point3f planeCenter = bbox.Center() + planeAxis*planeOffset*lengthDiag;
                int numAdd = 0;

//                int i=4;  //if I want to apply only the plan number i I decomment this variable and comment the cycle
                for(int i = 0; i < planeNum; ++i)   //cropping the plans
                {
//                    Log("######## LAYER %i", i);
                    planeCenter[axis] = start + planeDist*i;;

                    slicingPlane.Init(planeCenter,planeAxis);

                    layername.sprintf("EdgeMesh_%d_%d",pl,numAdd);
                    cap= m.addNewMesh("",qPrintable(layername));
                    vcg::IntersectionPlaneMesh<CMeshO, CMeshO, float>(base->cm, slicingPlane, cap->cm );

                    tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);
                    if(cap->cm.edge.size()>= 3)
                    {
                        Incastri temp;

//                        int j=1;      //if I want to apply only the rectangle number j I decomment this variable and comment the cycle
                        for(int j = 0; j <rectNum ; ++j)   //clipping the rectangles
                        {
//                            Log("#### RECTANGLE %i", j);
                            float newDist = start + planeDist*j;
                            modifyOnY(lati, newDist+epsTmp, newDist-epsTmp);

                            temp = subtraction(cap->cm, lati, axis, axisOrthog, axisJoint, planeCenter[axis]);

                            if(temp == NOT_PLANE) {Log("ATTENTION! The IntersectionPlaneMesh did not return a plane silhouette in the current plane!"); m.delMesh(cap); break;}
                            if(temp== NOT_SAGOME) {Log("ATTENTION! The IntersectionPlaneMesh did not return a simple closed silhouette for the plane %i, on axis %i",i, pl); m.delMesh(cap); break;}
                        }
                        if(temp > NOT_SAGOME)
                        {
                            ev.push_back(&(cap->cm));   //add the silhouette to those for export to SVG

                            layername.sprintf("CappedSlice_%d_%d",pl,numAdd);   //add the silhouettes converted in mesh
                            cap2= m.addNewMesh("",qPrintable(layername));
                            tri::CapEdgeMesh(cap->cm, cap2->cm);

                            layername.sprintf("Extruded_%d_%d",pl,numAdd++);      //add the mesh extruded
                            extr= m.addNewMesh("",qPrintable(layername));
                            cap2->updateDataMask(MeshModel::MM_FACEFACETOPO);
                            extr->updateDataMask(MeshModel::MM_FACEFACETOPO);
                            tri::UpdateTopology<CMeshO>::FaceFace(cap2->cm);
                            tri::ExtrudeBoundary<CMeshO>(cap2->cm,extr->cm,eps,planeAxis);

                            if(hideEdge) cap->visible =false;
                            if(hidePlanes) cap2->visible =false;
                            if(hideExtrudes) extr->visible =false;
                        }
                    }else m.delMesh(cap);   //if the intersection does not exist I reject the result
                }

                QString fname;//= parlst.getSaveFileName("filename");
                if(fname=="") fname.sprintf("Slice_%d.svg",pl);
                if (!fname.endsWith(".svg")) fname+=".svg";
                tri::io::ExporterSVG<CMeshO>::Save(ev, fname.toStdString().c_str(), pr);

                ev.clear();

                planeAxis[axis] = 0.0f;
                planeAxis[axisOrthog] = 1.0f;

                flipOnX(lati);

                pr.sizeCm = sizeCmOrth;
                pr.projDir = planeAxis;

                Axis aSwap = axis;
                axis = axisOrthog;
                axisOrthog = aSwap;
            }


            if(hideEdge) cap->visible =false;
            if(hidePlanes) cap2->visible =false;
            if(hideExtrudes) extr->visible =false;

            if(hideBase) base->visible =false;
            if(hideBase) base->visible =false;

        }
        break;
    }
    return true;
}

MeshFilterInterface::FilterClass ExtraFilter_SlicePlugin::getClass(QAction *filter)
{
    switch(ID(filter))
    {
        case FP_WAFFLE_SLICE :
        return MeshFilterInterface::Measure;
        default: assert(0);
    }
    return MeshFilterInterface::Generic;
}

/*
bool ExtraFilter_SlicePlugin::autoDialog(QAction *action)
{
    switch(ID(action))
    {
        case FP_WAFFLE_SLICE :
        default: return false;
    }
}
*/

bool compareX(JointPoint v0, JointPoint v1)
{
    return v0.p.X() < v1.p.X();
}

bool compareY(JointPoint v0, JointPoint v1)
{
    return v0.p.Y() < v1.p.Y();
}

bool compareInvY(JointPoint v0, JointPoint v1)
{
    return v0.p.Y() > v1.p.Y();
}

void ExtraFilter_SlicePlugin::addAndBreak(CMeshO &em, Point3f & pJoint, const Axis &axisOrthog, const Axis &axisJoint, const JointPoint & jp, const CMeshO::VertexIterator vi, const CMeshO::EdgeIterator ei)
{
//Log("Edge break %i", jp.e);
//Log("New Edge %i", &(*ei) - &(em.edge[0]));
//Log("V1 moved to the new edge %i", em.edge[jp.e].V(1) - &(em.vert[0]));
//og("V Joint %i", &(*vi) - &(em.vert[0]));
//Log("IndV %i", jp.indV);    //indice del vertice esterno o collineare
//Log("Collinear %i", jp.collinear);

    if(jp.collinear)    ////it's like if you move (you copy them and delete the old) the vertex collinear and one of the two vertices
    {
        (*vi).P() = em.edge[jp.e].V(jp.indV)->P();  //I copy the collinear joint vertex (must be between the points of joint, the end of the vector of vertices)

        CEdgeO *e2 = em.edge[jp.e].EEp(jp.indV);    //I move on the edge that touches the collinear vertex
        int indV2 = em.edge[jp.e].EEi(jp.indV);     //index of the collinear vertex on the new edge
        e2->V(indV2) = &(*vi);                      //replace the point (they are identical, but the new one is placed between the points of joint)

        int otherV =(jp.indV+1)%2;
        ei->V(otherV) = em.edge[jp.e].V(otherV);    //I copy the other vertex (not collinear) in the new edge
        ei->V(jp.indV) = &(*vi);                    //I put the collinear Joint point as the second vertex

        em.edge[jp.e].V(otherV) = em.edge[jp.e].V(jp.indV);                     //I completely disconnects the old edge (V1 ad V2 are equal to the vertex that will be removed; I do not delete immediately but only after removing internal arcs, so when I reconstruct the topology, there are no problems)
        tri::Allocator<CMeshO>::DeleteVertex(em,*( em.edge[jp.e].V(jp.indV) )); //old point is removed
        tri::Allocator<CMeshO>::DeleteEdge(em, em.edge[jp.e]);                  //the previous edge is eliminated because it was replaced
        //I do not immediately remove them, because I still find even the inner edge to remove

    }else
    {
        //I BREAK AN EDGE IN TWO
        //I convert the intersection point in 3D (the third coordinate pJoint [axis] is always the same for all points)
        pJoint[axisJoint] = jp.p.X();
        pJoint[axisOrthog] = jp.p.Y();

        //I add one only vertex for the point of intersection (the two edge that using the the vertex of the current edge and the new vertex of intersection)
        (*vi).P() = pJoint;     //I add the new vertex of intersection
        //I will break the current edge in two on the intersecting point: just add only the second edge and for the first edge I using the current edge

        //for the new edge
        ei->V(0) = &(*vi);                  //the vertex 0 is the new joint point
        ei->V(1) =em.edge[jp.e].V(jp.indV); //the vertex 1 is the il vertex on the correct side (indV1) of the current edge

        //the edge 1, which was created above the current edge, have the vertex 0 unchanged, while ...
        em.edge[jp.e].V(jp.indV) = &(*vi);       //...the vertex 1 becomes the new vertex of intersection
    }

}

bool isNotApproximate(const float &f0, const float &f1)
{
        return (f0 - numeric_limits<float>::epsilon() >= f1) || (f1 >= f0 + numeric_limits<float>::epsilon());
}

bool inOrthogonalEdge(const float &rangeMin, const float &rangeMax, const float &equal, const float &coordInRange, const float &coordEq)
{

    if( (rangeMin<= coordInRange && coordInRange <= rangeMax) && (!isNotApproximate(equal, coordEq)) ) return true;
    return false;
}

Incastri ExtraFilter_SlicePlugin::subtraction(CMeshO &em, Segment2f lati[], const Axis &axis, const Axis &axisOrthog, const Axis &axisJoint, const float height)
{
    Incastri inc  = NILL;

    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);


    tri::UpdateTopology<CMeshO>::EdgeEdge(em);

    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);

//    assert(em.vert.size() == em.edge.size());   //se ogni sagoma è chiusa deve valere
    assert(axis != axisOrthog && axisOrthog != axisJoint && axisJoint != axis);

    // 1- SEARCH OF INTERSECTION POINTS BETWEEN MESH AND RECTANGLE
//    Log("PHASE 1");
    tri::UpdateTopology<CMeshO>::EdgeEdge(em);
    vcg::tri::UpdateFlags<CMeshO>::EdgeClearV(em);  // I set the flags to see if they were visited by the sub-cycle (who visit one silhouette at a time)

    Segment2f seg;          //variables used to convert the edge examined in 2D so that you can use the intersection function
    Point2f p0_2D, p1_2D;
    Point2f pJoint2D;       //2D intersecton point initialized from intersection test

    //vectors that store the intersection points found, one on each side of the rectangle (3 side)
    vector<JointPoint> jointPointsL1;
    vector<JointPoint> jointPointsL2;
    vector<JointPoint> jointPointsl1;

    bool cas0, cas1;    //for the intersection whith l1 I have to see if the rectangle is above or below the origin to choose indV
    float xMax, xMin;
    bool rectPos = lati[L1].P1().X() > 0;   //checks if the rectangle is positive or not

    if(rectPos)
    {
        cas0=false;
        cas1=true;
        xMax = lati[L1].P1().X();
        xMin = 0;
    }
    else{
        cas0=true;
        cas1=false;
        xMax = 0;
        xMin = lati[L1].P1().X();
    }

    float & yMax = lati[l1].P0().Y();
    float & yMin = lati[l1].P1().Y();

//Log("--- L1:(%f %f) (%f %f)",lati[L1].P0().X(),lati[L1].P0().Y(),lati[L1].P1().X(),lati[L1].P1().Y());
//Log("--- L2:(%f %f) (%f %f)",lati[L2].P0().X(),lati[L2].P0().Y(),lati[L2].P1().X(),lati[L2].P1().Y());
//Log("--- l1:(%f %f) (%f %f)",lati[l1].P0().X(),lati[l1].P0().Y(),lati[l1].P1().X(),lati[l1].P1().Y());

    for(size_t i = 0; i < em.edge.size(); i++)      // ciclo sulle diverse sagome
    {
        if (!em.edge[i].IsV())
        {
            edge::Pos<CEdgeO> eStart(&em.edge[i],0);    //the start edge
            edge::Pos<CEdgeO> eCorr = eStart;
//Log("New silhouette");
            bool enableL1,enableL2,enablel1;    //flag to enable the test of intersection with the 3 sides of the rectangle; to add or not the collinear points
            enableL1=enableL2=enablel1=true;    //the first time that I met a collinear vertez I do not know; the second time if I continue on same direction the I add it (otherwise it is a bounce)
            bool lastDir;                       //flag to know if I was in or out of the last time that I met a collinear point: can be positive or negative
            JointPoint firstCollinearP;         //for the collinear vertex, I look at the two adjacent edge to see if it actually passes through the side and in this case I re-add it

            do
            {
                eCorr.E()->SetV();
                Point3f &p = eCorr.V()->P();

                if(isNotApproximate(height, p[axis]) ) return NOT_PLANE;
//Log("p axis %f, height %f",p[axis],height);

                Point3f &p0 = eCorr.E()->V(0)->P();
                Point3f &p1 = eCorr.E()->V(1)->P();

                if(p0==p1){eCorr.NextE();Log("There are arcs degenerate");continue;} //salto gli archi degeneri


                //convert the vertices in 2D: for as I have defined the clipping rectangle, the X is along axisJoint and the Y is long axisOrthog
                p0_2D.X() = p0[axisJoint];
                p0_2D.Y() = p0[axisOrthog];
                p1_2D.X() = p1[axisJoint];
                p1_2D.Y() = p1[axisOrthog];


                //initialize the 2D segment to be used for the intersection test
                seg.Set(p0_2D,p1_2D);

                int ei = eCorr.E() - &(em.edge[0]); //I do not consider even cases where the edge is collinear with one side

                if( ((!enableL1)||(!enableL2)||(!enablel1)) && (seg.P0().Y()==seg.P1().Y()||seg.P0().X() == seg.P1().X()) ) {eCorr.NextE(); continue;};  //non considero neanche i casi in cui l'edge è collineare ad uno dei lati

                //a segment can intersect 2 sides simultaneously (but not 3), then I can not put else on 3 sides of the cases

                if( isNotApproximate(seg.P0().Y(), seg.P1().Y()) )
                {
                    int indV = (seg.P0().Y() > seg.P1().Y()) ? 0 : 1;  //index of the external vertex of the rectangle; this edge will be replaced with the point of joint to form the first edge of the splitting

                    // L1
                    //one of the vertices of seg could coincide with the intersection point. In this case means that the point is on the side (I do a separate test, because SegmentSegmentIntersection works badly with collinearity)
                    if(inOrthogonalEdge(xMin,xMax,yMax,seg.P0().X(), seg.P0().Y()))
                    {
                        if(enableL1)    //the first time that I meet a collinear point I do not consider it, but i set a flag: the next joint point that i meet, will certainly be collinear point
                        {
                            enableL1=false;
                            lastDir = (seg.P1().Y() > yMax) ? true : false; //to see the direction I have to look at the other point that is not collinear
                            JointPoint newJoint(seg.P0(), ei, 0, true);

                            firstCollinearP = newJoint;     //I keep aside the collinear point found, until I came out or not
//Log("L1 p0 not add: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //The second time that I meet a colinear
                        {
//Log("L1 p0 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL1=true;
                            bool dir = (seg.P1().Y() > yMax) ? true : false;
                            if(lastDir!=dir)    //add only if it is not a bounce, if I cross the axis
                            {
                                jointPointsL1.push_back(firstCollinearP);
//Log("L1 p0 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(inOrthogonalEdge(xMin,xMax,yMax,seg.P1().X(), seg.P1().Y()))
                    {
                        if(enableL1)
                        {
                            enableL1=false;
                            lastDir = (seg.P1().Y() > yMax) ? true : false;
                            JointPoint newJoint(seg.P1(), ei, 1, true);

                            firstCollinearP = newJoint;
//Log("L1 p1 not add: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else
                        {
//Log("L1 p1 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL1=true;
                            bool dir = (seg.P0().Y() > yMax) ? true : false;
                            if(lastDir!=dir)
                            {
                                jointPointsL1.push_back(firstCollinearP);
//Log("L1 p1 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(vcg::SegmentSegmentIntersection(lati[L1],seg,pJoint2D))    //thanks to the previous tests, this case does intersection only when they are not collinear
                    {
                        if(!enablel1)
                        {
                            if(rectPos == lastDir>0)
                            {
                                jointPointsl1.push_back(firstCollinearP);
                                enablel1 = true;
                            }
//Log("add old");
                        }else
                        {
                            JointPoint newJoint(pJoint2D, ei, indV, false);
                            jointPointsL1.push_back(newJoint);
//Log("Inters L1: %i,(%f %f) (%f %f),(%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X(),pJoint2D.Y());
//c1 = true;
                        }
                    }

                    // L2
                    indV = (seg.P0().Y() < seg.P1().Y()) ? false : true;
                    if(inOrthogonalEdge(xMin,xMax,yMin,seg.P0().X(), seg.P0().Y()))
                    {
                        if(enableL2)
                        {
                            enableL2=false;
                            lastDir = (seg.P1().Y() < yMin) ? true : false;
                            JointPoint newJoint(seg.P0(), ei, 0, true);

                            firstCollinearP = newJoint;
//Log("L2 p0 not add: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
//Log("L2 p0 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL2=true;
                            bool dir = (seg.P1().Y() < yMin) ? true : false;
                            if(lastDir!=dir)
                            {
                                jointPointsL2.push_back(firstCollinearP);
//Log("L2 p0 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(inOrthogonalEdge(xMin,xMax,yMin,seg.P1().X(), seg.P1().Y()))
                    {
                        if(enableL2)
                        {
                            enableL2=false;
                            lastDir = (seg.P0().Y() < yMin) ? true : false;
                            JointPoint newJoint(seg.P1(), ei, 1, true);

                            firstCollinearP = newJoint;
//Log("L2 p1 not add: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else
                        {
//Log("L2 p1 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL2=true;
                            bool dir = (seg.P0().Y() < yMin) ? true : false;
                            if(lastDir!=dir)
                            {
                                jointPointsL2.push_back(firstCollinearP);
//Log("L2 p1 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(vcg::SegmentSegmentIntersection(lati[L2],seg,pJoint2D))
                    {
                        if(!enablel1)
                        {
                            if(rectPos == lastDir>0)
                            {
                                jointPointsl1.push_back(firstCollinearP);
                                enablel1 = true;
                            }
//Log("add old");
                        }else
                        {
//Log("Inters L2: %i,(%f %f) (%f %f),(%f %f) (%f %f),(%f %f)",ei,lati[L2].P0().X(),lati[L2].P0().Y(),lati[L2].P1().X(),lati[L2].P1().Y(),seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X(),pJoint2D.Y());
                            JointPoint newJoint(pJoint2D, ei, indV, false);
                            jointPointsL2.push_back(newJoint);
                        }
                    }
                }

                if( isNotApproximate(seg.P0().X(), seg.P1().X()) )//(seg.P0().X() != seg.P1().X())
                {
                    int indV = (seg.P0().X() < seg.P1().X()) ? cas0 : cas1;

                    // l1
                    if(inOrthogonalEdge(yMin,yMax,0.0,seg.P0().Y(), seg.P0().X()))
                    {
                        if(enablel1)
                        {
                            enablel1=false;
                            lastDir = (seg.P1().X() < 0.0) ? false : true;
                            JointPoint newJoint(seg.P0(), ei, 0, true);

                            firstCollinearP = newJoint;
//Log("l1 p0 not add: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else
                        {
                            enablel1=true;
                            bool dir = (seg.P1().X() < 0.0) ? false : true;
//Log("l1 p0 dir [%i %i]: %i, (%f %f) (%f %f)",lastDir,dir,ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            if(lastDir!=dir)
                            {
                                jointPointsl1.push_back(firstCollinearP);
//Log("l1 p0 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }
                    else if(inOrthogonalEdge(yMin,yMax,0.0,seg.P1().Y(), seg.P1().X()))
                    {
                        if(enablel1)
                        {
                            enablel1=false;
                            lastDir = (seg.P0().X() < 0.0) ? false : true;
                            JointPoint newJoint(seg.P1(), ei, 1, true);

                            firstCollinearP = newJoint;
//Log("l1 p1 not add: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else
                        {
                            enablel1=true;
                            bool dir = (seg.P0().X() < 0.0) ? false : true;
//Log("l1 p1 dir [%i %i]: %i, (%f %f) (%f %f)",lastDir,dir,ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            if(lastDir!=dir)
                            {
                                jointPointsl1.push_back(firstCollinearP);
//Log("l1 p1 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(vcg::SegmentSegmentIntersection(lati[l1],seg,pJoint2D))
                    {
                        if(!enableL1)
                        {
//Log("add old");
                            if(lastDir<xMax)
                            {
                                jointPointsL1.push_back(firstCollinearP);
                                enableL1 = true;
                            }
                        }else if(!enableL2)
                        {
//Log("add old");
                            if(lastDir>xMin)
                            {
                                jointPointsL2.push_back(firstCollinearP);
                                enableL2 = true;
                            }
                        }else
                        {
                            JointPoint newJoint(pJoint2D, ei, indV, false);
                            jointPointsl1.push_back(newJoint);
//Log("Inters l1: %i, (%f %f) (%f %f),(%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X(),pJoint2D.Y());
                        }
                    }
                }
//Log("ALL: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                edge::Pos<CEdgeO> prec = eCorr;
                eCorr.NextE();
                if(prec==eCorr) return NOT_SAGOME;

            }while(eCorr != eStart);//(!eCorr.E()->IsV());//(eCorr != eStart); //&& eCorr != NULL && eCorr != ePrec

        }
    }

    int nJP = jointPointsL1.size() + jointPointsl1.size() + jointPointsL2.size();

//if(nJP%2 != 0)Log("ERROR: I found at least one joint too many");

    // 2- ADDITION OF INTERSECTION POINTS: BROKEN THE EDGE
//    Log("PHASE 2");
    // along one long side of the rectangle (L1 or L2) whenever I enter in the silhouette with a joint I have to switch from another point of joint to exit.
    // So if I have an even number of joint points along one side, after that I walked that I am out of silhouette.
    // If the number of joint are odd, at the end I am still in the silhouette and I have to add the point of the rectangle.
    // Note 1: After the addition, I have again an even number of vertices on L1 and L2.
    // Note 2: If l1 is odd or even directly dependent on L1 and L2.
    // Note 3: nJP is surely even, because of following the path of L1-l1-L2 delivery from outside, I always through a joint and at the end back out (and of course the total of points remains the same, even when I add the the point of triangle)

    Point3f pJoint;         //point 3D of intersection, which is used to convert the pJoint2D and insert them in the edgeMesh
    pJoint[axis] = height;  //the third coordinate is the same for all points, because we work in 2D

    //I order the vertices of intersection along the sides of the rectangle
    std::sort(jointPointsl1.begin(), jointPointsl1.end(), compareX);    //lo ordino per X

    //I order the long sides depending on whether the rectangle has length positive or negative
    if(cas0)    //se ha lunghezza positiva
    {
        std::sort(jointPointsL1.begin(), jointPointsL1.end(), compareY);
        std::sort(jointPointsL2.begin(), jointPointsL2.end(), compareY);
    }else
    {
        std::sort(jointPointsL1.begin(), jointPointsL1.end(), compareInvY);
        std::sort(jointPointsL2.begin(), jointPointsL2.end(), compareInvY);
    }
//Log("TYPE L1 %i, l1 %i, L2 %i",jointPointsL1.size(),jointPointsl1.size(),jointPointsL2.size());


    //for the edge of closure that I add later (after removing internal edge) what I do is keep an index ivS that indicates where I started to insert the new vertices and then I insert the vertices in pairs and in order, so in the fourth phase I rebuke them in pairs and I create the edge (taken apart the edge that touch the corners of rectangle)

    size_t iL1, iL2, il1, endl1, ivS, ieS;
    ivS = em.vert.size();                   //index of the first of the new vertices (used later to create the edge of closure)
    ieS = em.edge.size();

    endl1 = jointPointsl1.size();       //queue index of the joint points on the l1

    CMeshO::EdgeIterator ei = vcg::tri::Allocator<CMeshO>::AddEdges(em,nJP);    //to break at the intersection point I add the edge: are nJP, because in breaking I re-use the broken edge

    int dispL1 = jointPointsL1.size()%2;
    int dispL2 = jointPointsL2.size()%2;
    int addEC = dispL1 + dispL2;
    int nEC = nJP + addEC;  //if need be I have to count the edges

    CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertices(em,nEC);

//    assert( (jointPointsl1.size()!=1) || (!dispL1 && dispL2 && jointPointsl1.size() == 1) || (dispL1 && !dispL2 && jointPointsl1.size() == 1) );  // se L1 e L2 sono entrambi dispari l1 non può avere un solo vertice : o ne ha o almeno 2

    //first check if there are the point of the rectangle and if it's true I insert in order of the sides of the rectanghe (L1-L2-L1) the couples who are joint
    if(dispL1)  //if for L1 I have a odd number of points, the first point should be treated separately along the angle of rectangle.
    {
        //VERTEX IN L1
        //L1 has Y increased (compared to L2), then gave the edge to break, his vertex (with increased Y) is put in the new edge and is replaced by the joint point
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL1[0], vi, ei);
        vi++; ei++;
        //VERTEX OF RECTANGLE: just add directly the vertex to the mesh
        pJoint[axisJoint] = lati[L1].P0().X();
        pJoint[axisOrthog] = lati[L1].P0().Y();
        (*vi).P() = pJoint;
        vi++;       // adding the vertex of the rectangle does not have to move forward edge (the edge for this vertex will be added in the fourth phase)
        iL1 = 1;

        //VERTEX IN l1
        if(endl1 > 0)   //if vector for l1 is not empty I have to take a vertex from the bottom
        {
            endl1--;
            addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[endl1], vi, ei);
            vi++;
            ei++;
            nEC = 2;    //I am enter in L1 and I leave from l1 (reuse NEC)
        }else nEC = 3;  //I am enter in L1 and I leave from L2
    }else {iL1 = 0; nEC = 0;}

    if(dispL2)  //if for L2 I have a odd number of points, the first point should be treated separately along the angle of rectangle.
    {
        //VERTEX IN l1
        if(endl1 > 0)
        {
            addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[0], vi, ei);
            il1 = 1;
            vi++;
            ei++;
            assert(nEC==0 || nEC ==2);
            nEC+=2;
        } else {il1 = 0; assert(nEC==3);}
        //VERTEX OF RECTANGLE: just add directly the vertex to the mesh
        pJoint[axisJoint] = lati[L2].P0().X();
        pJoint[axisOrthog] = lati[L2].P0().Y();
        (*vi).P() = pJoint;
        vi++;
        iL2 = 1;
        //VERTEX IN L2
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL2[0], vi, ei);
        vi++; ei++;
    }else{iL2 = 0; il1 = 0;}

    assert( iL2<2 && iL1<2 && il1<2 && il1<=endl1);

    // All other points are taken in pairs: I can move in 3 successive cycles of 2 step, because I placed so that the indices remain an even number of vertices
    for(; iL1  < jointPointsL1.size(); vi++, iL1++, ei++)
    {
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL1[iL1], vi, ei);
        vi++; iL1++; ei++;
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL1[iL1], vi, ei);
    }
    for(; iL2  < jointPointsL2.size(); vi++, iL2++, ei++)
    {
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL2[iL2], vi, ei);
        vi++; iL2++; ei++;
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL2[iL2], vi, ei);
    }
    for(; il1  < endl1; vi++, il1++, ei++)
    {
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[il1], vi, ei);
        vi++; il1++; ei++;
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[il1], vi, ei);
    }

    tri::UpdateTopology<CMeshO>::EdgeEdge(em);

    // 3- REMOVING THE EDGE THAT ARE INSIDE THE RECTANGLE
//    Log("PHASE 3");
    vcg::tri::UpdateFlags<CMeshO>::EdgeClearV(em);  // set the flags to see if they were visited by the sub-cycle (visiting one silhouette at a time)

    CVertexO *firstJV = &(em.vert[0]) + ivS; // address of the first point of joint
    CEdgeO *firstE = &(em.edge[0]) + ieS;
    CEdgeO *endE = &(em.edge[0]) + em.edge.size();

    //from firstE included we have only external edge of the rectangle (but not ALL external) that have a vertex on the rectangle
    for(CEdgeO *corrE = firstE; corrE < endE; corrE++)   //starting from the new edge just entered surely take all the clippings and we start from edge external (for construction) which have at least one vertex is the joint
    {
        if (!corrE->IsV())
        {
            corrE->SetV();   //theoretically it would not (can not exist outside edge only if the silhouette is closed and I can not go through any other routes for this edge)
//Log("true START e %i %i %i", &(*corrE)-&(em.edge[0]), &(*(corrE->V(0)))-&(em.vert[0]),&(*(corrE->V(1)))-&(em.vert[0]));
            char ind = (corrE->V(0) >= firstJV) ? 0 : 1;  //take the joint vertex
//if(corrE->V(ind) < firstJV) Log("ERROR!");

            edge::Pos<CEdgeO> posCorr(&(*corrE),ind);
            posCorr.FlipE(); //flipping the edge and having a joint vertex I enter in the rectangle
            tri::Allocator<CMeshO>::DeleteEdge(em,*(posCorr.E())); // mark that the edge current is eliminated

//Log("START e %i %i %i", &(*posCorr.E())-&(em.edge[0]), &(*posCorr.E()->V(0))-&(em.vert[0]),&(*posCorr.E()->V(1))-&(em.vert[0]));
            do
            {
                posCorr.FlipV();
                posCorr.FlipE();
                posCorr.E()->SetV();

                //if not meet a Joint, posCorr refers to  interior edge and vertex
                if(posCorr.V() < firstJV)
                {
                    tri::Allocator<CMeshO>::DeleteVertex(em,*(posCorr.V()));
                    tri::Allocator<CMeshO>::DeleteEdge(em,*(posCorr.E())); // mark that the edge current is eliminated
//if(posCorr.E() < firstE) Log("ERROR");    //until I am inside I can not find any new edge

                }
                else    //in this case we are out of the rectangle, because this is a joint vertex
                {
//Log("e %i %i %i", &(*posCorr.E())-&(em.edge[0]), &(*posCorr.E()->V(0))-&(em.vert[0]),&(*posCorr.E()->V(1))-&(em.vert[0]));
                    ;
                }
            }while(posCorr.V() < firstJV);
        }
    }

    // 4- ADDING THE EDGE OF CLOSURE
//    Log("PHASE 4");
    nJP = (nJP/2)+addEC;      //the edge of closure is equal to the number of vertices divided by 2 + the edge the point of the rectangle
//?? nJP = em.vert.size() - em.edge.size();
    ei = vcg::tri::Allocator<CMeshO>::AddEdges(em,nJP);
    CMeshO::EdgeIterator eEnd = ei+nJP;

    if(nEC==2)  //there is only one point of the rectangle (2 edge adjacent to initialize)
    {
        //e0
        ei->V(0)=firstJV;   //v0
        firstJV++;
        ei->V(1)=firstJV;   //v1
        ei++;   //e1 (not move on new vertex)
        ei->V(0)=firstJV;   //v1
        firstJV++;
        ei->V(1)=firstJV;   //v2

        ei++;
        firstJV++;

    }else if(nEC==3)
    {
        //e0
        ei->V(0)=firstJV;   //v0
        firstJV++;
        ei->V(1)=firstJV;   //v1
        ei++;   //e1 (not move on new vertex)
        ei->V(0)=firstJV;   //v1
        firstJV++;
        ei->V(1)=firstJV;   //v2
        ei++;   //e2
        ei->V(0)=firstJV;   //v2
        firstJV++;
        ei->V(1)=firstJV;   //v3

        ei++;
        firstJV++;

    }else if(nEC==4)
    {
        //e0
        ei->V(0)=firstJV;   //v0
        firstJV++;
        ei->V(1)=firstJV;   //v1
        ei++;   //e1
        ei->V(0)=firstJV;   //v1
        firstJV++;
        ei->V(1)=firstJV;   //v2
        firstJV++;// (in this case I move on new vertex)
        ei++;   //e2
        ei->V(0)=firstJV;   //v3
        firstJV++;
        ei->V(1)=firstJV;   //v4
        ei++;   //e1
        ei->V(0)=firstJV;   //v4
        firstJV++;
        ei->V(1)=firstJV;   //v5

        ei++;
        firstJV++;
    }

/*
//I can replace the previous part with an unroll of a for
template<int N>
struct StaticUnroller
{
        static void step()
        {
                ei->V(0)=firstJV;
                firstJV++;
                ei->V(1)=firstJV;	//per l'iterazione successiva non avanzo con il secondo vertice
                ei++;
                StaticUnroller<int N-1>::step();
        }
}

//stop of StaticUnroller
template<int 0>
struct StaticUnroller
{
        static void step() { }
}


    if(incSpig==??)
    {
        StaticUnroller<3>::step();
    }else if(incSpig==??)
    {
        StaticUnroller<2>::step();
        firstJV++;  // stavolta sposto il vertice
        StaticUnroller<2>::step();
    }

*/
    for(; ei < eEnd; firstJV++, ei++)
    {
        ei->V(0)=firstJV;
        firstJV++;
        ei->V(1)=firstJV;
    }

    //Only at the end I delete everything not I need (so I have no problem with index at the previous phase)
    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);

//    assert(em.vert.size() == em.edge.size());

    tri::UpdateTopology<CMeshO>::EdgeEdge(em);

    return inc;
}


Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
