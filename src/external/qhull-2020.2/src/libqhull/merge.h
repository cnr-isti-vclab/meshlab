/*<html><pre>  -<a                             href="qh-merge.htm"
  >-------------------------------</a><a name="TOP">-</a>

   merge.h
   header file for merge.c

   see qh-merge.htm and merge.c

   Copyright (c) 1993-2020 C.B. Barber.
   $Id: //main/2019/qhull/src/libqhull/merge.h#3 $$Change: 2953 $
   $DateTime: 2020/05/21 22:05:32 $$Author: bbarber $
*/

#ifndef qhDEFmerge
#define qhDEFmerge 1

#include "libqhull.h"


/*============ -constants- ==============*/

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="qh_ANGLEnone">-</a>

  qh_ANGLEnone
    indicates missing angle for mergeT->angle
*/
#define qh_ANGLEnone 2.0

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="MRG">-</a>

  MRG... (mergeType)
    indicates the type of a merge (mergeT->type)
    MRGcoplanar...MRGtwisted set by qh_test_centrum_merge, qh_test_nonsimplicial_merge
*/
typedef enum {  /* must match mergetypes[] */
  MRGnone= 0,
                  /* MRGcoplanar..MRGtwisted go into qh.facet_mergeset for qh_all_merges 
                     qh_compare_facetmerge selects lower mergetypes for merging first */
  MRGcoplanar,          /* (1) centrum coplanar if centrum ('Cn') or vertex not clearly above or below neighbor */
  MRGanglecoplanar,     /* (2) angle coplanar if angle ('An') is coplanar */
  MRGconcave,           /* (3) concave ridge */
  MRGconcavecoplanar,   /* (4) concave and coplanar ridge, one side concave, other side coplanar */
  MRGtwisted,           /* (5) twisted ridge, both concave and convex, facet1 is wider */
                  /* MRGflip go into qh.facet_mergeset for qh_flipped_merges */
  MRGflip,              /* (6) flipped facet if qh.interior_point is above facet, w/ facet1 == facet2 */
                  /* MRGdupridge go into qh.facet_mergeset for qh_forcedmerges */
  MRGdupridge,          /* (7) dupridge if more than two neighbors.  Set by qh_mark_dupridges for qh_MERGEridge */
                  /* MRGsubridge and MRGvertices go into vertex_mergeset */
  MRGsubridge,          /* (8) merge pinched vertex to remove the subridge of a MRGdupridge */
  MRGvertices,          /* (9) merge pinched vertex to remove a facet's ridges with the same vertices */
                  /* MRGdegen, MRGredundant, and MRGmirror go into qh.degen_mergeset */
  MRGdegen,             /* (10) degenerate facet (!enough neighbors) facet1 == facet2 */
  MRGredundant,         /* (11) redundant facet (vertex subset) */
                        /* merge_degenredundant assumes degen < redundant */
  MRGmirror,            /* (12) mirror facets: same vertices due to null facets in qh_triangulate 
                           f.redundant for both facets*/
                  /* MRGcoplanarhorizon for qh_mergecycle_all only */
  MRGcoplanarhorizon,   /* (13) new facet coplanar with the horizon (qh_mergecycle_all) */
  ENDmrg
} mergeType;

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="qh_MERGEapex">-</a>

  qh_MERGEapex
    flag for qh_mergefacet() to indicate an apex merge
*/
#define qh_MERGEapex     True

/*============ -structures- ====================*/

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="mergeT">-</a>

  mergeT
    structure used to merge facets
*/

typedef struct mergeT mergeT;
struct mergeT {         /* initialize in qh_appendmergeset */
  realT   angle;        /* cosine of angle between normals of facet1 and facet2, 
                           null value and right angle is 0.0, coplanar is 1.0, narrow is -1.0 */
  realT   distance;     /* absolute value of distance between vertices, centrum and facet, or vertex and facet */
  facetT *facet1;       /* will merge facet1 into facet2 */
  facetT *facet2;
  vertexT *vertex1;     /* will merge vertext1 into vertex2 for MRGsubridge or MRGvertices */
  vertexT *vertex2;
  ridgeT  *ridge1;      /* the duplicate ridges resolved by MRGvertices */
  ridgeT  *ridge2;      /* merge is deleted if either ridge is deleted (qh_delridge) */
  mergeType mergetype;
};


/*=========== -macros- =========================*/

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="FOREACHmerge_">-</a>

  FOREACHmerge_( merges ) {...}
    assign 'merge' to each merge in merges

  notes:
    uses 'mergeT *merge, **mergep;'
    if qh_mergefacet(),
      restart or use qh_setdellast() since qh.facet_mergeset may change
    see <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHmerge_(merges) FOREACHsetelement_(mergeT, merges, merge)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHmergeA_">-</a>

  FOREACHmergeA_( vertices ) { ... }
    assign 'mergeA' to each merge in merges

  notes:
    uses 'mergeT *mergeA, *mergeAp;'
    see <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHmergeA_(merges) FOREACHsetelement_(mergeT, merges, mergeA)

/*-<a                             href="qh-poly.htm#TOC"
  >--------------------------------</a><a name="FOREACHmerge_i_">-</a>

  FOREACHmerge_i_( vertices ) { ... }
    assign 'merge' and 'merge_i' for each merge in mergeset

  declare:
    mergeT *merge;
    int     merge_n, merge_i;

  see:
    <a href="qset.h#FOREACHsetelement_i_">FOREACHsetelement_i_</a>
*/
#define FOREACHmerge_i_(mergeset) FOREACHsetelement_i_(mergeT, mergeset, merge)

/*============ prototypes in alphabetical order after pre/postmerge =======*/

void    qh_premerge(int apexpointid, realT maxcentrum, realT maxangle);
void    qh_postmerge(const char *reason, realT maxcentrum, realT maxangle,
             boolT vneighbors);
void    qh_all_merges(boolT othermerge, boolT vneighbors);
void    qh_all_vertexmerges(int apexpointid, facetT *facet, facetT **retryfacet);
void    qh_appendmergeset(facetT *facet, facetT *neighbor, mergeType mergetype, coordT dist, realT angle);
void    qh_appendvertexmerge(vertexT *vertex, vertexT *destination, mergeType mergetype, realT distance, ridgeT *ridge1, ridgeT *ridge2);
setT   *qh_basevertices(facetT *samecycle);
void    qh_check_dupridge(facetT *facet1, realT dist1, facetT *facet2, realT dist2);
void    qh_checkconnect(void /* qh.new_facets */);
void    qh_checkdelfacet(facetT *facet, setT *mergeset);
void    qh_checkdelridge(void /* qh.visible_facets, vertex_mergeset */);
boolT   qh_checkzero(boolT testall);
int     qh_compare_anglemerge(const void *p1, const void *p2);
int     qh_compare_facetmerge(const void *p1, const void *p2);
int     qh_comparevisit(const void *p1, const void *p2);
void    qh_copynonconvex(ridgeT *atridge);
void    qh_degen_redundant_facet(facetT *facet);
void    qh_drop_mergevertex(mergeT *merge);
void    qh_delridge_merge(ridgeT *ridge);
vertexT *qh_find_newvertex(vertexT *oldvertex, setT *vertices, setT *ridges);
vertexT *qh_findbest_pinchedvertex(mergeT *merge, vertexT *apex, vertexT **pinchedp, realT *distp /* qh.newfacet_list */);
vertexT *qh_findbest_ridgevertex(ridgeT *ridge, vertexT **pinchedp, coordT *distp);
void    qh_findbest_test(boolT testcentrum, facetT *facet, facetT *neighbor,
           facetT **bestfacet, realT *distp, realT *mindistp, realT *maxdistp);
facetT *qh_findbestneighbor(facetT *facet, realT *distp, realT *mindistp, realT *maxdistp);
void    qh_flippedmerges(facetT *facetlist, boolT *wasmerge);
void    qh_forcedmerges(boolT *wasmerge);
void    qh_freemergesets(void);
void    qh_getmergeset(facetT *facetlist);
void    qh_getmergeset_initial(facetT *facetlist);
boolT   qh_getpinchedmerges(vertexT *apex, coordT maxdupdist, boolT *iscoplanar /* qh.newfacet_list, vertex_mergeset */);
boolT   qh_hasmerge(setT *mergeset, mergeType type, facetT *facetA, facetT *facetB);
void    qh_hashridge(setT *hashtable, int hashsize, ridgeT *ridge, vertexT *oldvertex);
ridgeT *qh_hashridge_find(setT *hashtable, int hashsize, ridgeT *ridge,
              vertexT *vertex, vertexT *oldvertex, int *hashslot);
void    qh_initmergesets(void);
void    qh_makeridges(facetT *facet);
void    qh_mark_dupridges(facetT *facetlist, boolT allmerges);
void    qh_maybe_duplicateridge(ridgeT *ridge);
void    qh_maybe_duplicateridges(facetT *facet);
void    qh_maydropneighbor(facetT *facet);
int     qh_merge_degenredundant(void);
void    qh_merge_nonconvex(facetT *facet1, facetT *facet2, mergeType mergetype);
void    qh_merge_pinchedvertices(int apexpointid /* qh.newfacet_list */);
void    qh_merge_twisted(facetT *facet1, facetT *facet2);
void    qh_mergecycle(facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_all(facetT *facetlist, boolT *wasmerge);
void    qh_mergecycle_facets(facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_neighbors(facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_ridges(facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_vneighbors(facetT *samecycle, facetT *newfacet);
void    qh_mergefacet(facetT *facet1, facetT *facet2, mergeType mergetype, realT *mindist, realT *maxdist, boolT mergeapex);
void    qh_mergefacet2d(facetT *facet1, facetT *facet2);
void    qh_mergeneighbors(facetT *facet1, facetT *facet2);
void    qh_mergeridges(facetT *facet1, facetT *facet2);
void    qh_mergesimplex(facetT *facet1, facetT *facet2, boolT mergeapex);
void    qh_mergevertex_del(vertexT *vertex, facetT *facet1, facetT *facet2);
void    qh_mergevertex_neighbors(facetT *facet1, facetT *facet2);
void    qh_mergevertices(setT *vertices1, setT **vertices);
setT   *qh_neighbor_intersections(vertexT *vertex);
setT   *qh_neighbor_vertices(vertexT *vertex, setT *subridge);
void    qh_neighbor_vertices_facet(vertexT *vertexA, facetT *facet, setT **vertices);
void    qh_newvertices(setT *vertices);
mergeT *qh_next_vertexmerge(void);
facetT *qh_opposite_horizonfacet(mergeT *merge, vertexT **vertex);
boolT   qh_reducevertices(void);
vertexT *qh_redundant_vertex(vertexT *vertex);
boolT   qh_remove_extravertices(facetT *facet);
void    qh_remove_mergetype(setT *mergeset, mergeType type);
void    qh_rename_adjacentvertex(vertexT *oldvertex, vertexT *newvertex, realT dist);
vertexT *qh_rename_sharedvertex(vertexT *vertex, facetT *facet);
boolT   qh_renameridgevertex(ridgeT *ridge, vertexT *oldvertex, vertexT *newvertex);
void    qh_renamevertex(vertexT *oldvertex, vertexT *newvertex, setT *ridges,
                        facetT *oldfacet, facetT *neighborA);
boolT   qh_test_appendmerge(facetT *facet, facetT *neighbor, boolT simplicial);
void    qh_test_degen_neighbors(facetT *facet);
boolT   qh_test_centrum_merge(facetT *facet, facetT *neighbor, realT angle, boolT okangle);
boolT   qh_test_nonsimplicial_merge(facetT *facet, facetT *neighbor, realT angle, boolT okangle);
void    qh_test_redundant_neighbors(facetT *facet);
boolT   qh_test_vneighbors(void /* qh.newfacet_list */);
void    qh_tracemerge(facetT *facet1, facetT *facet2, mergeType mergetype);
void    qh_tracemerging(void);
void    qh_undo_newfacets(void);
void    qh_updatetested(facetT *facet1, facetT *facet2);
setT   *qh_vertexridges(vertexT *vertex, boolT allneighbors);
void    qh_vertexridges_facet(vertexT *vertex, facetT *facet, setT **ridges);
void    qh_willdelete(facetT *facet, facetT *replace);

#endif /* qhDEFmerge */
