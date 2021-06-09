/*******************************************************************************
    Copyright (c) 2021, Andrea Maggiordomo, Paolo Cignoni and Marco Tarini

    This file is part of TextureDefrag, a reference implementation for
    the paper ``Texture Defragmentation for Photo-Reconstructed 3D Models''
    by Andrea Maggiordomo, Paolo Cignoni and Marco Tarini.

    TextureDefrag is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    TextureDefrag is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with TextureDefrag. If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/

#include "seam_remover.h"
#include "mesh.h"
#include "mesh_attribute.h"
#include "mesh_graph.h"
#include "matching.h"
#include "intersection.h"
#include "shell.h"
#include "arap.h"
#include "timer.h"
#include "logging.h"


#include <fstream>
#include <iomanip>
#include <unordered_set>

#include <vcg/complex/algorithms/clean.h>


constexpr double PENALTY_MULTIPLIER = 2.0;


struct Perf {
    double t_init;
    double t_seamdata;
    double t_alignmerge;
    double t_optimization_area;
    double t_optimize;
    double t_optimize_build;
    double t_optimize_arap;
    double t_check_before;
    double t_check_after;
    double t_accept;
    double t_reject;
    Timer timer;
};


static void InsertNewClusterInQueue(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params);
static CostInfo ComputeCost(ClusteredSeamHandle csh, GraphHandle graph, const AlgoParameters& params, double penalty);
static inline double GetPenalty(ClusteredSeamHandle csh, AlgoStateHandle state);
static inline bool Valid(const WeightedSeam& ws, ConstAlgoStateHandle state);
static inline void PurgeQueue(AlgoStateHandle state);
static void ComputeSeamData(SeamData& sd, ClusteredSeamHandle csh, GraphHandle graph, AlgoStateHandle state);
static OffsetMap AlignAndMerge(ClusteredSeamHandle csh, SeamData& sd, const MatchingTransform& mi, const AlgoParameters& params);
static void ComputeOptimizationArea(SeamData& sd, Mesh& mesh, OffsetMap& om);
static std::unordered_set<Mesh::VertexPointer> ComputeVerticesWithinOffsetThreshold(Mesh& m, const OffsetMap& om, const SeamData& sd);
static CheckStatus CheckBoundaryAfterAlignment(SeamData& sd);
static CheckStatus CheckAfterLocalOptimization(SeamData& sd, AlgoStateHandle state, const AlgoParameters& params);
static CheckStatus OptimizeChart(SeamData& sd, GraphHandle graph, bool fixIntersectingEdges);
static void AcceptMove(const SeamData& sd, AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params);
static void RejectMove(const SeamData& sd, AlgoStateHandle state, GraphHandle graph, CheckStatus status);
static void EraseSeam(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph);
static void InvalidateCluster(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph, CheckStatus status, double penaltyMultiplier);
static void RestoreChartAttributes(ChartHandle c, Mesh& m, std::vector<int>::const_iterator itvi,  std::vector<vcg::Point2d>::const_iterator ittc);
static CostInfo ReduceSeam(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params);


Perf perf = {};

#define PERF_TIMER_RESET (perf = {}, perf.timer.Reset())
#define PERF_TIMER_START double perf_timer_t0 = perf.timer.TimeElapsed()
#define PERF_TIMER_ACCUMULATE(field) perf.field += perf.timer.TimeElapsed() - perf_timer_t0
#define PERF_TIMER_ACCUMULATE_FROM_PREVIOUS(field) perf.field += perf.timer.TimeSinceLastCheck()

//static int statsCheck[10] = {};
//static int feasibility[6] = {};

static std::vector<int> statsCheck(CheckStatus::_END, 0);
static std::vector<int> feasibility(CostInfo::MatchingValue::_END, 0);

static vcg::Color4b statusColor[] = {
    vcg::Color4b::White, // PASS=0,
    vcg::Color4b::Gray , // FAIL_LOCAL_OVERLAP,
    vcg::Color4b::Red, // FAIL_GLOBAL_OVERLAP_BEFORE,
    vcg::Color4b::Green, // FAIL_GLOBAL_OVERLAP_AFTER_OPT, // border of the optimization area self-intersects
    vcg::Color4b::LightGreen, // FAIL_GLOBAL_OVERLAP_AFTER_BND, // border of the optimzation area hit the fixed border
    vcg::Color4b::LightBlue, // FAIL_DISTORTION_LOCAL,
    vcg::Color4b::Blue, // FAIL_DISTORTION_LOCAL,
    vcg::Color4b::LightRed, // FAIL_TOPOLOGY
    vcg::Color4b::Yellow, // FAIL_NUMERICAL_ERROR
    vcg::Color4b::White, // UNKNOWN
    vcg::Color4b(176, 0, 255, 255) // FAIL_GLOBAL_OVERLAP_UNFIXABLE
};

static vcg::Color4b mvColor[] = {
    vcg::Color4b::White, //   FEASIBLE=0,
    vcg::Color4b::Black, //   ZERO_AREA,
    vcg::Color4b::Cyan, //   UNFEASIBLE_BOUNDARY,
    vcg::Color4b::Magenta //   UNFEASIBLE_MATCHING,
};

static int accept = 0;
static int reject = 0;

static int num_retry = 0;
static int retry_success = 0;

double mincost = 100000;
double maxcost = -1;

double min_energy = 10000000000;
double max_energy = 0;

static void ClearGlobals()
{
    for (unsigned i = 0; i < statsCheck.size(); ++i)
        statsCheck[i] = 0;
    for (unsigned i = 0; i < feasibility.size(); ++i)
        feasibility[i] = 0;
    accept = 0;
    reject = 0;

    num_retry = 0;
    retry_success = 0;
}

void LogExecutionStats()
{
    LOG_INFO    << "======== EXECUTION STATS ========";
    LOG_INFO    << "INIT       " << std::fixed << std::setprecision(3) << perf.t_init / perf.timer.TimeElapsed()                                << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_init << " secs";
    LOG_INFO    << "SEAM       " << std::fixed << std::setprecision(3) << perf.t_seamdata / perf.timer.TimeElapsed()                            << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_seamdata << " secs";
    LOG_INFO    << "MERGE      " << std::fixed << std::setprecision(3) << perf.t_alignmerge / perf.timer.TimeElapsed()                          << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_alignmerge << " secs";
    LOG_INFO    << "AREA OPT   " << std::fixed << std::setprecision(3) << perf.t_optimization_area / perf.timer.TimeElapsed()                   << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_optimization_area << " secs";
    LOG_INFO    << "OPTIMIZE   " << std::fixed << std::setprecision(3) << perf.t_optimize / perf.timer.TimeElapsed()                            << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_optimize << " secs";
    LOG_VERBOSE << "  BUILD    " << std::fixed << std::setprecision(3) << perf.t_optimize_build / perf.timer.TimeElapsed()                      << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_optimize_build << " secs";
    LOG_VERBOSE << "  ARAP     " << std::fixed << std::setprecision(3) << perf.t_optimize_arap / perf.timer.TimeElapsed()                       << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_optimize_arap << " secs";
    LOG_INFO    << "CHECK      " << std::fixed << std::setprecision(3) << (perf.t_check_before + perf.t_check_after) / perf.timer.TimeElapsed() << " , " << std::defaultfloat << std::setprecision(6)<< (perf.t_check_before + perf.t_check_after) << " secs";
    LOG_VERBOSE << "  BEFORE   " << std::fixed << std::setprecision(3) << perf.t_check_before / perf.timer.TimeElapsed()                        << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_check_before << " secs";
    LOG_VERBOSE << "  AFTER    " << std::fixed << std::setprecision(3) << perf.t_check_after / perf.timer.TimeElapsed()                         << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_check_after << " secs";
    LOG_INFO    << "ACCEPT     " << std::fixed << std::setprecision(3) << perf.t_accept / perf.timer.TimeElapsed()                              << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_accept << " secs";
    LOG_INFO    << "  count:                    " << accept;
    LOG_INFO    << "  with retry:               " << retry_success;
    LOG_VERBOSE << "  min energy:               " << min_energy;
    LOG_VERBOSE << "  max energy:               " << max_energy;
    LOG_INFO    << "REJECT     " << std::fixed << std::setprecision(3) << perf.t_reject / perf.timer.TimeElapsed()                              << " , " << std::defaultfloat << std::setprecision(6)<< perf.t_reject << " secs";
    LOG_INFO    << "  count:                    " << reject;
    LOG_INFO    << "  with retry:               " << num_retry - retry_success;
    LOG_VERBOSE << "  local overlaps            " << statsCheck[FAIL_LOCAL_OVERLAP];
    LOG_VERBOSE << "  global overlaps before    " << statsCheck[FAIL_GLOBAL_OVERLAP_BEFORE];
    LOG_VERBOSE << "  global overlaps after opt " << statsCheck[FAIL_GLOBAL_OVERLAP_AFTER_OPT];
    LOG_VERBOSE << "  global overlaps after bnd " << statsCheck[FAIL_GLOBAL_OVERLAP_AFTER_BND];
    LOG_VERBOSE << "  global overlaps unfixable " << statsCheck[FAIL_GLOBAL_OVERLAP_UNFIXABLE];
    LOG_VERBOSE << "  distortion (local)        " << statsCheck[FAIL_DISTORTION_LOCAL];
    LOG_VERBOSE << "  distortion (global)       " << statsCheck[FAIL_DISTORTION_GLOBAL];
    LOG_VERBOSE << "  topology                  " << statsCheck[FAIL_TOPOLOGY];
    LOG_VERBOSE << "  numerical error           " << statsCheck[FAIL_NUMERICAL_ERROR];
    LOG_VERBOSE << "    FEASIBILITY";
    LOG_VERBOSE << "      feasible              " << feasibility[CostInfo::FEASIBLE];
    LOG_VERBOSE << "      unfeasible boundary   " << feasibility[CostInfo::UNFEASIBLE_BOUNDARY];
    LOG_VERBOSE << "      unfeasible matching   " << feasibility[CostInfo::UNFEASIBLE_MATCHING];
    LOG_INFO    << "TOTAL      " << std::fixed << std::setprecision(3) << perf.timer.TimeElapsed() / perf.timer.TimeElapsed()          << " , " << std::defaultfloat << std::setprecision(6)<< perf.timer.TimeElapsed() << " secs";
    LOG_VERBOSE << "Minimum computed cost is " << mincost;
    LOG_VERBOSE << "Maximum computed cost is " << maxcost;
    LOG_INFO    << "===================================";
}

static void PrintStateInfo(AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params)
{
    std::set<ClusteredSeamHandle> moveSet;

    for (auto& entry : state->chartSeamMap) {
        for (auto csh : entry.second) {
            moveSet.insert(csh);
        }
    }

    LOG_VERBOSE << "Status of the residual " << moveSet.size() << " operations:";

    int nstat[100] = {};
    int mstat[100] = {};
    for (auto csh : moveSet) {
        auto it = state->status.find(csh);
        ensure(it != state->status.end());
        ensure(it->second != PASS);
        CostInfo ci = ComputeCost(csh, graph, params, GetPenalty(csh, state));
        nstat[state->status[csh]]++;
        mstat[ci.mvalue]++;
    }

    LOG_VERBOSE << "PASS                          " << nstat[CheckStatus::PASS];
    LOG_VERBOSE << "FAIL_LOCAL_OVERLAP            " << nstat[CheckStatus::FAIL_LOCAL_OVERLAP];
    LOG_VERBOSE << "FAIL_GLOBAL_OVERLAP_BEFORE    " << nstat[CheckStatus::FAIL_GLOBAL_OVERLAP_BEFORE];
    LOG_VERBOSE << "FAIL_GLOBAL_OVERLAP_AFTER_OPT " << nstat[CheckStatus::FAIL_GLOBAL_OVERLAP_AFTER_OPT];
    LOG_VERBOSE << "FAIL_GLOBAL_OVERLAP_AFTER_BND " << nstat[CheckStatus::FAIL_GLOBAL_OVERLAP_AFTER_BND];
    LOG_VERBOSE << "FAIL_GLOBAL_OVERLAP_UNFIXABLE " << nstat[CheckStatus::FAIL_GLOBAL_OVERLAP_UNFIXABLE];
    LOG_VERBOSE << "FAIL_DISTORTION_LOCAL         " << nstat[CheckStatus::FAIL_DISTORTION_LOCAL];
    LOG_VERBOSE << "FAIL_DISTORTION_GLOBAL        " << nstat[CheckStatus::FAIL_DISTORTION_GLOBAL];
    LOG_VERBOSE << "FAIL_TOPOLOGY                 " << nstat[CheckStatus::FAIL_TOPOLOGY];
    LOG_VERBOSE << "FAIL_NUMERICAL_ERROR          " << nstat[CheckStatus::FAIL_NUMERICAL_ERROR];
    LOG_VERBOSE << "UNKNOWN                       " << nstat[CheckStatus::UNKNOWN];
    LOG_VERBOSE << "  - FEASIBLE                         " << mstat[CostInfo::MatchingValue::FEASIBLE];
    LOG_VERBOSE << "  - ZERO_AREA                        " << mstat[CostInfo::MatchingValue::ZERO_AREA];
    LOG_VERBOSE << "  - UNFEASIBLE_BOUNDARY              " << mstat[CostInfo::MatchingValue::UNFEASIBLE_BOUNDARY];
    LOG_VERBOSE << "  - UNFEASIBLE_MATCHING              " << mstat[CostInfo::MatchingValue::UNFEASIBLE_MATCHING];
    LOG_VERBOSE << "  - REJECTED                         " << mstat[CostInfo::MatchingValue::REJECTED];

}

void PrepareMesh(Mesh& m, int *vndup)
{
    int dupVert = tri::Clean<Mesh>::RemoveDuplicateVertex(m);
    if (dupVert > 0)
        LOG_INFO << "Removed " << dupVert << " duplicate vertices";

    int zeroArea = tri::Clean<Mesh>::RemoveZeroAreaFace(m);
    if (zeroArea > 0)
        LOG_INFO << "Removed " << zeroArea << " zero area faces";

    tri::UpdateTopology<Mesh>::FaceFace(m);

    // orient faces coherently
    bool wasOriented, isOrientable;
    tri::Clean<Mesh>::OrientCoherentlyMesh(m, wasOriented, isOrientable);

    tri::UpdateTopology<Mesh>::FaceFace(m);

    int numRemovedFaces = tri::Clean<Mesh>::RemoveNonManifoldFace(m);
    if (numRemovedFaces > 0)
        LOG_INFO << "Removed " << numRemovedFaces << " non-manifold faces";

    tri::Allocator<Mesh>::CompactEveryVector(m);
    tri::UpdateTopology<Mesh>::FaceFace(m);

    Compute3DFaceAdjacencyAttribute(m);

    CutAlongSeams(m);
    tri::Allocator<Mesh>::CompactEveryVector(m);

    *vndup = m.VN();

    tri::UpdateTopology<Mesh>::FaceFace(m);
    while (tri::Clean<Mesh>::SplitNonManifoldVertex(m, 0))
        ;
    tri::UpdateTopology<Mesh>::VertexFace(m);

    tri::Allocator<Mesh>::CompactEveryVector(m);
}

AlgoStateHandle InitializeState(GraphHandle graph, const AlgoParameters& algoParameters)
{
    PERF_TIMER_RESET;
    PERF_TIMER_START;

    AlgoStateHandle state = std::make_shared<AlgoState>();
    ARAP::ComputeEnergyFromStoredWedgeTC(graph->mesh, &state->arapNum, &state->arapDenom);
    state->inputUVBorderLength = 0;
    state->currentUVBorderLength = 0;

    BuildSeamMesh(graph->mesh, state->sm, graph);
    std::vector<SeamHandle> seams = GenerateSeams(state->sm);

    // disconnecting seams are (initially) clustered by chart adjacency
    // non-disconnecting seams are not clustered (segment granularity)

    std::vector<ClusteredSeamHandle> cshvec = ClusterSeamsByChartId(seams);
    int ndisconnecting = 0;
    int nself = 0;

    for (auto csh : cshvec) {
        ChartPair charts = GetCharts(csh, graph);
        if (charts.first == charts.second)
            nself++;
        else
            ndisconnecting++;
        InsertNewClusterInQueue(csh, state, graph, algoParameters);
    }
    LOG_INFO << "Found " << ndisconnecting << " disconnecting seams";
    LOG_INFO << "Found " << nself << " non-disconnecting seams";

    // sanity check
    //for (auto& entry : state->chartSeamMap) {
    //    LOG_INFO << entry.first;
    //    ensure(entry.second.size() >= (graph->GetChart(entry.first)->adj.size()));
    //}

    for (const auto& ch : graph->charts) {
        state->inputUVBorderLength += ch.second->BorderUV();
        state->currentUVBorderLength += ch.second->BorderUV();
    }

    PERF_TIMER_ACCUMULATE(t_init);
    return state;
}

void GreedyOptimization(GraphHandle graph, AlgoStateHandle state, const AlgoParameters& params)
{
    ClearGlobals();

    Timer timer;

    PrintStateInfo(state, graph, params);

    LOG_INFO << "Atlas energy before optimization is " << ARAP::ComputeEnergyFromStoredWedgeTC(graph->mesh, nullptr, nullptr);

    int k = 0;
    while (state->queue.size() > 0) {

        if (state->queue.size() > 5 * state->cost.size())
            PurgeQueue(state);

        if (state->queue.size() == 0) {
            LOG_INFO << "Queue is empty, interrupting.";
            break;
        }

        if (params.timelimit > 0 && timer.TimeElapsed() > params.timelimit) {
            LOG_INFO << "Timelimit hit, interrupting.";
            break;
        }

        if (params.UVBorderLengthReduction > (state->currentUVBorderLength / state->inputUVBorderLength)) {
            LOG_INFO << "Target UV border reduction reached, interrupting.";
            break;
        }

        WeightedSeam ws = state->queue.top();
        state->queue.pop();
        if (Valid(ws, state)) {
            if (ws.second == Infinity()) {
                // sanity check
                for (auto& entry : state->cost)
                    ensure(entry.second == Infinity());
                LOG_INFO << "Queue is empty, interrupting.";
                break;
            } else {
                ++k;
                if ((k % 200) == 0) {
                    LOG_INFO << "Logging execution stats after " << k << " iterations";
                    LogExecutionStats();
                }
                SeamData sd;
                ComputeSeamData(sd, ws.first, graph, state);
                LOG_DEBUG << "  Chart ids are " << sd.a->id << " " << sd.b->id << " (areas = " << sd.a->AreaUV() << ", " << sd.b->AreaUV() << ")";

                OffsetMap om = AlignAndMerge(ws.first, sd, state->transform[ws.first], params);

                ComputeOptimizationArea(sd, graph->mesh, om);

                // when merging two charts, check if they collide outside the optimization area

                CheckStatus status = (sd.a != sd.b) ? CheckBoundaryAfterAlignment(sd) : PASS;

                if (status == PASS)
                    status = OptimizeChart(sd, graph, false);

                if (status == PASS)
                    status = CheckAfterLocalOptimization(sd, state, params);

                while (status == FAIL_GLOBAL_OVERLAP_AFTER_OPT || status == FAIL_GLOBAL_OVERLAP_AFTER_BND) {
                    LOG_DEBUG << "Global overlaps detected after ARAP optimization, fixing edges";
                    CheckStatus iterStatus = OptimizeChart(sd, graph, true);
                    if (iterStatus == _END)
                        break;
                    else
                        status = CheckAfterLocalOptimization(sd, state, params);
                }

                statsCheck[status]++;

                if (status == PASS) {
                    AcceptMove(sd, state, graph, params);
                    ColorizeSeam(sd.csh, vcg::Color4b(255, 69, 0, 255));
                    accept++;
                    LOG_DEBUG << "Accepted operation";
                } else {
                    RejectMove(sd, state, graph, status);
                    reject++;
                    LOG_DEBUG << "Rejected operation";
                }
            }
        }
    }
    PrintStateInfo(state, graph, params);
    LogExecutionStats();
    LOG_INFO << "Atlas energy after optimization is " << ARAP::ComputeEnergyFromStoredWedgeTC(graph->mesh, nullptr, nullptr);
}

void Finalize(GraphHandle graph, int *vndup)
{
    std::unordered_set<Mesh::ConstVertexPointer> vset;
    for (const MeshFace& f : graph->mesh.face)
        for (int i = 0; i < 3; ++i)
            vset.insert(f.cV(i));

    *vndup = (int) vset.size();

    tri::Clean<Mesh>::RemoveDuplicateVertex(graph->mesh);
    tri::Clean<Mesh>::RemoveUnreferencedVertex(graph->mesh);
    tri::UpdateTopology<Mesh>::VertexFace(graph->mesh);
}

// -- static functions ---------------------------------------------------------

static void InsertNewClusterInQueue(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params)
{
    ColorizeSeam(csh, vcg::Color4b::White);

    CostInfo ci = ComputeCost(csh, graph, params, GetPenalty(csh, state));

    if (params.reduce) {
        while (ci.mvalue == CostInfo::UNFEASIBLE_MATCHING) {
            ci = ReduceSeam(csh, state, graph, params);
        }
    }

    ColorizeSeam(csh, mvColor[ci.mvalue]);

    feasibility[ci.mvalue]++;

    if (ci.cost != Infinity()) {
        mincost = std::min(mincost, ci.cost);
        maxcost = std::max(maxcost, ci.cost);
    }

    state->queue.push(std::make_pair(csh, ci.cost));
    state->cost[csh] = ci.cost;
    state->transform[csh] = ci.matching;
    state->status[csh] = UNKNOWN;
    state->mvalue[csh] = ci.mvalue;

    // initialize chart-to-seam map
    ChartPair p = GetCharts(csh, graph);
    state->chartSeamMap[p.first->id].insert(csh);
    state->chartSeamMap[p.second->id].insert(csh);

    // add cluster to endpoint map
    std::set<int> endpoints = GetEndpoints(csh);
    for (auto vi : endpoints)
        state->emap[vi].insert(csh);
}

// this function returns true if there exists a sequence of at most maxSteps operations
// that results in a UV-island being formed in the graph AND involves a and b
static bool IslandLookahead(ChartHandle a, ChartHandle b, int maxSteps)
{
    // if there are too many candidates, exit early
    if (a->adj.size() > (unsigned) maxSteps || b->adj.size() > (unsigned) maxSteps)
        return false;

    std::unordered_set<ChartHandle> nab;
    nab.insert(a->adj.begin(), a->adj.end());
    nab.insert(b->adj.begin(), b->adj.end());

    for (auto c : nab) {
        std::stack<ChartHandle> s;
        s.push(a);

        std::set<ChartHandle> visited = {c, a}; // prevent the visit from reaching c
        int steps = 0;

        // start the visit
        while (!s.empty()) {
            ChartHandle sc = s.top();
            s.pop();
            visited.insert(sc);
            for (auto ac : sc->adj) {
                if (visited.find(ac) == visited.end()) {
                    s.push(ac);
                    steps++;
                }
                if (steps > maxSteps)
                    break;
            }
            if (steps > maxSteps)
                break;
        }

        // if the stack is empty we could not advance the visit
        // this means that the visited component would only be adjacent to c
        //   => the visited component is an island whose only adjacency is c
        if (s.empty())
            return true;
    }

    return false;
}

static CostInfo ComputeCost(ClusteredSeamHandle csh, GraphHandle graph, const AlgoParameters& params, double penalty)
{
    bool swapped;
    ChartPair charts = GetCharts(csh, graph, &swapped);

    ChartHandle a = charts.first;
    ChartHandle b = charts.second;
    if (a->AreaUV() == 0 || b->AreaUV() == 0 || a->Area3D() == 0 || b->Area3D() == 0) {
        return { Infinity(), {}, CostInfo::ZERO_AREA };
    }

    std::vector<vcg::Point2d> bpa;
    std::vector<vcg::Point2d> bpb;

    ExtractUVCoordinates(csh, bpa, bpb, {a->id});

    MatchingTransform mi = MatchingTransform::Identity();
    // if seam is disconnecting compute the actual matching
    if (a != b)
        mi = ComputeMatchingRigidMatrix(bpa, bpb);

    std::map<RegionID, double> bmap;
    double seamLength3D = 0;

    int ne = 0;
    SeamMesh& seamMesh = csh->sm;
    for (SeamHandle sh : csh->seams) {
        for (int iedge : sh->edges) {
            SeamEdge& edge = seamMesh.edge[iedge];
            bmap[edge.fa->id] += (edge.fa->V0(edge.ea)->T().P() - edge.fa->V1(edge.ea)->T().P()).Norm();
            bmap[edge.fb->id] += (edge.fb->V0(edge.eb)->T().P() - edge.fb->V1(edge.eb)->T().P()).Norm();
            seamLength3D += (edge.fa->P0(edge.ea) - edge.fa->P1(edge.ea)).Norm();
            ne++;
        }
    }

    CostInfo ci;
    ci.matching = mi;
    ci.mvalue = CostInfo::FEASIBLE;

    if (a != b) {
        double maxSeamToBoundaryRatio = std::max(bmap[a->id] / a->BorderUV(), bmap[b->id] / b->BorderUV());
        if (maxSeamToBoundaryRatio < params.boundaryTolerance && (!params.visitComponents || !IslandLookahead(a, b, 5))) {
            ci.cost = Infinity();
            ci.mvalue = CostInfo::UNFEASIBLE_BOUNDARY;
            return ci;
        }
    }

    double totErr = MatchingErrorTotal(mi, bpa, bpb);
    double avgErr = totErr / (double) bpa.size();

    if (avgErr > params.matchingThreshold * ((bmap[a->id] + bmap[b->id]) / 2.0)) {
        ci.cost = Infinity();
        ci.mvalue = CostInfo::UNFEASIBLE_MATCHING;
        return ci;
    }

    double lossgain = avgErr * std::pow(std::min(a->BorderUV() / bmap[a->id], b->BorderUV() / bmap[b->id]), params.expb);
    double sizebonus = std::min(a->AreaUV(), b->AreaUV());

    ci.cost = lossgain * sizebonus;

    if (ci.cost == 0 && penalty > 1.0)
        ci.cost = 1;

    ci.cost *= penalty;

    return ci;
}

static inline double GetPenalty(ClusteredSeamHandle csh, AlgoStateHandle state)
{
    if (state->penalty.find(csh) == state->penalty.end())
        state->penalty[csh] = 1.0;
    return state->penalty[csh];
}

static inline bool Valid(const WeightedSeam& ws, ConstAlgoStateHandle state)
{
    auto it = state->cost.find(ws.first);
    return (it != state->cost.end() && it->second == ws.second);
}

static inline void PurgeQueue(AlgoStateHandle state)
{
    std::unordered_set<ClusteredSeamHandle> valid;
    while (!state->queue.empty()) {
        WeightedSeam ws = state->queue.top();
        if (Valid(ws, state) && ws.second != Infinity())
            valid.insert(ws.first);
        state->queue.pop();
    }
    ensure(state->queue.empty());
    for (ClusteredSeamHandle csh : valid)
        state->queue.push(std::make_pair(csh, state->cost[csh]));
}

static void ComputeSeamData(SeamData& sd, ClusteredSeamHandle csh, GraphHandle graph, AlgoStateHandle state)
{
    PERF_TIMER_START;

    sd.csh = csh;

    ChartPair charts = GetCharts(csh, graph);
    sd.a = charts.first;
    sd.b = charts.second;

    if (state->failed[sd.a->id].count(sd.b->id) > 0)
        num_retry++;

    Mesh& m = graph->mesh;

    sd.texcoorda.reserve(3 * sd.a->FN());
    sd.vertexinda.reserve(3 * sd.a->FN());
    for (auto fptr : sd.a->fpVec) {
        sd.texcoorda.push_back(fptr->V(0)->T().P());
        sd.texcoorda.push_back(fptr->V(1)->T().P());
        sd.texcoorda.push_back(fptr->V(2)->T().P());
        sd.vertexinda.push_back(tri::Index(m, fptr->V(0)));
        sd.vertexinda.push_back(tri::Index(m, fptr->V(1)));
        sd.vertexinda.push_back(tri::Index(m, fptr->V(2)));
    }

    if (sd.a != sd.b) {
        sd.texcoordb.reserve(3 * sd.b->FN());
        sd.vertexindb.reserve(3 * sd.b->FN());
        for (auto fptr : sd.b->fpVec) {
            sd.texcoordb.push_back(fptr->V(0)->T().P());
            sd.texcoordb.push_back(fptr->V(1)->T().P());
            sd.texcoordb.push_back(fptr->V(2)->T().P());
            sd.vertexindb.push_back(tri::Index(m, fptr->V(0)));
            sd.vertexindb.push_back(tri::Index(m, fptr->V(1)));
            sd.vertexindb.push_back(tri::Index(m, fptr->V(2)));
        }
    }

    sd.inputUVBorderLength = sd.a->BorderUV();
    if (sd.a != sd.b)
        sd.inputUVBorderLength += sd.b->BorderUV();

    PERF_TIMER_ACCUMULATE(t_seamdata);
}

static void WedgeTexFromVertexTex(ChartHandle c)
{
    for (auto fptr : c->fpVec)
        for (int i = 0; i < 3; ++i)
            fptr->WT(i).P() = fptr->V(i)->T().P();
}

static OffsetMap AlignAndMerge(ClusteredSeamHandle csh, SeamData& sd, const MatchingTransform& mi, const AlgoParameters& params)
{
    PERF_TIMER_START;

    OffsetMap om;

    // align
    if (sd.a != sd.b) {
        std::unordered_set<Mesh::VertexPointer> visited;
        for (auto fptr : sd.b->fpVec) {
            for (int i = 0; i < 3; ++i) {
                if (visited.count(fptr->V(i)) == 0) {
                    visited.insert(fptr->V(i));
                    fptr->V(i)->T().P() = mi.Apply(fptr->V(i)->T().P());
                }
            }
        }
    }

    // elect representative vertices for the merge (only 1 vert must be referenced after the merge)
    SeamMesh& seamMesh = csh->sm;
    for (SeamHandle sh : csh->seams) {
        for (int iedge : sh->edges) {
            SeamEdge& edge = seamMesh.edge[iedge];

            Mesh::VertexPointer v0a = edge.fa->V0(edge.ea);
            Mesh::VertexPointer v1a = edge.fa->V1(edge.ea);
            Mesh::VertexPointer v0b = edge.fb->V0(edge.eb);
            Mesh::VertexPointer v1b = edge.fb->V1(edge.eb);

            sd.seamVertices.insert(v0a);
            sd.seamVertices.insert(v1a);
            sd.seamVertices.insert(v0b);
            sd.seamVertices.insert(v1b);

            if (v0a->P() != edge.V(0)->P())
                std::swap(v0a, v1a);
            if (v0b->P() != edge.V(0)->P())
                std::swap(v0b, v1b);

            if (sd.mrep.count(v0a) == 0)
                sd.evec[edge.V(0)].push_back(v0a);
            sd.mrep[v0a] = sd.evec[edge.V(0)].front();

            if (sd.mrep.count(v1a) == 0)
                sd.evec[edge.V(1)].push_back(v1a);
            sd.mrep[v1a] = sd.evec[edge.V(1)].front();

            if (sd.mrep.count(v0b) == 0)
                sd.evec[edge.V(0)].push_back(v0b);
            sd.mrep[v0b] = sd.evec[edge.V(0)].front();

            if (sd.mrep.count(v1b) == 0)
                sd.evec[edge.V(1)].push_back(v1b);
            sd.mrep[v1b] = sd.evec[edge.V(1)].front();
        }
    }

    for (auto vp : sd.seamVertices) {
        std::vector<Mesh::FacePointer> faces;
        std::vector<int> indices;
        face::VFStarVF(vp, faces, indices);
        sd.vfTopologyFaceSet.insert(faces.begin(), faces.end());
    }

    // update vertex references
    for (auto fptr : sd.a->fpVec) {
        for (int i = 0; i < 3; ++i)
            if (sd.mrep.count(fptr->V(i)))
                fptr->V(i) = sd.mrep[fptr->V(i)];
    }
    if (sd.a != sd.b) {
        for (auto fptr : sd.b->fpVec) {
            for (int i = 0; i < 3; ++i)
                if (sd.mrep.count(fptr->V(i)))
                    fptr->V(i) = sd.mrep[fptr->V(i)];
        }
    }

    // update topologies. face-face is trivial

    // face-face
    for (SeamHandle sh : csh->seams) {
        for (int iedge : sh->edges) {
            SeamEdge& edge = seamMesh.edge[iedge];
            edge.fa->FFp(edge.ea) = edge.fb;
            edge.fa->FFi(edge.ea) = edge.eb;
            edge.fb->FFp(edge.eb) = edge.fa;
            edge.fb->FFi(edge.eb) = edge.ea;
        }
    }

    {
        for (Mesh::VertexPointer vp : sd.seamVertices) {
            vp->VFp() = 0;
            vp->VFi() = 0;
        }

        for (Mesh::FacePointer fptr : sd.vfTopologyFaceSet) {
            for (int i = 0; i < 3; ++i) {
                if (sd.seamVertices.find(fptr->V(i)) != sd.seamVertices.end()) {
                    (*fptr).VFp(i) = (*fptr).V(i)->VFp();
                    (*fptr).VFi(i) = (*fptr).V(i)->VFi();
                    (*fptr).V(i)->VFp() = &(*fptr);
                    (*fptr).V(i)->VFi() = i;
                }
            }
        }
    }

    // compute average positions and displacements
    for (auto& entry : sd.evec) {
        vcg::Point2d sumpos = vcg::Point2d::Zero();
        for (auto vp : entry.second) {
            sumpos += vp->T().P();
        }
        vcg::Point2d avg = sumpos / (double) entry.second.size();

        double maxOffset = 0;
        for (auto& vp : entry.second)
            maxOffset = std::max(maxOffset, params.offsetFactor * (vp->T().P() - avg).Norm());

        om[entry.second.front()] = maxOffset;
        entry.second.front()->T().P() = avg;
    }

    PERF_TIMER_ACCUMULATE(t_alignmerge);
    return om;
}

static void ComputeOptimizationArea(SeamData& sd, Mesh& mesh, OffsetMap& om)
{
    PERF_TIMER_START;

    std::vector<Mesh::FacePointer> fpvec;
    fpvec.insert(fpvec.end(), sd.a->fpVec.begin(), sd.a->fpVec.end());
    if (sd.a != sd.b)
        fpvec.insert(fpvec.end(), sd.b->fpVec.begin(), sd.b->fpVec.end());

    sd.verticesWithinThreshold = ComputeVerticesWithinOffsetThreshold(mesh, om, sd);
    sd.optimizationArea.clear();

    auto ffadj = Get3DFaceAdjacencyAttribute(mesh);
    for (auto fptr : fpvec) {
        bool addFace = false;
        bool edgeManifold = true;
        for (int i = 0; i < 3; ++i) {
            edgeManifold &= IsEdgeManifold3D(mesh, *fptr, i, ffadj);
            if (sd.verticesWithinThreshold.find(fptr->V(i)) != sd.verticesWithinThreshold.end())
                addFace = true;
        }
        if (addFace && edgeManifold)
            sd.optimizationArea.insert(fptr);
        if (addFace && !edgeManifold) {
            sd.verticesWithinThreshold.erase(fptr->V(0));
            sd.verticesWithinThreshold.erase(fptr->V(1));
            sd.verticesWithinThreshold.erase(fptr->V(2));
        }
    }

    for (auto fptr : sd.optimizationArea) {
        sd.texcoordoptVert.push_back(fptr->V(0)->T().P());
        sd.texcoordoptVert.push_back(fptr->V(1)->T().P());
        sd.texcoordoptVert.push_back(fptr->V(2)->T().P());

        sd.texcoordoptWedge.push_back(fptr->WT(0).P());
        sd.texcoordoptWedge.push_back(fptr->WT(1).P());
        sd.texcoordoptWedge.push_back(fptr->WT(2).P());

    }

    {
        sd.inputNegativeArea = 0;
        sd.inputAbsoluteArea = 0;
        for (auto fptr : sd.optimizationArea) {
            vcg::Point2d uv0in = fptr->WT(0).P();
            vcg::Point2d uv1in = fptr->WT(1).P();
            vcg::Point2d uv2in = fptr->WT(2).P();

            double inputAreaUV = ((uv1in - uv0in) ^ (uv2in - uv0in)) / 2.0;
            if (inputAreaUV < 0)
                sd.inputNegativeArea += inputAreaUV;
            sd.inputAbsoluteArea += std::abs(inputAreaUV);
        }
    }

    PERF_TIMER_ACCUMULATE(t_optimization_area);
}

/* Visit vertices starting from the merged ones, subject to the distance budget
 * stored in the OffsetMap object. */
static std::unordered_set<Mesh::VertexPointer> ComputeVerticesWithinOffsetThreshold(Mesh& m, const OffsetMap& om, const SeamData& sd)
{
    // typedef for heap nodes
    typedef std::pair<Mesh::VertexPointer, double> VertexNode;

    // comparison operator for the max-heap
    auto cmp = [] (const VertexNode& v1, const VertexNode& v2) { return v1.second < v2.second; };

    std::unordered_set<Mesh::VertexPointer> vset;

    // distance budget map
    OffsetMap dist;
    // heap
    std::vector<VertexNode> h;

    for (const auto& entry : om) {
        h.push_back(std::make_pair(entry.first, entry.second));
        dist[entry.first] = entry.second;
    }

    std::make_heap(h.begin(), h.end());

    while (!h.empty()) {
        std::pop_heap(h.begin(), h.end(), cmp);
        VertexNode node = h.back();
        h.pop_back();
        if (node.second == dist[node.first]) {
            std::vector<Mesh::FacePointer> faces;
            std::vector<int> indices;
            face::VFStarVF(node.first, faces, indices);

            for (unsigned i = 0; i < faces.size(); ++i) {
                if(faces[i]->id != sd.a->id && faces[i]->id != sd.b->id){
                    LOG_ERR << "issue at face " << tri::Index(m, faces[i]);
                }
                ensure(faces[i]->id == sd.a->id || faces[i]->id == sd.b->id);

                // if either neighboring vertex is seen with more spare distance,
                // update the distance map

                int e1 = indices[i];
                Mesh::VertexPointer v1 = faces[i]->V1(indices[i]);
                double d1 = dist[node.first] - EdgeLengthUV(*faces[i], e1);

                if (d1 >= 0 && (dist.find(v1) == dist.end() || dist[v1] < d1)) {
                    dist[v1] = d1;
                    h.push_back(std::make_pair(v1, d1));
                    std::push_heap(h.begin(), h.end(), cmp);
                }

                int e2 = (indices[i]+2)%3;
                Mesh::VertexPointer v2 = faces[i]->V2(indices[i]);
                double d2 = dist[node.first] - EdgeLengthUV(*faces[i], e2);

                if (d2 >= 0 && (dist.find(v2) == dist.end() || dist[v2] < d2)) {
                    dist[v2] = d2;
                    h.push_back(std::make_pair(v2, d2));
                    std::push_heap(h.begin(), h.end(), cmp);
                }
            }
        }
    }

    for (const auto& entry : dist)
        vset.insert(entry.first);

    LOG_DEBUG << "vset.size() == " << vset.size();

    return vset;
}

static std::vector<HalfEdge> ExtractHalfEdges(const std::vector<ChartHandle>& charts, const vcg::Box2d& box, bool internalOnly)
{
    std::vector<HalfEdge> hvec;
    for (auto ch : charts)
        for (auto fptr : ch->fpVec)
            for (int i = 0; i < 3; ++i)
                if ((!internalOnly || !face::IsBorder(*fptr, i)) && SegmentBoxIntersection(Segment(fptr->V0(i)->T().P(), fptr->V1(i)->T().P()), box))
                    hvec.push_back(HalfEdge{fptr, i});
    return hvec;
}

static CheckStatus CheckBoundaryAfterAlignmentInner(SeamData& sd)
{
    ensure(sd.a != sd.b);

    // check if the borders of the fixed areas of a and b intersect each other
    std::vector<HalfEdge> aVec;
    for (auto fptr : sd.a->fpVec)
        if (sd.optimizationArea.find(fptr) == sd.optimizationArea.end())
            for (int i = 0; i < 3; ++i)
                if (face::IsBorder(*fptr, i) || (sd.optimizationArea.find(fptr->FFp(i)) != sd.optimizationArea.end()))
                    aVec.push_back(HalfEdge{fptr, i});

    std::vector<HalfEdge> bVec;
    for (auto fptr : sd.b->fpVec)
        if (sd.optimizationArea.find(fptr) == sd.optimizationArea.end())
            for (int i = 0; i < 3; ++i)
                if (face::IsBorder(*fptr, i) || (sd.optimizationArea.find(fptr->FFp(i)) != sd.optimizationArea.end()))
                    bVec.push_back(HalfEdge{fptr, i});

    if ((aVec.size() > 0) && (bVec.size() > 0)) {
        std::vector<HalfEdgePair> heVec = CrossIntersection(aVec, bVec);
        if (heVec.size() > 0)
            return FAIL_GLOBAL_OVERLAP_BEFORE;
    }
#if 0
    vcg::Box2d box;
    for (auto fptr : sd.b->fpVec)
        for (int i = 0; i < 3; ++i)
            box.Add(fptr->V(i)->T().P());

    // also check if the edges of b overlap the edges of a (only check the edges inside the bbox of b)
    aVec = ExtractHalfEdges({sd.a}, box, false);
    bVec = ExtractHalfEdges({sd.b}, box, false);
    if ((aVec.size() > 0) && (bVec.size() > 0)) {
        std::vector<HalfEdgePair> heVec = CrossIntersection(aVec, bVec);
        if (heVec.size() > 0)
            return FAIL_GLOBAL_OVERLAP_BEFORE;
    }
#endif

    return PASS;
}

static CheckStatus CheckBoundaryAfterAlignment(SeamData& sd)
{
    PERF_TIMER_START;
    LOG_DEBUG << "Running CheckBoundaryAfterAlignment()";
    CheckStatus status = CheckBoundaryAfterAlignmentInner(sd);
    PERF_TIMER_ACCUMULATE(t_check_before);
    return status;
}

static CheckStatus CheckAfterLocalOptimizationInner(SeamData& sd, AlgoStateHandle state, const AlgoParameters& params)
{
    double newArapVal = (state->arapNum + (sd.outputArapNum - sd.inputArapNum)) / state->arapDenom;
    if (newArapVal > params.globalDistortionThreshold)
        return FAIL_DISTORTION_GLOBAL;

    double localDistortion = sd.outputArapNum / sd.outputArapDenom;
    if (localDistortion > params.distortionTolerance) {
        return FAIL_DISTORTION_LOCAL;
    }

    /* Check if the folded area is too large */
    double outputNegativeArea = 0;
    double outputAbsoluteArea = 0;
    for (auto fptr : sd.optimizationArea) {
        double areaUV = AreaUV(*fptr);
        if (areaUV < 0)
            outputNegativeArea += areaUV;
        outputAbsoluteArea += std::abs(areaUV);
    }

    double inputRatio = std::abs(sd.inputNegativeArea / sd.inputAbsoluteArea);
    double outputRatio = std::abs(outputNegativeArea / outputAbsoluteArea);

    if (outputRatio > inputRatio) {
        return FAIL_LOCAL_OVERLAP;
    }

    // Functions to detect if the half-edges have already been fixed (in which case detecting the intersection is meaningless,
    // the half-edges were intersecting to begin with)

    auto FixedPair = [&] (const HalfEdgePair& hep) -> bool {
        return /*hep.first.fp->id == hep.second.fp->id
                &&*/ sd.fixedVerticesFromIntersectingEdges.find(hep.first.V0()) != sd.fixedVerticesFromIntersectingEdges.end()
                && sd.fixedVerticesFromIntersectingEdges.find(hep.first.V1()) != sd.fixedVerticesFromIntersectingEdges.end()
                && sd.fixedVerticesFromIntersectingEdges.find(hep.second.V0()) != sd.fixedVerticesFromIntersectingEdges.end()
                && sd.fixedVerticesFromIntersectingEdges.find(hep.second.V1()) != sd.fixedVerticesFromIntersectingEdges.end();
    };

    auto FixedFirst = [&] (const HalfEdgePair& hep) -> bool {
        return /*hep.first.fp->id == hep.second.fp->id
                &&*/ sd.fixedVerticesFromIntersectingEdges.find(hep.first.V0()) != sd.fixedVerticesFromIntersectingEdges.end()
                && sd.fixedVerticesFromIntersectingEdges.find(hep.first.V1()) != sd.fixedVerticesFromIntersectingEdges.end();
    };

    // ensure the optimization border does not self-intersect
    std::vector<HalfEdge> sVec;
    for (auto fptr : sd.optimizationArea)
        for (int i = 0; i < 3; ++i)
            if (face::IsBorder(*fptr, i) || (sd.optimizationArea.find(fptr->FFp(i)) == sd.optimizationArea.end()))
                sVec.push_back(HalfEdge{fptr, i});

    if (sVec.size() > 0) {
        sd.intersectionOpt = Intersection(sVec);
        sd.intersectionOpt.erase(std::remove_if(sd.intersectionOpt.begin(), sd.intersectionOpt.end(), FixedPair), sd.intersectionOpt.end());
        if (sd.intersectionOpt.size() > 0) {
            return FAIL_GLOBAL_OVERLAP_AFTER_OPT;
        }
    }

    // also ensure the optimization border does not intersect the border of the fixed area
    // note that this check is not suficient, we should make sure that the optimization AREA
    // does not intersect with the non-optimized area. This check should be done either with
    // rasterization or triangle intersections
    std::vector<HalfEdge> nopVecBorder;
    for (auto fptr : sd.a->fpVec)
        if (sd.optimizationArea.find(fptr) == sd.optimizationArea.end())
            for (int i = 0; i < 3; ++i)
                if (face::IsBorder(*fptr, i) /* || (sd.optimizationArea.find(fptr->FFp(i)) != sd.optimizationArea.end()) */)
                    nopVecBorder.push_back(HalfEdge{fptr, i});

    if (sd.a != sd.b) {
        for (auto fptr : sd.b->fpVec)
            if (sd.optimizationArea.find(fptr) == sd.optimizationArea.end())
                for (int i = 0; i < 3; ++i)
                    if (face::IsBorder(*fptr, i) /* || (sd.optimizationArea.find(fptr->FFp(i)) != sd.optimizationArea.end()) */)
                        nopVecBorder.push_back(HalfEdge{fptr, i});
    }

    if (sVec.size() > 0 && nopVecBorder.size() > 0) {
        sd.intersectionBoundary = CrossIntersection(sVec, nopVecBorder);
        sd.intersectionBoundary.erase(std::remove_if(sd.intersectionBoundary.begin(), sd.intersectionBoundary.end(), FixedFirst), sd.intersectionBoundary.end());
        if (sd.intersectionBoundary.size() > 0) {
            return FAIL_GLOBAL_OVERLAP_AFTER_BND;
        }
    }

    // also ensure that the optimization border does not overlap any internal edge (inside or outside the optimization area)
    // to speed things up, only check edges that are inside the bbox of the opt area
    vcg::Box2d optBox;
    for (auto fptr : sd.optimizationArea)
        for (int i = 0; i < 3; ++i)
            optBox.Add(fptr->V(i)->T().P());

    std::vector<HalfEdge> internal = ExtractHalfEdges({sd.a, sd.b}, optBox, true); // internal only

    if (sVec.size() > 0 && internal.size() > 0) {
        sd.intersectionInternal = CrossIntersection(sVec, internal);
        sd.intersectionInternal.erase(std::remove_if(sd.intersectionInternal.begin(), sd.intersectionInternal.end(), FixedFirst), sd.intersectionInternal.end());
        if (sd.intersectionInternal.size() > 0) {
            return FAIL_GLOBAL_OVERLAP_AFTER_BND;
        }
    }

    vcg::Box2d box;
    for (auto fptr : sd.b->fpVec)
        for (int i = 0; i < 3; ++i)
            box.Add(fptr->V(i)->T().P());

    // also check if the edges of b overlap the edges of a (only check the edges inside the bbox of b)
    std::vector<HalfEdge> aVec = ExtractHalfEdges({sd.a}, box, false);
    std::vector<HalfEdge> bVec = ExtractHalfEdges({sd.b}, box, false);
    if ((aVec.size() > 0) && (bVec.size() > 0)) {
        std::vector<HalfEdgePair> heVec = CrossIntersection(aVec, bVec);
        if (heVec.size() > 0)
            return FAIL_GLOBAL_OVERLAP_UNFIXABLE;
    }

    return PASS;
}

static CheckStatus CheckAfterLocalOptimization(SeamData& sd, AlgoStateHandle state, const AlgoParameters& params)
{
    PERF_TIMER_START;
    LOG_DEBUG << "Running CheckAfterLocalOptimization()";
    CheckStatus status = CheckAfterLocalOptimizationInner(sd, state, params);
    PERF_TIMER_ACCUMULATE(t_check_after);
    return status;
}

static CheckStatus OptimizeChart(SeamData& sd, GraphHandle graph, bool fixIntersectingEdges)
{
    PERF_TIMER_START;

    // Create a support face group that contains only the faces that must be
    // optimized with arap, i.e. the faces that have at least one vertex in vset
    // also initialize the tex coords to the stored tex coords
    // (needed if this is called more than once)
    auto itV = sd.texcoordoptVert.begin();
    auto itW = sd.texcoordoptWedge.begin();
    FaceGroup support(graph->mesh, INVALID_ID);
    for (auto fptr : sd.optimizationArea) {
        support.AddFace(fptr);
        fptr->V(0)->T().P() = *itV++; fptr->WT(0).P() = *itW++;
        fptr->V(1)->T().P() = *itV++; fptr->WT(1).P() = *itW++;
        fptr->V(2)->T().P() = *itV++; fptr->WT(2).P() = *itW++;
    }

    // before updating the wedge tex coords, compute the arap contribution of the optimization area
    // WARNING: it is critial that at this point the wedge tex coords HAVE NOT YET BEEN UPDATED
    ARAP::ComputeEnergyFromStoredWedgeTC(support.fpVec, graph->mesh, &sd.inputArapNum, &sd.inputArapDenom);

    WedgeTexFromVertexTex(sd.a);
    if (sd.a != sd.b)
        WedgeTexFromVertexTex(sd.b);

    LOG_DEBUG << "Building shell...";

    sd.shell.Clear();
    sd.shell.ClearAttributes();
    bool singleComponent = BuildShellWithTargetsFromUV(sd.shell, support, 1.0);

    if (!singleComponent)
        LOG_DEBUG << "Shell is not single component";

    // Use the existing texture coordinates as starting point for ARAP

    // Copy the wedge texcoords
    for (unsigned i = 0; i < support.FN(); ++i) {
        auto& sf = sd.shell.face[i];
        auto& f = *(support.fpVec[i]);
        for (int j = 0; j < 3; ++j) {
            sf.WT(j) = f.V(j)->T();
            sf.V(j)->T() = sf.WT(j);
        }
    }

    // split non manifold vertices
    while (tri::Clean<Mesh>::SplitNonManifoldVertex(sd.shell, 0.3))
        ;
    ensure(tri::Clean<Mesh>::CountNonManifoldEdgeFF(sd.shell) == 0);

    CutAlongSeams(sd.shell);

    int nholes = tri::Clean<Mesh>::CountHoles(sd.shell);
    int genus = tri::Clean<Mesh>::MeshGenus(sd.shell);
    if (nholes == 0 || genus != 0) {
        return FAIL_TOPOLOGY;
    }

    if (singleComponent && nholes > 1)
        CloseHoles3D(sd.shell);

    SyncShellWithUV(sd.shell);

    PERF_TIMER_ACCUMULATE(t_optimize_build);

    LOG_DEBUG << "Optimizing...";
    ARAP arap(sd.shell);
    arap.SetMaxIterations(100);

    // select the vertices, using the fact that the faces are mirrored in
    // the support object

    for (unsigned i = 0; i < support.FN(); ++i) {
        for (int j = 0; j < 3; ++j) {
            if (sd.verticesWithinThreshold.find(support.fpVec[i]->V(j)) == sd.verticesWithinThreshold.end()) {
                ensure(sd.shell.face[i].IsHoleFilling() == false);
                sd.shell.face[i].V(j)->SetS();
            }
        }
    }

    if (fixIntersectingEdges) {
        unsigned fixedBefore = sd.fixedVerticesFromIntersectingEdges.size();
        for (auto hep : sd.intersectionOpt) {
            sd.fixedVerticesFromIntersectingEdges.insert(hep.first.fp->V0(hep.first.e));
            sd.fixedVerticesFromIntersectingEdges.insert(hep.first.fp->V1(hep.first.e));
            sd.fixedVerticesFromIntersectingEdges.insert(hep.second.fp->V0(hep.second.e));
            sd.fixedVerticesFromIntersectingEdges.insert(hep.second.fp->V1(hep.second.e));
        }
        for (auto hep : sd.intersectionBoundary) {
            HalfEdge he = hep.first;
            sd.fixedVerticesFromIntersectingEdges.insert(he.fp->V0(he.e));
            sd.fixedVerticesFromIntersectingEdges.insert(he.fp->V1(he.e));
        }
        for (auto hep : sd.intersectionInternal) {
            HalfEdge he = hep.first;
            sd.fixedVerticesFromIntersectingEdges.insert(he.fp->V0(he.e));
            sd.fixedVerticesFromIntersectingEdges.insert(he.fp->V1(he.e));
        }
        if (fixedBefore == sd.fixedVerticesFromIntersectingEdges.size())
            return _END;

        for (unsigned i = 0; i < support.FN(); ++i) {
            for (int j = 0; j < 3; ++j) {
                if (sd.fixedVerticesFromIntersectingEdges.find(support.fpVec[i]->V(j)) != sd.fixedVerticesFromIntersectingEdges.end())
                    sd.shell.face[i].V(j)->SetS();
            }
        }
    }

    // Fix the selected vertices of the shell;
    int nfixed = arap.FixSelectedVertices();
    LOG_DEBUG << "Fixed " << nfixed << " vertices";
    double tol = 0.02;
    while (nfixed < 2) {
        LOG_DEBUG << "Not enough selected vertices found, fixing random edge with tolerance " << tol;
        nfixed += arap.FixRandomEdgeWithinTolerance(tol);
        tol += 0.02;
    }
    ensure(nfixed > 0);

    LOG_DEBUG << "Solving...";
    sd.si = arap.Solve();

    PERF_TIMER_ACCUMULATE_FROM_PREVIOUS(t_optimize_arap);

    SyncShellWithUV(sd.shell);

    LOG_DEBUG << "Syncing chart...";

    ensure(HasFaceIndexAttribute(sd.shell));
    auto ia = GetFaceIndexAttribute(sd.shell);
    for (auto& sf : sd.shell.face) {
        if (!sf.IsHoleFilling()) {
            auto& f = (graph->mesh).face[ia[sf]];
            for (int k = 0; k < 3; ++k) {
                f.WT(k).P() = sf.V(k)->T().P();
                f.V(k)->T().P() = sf.V(k)->T().P();
            }
        }
    }

    if (!sd.si.numericalError)
        ARAP::ComputeEnergyFromStoredWedgeTC(support.fpVec, graph->mesh, &sd.outputArapNum, &sd.outputArapDenom);

    PERF_TIMER_ACCUMULATE(t_optimize);

    return sd.si.numericalError ? FAIL_NUMERICAL_ERROR : PASS;
}

static bool SeamInterceptsOptimizationArea(ClusteredSeamHandle csh, const SeamData& sd)
{
    const SeamMesh& sm = csh->sm;
    for (auto sh : csh->seams) {
        for (int i : sh->edges) {
            const SeamEdge& edge = sm.edge[i];
            if ((sd.optimizationArea.find(edge.fa) != sd.optimizationArea.end()) || (sd.optimizationArea.find(edge.fb) != sd.optimizationArea.end()))
                return true;
        }
    }
    return false;
}

static void AcceptMove(const SeamData& sd, AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params)
{
    PERF_TIMER_START;

    if (min_energy > sd.si.finalEnergy)
        min_energy = sd.si.finalEnergy;
    if (max_energy < sd.si.finalEnergy)
        max_energy = sd.si.finalEnergy;

    state->changeSet.insert(sd.optimizationArea.begin(), sd.optimizationArea.end());

    std::vector<SeamHandle> shared;
    std::set<ClusteredSeamHandle> sharedClusters; // clusters that can be aggregated after the merge
    std::set<ClusteredSeamHandle> independentClusters; // clusters not directly impacted by the merge

    std::set<ClusteredSeamHandle> selfClusters;

    if (sd.a != sd.b) {
        // ``disjoint'' seams, i.e. seams between B and C with C not in N(a)
        // are inherited by A
        for (auto csh : state->chartSeamMap[sd.b->id]) {
            ChartPair p = GetCharts(csh, graph);
            ChartHandle c = (p.first == sd.b) ? p.second : p.first;
            if (c == sd.a || c == sd.b) {
                selfClusters.insert(csh);
            } else if (sd.a->adj.find(c) == sd.a->adj.end()) {
                independentClusters.insert(csh);
            } else {
                ensure(c->adj.find(sd.a) != c->adj.end());
                ensure(c->adj.find(sd.b) != c->adj.end());
                ensure(sharedClusters.count(csh) == 0);
                sharedClusters.insert(csh);
                for (auto sh : csh->seams)
                    shared.push_back(sh);
            }
        }

        // we also need to recompute the cost of seams between A and C with C not in N(b)
        for (auto csh : state->chartSeamMap[sd.a->id]) {
            ChartPair p = GetCharts(csh, graph);
            ChartHandle c = (p.first == sd.a) ? p.second : p.first;
            if (c == sd.a || c == sd.b) {
                selfClusters.insert(csh);
            } else if (sd.b->adj.find(c) == sd.b->adj.end()) {
                independentClusters.insert(csh);
            } else {
                ensure(c->adj.find(sd.a) != c->adj.end());
                ensure(c->adj.find(sd.b) != c->adj.end());
                ensure(sharedClusters.count(csh) == 0);
                sharedClusters.insert(csh);
                for (auto sh : csh->seams)
                    shared.push_back(sh);
            }
        }

        /*
        for (auto x : std::set<ChartHandle>{sd.a, sd.b}) {
            for (auto csh : state->chartSeamMap[x->id]) {
                ChartPair p = GetCharts(csh, graph);
                ChartHandle c = (p.first == x) ? p.second : p.first;
                if ((sd.a->adj.find(c) != sd.a->adj.end()) && (sd.b->adj.find(c) != sd.b->adj.end())) {
                    ensure(c != sd.a);
                    ensure(c != sd.b);
                    ensure(sharedClusters.count(csh) == 0);
                    sharedClusters.insert(csh);
                    for (auto sh : csh->seams)
                        shared.push_back(sh);
                }
            }
        }
        */

        // update the MeshGraph object
        for (auto fptr : sd.b->fpVec)
            fptr->id = sd.a->Fp()->id;
        sd.a->fpVec.insert(sd.a->fpVec.end(), sd.b->fpVec.begin(), sd.b->fpVec.end());

        sd.a->adj.erase(sd.b);
        for (auto c : sd.b->adj) {
            if (c != sd.a) { // chart a is now (if it wasn't already) adjacent to c
                c->adj.erase(sd.b);
                c->adj.insert(sd.a);
                sd.a->adj.insert(c);
            }
        }
        graph->charts.erase(sd.b->id);

        // update state
        state->chartSeamMap.erase(sd.b->id);
        std::set<RegionID>& failed_b = state->failed[sd.b->id];
        state->failed[sd.a->id].insert(failed_b.begin(), failed_b.end());
        state->failed.erase(sd.b->id);
    } else {
        // if removing a non-disconnecting seam then all the clusters are independent
        independentClusters.insert(state->chartSeamMap[sd.b->id].begin(), state->chartSeamMap[sd.b->id].end());
        independentClusters.erase(sd.csh);
    }

    // invalidate cache
    sd.a->ParameterizationChanged();

    // update current UV border length
    double deltaUVBorderLength = sd.a->BorderUV() - sd.inputUVBorderLength;
    state->currentUVBorderLength += deltaUVBorderLength;

    // update atlas energy
    state->arapNum += (sd.outputArapNum - sd.inputArapNum);
    state->arapDenom += (sd.outputArapDenom - sd.inputArapDenom);

    if (state->failed[sd.a->id].count(sd.b->id) > 0)
        retry_success++;

    // Erase seam
    EraseSeam(sd.csh, state, graph);
    state->penalty.erase(sd.csh);

    for (auto csh : independentClusters) {
        auto it = state->status.find(csh);
        ensure(it != state->status.end());

        CheckStatus clusterStatus = it->second;
        ensure(clusterStatus != PASS);

        CostInfo::MatchingValue mv = state->mvalue[csh];

        EraseSeam(csh, state, graph);

        bool invalidate = (clusterStatus == CheckStatus::FAIL_GLOBAL_OVERLAP_BEFORE)
                || (clusterStatus == CheckStatus::FAIL_GLOBAL_OVERLAP_AFTER_OPT)
                || (clusterStatus == CheckStatus::FAIL_GLOBAL_OVERLAP_AFTER_BND)
                || (clusterStatus == CheckStatus::FAIL_GLOBAL_OVERLAP_UNFIXABLE && !SeamInterceptsOptimizationArea(csh, sd))
                || (clusterStatus == CheckStatus::FAIL_TOPOLOGY);

        if (invalidate || (params.ignoreOnReject && mv == CostInfo::REJECTED))
            InvalidateCluster(csh, state, graph, clusterStatus, 1.0);
        else
            InsertNewClusterInQueue(csh, state, graph, params);
    }

    for (auto csh : sharedClusters)
        EraseSeam(csh, state, graph);

    std::vector<ClusteredSeamHandle> cshvec = ClusterSeamsByChartId(shared);
    for (auto csh : cshvec) {
        InsertNewClusterInQueue(csh, state, graph, params);
    }

    if (params.visitComponents) {
        // if potential islands are allowed to ignore the boundary length limit,
        // then check if any chart adjacent to a or b becomes a potential island
        // after the merge and activate the corresponding seams below threshold

        std::set<ClusteredSeamHandle> unfeasibleBoundaryAdj;
        for (ChartHandle c : sd.a->adj)
            for (auto csh : state->chartSeamMap[c->id])
                if (state->mvalue[csh] == CostInfo::MatchingValue::UNFEASIBLE_BOUNDARY)
                    unfeasibleBoundaryAdj.insert(csh);

        for (ClusteredSeamHandle csh : unfeasibleBoundaryAdj) {
            EraseSeam(csh, state, graph);
            InsertNewClusterInQueue(csh, state, graph, params);
        }
    }

    PERF_TIMER_ACCUMULATE(t_accept);
}

static void RejectMove(const SeamData& sd, AlgoStateHandle state, GraphHandle graph, CheckStatus status)
{
    PERF_TIMER_START;

    Mesh& m = graph->mesh;

    // restore texture coordinates and indices
    RestoreChartAttributes(sd.a, m, sd.vertexinda.begin(), sd.texcoorda.begin());
    if (sd.a != sd.b)
        RestoreChartAttributes(sd.b, m, sd.vertexindb.begin(), sd.texcoordb.begin());

    // restore face-face topology
    SeamMesh& seamMesh = sd.csh->sm;
    for (SeamHandle sh : sd.csh->seams) {
        for (int iedge : sh->edges) {
            const SeamEdge& edge = seamMesh.edge[iedge];
            edge.fa->FFp(edge.ea) = edge.fa;
            edge.fa->FFi(edge.ea) = edge.ea;
            edge.fb->FFp(edge.eb) = edge.fb;
            edge.fb->FFi(edge.eb) = edge.eb;
        }
    }

    // restore vertex-face topology
    // iterate over emap, and split the lists according to the original topology
    // recall that we never touched any vertex topology attribute
    {
        for (Mesh::VertexPointer vp : sd.seamVertices) {
            vp->VFp() = 0;
            vp->VFi() = 0;
        }

        for (Mesh::FacePointer fptr : sd.vfTopologyFaceSet) {
            for (int i = 0; i < 3; ++i) {
                if (sd.seamVertices.find(fptr->V(i)) != sd.seamVertices.end()) {
                    (*fptr).VFp(i) = (*fptr).V(i)->VFp();
                    (*fptr).VFi(i) = (*fptr).V(i)->VFi();
                    (*fptr).V(i)->VFp() = &(*fptr);
                    (*fptr).V(i)->VFi() = i;
                }
            }
        }
    }

    EraseSeam(sd.csh, state, graph);

    InvalidateCluster(sd.csh, state, graph, status, PENALTY_MULTIPLIER);
    if (sd.a != sd.b)
        state->failed[sd.a->id].insert(sd.b->id);

    PERF_TIMER_ACCUMULATE(t_reject);
}

static void EraseSeam(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph)
{
    ensure(csh->size() > 0);

    std::size_t n = state->cost.erase(csh);
    ensure(n > 0);

    n = state->transform.erase(csh);
    ensure(n > 0);

    n = state->status.erase(csh);
    ensure(n > 0);

    n = state->mvalue.erase(csh);
    ensure(n > 0);

    ChartPair charts = GetCharts(csh, graph);

    // the following check are needed because AcceptMove() may erase seams after
    // fiddling with the chartSeamMap...
    if (state->chartSeamMap.find(charts.first->id) != state->chartSeamMap.end())
        state->chartSeamMap[charts.first->id].erase(csh);

    if (state->chartSeamMap.find(charts.second->id) != state->chartSeamMap.end())
        state->chartSeamMap[charts.second->id].erase(csh);

    // erase seam from endpoint map
    std::set<int> endpoints = GetEndpoints(csh);
    for (auto vi : endpoints) {
        unsigned n = state->emap[vi].erase(csh);
        ensure(n > 0);
    }
}

static void InvalidateCluster(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph, CheckStatus status, double penaltyMultiplier)
{
    ColorizeSeam(csh, statusColor[status]);

    CostInfo ci;
    ci.cost = Infinity();
    ci.mvalue = CostInfo::REJECTED;
    ci.matching = MatchingTransform::Identity();

    state->queue.push(std::make_pair(csh, ci.cost));
    state->cost[csh] = Infinity();
    state->transform[csh] = ci.matching;
    state->status[csh] = status;
    state->mvalue[csh] = ci.mvalue;

    ChartPair p = GetCharts(csh, graph);
    state->chartSeamMap[p.first->id].insert(csh);
    state->chartSeamMap[p.second->id].insert(csh);

    // add penalty if the cluster is later re-evaluated
    double penalty = GetPenalty(csh, state);
    state->penalty[csh] = penalty * penaltyMultiplier;

    // add cluster to endpoint map
    std::set<int> endpoints = GetEndpoints(csh);
    for (auto vi : endpoints)
        state->emap[vi].insert(csh);
}

static void RestoreChartAttributes(ChartHandle c, Mesh& m, std::vector<int>::const_iterator itvi,  std::vector<vcg::Point2d>::const_iterator ittc)
{
    for (auto fptr : c->fpVec) {
        for (int i = 0; i < 3; ++i) {
            fptr->V(i) = &m.vert[*itvi++];
            fptr->V(i)->T().P() = *ittc;
            fptr->WT(i).P() = *ittc++;
        }
    }
}

static CostInfo ReduceSeam(ClusteredSeamHandle csh, AlgoStateHandle state, GraphHandle graph, const AlgoParameters& params)
{
    ClusteredSeamHandle reduced = nullptr;

    double totlen = ComputeSeamLength3D(csh);

    SeamMesh& sm = csh->sm;

    // reduce ``forward''
    ClusteredSeamHandle fwd, bwd;
    {
        fwd = std::make_shared<ClusteredSeam>(sm);
        double lenfwd = 0;
        auto seamHandleIt = csh->seams.begin();
        while (lenfwd < params.reductionFactor * totlen && seamHandleIt != csh->seams.end()) {
            SeamHandle sh  = *seamHandleIt;
            SeamHandle shnew = std::make_shared<Seam>(sm);

            std::map<SeamMesh::VertexPointer, int> visited;
            for (int e : sh->edges) {
                if (lenfwd >= params.reductionFactor * totlen)
                    break;
                shnew->edges.push_back(e);
                visited[sm.edge[e].V(0)]++;
                visited[sm.edge[e].V(1)]++;
                lenfwd += (sm.edge[e].P(0) - sm.edge[e].P(1)).Norm();
            }

            if (shnew->edges.size() == sh->edges.size()) {
                shnew->endpoints = sh->endpoints;
            } else {
                for (auto& entry : visited) {
                    if (entry.second == 1) {
                        shnew->endpoints.push_back(tri::Index(sm, entry.first));
                    }
                }
                if (tri::Index(sm, sm.edge[shnew->edges.front()].V(0)) != (unsigned) shnew->endpoints.front()
                        && tri::Index(sm, sm.edge[shnew->edges.front()].V(1)) != (unsigned) shnew->endpoints.front()) {
                    std::reverse(shnew->endpoints.begin(), shnew->endpoints.end());
                }
            }

            fwd->seams.push_back(shnew);
        }
    }

    // reduce ``backward''
    {
        bwd = std::make_shared<ClusteredSeam>(sm);
        double lenbwd = 0;
        auto seamHandleIt = csh->seams.rbegin();
        while (lenbwd < params.reductionFactor * totlen && seamHandleIt != csh->seams.rend()) {
            SeamHandle sh  = *seamHandleIt;
            SeamHandle shnew = std::make_shared<Seam>(sm);

            std::map<SeamMesh::VertexPointer, int> visited;
            for (auto ei = sh->edges.rbegin(); ei != sh->edges.rend(); ++ei) {
                if (lenbwd >= params.reductionFactor * totlen)
                    break;
                shnew->edges.push_back(*ei);
                visited[sm.edge[*ei].V(0)]++;
                visited[sm.edge[*ei].V(1)]++;
                lenbwd += (sm.edge[*ei].P(0) - sm.edge[*ei].P(1)).Norm();
            }
            std::reverse(shnew->edges.begin(), shnew->edges.end());

            if (shnew->edges.size() == sh->edges.size()) {
                shnew->endpoints = sh->endpoints;
            } else {
                for (auto& entry : visited) {
                    if (entry.second == 1) {
                        shnew->endpoints.push_back(tri::Index(sm, entry.first));
                    }
                }
                if (tri::Index(sm, sm.edge[shnew->edges.front()].V(0)) != (unsigned) shnew->endpoints.front()
                        && tri::Index(sm, sm.edge[shnew->edges.front()].V(1)) != (unsigned) shnew->endpoints.front()) {
                    std::reverse(shnew->endpoints.begin(), shnew->endpoints.end());
                }
            }

            bwd->seams.push_back(shnew);
        }
        std::reverse(bwd->seams.begin(), bwd->seams.end());
    }

    CostInfo cfwd = ComputeCost(fwd, graph, params, GetPenalty(csh, state));
    CostInfo cbwd = ComputeCost(bwd, graph, params, GetPenalty(csh, state));

    if (cfwd.cost < cbwd.cost) {
        csh->seams = fwd->seams;
        return cfwd;
    } else {
        csh->seams = bwd->seams;
        return cbwd;
    }
}
