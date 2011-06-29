
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
            parlst.addParam(new RichFloat("eps",0.08,"Thickness of a planes","Thickness of the plans generated. Must be between 0 and 1. If > 1 the extrusion of the plans overlap."));
//            parlst.addParam(new RichFloat("eps",1.0,"Medium thickness","Thickness of the medium where the pieces will be cut away"));
            parlst.addParam(new RichFloat("spacing",0.1,"Space between two planes", "Distance between planes. Must be between 0 and 1 and indicates how many parts to cut the figure."));
//            parlst.addParam(new RichFloat("planeNum",1,"Number of planes", "Number of plans that are automatically generated."));
            parlst.addParam(new RichBool("hideBase",true,"Hide Original Mesh","Hide the Original Mesh"));
            parlst.addParam(new RichBool("hideEdge",true,"Hide Edge Mesh","Hide the Generated Edge Mesh"));
            parlst.addParam(new RichBool("hidePlanes",false,"Hide Planes","Hide the Generated Slicing Planes"));
            parlst.addParam(new RichBool("hideExtrudes",true,"Hide Extrusions","Hide the Generated Estrusions"));
//            parlst.addParam(new RichBool("singleFile", true, "Single SVG","Automatically generate a series of cross-sections along the whole length of the object and store each plane in a separate SVG file. The distance between each plane is given by the step value below"));
        }
        break;
    default : assert(0);
    }
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
            switch(axis)
            {
            case X:
                {
                    axisOrthog = (Axis) Succ<X>::value;
                    axisJoint = (Axis) Succ<Succ<X>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[1],sizeCm[2]);
                    prOrth.sizeCm = Point2f(sizeCm[0],sizeCm[2]);
                }
                break;
            case Y:
                {
                    axisOrthog = (Axis) Succ<Y>::value;
                    axisJoint = (Axis) Succ<Succ<Y>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[0],sizeCm[2]);
                    prOrth.sizeCm = Point2f(sizeCm[0],sizeCm[1]);
                }
                break;
            case Z:
                {
                    axisOrthog = (Axis) Succ<Z>::value;
                    axisJoint = (Axis) Succ<Succ<Z>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[0],sizeCm[1]);
                    prOrth.sizeCm = Point2f(sizeCm[1],sizeCm[2]);
                }
                break;
            }

            Log("sizecm %fx%f",pr.sizeCm[0],pr.sizeCm[1]);

            vector<CMeshO*> ev;

            pr.lineWidthPt=200;
            pr.scale=2/maxdim;
            float planeOffset = parlst.getFloat("planeOffset");
            Point3f planeCenter;
            Plane3f slicingPlane;

            if(parlst.getFloat("spacing") >= 1)
            {
                Log("the selected distance between the planes is greater than 1. The filter had no effect");
                return false;
            }
            const float planeDist = maxdim * parlst.getFloat("spacing");
//            const int planeNum = parlst.getFloat("planeNum");

  //          const float planeDist = (planeNum == 1) ? 1 : ( (bbox.Dim()*planeAxis)/ planeNum-1 );
            const int planeNum = (planeDist == 0) ? 1 : ( ((bbox.Dim()*planeAxis)/planeDist)+1 ); //evito la divisione per 0

            float eps =planeDist*2* parlst.getFloat("eps");
//            const float eps= planeDist / parlst.getFloat("eps");
//            eps=maxdim*(eps/length);

            Log("dddddddddd %f %f %f %f %f",eps, planeDist, planeNum, maxdim, parlst.getFloat("spacing"));

//            if(planeNum <= 0) return true;


            pr.numCol=(int)(max((int)sqrt(planeNum*1.0f),2)+1);
            pr.numRow=(int)(planeNum*1.0f/pr.numCol)+1;
            pr.projDir = planeAxis;
            pr.projCenter =  m.mm()->cm.bbox.Center();
            pr.enumeration = true;

            const float lengthDiag = bbox.Diag()/2.0;

            planeCenter = bbox.Center() + planeAxis*planeOffset*lengthDiag;
            planeCenter[axis] -= (planeNum/2) * planeDist;  //partendo dal centro devo andare indietro della metà


            Segment2f lati[3]; //il rettangolo: è formato da 3 segmenti, il quarto lato lo ignoro, tanto non lo uso mai

            const float epsTmp = eps/2.0;
            if(eps < 1)
                generateRectSeg(0, epsTmp, bbox.Diag()*2, lati);
            else
            {
                generateRectSeg(0, maxdim, bbox.Diag()*2, lati);
                Log("thickness is greater than 1: the extrusions will overlap");
            }

            MeshModel* cap;
            MeshModel* cap2;
            MeshModel* extr;
            QString layername;

            float start = planeCenter[axis];
//*
            Incastri inc[planeNum][planeNum];
            //planeNum
            int numAdd = 0;
            for(int i = 0; i < planeNum; ++i)
            {
//                Log("######## LAYER %i", i);
                planeCenter[axis] = start + planeDist*i;;

                //planeCenter[axis] += planeDist;
                slicingPlane.Init(planeCenter,planeAxis);

                //this is used to generate svg slices
                layername.sprintf("EdgeMesh_%d",numAdd);
                cap= m.addNewMesh("",qPrintable(layername));
                vcg::IntersectionPlaneMesh<CMeshO, CMeshO, float>(base->cm, slicingPlane, cap->cm );

                tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);
                if(cap->cm.edge.size()>= 3)     //se la mesh ha intersezione con il piano e genera almeno una faccia
                {
                    Incastri temp;
                    if(eps >= 1)
                    {
                        temp = subtraction(cap->cm, lati, axis, axisOrthog, axisJoint, planeCenter[axis]);
                        if(temp == NOT_PLANE) {Log("ATTENTION! The IntersectionPlaneMesh did not return a plane silhouette in the current plane!"); m.delMesh(cap); break;}
                        if(temp== NOT_SAGOME) {Log("ATTENTION! The IntersectionPlaneMesh did not return a simple closed silhouette for the plane %i.",i); break;;}
                    }
                    else
                        for(int j = 0; j < planeNum; ++j)   //ritaglio  le intersezioni
                        {
//                            Log("#### RECTANGLE %i", j);
                            float newDist = start + planeDist*j;
                            float yMag = epsTmp + newDist;
                            float yMin = -epsTmp + newDist;
                            // shifta rect (solo su Y ovviamente)
                            lati[L1].P0().Y() = yMag;
                            lati[L1].P1().Y() = yMag;
                            lati[L2].P0().Y() = yMin;
                            lati[L2].P1().Y() = yMin;
                            lati[l1].P0().Y() = yMag;
                            lati[l1].P1().Y() = yMin;

                            temp = subtraction(cap->cm, lati, axis, axisOrthog, axisJoint, planeCenter[axis]);

                            inc[i][j] = temp;
                            if(temp == NOT_PLANE) {Log("ATTENTION! The IntersectionPlaneMesh did not return a plane silhouette in the current plane!"); m.delMesh(cap); break;}
                            if(temp== NOT_SAGOME) {Log("ATTENTION! The IntersectionPlaneMesh did not return a simple closed silhouette for the plane %i.",i); m.delMesh(cap); break;}
//                            if(temp==BROKEN) Log("ATTENTION! The first plane %i is broken fron the plane %i",i,j);
                        }
                    if(temp > NOT_SAGOME)
                    {
                        ev.push_back(&(cap->cm));                       //aggiungo alle sagome da esportare in SVG

                        layername.sprintf("CappedSlice_%d",numAdd);   //aggiungo la sagoma riempita
                        cap2= m.addNewMesh("",qPrintable(layername));
                        tri::CapEdgeMesh(cap->cm, cap2->cm);

                        layername.sprintf("Extruded_%d",numAdd++);      //aggiungo la sagoma estrusa
                        extr= m.addNewMesh("",qPrintable(layername));
                        cap2->updateDataMask(MeshModel::MM_FACEFACETOPO);
                        extr->updateDataMask(MeshModel::MM_FACEFACETOPO);
                        tri::UpdateTopology<CMeshO>::FaceFace(cap2->cm);
                        tri::ExtrudeBoundary<CMeshO>(cap2->cm,extr->cm,eps,planeAxis);

                        if(hideEdge) cap->visible =false;
                        if(hidePlanes) cap2->visible =false;
                        if(hideExtrudes) extr->visible =false;
                    }
                }else m.delMesh(cap);   //se non esiste intersezione non consider neanche il risultato
            }

            QString fname;//= parlst.getSaveFileName("filename");
            if(fname=="") fname="C:/Slice.svg";
            if (!fname.endsWith(".svg")) fname+=".svg";

            tri::io::ExporterSVG<CMeshO>::Save(ev, fname.toStdString().c_str(), pr);

//*/

//*
            vector<CMeshO*> evOrth;

            planeAxis[axis] = 0.0f;
            planeAxis[axisOrthog] = 1.0f;

            planeCenter = bbox.Center() + planeAxis*planeOffset*lengthDiag;
            planeCenter[axisOrthog] -= (planeNum/2) * planeDist;  //partendo dal centro devo andare indietro della metà

            if(eps < 1)
                 generateRectSeg(0, epsTmp, -bbox.Diag()*2, lati);
            else
            {
                generateRectSeg(0, maxdim, -bbox.Diag()*2, lati);
                Log("thickness is greater than 1: the extrusions will overlap");
            }

            prOrth.lineWidthPt=200;
            prOrth.scale=2/maxdim;
            prOrth.numCol=(int)(max((int)sqrt(planeNum*1.0f),2)+1);
            prOrth.numRow=(int)(planeNum*1.0f/prOrth.numCol)+1;
            prOrth.projDir = planeAxis;
            prOrth.projCenter =  m.mm()->cm.bbox.Center();
            prOrth.enumeration = true;

            Incastri situationForPlaneI[planeNum];
            Incastri situationForPlaneJ[planeNum];

            for(int k = 0; k < planeNum; ++k)
            {
                situationForPlaneI[k] = NILL;
                situationForPlaneJ[k] = NILL;
            }

            numAdd = 0;
            //planeNum
            for(int j = 0; j < planeNum; ++j)
            {
//                Log("######## LAYER INV %i", j);
                planeCenter[axisOrthog] = start + planeDist*j;
//                planeCenter[axisOrthog] += planeDist;

                slicingPlane.Init(planeCenter,planeAxis);

                //this is used to generate svg slices
                layername.sprintf("EdgeMesh_O_%d",numAdd);
                cap= m.addNewMesh("",qPrintable(layername));
                vcg::IntersectionPlaneMesh<CMeshO, CMeshO, float>(base->cm, slicingPlane, cap->cm );

                tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);

                if(cap->cm.edge.size()>= 3)     //se la mesh ha intersezione con il piano e genera almeno una faccia
                {
                    Incastri temp;
                    if(eps >= 1)
                    {
                        temp = subtraction(cap->cm, lati, axisOrthog, axis, axisJoint, planeCenter[axisOrthog]);
                        if(temp == NOT_PLANE) {Log("ATTENTION! The IntersectionPlaneMesh did not return a plane silhouette in the current plane!"); m.delMesh(cap); break;}
                        if(temp== NOT_SAGOME) {Log("ATTENTION! The IntersectionPlaneMesh did not return a simple closed silhouette for the plane %i.",j); break;;}
                    } else
                    for(int i = 0; i < planeNum; ++i)
                    {
//                      Log("#### RECTANGLE %i", i);

                        if(inc[i][j]!=ZERO_INCASTRO)   //se le due sagome non si incastravano al ciclo prima non lo fanno neanche ora
                        {
                            float newDist = start + planeDist*i;
                            float yMag = epsTmp + newDist;
                            float yMin = -epsTmp + newDist;
                            // shifta rect (solo su Y ovviamente)
                            lati[L1].P0().Y() = yMag;
                            lati[L1].P1().Y() = yMag;
                            lati[L2].P0().Y() = yMin;
                            lati[L2].P1().Y() = yMin;
                            lati[l1].P0().Y() = yMag;
                            lati[l1].P1().Y() = yMin;

                            temp = subtraction(cap->cm, lati, axisOrthog, axis, axisJoint, planeCenter[axisOrthog]);

                            if(temp == NOT_PLANE) {Log("ATTENTION! The IntersectionPlaneMesh did not return a plane silhouette in the current plane!"); m.delMesh(cap); break;}
                            if(temp== NOT_SAGOME) {Log("ATTENTION! The IntersectionPlaneMesh did not return a simple closed silhouette for the plane %i.",j); m.delMesh(cap); break;;}
//                            if(temp==BROKEN) Log("ATTENTION! The second plane %i is broken from the plane %i",j,i);
//                            if(temp < inc[i][j]) temp = inc[i][j];  //mi interessa che almeno uno dei 2 piani regga
//                            if(situationForPlaneI[i] < temp) situationForPlaneI[i] = temp;
//                            if(situationForPlaneJ[j] < temp) situationForPlaneJ[j] = temp;
                        }
                    }
                   if(temp > NOT_SAGOME)
                    {
                        evOrth.push_back(&(cap->cm));

                        layername.sprintf("CappedSlice_O_%d",numAdd);
                        cap2= m.addNewMesh("",qPrintable(layername));
                        tri::CapEdgeMesh(cap->cm, cap2->cm);

                        layername.sprintf("Extruded_O_%d",numAdd++);
                        extr= m.addNewMesh("",qPrintable(layername));
                        cap2->updateDataMask(MeshModel::MM_FACEFACETOPO);
                        extr->updateDataMask(MeshModel::MM_FACEFACETOPO);
                        tri::UpdateTopology<CMeshO>::FaceFace(cap2->cm);
                        tri::ExtrudeBoundary<CMeshO>(cap2->cm,extr->cm,eps,planeAxis);

                        if(hideEdge) cap->visible =false;
                        if(hidePlanes) cap2->visible =false;
                        if(hideExtrudes) extr->visible =false;
                    }
                }else m.delMesh(cap);   //se non esiste intersezione non consider neanche il risultato
            }

            QString fnameOrth;//= parlst.getSaveFileName("filename");
            if(fnameOrth=="") fnameOrth="C:/SliceOrth.svg";
            if (!fnameOrth.endsWith("Orth.svg")) fnameOrth+="Orth.svg";

            tri::io::ExporterSVG<CMeshO>::Save(evOrth, fnameOrth.toStdString().c_str(), prOrth);

/*
            for(int k = 0; k < planeNum; ++k)
            {
                if(situationForPlaneI[k] < TWO_CONTACT) Log("ATTENTION! The first plane %i has no joints",k);
                else if(situationForPlaneI[k] == TWO_CONTACT) Log("ATTENTION! The first plane %i may not be stable",k);

                if(situationForPlaneJ[k] < TWO_CONTACT) Log("ATTENTION! The second plane %i has no joints",k);
                else if(situationForPlaneJ[k] == TWO_CONTACT) Log("ATTENTION! The second plane %i may not be stable",k);
            }
//*/
            if(hideEdge) cap->visible =false;
            if(hidePlanes) cap2->visible =false;
            if(hideExtrudes) extr->visible =false;

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

void ExtraFilter_SlicePlugin::generateRectSeg(const float &dist, const float &epsTmp, const float &lengthDiag, Segment2f lati[])
{
    //?? rimuovi dist yMag e yMin
    //variabili di supporto
    float yMag = dist+epsTmp;
    float yMin = dist-epsTmp;

    //punti per il rettangolo
    Point2f a1,b1,a2,b2;    //a1 e a2 sono i punti + interni
    a1.X() = 0;
    a1.Y() = yMag;

    a2.X() = 0;
    a2.Y() = yMin;

    b1.X() = lengthDiag;
    b1.Y() = yMag;

    b2.X() = lengthDiag;
    b2.Y() = yMin;

    //inizializzo i segmenti
    lati[L1].Set(a1,b1);    //lato con Y maggiore
    lati[L2].Set(a2,b2);    //lato con Y minore
    lati[l1].Set(a1,a2);    //lato minore che passa per il centro (0,0)
    //il lato minore più lontano non sereve

    //per recuperare
    //yMag == lati[L1].P0.Y();
    //yMin == lati[L2].P0.Y();
}

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
//Log("Edge spezzato %i", jp.e);
//Log("Edge nuovo %i", &(*ei) - &(em.edge[0]));
//Log("Indirizzo V1 spostato su nuovo edge %i", em.edge[jp.e].V(1) - &(em.vert[0]));
//og("Indirizzo V Joint %i", &(*vi) - &(em.vert[0]));
//Log("IndV %i", jp.indV);    //indice del vertice esterno o collineare
//Log("Collineare %i", jp.collinear);

    if(jp.collinear) //e' come se spostassi (li copio e cancello i vecchi) il vertice collineare ed uno dei 2 vertici che vi incidono tra edge e vertici di joint
    {
        (*vi).P() = em.edge[jp.e].V(jp.indV)->P();  //duplico il vertice collineare di joint (deve stare tra i punti di joint, in fondo al vettore dei vertici)
//        (*vi).SetV();

        CEdgeO *e2 = em.edge[jp.e].EEp(jp.indV);    //passo all'altro edge incidente sul vertice collineare
        int indV2 = em.edge[jp.e].EEi(jp.indV);     //indice del vertice collineare sul nuovo edge
        e2->V(indV2) = &(*vi);      //sostituisco il punto (sono identici ma il nuovo è messo tra i punti di joint)
//Log("altro edge %i",&(*e2) - &(em.edge[0]));
//Log("indV2 %i",indV2);

        int otherV =(jp.indV+1)%2;
        Log("bubu %i",otherV);
        ei->V(otherV) = em.edge[jp.e].V(otherV);    //copio l'altro vertice (non collineare) nel nuovo edge
        ei->V(jp.indV) = &(*vi);                    //metto il punto di Joint collineare come altro vertice

        em.edge[jp.e].V(otherV) = em.edge[jp.e].V(jp.indV);     //sconnetto completametne il vecchio edge (riferisce solo al vertice che verrà eliminato: lo rendendolo degenere); cosi quando ricostruisco la topologia non da noie (non lo elimino subito ma solo dopo aver rimosso gli archi interni)
        tri::Allocator<CMeshO>::DeleteVertex(em,*( em.edge[jp.e].V(jp.indV) )); //segno che il vecchio punto va rimosso
        tri::Allocator<CMeshO>::DeleteEdge(em, em.edge[jp.e]);                  //segno che l'edge precedente va eliminato perche' sostituito
        //dopo non li rimuovo subito tanto devo trovare anche gli edge interni da rimuovere

    }else
    {
        //SPEZZO IN DUE UN EDGE
        //converto il punto di intersezione in 3D (la terza coordinata pJoint[axis] è sempre la stessa per tutti i punti)
        pJoint[axisJoint] = jp.p.X();
        pJoint[axisOrthog] = jp.p.Y();

        //aggiungo un solo vertice per il punto di intersezione (i 2 edge usano i vertici dell'edge corrente e in + solo il nuovo vertice d'intersezione)
        (*vi).P() = pJoint;     //aggiungo il nuovo vertice di intersezione
//        (*vi).SetV();           //indico che è un punto di Joint per la successiva fase di cancellazione (non serve: sono tutti in fondo al vector)
        //spezzo l'edge corrente in due edge sul punto di intersezione: aggiungo solo il secondo edge e per il primo edge uso l'edge corrente

        //il nuovo edge ha...
        ei->V(0) = &(*vi);              //... come vertice 0 il nuovo vertice di intersezione e ...

        ei->V(1) =em.edge[jp.e].V(jp.indV);   //... come vertice 1 setto il vertice dal lato giusto (indV1) dell'edge corrente

        //l'edge 1, fatto sull'edge corrente, ha il vertice 0 inalterato, mentre...
        em.edge[jp.e].V(jp.indV) = &(*vi);       //...il vertice 1 diventa il nuovo vertice di intersezione
    }

}

bool inOrthogonalEdge(const float &rangeMin, const float &rangeMax, const float &equal, const float &coordInRange, const float &coordEq)
{
    if(rangeMin<= coordInRange && coordInRange <= rangeMax && equal == coordEq) return true;
    return false;
}

Incastri ExtraFilter_SlicePlugin::subtraction(CMeshO &em, Segment2f lati[], const Axis &axis, const Axis &axisOrthog, const Axis &axisJoint, const float height)
{
    //dalla mesh testata venivano mesh con troppi vertici rispetto al numero di edge (che dovrebbero essere uguali)
    Incastri inc;

    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);


    tri::UpdateTopology<CMeshO>::EdgeEdge(em);

    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);

//    assert(em.vert.size() == em.edge.size());   //se ogni sagoma è chiusa deve valere
    assert(axis != axisOrthog && axisOrthog != axisJoint && axisJoint != axis);

    // 1- RICERCA DEI PUNTI DI INTERSEZIONE TRA MESH E RETTANGOLO
//    Log("FASE 1");
    tri::UpdateTopology<CMeshO>::EdgeEdge(em);
    vcg::tri::UpdateFlags<CMeshO>::EdgeClearV(em);  // setto i flag per sapere se sono stati visitati dal sottociclo (che visita una sagoma alla volta)

    //variabili usate per convertire l'edge esaminato in 2D cosi da poter usare la funzione di intersezione
    Segment2f seg;
    Point2f p0_2D, p1_2D;

    Point2f pJoint2D;       //punto di intersezione 2D inizializzato dal test di intersezione

    //vettori che memorizzano i punti di intersezione trovati, uno per ogni lato del rettangolo
    vector<JointPoint> jointPointsL1;
    vector<JointPoint> jointPointsL2;
    vector<JointPoint> jointPointsl1;

    bool cas0, cas1;    //per l'intersezione con il lato minore devo vedere se il rettangolo è sopra o sotto l'origine per scegliere indV
    float xMax, xMin;
    bool rectPos = lati[L1].P1().X() > 0;   //controlla se il rettangolo è positivo o meno
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
            edge::Pos<CEdgeO> eStart(&em.edge[i],0);    //memorizza l'edge di partenza
            edge::Pos<CEdgeO> eCorr = eStart;           //itera sugli edge
//Log("Nuova Forma");
            bool enableL1,enableL2,enablel1;    //flag che gestiscono abilitano i test di intersezione con i 3 laii del rettangolo; gestiscono l'aggiunta o meno di punti collineari
            enableL1=enableL2=enablel1=true;    //la prima volta che incontro un colineare lo ignoro, la seconda se proseguo sulla stessa direzione lo aggiungo (altrimenti e' di rimbalzo)
            bool lastDir;                       //indica se ero dentro o fuori l'ultima volta che ho incontrato un colineare: puo' essere positiva o negativa
            JointPoint firstCollinearP;         //per i vertici collineari guardo i due edge adiacenti per vedere se si attraversa effettivamente il lato e in tal caso lo riaggiungo

            do
            {
                eCorr.E()->SetV();
                Point3f &p = eCorr.V()->P();

                if(! ((height - numeric_limits<float>::epsilon() < p[axis]) && (p[axis] < height + numeric_limits<float>::epsilon())) ) return NOT_PLANE;
//Log("p axis %f, height %f",p[axis],height);

                Point3f &p0 = eCorr.E()->V(0)->P();
                Point3f &p1 = eCorr.E()->V(1)->P();

                if(p0==p1){eCorr.NextE();Log("ci sono archi degeneri");continue;} //salto gli archi degeneri
//Log("p0 axis %f, p1 axis %f",test1,test2);

                float test1, test2;
                test1=p0[axis]*1; test2=p1[axis]*1;

                //converto i vertici in 2D: per come ho definito i retangli di ritaglio la X è lungo axisJoint e la Y è lungo axisOrthog
                p0_2D.X() = p0[axisJoint];
                p0_2D.Y() = p0[axisOrthog];
                p1_2D.X() = p1[axisJoint];
                p1_2D.Y() = p1[axisOrthog];


                //inizializzo il segmento 2D da usare per il test di intersezione
                seg.Set(p0_2D,p1_2D);

                int ei = eCorr.E() - &(em.edge[0]); //indice dell'edge corrente
//bool c1,c2,c3;
//c1=c2=c3= false;

                if( ((!enableL1)||(!enableL2)||(!enablel1)) && (seg.P0().Y()==seg.P1().Y()||seg.P0().X() == seg.P1().X()) ) break;  //non considero neanche i casi in cui l'edge è collineare ad uno dei lati
                //???????ma non entra mai nel caso seguente? se ho beccato un punto collineare su un lato posso saltare il controllo di segmenti collineari per quel lato ma non per gli altri due lati perché potrei uscire da loro

                //un segmento puo' intersecare contemporaneamente 2 lati (ma non 3) quindi non posso mettere else sui 3 casi dei lati

                if(seg.P0().Y() != seg.P1().Y())
                {
                    int indV = (seg.P0().Y() > seg.P1().Y()) ? 0 : 1;  //indice del vertice esterno al rettangolo; in questo edge verrà sostituito con il punto di joint per formare il primo edge dello spezzamento

                    // L1
                    //uno dei vertici di seg potrebbe coincidere con il punto di intersezione. In tal caso significa che giace sul lato (lo testo a parte perche' SegmentSegmentIntersection non lo gestisce bene)
                    if(inOrthogonalEdge(xMin,xMax,yMax,seg.P0().X(), seg.P0().Y()))
                    {
                        if(enableL1)    //la prima volta che incontro un colineare non lo considero ma segno che l'ho incontrato: il prossimo punto di joint che incontrer0' sara' sicuramente un colineare
                        {
                            enableL1=false;
                            lastDir = (seg.P1().Y() > yMax) ? true : false; //per vedere la direzione devo guardare l'altro punto che non e' colineare
                            JointPoint newJoint(seg.P0(), ei, 0, true);

                            firstCollinearP = newJoint;     //tengo da parte il punto collineare trovato fino a quando non vedo se sono uscito realmente o meno
//Log("L1 p0 non aggiunto: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
//Log("L1 p0 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL1=true;
                            bool dir = (seg.P1().Y() > yMax) ? true : false;
                            if(lastDir!=dir)    //aggiungo solo se non e' un punto di rimbalzo, ovvero se attraverso realmente l'asse
                            {
                                jointPointsL1.push_back(firstCollinearP);
//Log("L1 p0 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(inOrthogonalEdge(xMin,xMax,yMax,seg.P1().X(), seg.P1().Y()))
                    {
                        if(enableL1)    //la prima volta che incontro un colineare non lo considero ma segno che l'ho incontrato: il prossimo punto di joint che incontrer0' sara' sicuramente un colineare
                        {
                            enableL1=false;
                            lastDir = (seg.P1().Y() > yMax) ? true : false; //per vedere la direzione devo guardare l'altro punto che non e' colineare
                            JointPoint newJoint(seg.P1(), ei, 1, true);

                            firstCollinearP = newJoint;
//Log("L1 p1 non aggiunto: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
//Log("L1 p1 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL1=true;
                            bool dir = (seg.P0().Y() > yMax) ? true : false;
                            if(lastDir!=dir)    //aggiungo solo se non e' un punto di rimbalzo, ovvero se attraverso realmente l'asse
                            {
                                jointPointsL1.push_back(firstCollinearP);
//Log("L1 p1 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(vcg::SegmentSegmentIntersection(lati[L1],seg,pJoint2D))    //grazie ai test precedenti questo caso fa intersezione solo quando nn sono collineari
                    {
                        if(!enablel1)
                        {
                            if(rectPos == lastDir>0)
                            {
                                jointPointsl1.push_back(firstCollinearP);
                                enablel1 = true;
                            }
//Log("aggiunto vecchio");
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
                        if(enableL2)    //la prima volta che incontro un colineare non lo considero ma segno che l'ho incontrato: il prossimo punto di joint che incontrer0' sara' sicuramente un colineare
                        {
                            enableL2=false;
                            lastDir = (seg.P1().Y() < yMin) ? true : false;
                            JointPoint newJoint(seg.P0(), ei, 0, true);

                            firstCollinearP = newJoint;
//Log("L2 p0 non aggiunto: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
//Log("L2 p0 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL2=true;
                            bool dir = (seg.P1().Y() < yMin) ? true : false;
                            if(lastDir!=dir)    //aggiungo solo se non e' un punto di rimbalzo, ovvero se attraverso realmente l'asse
                            {
                                jointPointsL2.push_back(firstCollinearP);
//Log("L2 p0 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(inOrthogonalEdge(xMin,xMax,yMin,seg.P1().X(), seg.P1().Y()))
                    {
                        if(enableL2)    //la prima volta che incontro un colineare non lo considero ma segno che l'ho incontrato: il prossimo punto di joint che incontrer0' sara' sicuramente un colineare
                        {
                            enableL2=false;
                            lastDir = (seg.P0().Y() < yMin) ? true : false;
                            JointPoint newJoint(seg.P1(), ei, 1, true);

                            firstCollinearP = newJoint;
//Log("L2 p1 non aggiunto: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
//Log("L2 p1 dir: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            enableL2=true;
                            bool dir = (seg.P0().Y() < yMin) ? true : false;
                            if(lastDir!=dir)    //aggiungo solo se non e' un punto di rimbalzo, ovvero se attraverso realmente l'asse
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
//Log("aggiunto vecchio");
                        }else
                        {
//Log("Inters L2: %i,(%f %f) (%f %f),(%f %f) (%f %f),(%f %f)",ei,lati[L2].P0().X(),lati[L2].P0().Y(),lati[L2].P1().X(),lati[L2].P1().Y(),seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X(),pJoint2D.Y());
//c2 = true;
                            JointPoint newJoint(pJoint2D, ei, indV, false);
                            jointPointsL2.push_back(newJoint);
                        }
                    }
                }

                if(seg.P0().X() != seg.P1().X())
                {
                    int indV = (seg.P0().X() < seg.P1().X()) ? cas0 : cas1;

                    // l1
                    if(inOrthogonalEdge(yMin,yMax,0,seg.P0().Y(), seg.P0().X()))
                    {
                        if(enablel1)    //la prima volta che incontro un colineare non lo considero ma segno che l'ho incontrato: il prossimo punto di joint che incontrer0' sara' sicuramente un colineare
                        {
                            enablel1=false;
                            lastDir = (seg.P1().X() < 0) ? false : true;
                            JointPoint newJoint(seg.P0(), ei, 0, true);

                            firstCollinearP = newJoint;
//Log("l1 p0 non aggiunto: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
                            enablel1=true;
                            bool dir = (seg.P1().X() < 0) ? false : true;
//Log("l1 p0 dir [%i %i]: %i, (%f %f) (%f %f)",lastDir,dir,ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            if(lastDir!=dir)    //aggiungo solo se non e' un punto di rimbalzo, ovvero se attraverso realmente l'asse
                            {
                                jointPointsl1.push_back(firstCollinearP);
//Log("l1 p0 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }
                    else if(inOrthogonalEdge(yMin,yMax,0,seg.P1().Y(), seg.P1().X()))
                    {
                        if(enablel1)    //la prima volta che incontro un colineare non lo considero ma segno che l'ho incontrato: il prossimo punto di joint che incontrer0' sara' sicuramente un colineare
                        {
                            enablel1=false;
                            lastDir = (seg.P0().X() < 0) ? false : true;
                            JointPoint newJoint(seg.P1(), ei, 1, true);

                            firstCollinearP = newJoint;
//Log("l1 p1 non aggiunto: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                        }else   //la seconda volta che incontro un colineare
                        {
                            enablel1=true;
                            bool dir = (seg.P0().X() < 0) ? false : true;
//Log("l1 p1 dir [%i %i]: %i, (%f %f) (%f %f)",lastDir,dir,ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            if(lastDir!=dir)    //aggiungo solo se non e' un punto di rimbalzo, ovvero se attraverso realmente l'asse
                            {
                                jointPointsl1.push_back(firstCollinearP);
//Log("l1 p1 coll: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
                            }
                        }
                    }else if(vcg::SegmentSegmentIntersection(lati[l1],seg,pJoint2D))
                    {
                        if(!enableL1)
                        {
//Log("aggiunto vecchio");
                            if(lastDir<xMax)    //aggiungo il vecchio punto collineare trovato in precedenza solo se ero dentro
                            {
                                jointPointsL1.push_back(firstCollinearP);
                                enableL1 = true;    //sono uscito dall'altro lato
                            }
                        }else if(!enableL2)
                        {
//Log("aggiunto vecchio");
                            if(lastDir>xMin)
                            {
                                jointPointsL2.push_back(firstCollinearP);
                                enableL2 = true;
                            }
                        }else
                        {
                            JointPoint newJoint(pJoint2D, ei, indV, false);
                            jointPointsl1.push_back(newJoint);
//Log("Inters l1: %i, (%f %f) (%f %f),(%f %f) (%f %f),(%f %f)",ei,lati[l1].P0().X(),lati[l1].P0().Y(),lati[l1].P1().X(),lati[l1].P1().Y(),seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X(),pJoint2D.Y());
//c3 = true;
                        }
                    }
                }



//Log("ALL: %i, (%f %f) (%f %f)",ei,seg.P0().X(),seg.P0().Y(),seg.P1().X(),seg.P1().Y(),pJoint2D.X());
//test da rimovere
//if(c1){
//    if(c2)Log("ei L1 L2: %i", ei);
//    else if(c3)Log("ei L1 l1: %i", ei);
//}else if(c2&&c3)Log("ei L2 l1: %i", ei);

                edge::Pos<CEdgeO> prec = eCorr;
                eCorr.NextE();
                if(prec==eCorr) return NOT_SAGOME;

            }while(eCorr != eStart);//(!eCorr.E()->IsV());//(eCorr != eStart); //&& eCorr != NULL && eCorr != ePrec

        }
    }

    int nJP = jointPointsL1.size() + jointPointsl1.size() + jointPointsL2.size();
    if(nJP == 0) return ZERO_INCASTRO; // se non ho trovato punti di intersezione nn devo fare nulla: esco

if(nJP%2 != 0)Log("ERRORE: ho trovato almeno un punto di joint di troppo"); //cambia con assert
        /**************
        Log("Log per testare");
        Log("N vert %i", em.vert.size());
        Log("N edge %i", em.edge.size());

        Log("stampa vertici e edge");
        for(size_t i = 0; i < em.vert.size(); i++)
        {
            if(em.vert[i].IsD()) Log("{{{");
            Log("Vertex %i (%f,%f,%f)", i, em.vert[i].P().X(), em.vert[i].P().Y(), em.vert[i].P().Z());
            if(em.vert[i].IsD()) Log("}}}");
        }
        for(size_t i = 0; i < em.edge.size(); i++)
        {
            if(em.edge[i].IsD()) Log("{{{");
            CVertexO* start = &(em.vert[0]);
            Log("Edge %i (%i,%i)", i, &(*em.edge[i].V(0))-start,&(*em.edge[i].V(1))-start);
            if(em.edge[i].IsD()) Log("}}}");
        }
    //    **********/
// 2- AGGIUNTA DEI PUNTI DI INTERSEZIONE SPEZZANDO GLI EDGE DELLA MESH
//    Log("FASE 2");
    //percorrendo un lato lungo del rettangolo (L1 o L2) tutte le volte che entro nella sagoma attraverso un punto di joint devo passare da un'altro punto di joint per uscire dalla sagoma.
    //Dunque se ho un numero pari di punti di joint lungo un lato, dopo averlo percorso sono fuori dalla sagoma.
    //Se invece i joint sono dispari alla fine sono ancora dentro la sagoma e devo aggiungere lo spigolo del rettangolo.
    //Nota 1: quindi ho di nuovo un numero pari di vertici su L1 e L2.
    //Nota 2: se l1 sia pari o dispari dipende direttamente da L1 e L2.
    //Nota 3: nJP è sicuramente pari perchè seguendo il percorso L1-l1-L2 parto da fuori, passo sempre da un punto di joint e alla fine sono di nuovo fuori (e ovviamente resta pari anche se aggiungo i due spigoli)

    Point3f pJoint;         //punto di intersezione 3D, usato per convertire i pJoint2D in 3D ed inserirli nell'edgeMesh
    pJoint[axis] = height;  //la terza coordinata è uguale per tutti i punti dato che lavoriamo in 2D

// stampa dei punti di intersezione trovati
//for(int k=0;k<jointPointsl1.size();k++)
//    Log("(%f %f) %i, %i, %i",jointPointsl1[k].p.X(),jointPointsl1[k].p.Y(),jointPointsl1[k].e, jointPointsl1[k].l, jointPointsl1[k].indV);

    //ordino i vertici di intersezione lungo i lati del rettangolo
    std::sort(jointPointsl1.begin(), jointPointsl1.end(), compareX);    //lo ordino per X

    //i lati lunghi li ordino a seconda se il rettangolo ha lunghezza positiva o negativa
    if(cas0)    //se ha lunghezza positiva
    {
        std::sort(jointPointsL1.begin(), jointPointsL1.end(), compareY);
        std::sort(jointPointsL2.begin(), jointPointsL2.end(), compareY);
    }else
    {
        std::sort(jointPointsL1.begin(), jointPointsL1.end(), compareInvY);
        std::sort(jointPointsL2.begin(), jointPointsL2.end(), compareInvY);
    }
//Log("TIPO L1 %i, l1 %i, L2 %i",jointPointsL1.size(),jointPointsl1.size(),jointPointsL2.size());


    //per gli edge di chiusura che aggiungo in seguito (dopo aver rimosso gli edge interni) quello che faccio è tenere un indice ivS che indica da dove ho iniziato a inserire i nuovi vertici e poi inserisco i vertici a coppie in ordine, cosi successivamente li ripesco a coppie e creo gli edge (tratto a parte gli edge incidenti su spigoli)

    size_t iL1, iL2, il1, endl1, ivS, ieS;  //indici per scorrere
    ivS = em.vert.size();                   //indice del primo dei nuovi vertici (usato successivamente per creare gli edge di chiusura)
    ieS = em.edge.size();
    //il ciclo for principale che cerca vertici ed edge da rimuovere puo' anche fermarsi a prima dei nuovi edge aggiunti

    endl1 = jointPointsl1.size();       //indice di coda dei punti di joint sul lato l1

    //iteratori a verice ed edge nuovi da creare
    CMeshO::EdgeIterator ei = vcg::tri::Allocator<CMeshO>::AddEdges(em,nJP);    //di edge aggiungo quelli per quando spezzo un edge nel punto di intersezione: sono nJP dato che nello spezzare riuso l'edge spezzato

    int dispL1 = jointPointsL1.size()%2;
    int dispL2 = jointPointsL2.size()%2;
    int addEC = dispL1 + dispL2;
    int nEC = nJP + addEC; //se serve devo contare anche gli spigoli

    CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertices(em,nEC);

//    assert( (jointPointsl1.size()!=1) || (!dispL1 && dispL2 && jointPointsl1.size() == 1) || (dispL1 && !dispL2 && jointPointsl1.size() == 1) );  // se L1 e L2 sono entrambi dispari l1 non può avere un solo vertice : o ne ha o almeno 2

    if(jointPointsl1.size()!=0)inc = ONE_CONTACT;
    else inc = BROKEN;


    //prima controllo se ci sono spigoli e in caso inserisco in ordine di percorrenza di lati del rettangolo (L1-l1-L2) le parti che vanno congiunte
    if(dispL1)  //se per L1 ho un numero dispari di punti il primo punto va trattato a parte assieme allo spigolo.
    {
        //VERTICE IN L1
        //L1 ha Y maggiore (rispetto a L2) quindi dato l'edge da spezzare il suo vertice con Y maggiore va nel nuovo edge e viene sostituito dal punto di joint
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL1[0], vi, ei);
        vi++; ei++;
        //VERTICE SPIGOLO: aggiungo subito lo spigolo direttamente alla mesh
        pJoint[axisJoint] = lati[L1].P0().X();
        pJoint[axisOrthog] = lati[L1].P0().Y();
        (*vi).P() = pJoint;             //aggiungo il vertice spigolo
        vi++;       // aggiungendo lo spigolo non devo avanzare di edge (i suoi edge verranno aggiunti nella quarta fase)
        iL1 = 1;

        //VERTICE IN l1
        if(endl1 > 0)   //se l1 non è vuoto devo prendere un vertice dal fondo
        {
            endl1--;
            addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[endl1], vi, ei);
            vi++;
            ei++;
            nEC = 2;                        //(riuso nEC) sono etrato da L1 ed esco da l1
            inc = THREE_CONTACT;
        }else {nEC = 3; inc = TWO_CONTACT;} //sono etrato da L1 ed esco da L2
    }else {iL1 = 0; nEC = 0;}

    if(dispL2)  //se per L2 ho un numero dispari di punti il primo punto va trattato a parte assieme allo spigolo.
    {
        //VERTICE IN l1
        if(endl1 > 0)   //se l1 non è vuoto devo prendere un vertice
        {
            addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[0], vi, ei);
            il1 = 1;
            vi++;
            ei++;
            assert(nEC==0 || nEC ==2);
            nEC+=2; //sono entrato da l1 ed esco da L2
        } else {il1 = 0; assert(nEC==3); inc = TWO_CONTACT;} //nEC ha il valore giusto dal caso precedentesono entrato da L1 ed esco da L2
        //VERTICE SPIGOLO: aggiungo subito lo spigolo direttamente alla mesh
        pJoint[axisJoint] = lati[L2].P0().X();
        pJoint[axisOrthog] = lati[L2].P0().Y();
        (*vi).P() = pJoint;             //aggiungo il vertice spigolo
        vi++;   // aggiungendo lo spigolo non devo avanzare di edge (i suoi edge verranno aggiunti nella quarta fase)
        iL2 = 1;
        //VERTICE IN L2
        addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsL2[0], vi, ei);
        vi++; ei++;
    }else{iL2 = 0; il1 = 0;}

    assert( iL2<2 && iL1<2 && il1<2 && il1<=endl1);

    // Tutti gli altri punti sono presi a coppie: posso avanzare di 2 nei 3 cicli successivi perché ho sistemato gli indici in modo che rimanga un numero pari di vertici
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

    /**************
    Log("Log per testare");
    Log("N vert %i", em.vert.size());
    Log("N edge %i", em.edge.size());

    Log("stampa vertici e edge");
    for(size_t i = 0; i < em.vert.size(); i++)
    {
        if(em.vert[i].IsD()) Log("{{{");
        Log("Vertex %i (%f,%f,%f)", i, em.vert[i].P().X(), em.vert[i].P().Y(), em.vert[i].P().Z());
        if(em.vert[i].IsD()) Log("}}}");
    }
    for(size_t i = 0; i < em.edge.size(); i++)
    {
        if(em.edge[i].IsD()) Log("{{{");
        CVertexO* start = &(em.vert[0]);
        Log("Edge %i (%i,%i)", i, &(*em.edge[i].V(0))-start,&(*em.edge[i].V(1))-start);
        if(em.edge[i].IsD()) Log("}}}");
    }
//    **********/


    // 3- RIMOZIONE DEGLI EDGE INTERNI AL RETTANGOLO
//    Log("FASE 3");
    vcg::tri::UpdateFlags<CMeshO>::EdgeClearV(em);  // setto i flag per sapere se sono stati visitati dal sottociclo (che visita una sagoma alla volta)

    CVertexO *firstJV = &(em.vert[0]) + ivS; // indirizzo del primo punto di joint
    CEdgeO *firstE = &(em.edge[0]) + ieS;
    CEdgeO *endE = &(em.edge[0]) + em.edge.size();

    //da firstE compreso in poi abbiamo edge tutti esterni al rettangolo (ma non TUTTI gli esterni) che hanno un vertice sul rettangolo
    for(CEdgeO *corrE = firstE; corrE < endE; corrE++)   //partendo dai nuovi edge appena inseriti prendo sicuramente tutti i ritagli effettuati e parto sicuramente da edge esterni al retangolo (per costruzione) di cui almeno un vertice è di joint
    {
        if (!corrE->IsV())
        {
            //sembra che corrE->SetV() non funzioni
            corrE->SetV();   //teoricamente non servirebbe (non puo' esistere un unico edge esterno se la forma è chiusa e non ci posso passare per altri percorsi per questo edge)

            char ind = (corrE->V(0) >= firstJV) ? 0 : 1;  //guardo quale dei 2 vertici sia il vertice di Joint
//if(corrE->V(ind) < firstJV) Log("ERRORE!");//cambia con assert

            edge::Pos<CEdgeO> posCorr(&(*corrE),ind);
            posCorr.FlipE(); //flippando l'edge e avendo un vertice di joint entro nel rettangolo
            tri::Allocator<CMeshO>::DeleteEdge(em,*(posCorr.E())); // segno che l'edge corrente va eliminato

//Log("START e %i i %i", &(*posCorr.E())-&(em.edge[0]), &(*posCorr.E()->V(0))-&(em.vert[0]),&(*posCorr.E()->V(1))-&(em.vert[0]));
            do
            {
                posCorr.FlipV();
                posCorr.FlipE();

                //se non riattraverso un Joint posCorr riferisce ad edge e vertice interni
                if(posCorr.V() < firstJV)
                {
                    tri::Allocator<CMeshO>::DeleteVertex(em,*(posCorr.V()));
                    tri::Allocator<CMeshO>::DeleteEdge(em,*(posCorr.E())); // segno che l'edge corrente va eliminato
if(posCorr.E() < firstE)
                    assert(posCorr.E() < firstE);  //fin quando sono all'interno non posso trovare alcuno di nuovi edge
                }
                else    //in questo caso siamo usciti dal rettangolo poiche' il vertice e' di joint
                {
//Log("e %i i %i", &(*posCorr.E())-&(em.edge[0]), &(*posCorr.E()->V(0))-&(em.vert[0]),&(*posCorr.E()->V(1))-&(em.vert[0]));
                    posCorr.E()->SetV();    //l'edge e' divenuto esterno, quindi lo setto come visitato (potrei mettere un break), ma non so se sia tra i vecchi edge (modificati) o tra i nuovi
                }
            }while(posCorr.V() < firstJV);
        }
    }

    // 4- AGGIUNTA DEGLI EDGE DI CHIUSURA
//    Log("FASE 4");
    nJP = (nJP/2)+addEC;      //gli edge di chiusura sono pari al numero di vertici di Joint inseriti diviso 2 + gli edge per gli spigoli
//?? nJP = em.vert.size() - em.edge.size();
    ei = vcg::tri::Allocator<CMeshO>::AddEdges(em,nJP);
    CMeshO::EdgeIterator eEnd = ei+nJP;

    if(nEC==2)  //ci sta un solo spigolo (2 edge contigui da inizializzare)
    {
        //e0
        ei->V(0)=firstJV;   //v0
        firstJV++;
        ei->V(1)=firstJV;   //v1
        ei++;   //e1 (non mi sposto sul vertice successivo)
        ei->V(0)=firstJV;   //v1
        firstJV++;
        ei->V(1)=firstJV;   //v2

        ei++;
        firstJV++;

    }else if(nEC==3)  //ci sono i due spigoli e sono direttamente congiunti
    {
        //e0
        ei->V(0)=firstJV;   //v0
        firstJV++;
        ei->V(1)=firstJV;   //v1
        ei++;   //e1 (non mi sposto sul vertice successivo)
        ei->V(0)=firstJV;   //v1
        firstJV++;
        ei->V(1)=firstJV;   //v2
        ei++;   //e2
        ei->V(0)=firstJV;   //v2
        firstJV++;
        ei->V(1)=firstJV;   //v3

        ei++;
        firstJV++;

    }else if(nEC==4)    //ci sono i due spigoli ma non sono direttamente congiunti
    {
        //e0
        ei->V(0)=firstJV;   //v0
        firstJV++;
        ei->V(1)=firstJV;   //v1
        ei++;   //e1
        ei->V(0)=firstJV;   //v1
        firstJV++;
        ei->V(1)=firstJV;   //v2
        firstJV++;// stavolta sposto
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
//per il pezz precedente posso sostituirlo con uno srotolamento di un for
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

    //solo alla fine butto tutto ciò che non mi serve (cosi non ho problemi con gli indici)
    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);

//    assert(em.vert.size() == em.edge.size());   //è una condizione che alla fine, dopo aver rimosso gli edge ed i veritici di troppo deve sempre valere anche se si sconnettono delle sagome visto che ogni vertice connette esattamente 2 vertici (e viceversa) e sono tutte sagome chiuse

    tri::UpdateTopology<CMeshO>::EdgeEdge(em);

    return inc;
/**************
Log("Log per testare");
Log("N vert %i", em.vert.size());
Log("N edge %i", em.edge.size());

Log("stampa vertici e edge");
for(size_t i = 0; i < em.vert.size(); i++)
{
    if(em.vert[i].IsD()) Log("{{{");
    Log("Vertex %i (%f,%f,%f)", i, em.vert[i].P().X(), em.vert[i].P().Y(), em.vert[i].P().Z());
    if(em.vert[i].IsD()) Log("}}}");
}
for(size_t i = 0; i < em.edge.size(); i++)
{
    if(em.edge[i].IsD()) Log("{{{");
    CVertexO* start = &(em.vert[0]);
    Log("Edge %i (%i,%i)", i, &(*em.edge[i].V(0))-start,&(*em.edge[i].V(1))-start);
    if(em.edge[i].IsD()) Log("}}}");
}
**********/


}


Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
