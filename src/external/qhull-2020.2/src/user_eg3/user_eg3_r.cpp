#//! user_eg3_r.cpp -- Invoke rbox and qhull from C++

#include "libqhullcpp/RboxPoints.h"
#include "libqhullcpp/QhullError.h"
#include "libqhullcpp/QhullQh.h"
#include "libqhullcpp/QhullFacet.h"
#include "libqhullcpp/QhullFacetList.h"
#include "libqhullcpp/QhullFacetSet.h"
#include "libqhullcpp/QhullLinkedList.h"
#include "libqhullcpp/QhullPoint.h"
#include "libqhullcpp/QhullUser.h"
#include "libqhullcpp/QhullVertex.h"
#include "libqhullcpp/QhullVertexSet.h"
#include "libqhullcpp/Qhull.h"

#include <cstdio>   /* for printf() of help message */
#include <iomanip> // setw
#include <ostream>
#include <stdexcept>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

using orgQhull::Qhull;
using orgQhull::QhullError;
using orgQhull::QhullFacet;
using orgQhull::QhullFacetList;
using orgQhull::QhullFacetListIterator;
using orgQhull::QhullFacetSet;
using orgQhull::QhullFacetSetIterator;
using orgQhull::QhullPoint;
using orgQhull::QhullPoints;
using orgQhull::QhullPointsIterator;
using orgQhull::QhullQh;
using orgQhull::QhullUser;
using orgQhull::QhullVertex;
using orgQhull::QhullVertexList;
using orgQhull::QhullVertexListIterator;
using orgQhull::QhullVertexSet;
using orgQhull::QhullVertexSetIterator;
using orgQhull::RboxPoints;

int main(int argc, char **argv);
int user_eg3(int argc, char **argv);

char prompt[]= "\n========\n\
user_eg3 commands... -- demonstrate calling rbox and qhull from C++.\n\
\n\
user_eg3 is statically linked to qhullcpp and reentrant qhull.  If user_eg3\n\
fails immediately, it is probably linked to the non-reentrant qhull library.\n\
\n\
Commands:\n\
  eg-100               Run the example in qh-code.htm\n\
  eg-convex            'rbox d | qconvex o' with std::vector and C++ classes\n\
  eg-delaunay          'rbox y c | qdelaunay o' with std::vector and C++ classes\n\
  eg-voronoi           'rbox y c | qvoronoi o' with std::vector and C++ classes\n\
  eg-fifo              'rbox y c | qvoronoi FN Fi Fo' with QhullUser and qh_fprintf\n\
\n\
Rbox and Qhull commands:\n\
  rbox \"200 D4\" ...    Generate points from rbox\n\
  qhull \"d p\" ...      Run qhull with options and produce output\n\
  qhull-cout \"o\" ...   Run qhull with options and produce output to cout\n\
  qhull \"T1\" ...       Run qhull with level-1 trace to cerr\n\
  qhull-cout \"T1z\" ... Run qhull with level-1 trace to cout\n\
  facets               Print qhull's facets when done\n\
\n\
For example\n\
  user_eg3 rbox qhull\n\
  user_eg3 rbox qhull T1\n\
  user_eg3 rbox qhull d\n\
  user_eg3 rbox D2 10 2 \"s r 5\" qhull \"s p\" facets\n\
  user_eg3 eg-convex\n\
  user_eg3 rbox 10 eg-delaunay qhull \"d o\"\n\
  user_eg3 rbox D5 c P2 qhull d eg-delaunay\n\
  user_eg3 rbox \"D5 c P2\" qhull v eg-voronoi o\n\
  user_eg3 rbox D2 10 qhull \"v\" eg-fifo p Fi Fo\n\
";
// single quotes OK in Unix but not OK in Windows cmd.exe

/*--------------------------------------------
-user_eg3-  main procedure of user_eg3 application
*/
int main(int argc, char **argv){

    QHULL_LIB_CHECK

    if(argc==1){
        cout << prompt;
        return 0;
    }
    try{
        return user_eg3(argc, argv);
    }catch(QhullError &e){
        cerr << e.what() << std::endl;
        return e.errorCode();
    }
}//main

void printDoubles(const std::vector<double> &doubles)
{
    for(size_t i= 0; i < doubles.size(); i++){
        cout << std::setw(6) << doubles[i] << " ";
    }
}//printDoubles

void printInts(const std::vector<int> &ints)
{
    for(size_t i= 0; i < ints.size(); i++){
        cout << ints[i] << " ";
    }
}//printInts

void qconvex_o(const Qhull &qhull)
{
    int dim= qhull.hullDimension();
    int numfacets= qhull.facetList().count();
    int totneighbors= numfacets * dim;  /* incorrect for non-simplicial facets, see qh_countfacets */
    cout << dim << "\n" << qhull.points().size() << " " << numfacets << " " << totneighbors/2 << "\n";
    std::vector<std::vector<double> > points;
    // for(QhullPoint point : qhull.points())
    for(QhullPoints::ConstIterator i= qhull.points().begin(); i != qhull.points().end(); ++i){
        QhullPoint point= *i;
        points.push_back(point.toStdVector());
    }
    // for(std::vector<double> point : points){
    for(size_t j= 0; j < points.size(); ++j){
        std::vector<double> point= points[j];
        size_t n= point.size();
        for(size_t i= 0; i < n; ++i){
            if(i < n-1){
                cout << std::setw(6) << point[i] << " ";
            }else{
                cout << std::setw(6) << point[i] << "\n";
            }
        }
    }
    QhullFacetList facets= qhull.facetList();
    std::vector<std::vector<int> > facetVertices;
    // for(QhullFacet f : facets)
    QhullFacetListIterator j(facets);
    while(j.hasNext()){
        QhullFacet f= j.next();
        std::vector<int> vertices;
        if(!f.isGood()){
            // ignore facet
        }else if(!f.isTopOrient() && f.isSimplicial()){ /* orient the vertices like option 'o' */
            QhullVertexSet vs= f.vertices();
            vertices.push_back(vs[1].point().id());
            vertices.push_back(vs[0].point().id());
            for(int i= 2; i<(int)vs.size(); ++i){
                vertices.push_back(vs[i].point().id());
            }
            facetVertices.push_back(vertices);
        }else{  /* note: for non-simplicial facets, this code does not duplicate option 'o', see qh_facet3vertex and qh_printfacetNvertex_nonsimplicial */
            // for(QhullVertex vertex : f.vertices()){
            QhullVertexSetIterator k(f.vertices());
            while(k.hasNext()){
                QhullVertex vertex= k.next();
                QhullPoint p= vertex.point();
                vertices.push_back(p.id());
            }
            facetVertices.push_back(vertices);
        }
    }
    // for(std::vector<int> vertices : facetVertices)
    for(size_t k= 0; k<facetVertices.size(); ++k){
        std::vector<int> vertices= facetVertices[k];
        size_t n= vertices.size();
        cout << n << " ";
        for(size_t i= 0; i<n; ++i){
            cout << vertices[i] << " ";
        }
        cout << "\n";
    }
}//qconvex_o

void qdelaunay_o(const Qhull &qhull)
{
    // The Delaunay diagram is equivalent to the convex hull of a paraboloid, one dimension higher
    int hullDimension= qhull.hullDimension();

    // Input sites as a vector of vectors
    std::vector<std::vector<double> > inputSites;
    QhullPoints points= qhull.points();
    // for(QhullPoint point : points)
    QhullPointsIterator j(points);
    while(j.hasNext()){
        QhullPoint point= j.next();
        inputSites.push_back(point.toStdVector());
    }

    // Printer header and Voronoi vertices
    QhullFacetList facets= qhull.facetList();
    int numFacets= facets.count();
    size_t numRidges= numFacets*hullDimension/2;  // only for simplicial facets
    cout << hullDimension << "\n" << inputSites.size() << " " << numFacets << " " << numRidges << "\n";
    // for(std::vector<double> site : inputSites)
    for(size_t k= 0; k < inputSites.size(); ++k){
        std::vector<double> site= inputSites[k];
        size_t n= site.size();
        for(size_t i= 0; i<n; ++i){
            cout << site[i] << " ";
        }
        cout << "\n";
    }

    // Delaunay regions as a vector of vectors
    std::vector<std::vector<int> > regions;
    // for(QhullFacet f : facets)
    QhullFacetListIterator k(facets);
    while(k.hasNext()){
        QhullFacet f= k.next();
        std::vector<int> vertices;
        if(!f.isUpperDelaunay()){
            if(!f.isTopOrient() && f.isSimplicial()){ /* orient the vertices like option 'o' */
                QhullVertexSet vs= f.vertices();
                vertices.push_back(vs[1].point().id());
                vertices.push_back(vs[0].point().id());
                for(int i= 2; i<(int)vs.size(); ++i){
                    vertices.push_back(vs[i].point().id());
                }
            }else{  /* note: for non-simplicial facets, this code does not duplicate option 'o', see qh_facet3vertex and qh_printfacetNvertex_nonsimplicial */
                // for(QhullVertex vertex : f.vertices()){
                QhullVertexSetIterator i(f.vertices());
                while(i.hasNext()){
                    QhullVertex vertex= i.next();
                    QhullPoint p= vertex.point();
                    vertices.push_back(p.id());
                }
            }
            regions.push_back(vertices);
        }
    }
    // for(std::vector<int> vertices : regions)
    for(size_t k2= 0; k2 < regions.size(); ++k2){
        std::vector<int> vertices= regions[k2];
        size_t n= vertices.size();
        cout << n << " ";
        for(size_t i= 0; i<n; ++i){
            cout << vertices[i] << " ";
        }
        cout << "\n";
    }
}//qdelaunay_o

/***

Sample output for Fi
    rbox y c D2 | qhull v Fi Ta
    [QH9231]10
    [QH9271]5 0 5 [QH9272]0.9933067158065386 [QH9272]-0.115506572685835 [QH9273]-0.3427516509210136 [QH9274]
    ...
    rbox y c D2 | qhull v Fo Ta
    [QH9231]4
    [QH9271]5 3 4 [QH9272]-3.10259629632159e-16 [QH9272]     1 [QH9273]-1.551298148160795e-16 [QH9274]
    ...
*/
void qvoronoi_fifo(Qhull *qhull, const char *printOption)
{
    QhullUser results(qhull->qh());
    qhull->outputQhull(printOption); // qh_fprintf writes its results into 'results'
    int n= results.numResults();
    if(results.firstCode()!=9231){
        cout << "user_eg3 error (qvoronoi_fifo): 'qhull " << printOption << "' did not produce output for 'Fi' or 'Fo'\nqh_fprintf codes: ";
        printInts(results.codes());
        cout << "\n";
        return;
    }else if(n != results.numDoubles() || n != results.numInts()){
        cout << "user_eg3 error (qvoronoi_fifo): Expecting doubles and ints for " << n << " results.  Got " << results.numDoubles() << " doubles and " << results.numInts() << " ints.  Did an error occur?\nqh_fprintf codes: ";
        printInts(results.codes());
        cout << "\n";
        return;
    }
    cout << n << "\n";
    for(int i= 0; i<n; ++i){
        printInts(results.intsVector().at(i));
        cout << " ";
        printDoubles(results.doublesVector().at(i));
        cout << "\n";
    }
    if(results.codes().size()<30){
        cout << "\nMessage codes captured by qh_fprintf in QhullUser.cpp (qhull v Fo Ta):\n";
        printInts(results.codes());
        cout << "\n";
    }
}//qvoronoi_fifo

void qvoronoi_o(const Qhull &qhull)
{
    int voronoiDimension= qhull.hullDimension() - 1;
    int numfacets= qhull.facetCount();
    size_t numpoints= qhull.points().size();

    // Gather Voronoi vertices
    std::vector<std::vector<double> > voronoiVertices;
    std::vector<double> vertexAtInfinity;
    for(int i= 0; i<voronoiDimension; ++i){
        vertexAtInfinity.push_back(qh_INFINITE);
    }
    voronoiVertices.push_back(vertexAtInfinity);
    // for(QhullFacet facet : qhull.facetList())
    QhullFacetListIterator j(qhull.facetList());
    while(j.hasNext()){
        QhullFacet facet= j.next();
        if(facet.visitId() && facet.visitId()<numfacets){
            voronoiVertices.push_back(facet.getCenter().toStdVector());
        }
    }

    // Printer header and Voronoi vertices
    cout << voronoiDimension << "\n" << voronoiVertices.size() << " " << numpoints << " 1\n";
    // for(std::vector<double> voronoiVertex : voronoiVertices)
    for(size_t k= 0; k < voronoiVertices.size(); ++k){
        std::vector<double> voronoiVertex= voronoiVertices[k];
        size_t n= voronoiVertex.size();
        for(size_t i= 0; i<n; ++i){
            cout << voronoiVertex[i] << " ";
        }
        cout << "\n";
    }

    // Gather Voronoi regions
    std::vector<std::vector<int> > voronoiRegions(numpoints); // qh_printvoronoi calls qh_pointvertex via qh_markvoronoi
    // for(QhullVertex vertex : qhull.vertexList())
    QhullVertexListIterator j2(qhull.vertexList());
    while(j2.hasNext()){
        QhullVertex vertex= j2.next();
        size_t numinf= 0;
        std::vector<int> voronoiRegion;
        //for(QhullFacet neighbor : vertex.neighborFacets())
        QhullFacetSetIterator k2(vertex.neighborFacets());
        while(k2.hasNext()){
            QhullFacet neighbor= k2.next();
            if(neighbor.visitId()==0){
                if(!numinf){
                    numinf= 1;
                    voronoiRegion.push_back(0); // the voronoiVertex at infinity indicates an unbounded region
                }
            }else if(neighbor.visitId()<numfacets){
                voronoiRegion.push_back(neighbor.visitId());
            }
        }
        if(voronoiRegion.size() > numinf){
            int siteId= vertex.point().id();
            if(siteId>=0 && siteId<int(numpoints)){ // otherwise indicate qh.other_points
                voronoiRegions[siteId]= voronoiRegion;
            }
        }
    }

    // Print Voronoi regions by siteId
    // for(std::vector<int> voronoiRegion : voronoiRegions)
    for(size_t k3= 0; k3 < voronoiRegions.size(); ++k3){
        std::vector<int> voronoiRegion= voronoiRegions[k3];
        size_t n= voronoiRegion.size();
        cout << n;
        for(size_t i= 0; i<n; ++i){
            cout << " " << voronoiRegion[i];
        }
        cout << "\n";
    }
}//qvoronoi_o

// Nearly the same as qvoronoi_p -- the Voronoi vertex at infinity is not included, hence indices are one less
void qvoronoi_pfn(const Qhull &qhull)
{
    int voronoiDimension= qhull.hullDimension() - 1;
    int numfacets= qhull.facetCount();
    size_t numpoints= qhull.points().size();

    // Gather Voronoi vertices
    std::vector<std::vector<double> > voronoiVertices;
    // for(QhullFacet facet : qhull.facetList())
    QhullFacetListIterator j(qhull.facetList());
    while(j.hasNext()){
        QhullFacet facet= j.next();
        if(facet.visitId() && facet.visitId()<numfacets){
            voronoiVertices.push_back(facet.getCenter().toStdVector());
        }
    }

    // Printer header and Voronoi vertices
    cout << voronoiDimension << "\n";
    cout << voronoiVertices.size() << "\n";
    // for(std::vector<double> voronoiVertex : voronoiVertices)
    for(size_t k= 0; k < voronoiVertices.size(); ++k){
        std::vector<double> voronoiVertex= voronoiVertices[k];
        size_t n= voronoiVertex.size();
        for(size_t i= 0; i<n; ++i){
            cout << voronoiVertex[i] << " ";
        }
        cout << "\n";
    }

    // Gather Voronoi regions
    std::vector<std::vector<int> > voronoiRegions(numpoints); // qh_printvoronoi calls qh_pointvertex via qh_markvoronoi
    //for(QhullVertex vertex : qhull.vertexList()){
    QhullVertexListIterator j2(qhull.vertexList());
    while(j2.hasNext()){
        QhullVertex vertex= j2.next();
        size_t numinf= 0;
        std::vector<int> voronoiRegion;
        // for(QhullFacet neighbor : vertex.neighborFacets())
        QhullFacetSetIterator k(vertex.neighborFacets());
        while(k.hasNext()){
            QhullFacet neighbor= k.next();
            if(neighbor.visitId()==0){
                if(!numinf){
                    numinf= 1;
                    voronoiRegion.push_back(-1); // -1 indicates the Voronoi vertex at infinity
                }
            }else if(neighbor.visitId()<numfacets){
                voronoiRegion.push_back(neighbor.visitId()-1);
            }
        }
        if(voronoiRegion.size() > numinf){
            int siteId= vertex.point().id();
            if(siteId>=0 && siteId<int(numpoints)){ // otherwise would indicate qh.other_points
                voronoiRegions[siteId]= voronoiRegion;
            }
        }
    }

    // Print Voronoi regions by siteId
    cout << numpoints << "\n";
    // for(std::vector<int> voronoiRegion : voronoiRegions)
    for(size_t j3= 0; j3 < voronoiRegions.size(); ++j3){
        std::vector<int> voronoiRegion= voronoiRegions[j3];
        size_t n= voronoiRegion.size();
        cout << n;
        for(size_t i= 0; i<n; ++i){
            cout << " " << voronoiRegion[i];
        }
        cout << "\n";
    }
}//qvoronoi_pfn

int user_eg3(int argc, char **argv)
{
    bool printFacets= false;
    RboxPoints rbox;
    Qhull qhull;
    int readingRbox= 0;
    int readingQhull= 0;
    bool noRboxOutput= false;
    for(int i=1; i<argc; i++){
        if(strcmp(argv[i], "eg-100")==0){
            RboxPoints eg("100");
            Qhull q(eg, "");
            QhullFacetList facets= q.facetList();
            cout << facets;
        }else if(strcmp(argv[i], "eg-convex")==0 && readingQhull>1){
            cout << "\nInput points and facetlist for '" << qhull.qhullCommand() << "' via C++ classes\n";
            qconvex_o(qhull);
        }else if(strcmp(argv[i], "eg-convex")==0 && !rbox.isEmpty()){
            Qhull q(rbox, "");
            cout << "\nInput points and facetlist for convex hull of " << q.rboxCommand() << " via C++ classes\n";
            qconvex_o(q);
            noRboxOutput= true;
        }else if(strcmp(argv[i], "eg-convex")==0){
            cout << "\nA 3-d diamond (rbox d)\n";
            RboxPoints diamond("d");
            cout << diamond;
            cout << "\nInput points and facetlist of its convex hull (qhull o)\n";
            Qhull q(diamond, "o");
            q.setOutputStream(&cout);
            q.outputQhull();
            // q.outputQhull("o") produces the same output
            cout << "\nInput points and facetlist using std::vector and C++ classes\n";
            qconvex_o(q);
            cout << "\nIts outward pointing normals as vector plus offset (qhull n)\n";
            q.outputQhull("n");
        }else if(strcmp(argv[i], "eg-delaunay")==0 && readingQhull>1 && qhull.isDelaunay()){
            cout << "\nVertices and Delaunay regions of paraboloid from '" << qhull.qhullCommand() << "' via C++ classes\n";
            qdelaunay_o(qhull);
        }else if(strcmp(argv[i], "eg-delaunay")==0 && !rbox.isEmpty()){
            cout << "\nDelaunay triangulation of " << rbox.count() << " points as " << rbox.dimension()+1 << "-d paraboloid via C++ classes\n";
            Qhull q(rbox, "d");
            qdelaunay_o(q);
            noRboxOutput= true;
        }else if(strcmp(argv[i], "eg-delaunay")==0){
            cout << "\nA 2-d triangle in a square (rbox y c D2)\n";
            RboxPoints triangleSquare("y c D2");
            cout << triangleSquare;
            cout << "\nThe 2-d input sites are lifted to a 3-d paraboloid.\n";
            cout << "A Delaunay region is a facet of the paraboloid's convex hull.\n";
            cout << "\nThe Delaunay triangulation as input sites and Delaunay regions (qhull d o)\n";
            Qhull q(triangleSquare, "d o");
            q.setOutputStream(&cout);
            q.outputQhull();
            // q.outputQhull("o") produces the same output
            cout << "\nThe same results using std::vector and C++ classes\n";
            qdelaunay_o(q);
        }else if(strcmp(argv[i], "eg-voronoi")==0 && readingQhull>1 && qhull.isDelaunay()){
            cout << "\nVoronoi vertices and regions for '" << qhull.qhullCommand() << "' via C++ classes\n";
            bool isLower;            //not used
            int voronoiVertexCount;  //not used
            qhull.prepareVoronoi(&isLower, &voronoiVertexCount); // not needed if previous output from Qhull
            qvoronoi_o(qhull);
        }else if(strcmp(argv[i], "eg-voronoi")==0 && !rbox.isEmpty()){
            Qhull q(rbox, "v");
            cout << "\nVoronoi vertices and regions for " << q.rboxCommand() << " via C++ classes\n";
            bool isLower;
            int voronoiVertexCount;
            q.prepareVoronoi(&isLower, &voronoiVertexCount);
            qvoronoi_o(q);
            noRboxOutput= true;
        }else if(strcmp(argv[i], "eg-voronoi")==0){
            cout << "\nA 2-d triangle in a square (rbox y c D2)\n";
            RboxPoints triangleSquare("y c D2");
            cout << triangleSquare;
            cout << "\nIts Voronoi diagram as vertices and regions (qhull v o)\n";
            cout << "The Voronoi diagram is the dual of the Delaunay triangulation\n";
            cout << "Voronoi vertices are Delaunay regions, and Voronoi regions are Delaunay input sites\n";
            cout << "The Voronoi vertex at infinity is represented as '-10.101 -10.101'\n";
            Qhull q(triangleSquare, "v o");
            q.setOutputStream(&cout);
            q.outputQhull();
            // q.outputQhull("o") produces the same output
            cout << "\nThe same results using std::vector and C++ classes\n";
            cout << "Qhull::prepareVoronoi assigns facetT.visit_id and vertexT.neighbors\n";
            cout << "The Voronoi regions are rotated by one Voronoi vertex (prepareVoronoi occurs twice)\n";
            bool isLower;
            int voronoiVertexCount;
            q.prepareVoronoi(&isLower, &voronoiVertexCount); // Also called by q.outputQhull("o"), hence the rotated vertices
            qvoronoi_o(q);
        }else if(strcmp(argv[i], "eg-fifo")==0 && readingQhull>1 && qhull.isDelaunay()){
            cout << "\nVoronoi vertices for '" << qhull.qhullCommand() << "' via C++ classes\n";
            bool isLower;
            int voronoiVertexCount;
            qhull.prepareVoronoi(&isLower, &voronoiVertexCount);
            qvoronoi_pfn(qhull);
            cout << "\nHyperplanes for bounded facets between Voronoi regions via QhullUser and qh_fprintf\n";
            qvoronoi_fifo(&qhull, "Fi");
            cout << "\nHyperplanes for unbounded facets between Voronoi regions via QhullUser and qh_fprintf\n";
            qvoronoi_fifo(&qhull, "Fo");
        }else if(strcmp(argv[i], "eg-fifo")==0 && !rbox.isEmpty()){
            Qhull q(rbox, "v");
            cout << "\nVoronoi vertices for " << q.rboxCommand() << " via QhullUser and qh_fprintf\n";
            bool isLower;
            int voronoiVertexCount;
            q.prepareVoronoi(&isLower, &voronoiVertexCount);
            qvoronoi_pfn(q);
            cout << "\nHyperplanes for bounded facets between Voronoi regions via QhullUser and qh_fprintf\n";
            qvoronoi_fifo(&q, "Fi");
            cout << "\nHyperplanes for unbounded facets between Voronoi regions via QhullUser and qh_fprintf\n";
            qvoronoi_fifo(&q, "Fo");
            noRboxOutput= true;
        }else if(strcmp(argv[i], "eg-fifo")==0){
            cout << "\nA 2-d triangle in a square (rbox y c D2)\n";
            RboxPoints triangleSquare("y c D2");
            cout << triangleSquare;
            cout << "\nIts Voronoi vertices (qhull v p)\n";
            cout << "This is the first part of eg-voronoi, but without infinity\n";
            Qhull q(triangleSquare, "v p");
            q.setOutputStream(&cout);
            q.outputQhull();
            cout << "\nIts Voronoi regions (qhull v FN)\n";
            cout << "This is the second part of eg-voronoi, with ids one less\n";
            cout << "Regions are ordered by the corresponding input site.\n";
            q.outputQhull("FN");
            // q.outputQhull("p FN") produces the same outputs
            cout << "\nThe same results as 'qhull v p FN' using std::vector and C++ classes\n";
            cout << "Qhull::prepareVoronoi assigns facet.visit_id and vertex.neighbors\n";
            cout << "prepareVoronoi is also called by q.outputQhull(\"FN\"), hence the rotated vertices\n";
            bool isLower;
            int voronoiVertexCount;
            q.prepareVoronoi(&isLower, &voronoiVertexCount); // Also called by q.outputQhull("o"), hence the rotated vertices
            qvoronoi_pfn(q);
            cout << "\nHyperplanes for bounded facets between Voronoi regions (qhull v Fi)\n";
            cout << "Each hyperplane is the perpendicular bisector of 2 input sites.\n";
            q.outputQhull("Fi");
            cout << "\nHyperplanes for unbounded rays of unbounded facets (qhull v Fo)\n";
            cout << "Each ray goes through the midpoint of 2 input sites, oriented outwards\n";
            q.outputQhull("Fo");
            cout << "\nThe same result as 'qhull v Fi' using QhullUser and its custom qh_fprintf\n";
            cout << "qh_fprintf captures the output from qh_eachvoronoi in io_r.c (qhull v Fi Fo Ta)\n";
            qvoronoi_fifo(&q, "Fi");
            cout << "\nThe same result as 'qhull v Fo' using QhullUser and its custom qh_fprintf\n";
            qvoronoi_fifo(&q, "Fo");
        }else if(strcmp(argv[i], "rbox")==0){
            if(readingRbox!=0 || readingQhull!=0){
                cerr << "user_eg3 -- \"rbox\" must be first" << endl;
                return 1;
            }
            readingRbox++;
        }else if(strcmp(argv[i], "qhull")==0
        || strcmp(argv[i], "qhull-cout")==0){
            if(readingQhull){
                cerr << "user_eg3 -- only one \"qhull\" or \"qhull-cout\" allowed." << endl;
                return 1;
            }
            if(strcmp(argv[i], "qhull-cout")==0){
                qhull.setOutputStream(&cout);
            }
            if(rbox.isEmpty()){
                if(readingRbox){
                    if(rbox.dimension()==0){
                        rbox.setDimension(2);
                    }
                    rbox.appendPoints("10");
                }else{
                    cerr << "Enter dimension followed by count followed by coordinates.  End with ^Z (Windows) or ^D (Unix).\n";
                    rbox.appendPoints(cin);
                }
            }
            readingQhull++;
            readingRbox= 0;
        }else if(strcmp(argv[i], "facets")==0){
            printFacets= true;
        }else if(readingRbox){
            readingRbox++;
            cerr << "rbox " << argv[i] << endl;
            rbox.appendPoints(argv[i]);
            if(rbox.hasRboxMessage()){
                cerr << "user_eg3 " << argv[i] << " -- " << rbox.rboxMessage();
                return rbox.rboxStatus();
            }
        }else if(readingQhull){
            if(readingQhull==1){
                qhull.runQhull(rbox, argv[i]);
                qhull.outputQhull();
            }else{
                qhull.outputQhull(argv[i]);
            }
            readingQhull++;
            if(qhull.hasQhullMessage()){
                cerr << "\nResults of " << argv[i] << "\n" << qhull.qhullMessage();
                qhull.clearQhullMessage();
            }
        }else{
            cerr << "user_eg3 error: Expecting eg-100, eg-convex, eg-delaunay, eg-voronoi, eg-fifo, qhull, qhull-cout, or rbox.  Got " << argv[i] << endl;
            return 1;
        }
    }//foreach argv
    if(readingRbox && !noRboxOutput){
        cout << rbox;
        return 0;
    }
    if(readingQhull==1){ // e.g., rbox 10 qhull
        qhull.runQhull(rbox, "");
        qhull.outputQhull();
        if(qhull.hasQhullMessage()){
            cerr << "\nResults of qhull\n" << qhull.qhullMessage();
            qhull.clearQhullMessage();
        }
    }
    if(qhull.hasOutputStream()){
        return 0;
    }
    if(printFacets){
        QhullFacetList facets= qhull.facetList();
        cout << "\nFacets created by Qhull::runQhull()\n" << facets;
    }
    return 0;
}//user_eg3

