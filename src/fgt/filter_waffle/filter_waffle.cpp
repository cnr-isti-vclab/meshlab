
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
            parlst.addParam(new RichFloat("eps",0.3,"Medium thickness","Thickness of the medium where the pieces will be cut away"));
            parlst.addParam(new RichFloat("spacing",0.05,"Space between two planes", "Step value between each plane for automatically generating cross-sections."));
            parlst.addParam(new RichBool("singleFile", true, "Single SVG","Automatically generate a series of cross-sections along the whole length of the object and store each plane in a separate SVG file. The distance between each plane is given by the step value below"));
            parlst.addParam(new RichBool("capBase",true,"Cap input mesh holes","Eventually cap the holes of the input mesh before applying the filter"));
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
            Point3f planeAxis(0,0,0);
            Axis axis = (Axis) parlst.getEnum("planeAxis");
            planeAxis[axis] = 1.0f;

            float length = parlst.getFloat("length");

            // set common SVG Properties
            float maxdim=m.mm()->cm.bbox.Dim()[m.mm()->cm.bbox.MaxDim()];
            // eps to use in extrusion, is relative to maxdim
            // eps : length = eps_xtr : maxdim  -> eps_xtr = (eps*maxdim)/length
            // NOTE: change eps with eps_xtr everywhere below
            float eps=parlst.getFloat("eps");
            eps=maxdim*(eps/length);

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
                }
                break;
            case Y:
                {
                    axisOrthog = (Axis) Succ<Y>::value;
                    axisJoint = (Axis) Succ<Succ<Y>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[0],sizeCm[2]);
                }
                break;
            case Z:
                {
                    axisOrthog = (Axis) Succ<Z>::value;
                    axisJoint = (Axis) Succ<Succ<Z>::value>::value;
                    pr.sizeCm = Point2f(sizeCm[0],sizeCm[1]);
                }
                break;
            }

            Log("sizecm %fx%f",pr.sizeCm[0],pr.sizeCm[1]);

            pr.lineWidthPt=200;
            pr.scale=2/maxdim;
            float planeOffset = parlst.getFloat("planeOffset");
            Point3f planeCenter;
            Plane3f slicingPlane;

            float planeDist = parlst.getFloat("spacing");
            Box3f &bbox = m.mm()->cm.bbox;
/*            if(planeDist > bbox.Diag());
            {
                Log("The space between two planes must be less than the length of the mesh");
                return false;
            }
*/
            int planeNum = (planeDist == 0) ? 1 : ( ((bbox.Dim()*planeAxis)/planeDist)+1 ); //evito la divisione per 0

            pr.numCol=(int)(max((int)sqrt(planeNum*1.0f),2)+1);
            pr.numRow=(int)(planeNum*1.0f/pr.numCol)+1;
            pr.projDir = planeAxis;
            pr.projCenter =  m.mm()->cm.bbox.Center();

            vector<CMeshO*> ev;
            MeshModel* base = m.mm();
            CMeshO &cmBase = base->cm;
//            MeshModel* orig = m.mm();
//            CMeshO &cmOrig = orig->cm;


            //actual cut of the mesh

            if (tri::Clean<CMeshO>::CountNonManifoldEdgeFF(cmBase)>0 || (tri::Clean<CMeshO>::CountNonManifoldVertexFF(cmBase,false) != 0))
            {
                Log("Mesh is not two manifold, cannot apply filter");
                return false;
            }

            float lengthDiag = bbox.Diag()/2.0;
            planeCenter = bbox.min + planeAxis*planeOffset*lengthDiag;      //parto dal basso (bbox min)

            float epsTmp = eps/2.0;



//            for(int j = 0; j < planeNum; ++j)
//                float dist = 0.0+=planeDist;

//            float dist = 0.0;
//            Lati3 lati; //il quarto lato lo ignoro, tanto non lo uso mai
            Segment2f  lati[planeNum][3];

            //generateRectSeg(j, epsTmp, lengthDiag, lati);
            generateRectSeg(epsTmp, lengthDiag, lati[0]);

            MeshModel* cap;
            MeshModel* cap2;
            for(int i = 0; i < planeNum; ++i)
            {
//                Log("######## LAYER %i", i);
                planeCenter += planeAxis*planeDist;
                slicingPlane.Init(planeCenter,planeAxis);

                //this is used to generate svg slices
                cap= m.addNewMesh("","EdgeMesh");
                vcg::IntersectionPlaneMesh<CMeshO, CMeshO, float>(base->cm, slicingPlane, cap->cm );

                tri::Clean<CMeshO>::RemoveDuplicateVertex(cap->cm);
                ev.push_back(&(cap->cm));

                if(cap->cm.edge.size()> 0)
                {
                    subtraction(cap->cm, lati[0], axis, axisOrthog, axisJoint, planeCenter[axis]);
                }

                cap2= m.addNewMesh("","CappedSlice");
                tri::CapEdgeMesh(cap->cm, cap2->cm);

            }

            QString fname;//=parlst.getSaveFileName("filename");
            if(fname=="") fname="C:/Slice.svg";
            if (!fname.endsWith(".svg")) fname+=".svg";

            tri::io::ExporterSVG<CMeshO>::Save(ev, fname.toStdString().c_str(), pr);

        }
        break;
    }
    return true;
}



//void ExtraFilter_SlicePlugin::extrude(MeshDocument* /*doc*/,MeshModel* orig, MeshModel* dest, float eps, Point3f planeAxis)
//{
//    tri::ExtrudeBoundary<CMeshO>(orig->cm,dest->cm,eps,planeAxis);
//}

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

void ExtraFilter_SlicePlugin::generateRectSeg(const float &epsTmp, const float &lengthDiag, Segment2f lati[])
{
    //variabili di supporto
    float yMag = +epsTmp;
    float yMin = -epsTmp;

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
    lati[L1].Set(a1,b1);    //lato + a destra
    lati[L2].Set(a2,b2);    //lato + a sinistra
    lati[l1].Set(a1,a2);    //lato minore che passa per il centro
    //il lato minore più lontano non sereve
}

bool compareX(JointPoint v0, JointPoint v1)
{
    return v0.p.X() < v1.p.X();
}

bool compareY(JointPoint v0, JointPoint v1)
{
    return v0.p.Y() < v1.p.Y();
}

void ExtraFilter_SlicePlugin::addAndBreak(CMeshO &em, Point3f & pJoint, const Axis &axisOrthog, const Axis &axisJoint, const JointPoint & jp, const CMeshO::VertexIterator vi, const CMeshO::EdgeIterator ei)
{
//Log("Edge spezzato %i", jp.e);
//Log("Edge nuovo %i", &(*ei) - &(em.edge[0]));
//Log("Indirizzo V1 spostato su nuovo edge %i", em.edge[jp.e].V(1) - &(em.vert[0]));
//Log("Indirizzo V Joint %i", &(*vi) - &(em.vert[0]));

    //SPEZZO IN DUE UN EDGE
    //converto il punto di intersezione in 3D (la terza coordinata pJoint[axis] è sempre la stessa per tutti i punti)
    pJoint[axisJoint] = jp.p.X();
    pJoint[axisOrthog] = jp.p.Y();

    //aggiungo un solo vertice per il punto di intersezione (i 2 edge usano i vertici dell'edge corrente e in + solo il nuovo vertice d'intersezione)
    (*vi).P() = pJoint;     //aggiungo il nuovo vertice di intersezione
    (*vi).SetV();           //indico che è un punto di Joint per la successiva fase di cancellazione
    //spezzo l'edge corrente in due edge sul punto di intersezione: aggiungo solo il secondo edge e per il primo edge uso l'edge corrente

    //il nuovo edge ha...
    ei->V(0) = &(*vi);              //... come vertice 0 il nuovo vertice di intersezione e ...
    ei->V(1) =em.edge[jp.e].V(1);   //... come vertice 1 setto il vertice dal lato giusto (indV1) dell'edge corrente

    //l'edge 1, fatto sull'edge corrente, ha il vertice 0 inalterato, mentre...
    em.edge[jp.e].V(1) = &(*vi);       //...il vertice 1 diventa il nuovo vertice di intersezione
}

void ExtraFilter_SlicePlugin::subtraction(CMeshO &em, Segment2f lati[], const Axis &axis, const Axis &axisOrthog, const Axis &axisJoint, const float height)
{
    assert(axis != axisOrthog && axisOrthog != axisJoint && axisJoint != axis);

    //dalla mesh testata venivano mesh con troppi vertici rispetto al numero di edge (che dovrebbero essere uguali)
    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);

    // 1- RICERCA DEI PUNTI DI INTERSEZIONE TRA MESH E RETTANGOLO
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

    for(size_t i = 0; i < em.edge.size(); i++)      // ciclo sulle diverse sagome
    {
        if (!em.edge[i].IsV())
        {
            edge::Pos<CEdgeO> eStart(&em.edge[i],0);    //memorizza l'edge di partenza
            edge::Pos<CEdgeO> eCorr = eStart;           //itera sugli edge

            int numE = 0;
            do
            {
                numE++;

                eCorr.E()->SetV();

                Point3f &p0 = eCorr.E()->V(0)->P();
                Point3f &p1 = eCorr.E()->V(1)->P();

                //converto i vertici in 2D: per come ho definito i retangli di ritaglio la X è lungo axisJoint e la Y è lungo axisOrthog
                p0_2D.X() = p0[axisJoint];
                p0_2D.Y() = p0[axisOrthog];
                p1_2D.X() = p1[axisJoint];
                p1_2D.Y() = p1[axisOrthog];

                //inizializzo il segmento 2D da usare per il test di intersezione
                seg.Set(p0_2D,p1_2D);

                int ei = eCorr.E() - &(em.edge[0]); //indice dell'edge corrente
bool cc = false;
                //un segmento puo' intersecare contemporaneamente 2 lati (ma non 3) quindi non posso mettere else
                if(vcg::SegmentSegmentIntersection(lati[L1],seg,pJoint2D))
                {
                    JointPoint newJoint(pJoint2D, ei, L1);
                    jointPointsL1.push_back(newJoint);
if(cc){Log("PROBLEMAAAAAAAAAA");
cc = true;
Log("ei L1: %i", ei);}
                }
                if(vcg::SegmentSegmentIntersection(lati[L2],seg,pJoint2D))
                {
                    JointPoint newJoint(pJoint2D, ei, L2);
                    jointPointsL2.push_back(newJoint);
if(cc){Log("PROBLEMAAAAAAAAAA");
cc = true;
Log("ei L2: %i", ei);}
                }
                if(vcg::SegmentSegmentIntersection(lati[l1],seg,pJoint2D))
                {
                    JointPoint newJoint(pJoint2D, ei, l1);
                    jointPointsl1.push_back(newJoint);
if(cc){Log("PROBLEMAAAAAAAAAA");
cc = true;
Log("ei l1: %i", ei);}
                }
cc = false;
                eCorr.NextE();

            }while(eCorr != eStart); //&& eCorr != NULL && eCorr != ePrec

//            Log("edge visited: %i", numE);
        }
    }

    int nJP = jointPointsL1.size() + jointPointsl1.size() + jointPointsL2.size();
    if(nJP == 0) return; // se non ho trovato punti di intersezione nn devo fare nulla: esco

    // 2- AGGIUNTA DEI PUNTI DI INTERSEZIONE SPEZZANDO GLI EDGE DELLA MESH

    //percorrendo un lato lungo del rettangolo (L1 o L2) tutte le volte che entro nella sagoma attraverso un punto di joint devo passare da un'altro punto di joint per uscire dalla sagoma.
    //Dunque se ho un numero pari di punti di joint lungo un lato, dopo averlo percorso sono fuori dalla sagoma.
    //Se invece i joint sono dispari alla fine sono ancora dentro la sagoma e devo aggiungere lo spigolo del rettangolo.
    //Nota 1: quindi ho di nuovo un numero pari di vertici su L1 e L2.
    //Nota 2: se l1 sia pari o dispari dipende direttamente da L1 e L2.
    //Nota 3: nJP è sicuramente pari perchè seguendo il percorso L1-l1-L2 parto da fuori, passo sempre da un punto di joint e alla fine sono di nuovo fuori (e ovviamente resta pari anche se aggiungo i due spigoli)

    Point3f pJoint;         //punto di intersezione 3D, usato per convertire i pJoint2D in 3D ed inserirli nell'edgeMesh
    pJoint[axis] = height;  //la terza coordinata è uguale per tutti i punti dato che lavoriamo in 2D
//????TOGLI SOTTRAZIONE SOPRA
    //ordino i vertici di intersezione lungo i lati del rettangolo
    std::sort(jointPointsL1.begin(), jointPointsL1.end(), compareY);
    std::sort(jointPointsL2.begin(), jointPointsL2.end(), compareY);
    std::sort(jointPointsl1.begin(), jointPointsl1.end(), compareX);    //lo ordino per X

//??un singolo segmento puo' aver intersecato 2 lati del rettangolo ed in tal caso viene riferito da + di 1 punto di Joint, quindi nello spezzare devo mettere nel nuovo edge quello dalla parte giusta
//??L1 sta + a destra quindi dato l'edge da spezzare il suo vertice più a destra andrà nel nuovo edge e sarà sostituito dal punto di joint
//??int indV1 = (em.edge[jointPointsL1[iL1].e].V(0)->P().Y() > em.edge[jointPointsL1[iL1].e].V(1)->P().Y()) ? 0 : 1;

//?? PROBLEMA non ho considerato nessuno spigolo è dentro la mesh che viene intersecata solo da l1


    //per gli edge di chiusura che aggiungo in seguito (dopo aver rimosso gli edge interni) quello che faccio è tenere un indice ivS che indica da dove ho iniziato a inserire i nuovi vertici e poi inserisco i vertici a coppie in ordine, cosi successivamente li ripesco a coppie e creo gli edge (tratto a parte gli edge incidenti su spigoli)

    size_t iL1, iL2, il1, endl1, ivS;   //indici per scorrere
    ivS = em.vert.size();               //indice del primo dei nuovi vertici (usato successivamente per creare gli edge di chiusura)

    endl1 = jointPointsl1.size();       //indice di coda dei punti di joint sul lato l1

//???? ciclo for principale di rimozione vertici puoi farlo anche solo fino a qui
    //iteratori a verice ed edge nuovi da creare
    CMeshO::EdgeIterator ei = vcg::tri::Allocator<CMeshO>::AddEdges(em,nJP);    //di edge aggiungo quelli per quando spezzo un edge nel punto di intersezione: sono nJP dato che nello spezzare riuso l'edge spezzato

    int dispL1 = jointPointsL1.size()%2;
    int dispL2 = jointPointsL2.size()%2;
    int addEC = dispL1 + dispL2;
    int nEC = nJP + addEC; //se serve devo contare anche gli spigoli

    CMeshO::VertexIterator vi = vcg::tri::Allocator<CMeshO>::AddVertices(em,nEC);

//    assert( (jointPointsl1.size()!=1) || (!dispL1 && dispL2 && jointPointsl1.size() == 1) || (dispL1 && !dispL2 && jointPointsl1.size() == 1) );  // se L1 e L2 sono entrambi dispari l1 non può avere un solo vertice : o ne ha o almeno 2
    if(jointPointsL1.size()!=0 && jointPointsL2.size()!=0)
        if(jointPointsL1[0].e == jointPointsL2[0].e) Log("PROBLEMAAAAAAAAAA!!!");


    //prima controllo se ci sono spigoli e in caso inserisco in ordine di percorrenza di lati del rettangolo (L1-l1-L2) le parti che vanno congiunte
    if(dispL1)  //se per L1 ho un numero dispari di punti il primo punto va trattato a parte assieme allo spigolo.
    {
        //VERTICE IN L1
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
            nEC = 4;    //(riuso nEC)
        }else nEC = 2;
    }else {iL1 = 0; nEC = 0;}

    if(dispL2)  //se per L2 ho un numero dispari di punti il primo punto va trattato a parte assieme allo spigolo.
    {
        //VERTICE IN l1
        if(endl1 > 0)   //se l1 non è vuoto devo prendere un vertice
        {
            addAndBreak(em, pJoint, axisOrthog, axisJoint, jointPointsl1[il1], vi, ei);
            il1 = 1;
            vi++;
            ei++;
        } else {il1 = 0; nEC++;}
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


    // 3- RIMOZIONE DEGLI EDGE INTERNI AL RETTANGOLO
    tri::UpdateTopology<CMeshO>::EdgeEdge(em);
    vcg::tri::UpdateFlags<CMeshO>::EdgeClearV(em);  // setto i flag per sapere se sono stati visitati dal sottociclo (che visita una sagoma alla volta)

    CVertexO *firstJV = &(em.vert[0]) + ivS; // indirizzo del primo punto di joint

    //le y dei due lati lunghi del rettangolo, per verificare se un vertice è dentro
    float & y1 = lati[0].P0().Y();
    float & y2 = lati[1].P1().Y();

    unsigned int nDel, nVDel;
    nDel = nVDel = 0;

//    for(size_t i = 0; i < old em.edge.size(); i++)//non c'è bisogno che arrivo in fondo perché gli edge nuovi fanno parte sicuramente di qualche sagoma
    for(size_t i = 0; i < em.edge.size(); i++)
    {
        if (!em.edge[i].IsV())
        {

            edge::Pos<CEdgeO> eStart(&em.edge[i],0);    //memorizza l'edge di partenza
//??WHILE INUTILE SE LIMITO IL FOR ALL'INDICE VECCHIO
if(eStart.V() >= firstJV)Log("jjjj");
            while(eStart.V() >= firstJV)                //cerco un punto non di Joint
                eStart.NextE();

            edge::Pos<CEdgeO> eCorr = eStart;           //itera sugli edge

            bool dentro = ( (eStart.V()->P().X() > 0) && (eStart.V()->P().Y() < y1) && (eStart.V()->P().Y() > y2) );//&& !(eStart.V()->IsV() ) );
//Log("dentro %i", dentro);
int numE = 0;
            do
            {
numE++;
                eCorr.E()->SetV();

//                if(eStart.V()->IsV())
                if(eCorr.V() >= firstJV)   //se attraverso un punto di Joint
                    dentro = !dentro;
                else if(dentro)
{
                    tri::Allocator<CMeshO>::DeleteVertex(em,*(eCorr.V()));
nVDel++;
//Log("DEL!!!");
}

                if(dentro)  //se sono dentro non faccio altro che ...
{
                    tri::Allocator<CMeshO>::DeleteEdge(em,*(eCorr.E())); // ...segnare che l'edge corrente va eliminato
nDel++;
}

                eCorr.NextE();
            }while(eCorr != eStart); //&& ei != NULL && ei != ePrec
//Log("Nuova sagoma trovata numE: %i", numE);
        }
    }

    // 4- AGGIUNTA DEGLI EDGE DI CHIUSURA
    nJP = (nJP/2)+addEC;      //gli edge di chiusura sono pari al numero di vertici di Joint inseriti diviso 2 + gli edge per gli spigoli
    ei = vcg::tri::Allocator<CMeshO>::AddEdges(em,nJP);
    CMeshO::EdgeIterator eEnd = ei+nJP;


    if(nEC==3)  //ci sono i due spigoli e sono direttamente congiunti
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
        firstJV++;
        ei++;
    }


/*
//iterations of StaticUnroller
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


    if(incSpig==3)
    {
        StaticUnroller<3>::step();
    }else if(incSpig==4)
    {
        StaticUnroller<2>::step();
        firstJV++;  // stavolta sposto il vertice
        StaticUnroller<2>::step();
    }

*/

    for(; ei < eEnd; firstJV++, ei++)
    {
        Log("QUI NON CI DEVI ENTRARE PER ORA");
        ei->V(0)=firstJV;
        firstJV++;
        ei->V(1)=firstJV;
    }


    tri::Allocator<CMeshO>::CompactEdgeVector(em);
    tri::Allocator<CMeshO>::CompactVertexVector(em);

    tri::UpdateTopology<CMeshO>::EdgeEdge(em);


/*
Log per testare

Log("N vert Orig %i", em.vert.size());
Log("N edge Orig %i", em.edge.size());


Log("stampa vertici e edge");
for(size_t i = 0; i < em.vert.size(); i++)
{
    Log("Vertex %i (%f,%f,%f)", i, em.vert[i].P().X(), em.vert[i].P().Y(), em.vert[i].P().Z());
}
for(size_t i = 0; i < em.edge.size(); i++)
{
    CVertexO* start = &(em.vert[0]);
    Log("Edge %i (%i,%i)", i, &(*em.edge[i].V(0))-start,&(*em.edge[i].V(1))-start);
}
*/


//?????????metti controllo errore se torna indietro: la sagoma e' aperta

}


Q_EXPORT_PLUGIN(ExtraFilter_SlicePlugin)
