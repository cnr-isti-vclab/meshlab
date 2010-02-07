#include "RandomFillFilter.h"

#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/inertia.h>
#include <vcg/complex/trimesh/clean.h>

#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;
using namespace vcg;

int RandomFillFilter::m_filterType = -1;

RandomFillFilter::RandomFillFilter(){
    MeshSubFilter::m_currentFilterType += 1;
    m_filterType = MeshSubFilter::m_currentFilterType;
}

void RandomFillFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    MeshSubFilter::initParameterSet(action, md, par);
    par.addParam(new RichMesh("container", 0, &md, "Container mesh", "This mesh will act as a container for the filling mesh"));
    par.addParam(new RichMesh("filler", 0, &md, "Filler mesh", "The container mesh will be filled with this mesh"));
    par.addParam(new RichFloat("factor", 0.5f, "Volume ratio factor", "The ratio between the container and the filler object will be multiplied by this factor. The volume ratio determines the number of filling objects to be spawn."));
    par.addParam(new RichFloat("seconds", 1.f, "Simulation interval (sec)", "Physics simulation update interval in seconds"));
    par.addParam(new RichFloat("updateFrequency", 1.f, "Update frequency", "The frequency with which the simulation gets updated in case the objects spawn at the center of mass"));
    par.addParam(new RichBool("flipNormal", false, "Flip container normals", "If true the container normals will be flipped."));
    par.addParam(new RichBool("useRandomVertices", true, "Random spawn points", "If true the filling objects will spawn at random positions in the container mesh instead of being spawn at the center of mass"));
}

bool RandomFillFilter::applyFilter(QAction*, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(parametersAreNotCorrect(md, par))
        return false;

    MeshSubFilter::initialize(md, par, cb);
    if(cb != 0) (*cb)(0, "Physics renderization of the scene started...");

    MeshModel* container = par.getMesh("container");
    MeshModel* filler = par.getMesh("filler");
    int fillOffset = md.size();
    float gravity[3] = {0.0f, par.getBool("useRandomVertices") ? 0.0f : -9.8f, 0.0f};

    if(par.getBool("flipNormal")){
        vcg::tri::Clean<CMeshO>::FlipMesh(container->cm);
        tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(container->cm);
        container->clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    }

    m_engine.clear();
    m_engine.setGlobalForce(gravity);
    m_engine.setIterations(par.getInt("iterations"));
    m_engine.setMaxContacts(par.getInt("contacts"));
    m_engine.setBounciness(par.getFloat("bounciness"));
    m_engine.setFriction(par.getFloat("friction"));
    m_engine.registerTriMesh(*container, true);

    srand((unsigned)time(0));

    vcg::tri::UpdatePosition<CMeshO>::Matrix(filler->cm, filler->cm.Tr);
    filler->cm.Tr.SetIdentity();

    tri::Inertia<CMeshO> inertiaContainer, inertiaFiller;
    inertiaContainer.Compute(par.getMesh("container")->cm);
    inertiaFiller.Compute(par.getMesh("filler")->cm);

    int objects = abs(inertiaContainer.Mass()/inertiaFiller.Mass())*par.getFloat("factor");
    filler->cm.Tr.SetColumn(3, - inertiaFiller.CenterOfMass());

    //Restore old generated meshes
    int restoredMeshes = 0;
    for(int i = 0; i < md.size(); i++){
        if(md.getMesh(i)->fileName.find("randomFillMesh") == 0){
            m_engine.registerTriMesh(*md.getMesh(i));
            restoredMeshes++;
            m_engine.integrate(1.0f/par.getInt("fps"));
        }
    }

    int frequency = 1 / par.getFloat("updateFrequency");

    // To refactor when the right algorithm has be found
    if(par.getBool("useRandomVertices")){
        for(int i = 0; i < objects; i++){
            if(cb != 0) (*cb)(50.f*i/objects, "Computing...");
            addRandomObject(md, filler, getRandomOrigin(par), restoredMeshes + i);
            m_engine.registerTriMesh(*md.getMesh(fillOffset++));
        }
        for(int j = 0; j < par.getFloat("seconds") * par.getInt("fps"); j++){
            if(cb != 0) (*cb)(50 + 48.f*j/(par.getFloat("seconds") * par.getInt("fps")), "Computing...");
            m_engine.integrate(1.0f/par.getInt("fps"));
        }
    }else{
        for(int i = 0; i < objects; i++){
            if(cb != 0) (*cb)(98.f*i/objects, "Computing...");
            addRandomObject(md, filler, inertiaContainer.CenterOfMass(), i);
            m_engine.registerTriMesh(*md.getMesh(fillOffset++));

            if(i % frequency == 0)
                for(int j = 0; j < par.getFloat("seconds") * par.getInt("fps"); j++)
                    m_engine.integrate(1.0f/par.getInt("fps"));
        }
    }

    m_engine.updateTransform();
    filler->cm.Tr.SetIdentity();
    m_currentFilterType = m_filterType;

    if(par.getBool("flipNormal")){
        vcg::tri::Clean<CMeshO>::FlipMesh(container->cm);
        tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFace(container->cm);
        container->clearDataMask(MeshModel::MM_FACEFACETOPO | MeshModel::MM_FACEFLAGBORDER);
    }

    if(cb != 0) (*cb)(99, "Physics renderization of the scene completed...");
    return true;
}

bool RandomFillFilter::parametersAreNotCorrect(MeshDocument& md, RichParameterSet& par){
    return md.size() < 2 || par.getMesh("container") == 0 || par.getMesh("container") == par.getMesh("filler") || par.getInt("fps") <= 0 || par.getInt("iterations") <= 0 || par.getInt("contacts") <= 0 || par.getFloat("bounciness") < 0.f || par.getFloat("bounciness") > 1.f || par.getFloat("factor") < 0.f || par.getFloat("factor") > 1.f;
}

vcg::Point3<float> RandomFillFilter::getRandomOrigin(RichParameterSet& par){
    int randomVertex = float(rand())/RAND_MAX*(par.getMesh("container")->cm.vert.size() - 1);
    CVertexO& vertex = par.getMesh("container")->cm.vert[randomVertex];
    return vertex.P() + (vertex.N() * par.getMesh("filler")->cm.bbox.Diag());
}

void RandomFillFilter::addRandomObject(MeshDocument& md, MeshModel* filler, const vcg::Point3<float>& origin, int meshID){
    ostringstream meshName;
    meshName << "randomFillMesh" << meshID;
    MeshModel* meshCopy = md.addNewMesh(meshName.str().c_str());
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, filler->cm, false, true);
    meshCopy->cm.Tr = filler->cm.Tr;
    meshCopy->cm.Tr.SetColumn(3, meshCopy->cm.Tr.GetColumn3(3) + origin);
}
