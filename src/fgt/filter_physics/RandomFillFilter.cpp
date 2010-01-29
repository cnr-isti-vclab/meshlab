#include "RandomFillFilter.h"

#include <vcg/complex/trimesh/append.h>
#include <vcg/complex/trimesh/inertia.h>

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
    //par.addParam(new RichInt("fillCounter", 1, "Fill counter", "The number of object used to fill the container"));
    par.addParam(new RichBool("useRandomVertices", true, "Random spawn points", "If true the filling objects will spawn at random positions in the container mesh instead of being spawn at the center of mass"));
    //par.addParam(new RichMesh("randomVertices", 0, &md, "Random vertices mesh", "The filling objects will be positioned at randomly generated vertices of this mesh"));
}

bool RandomFillFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    if(md.size() < 2 || par.getMesh("container") == 0 || par.getMesh("container") == par.getMesh("filler") /*|| par.getInt("fillCounter") < 0*/ || par.getInt("fps") <= 0 || par.getInt("iterations") <= 0 || par.getInt("contacts") <= 0 || par.getFloat("bounciness") < 0.f || par.getFloat("bounciness") > 1.f)
        return false;

    if(cb != 0) (*cb)(0, "Physics renderization of the scene started...");

    MeshModel* container = par.getMesh("container");
    MeshModel* filler = par.getMesh("filler");
    int fillOffset = md.size();

    static float gravity[3] = {0.0f, -9.8f, 0.0f};
    m_engine.clear(md);
    m_engine.setGlobalForce(gravity);
    m_engine.setIterations(par.getInt("iterations"));
    m_engine.setMaxContacts(par.getInt("contacts"));
    m_engine.setBounciness(par.getFloat("bounciness"));
    m_engine.registerTriMesh(*container, true);

    srand((unsigned)time(0));

    tri::Inertia<CMeshO> inertiaContainer, inertiaFiller;
    inertiaContainer.Compute(par.getMesh("container")->cm);
    inertiaFiller.Compute(par.getMesh("filler")->cm);
    int objects = abs(inertiaContainer.Mass()/inertiaFiller.Mass())/2;

    if(par.getBool("useRandomVertices")){
        for(int i = 0; i < objects; i++){
            if(cb != 0) (*cb)(50.f*i/objects, "Computing...");
            addRandomObject(md, filler, getRandomOrigin(par), i);
            m_engine.registerTriMesh(*md.getMesh(fillOffset++));
        }

        for(int j = 0; j < par.getInt("fps"); j++){
            if(cb != 0) (*cb)(50 + 48.f*j/par.getInt("fps"), "Computing...");
            m_engine.integrate(1.0f/par.getInt("fps"));
        }
    }else{
        for(int i = 0; i < objects; i++){
            if(cb != 0) (*cb)(98.f*i/objects, "Computing...");
            addRandomObject(md, filler, inertiaContainer.CenterOfMass(), i);
            m_engine.registerTriMesh(*md.getMesh(fillOffset++));

            for(int j = 0; j < par.getInt("fps")/2; j++)
                m_engine.integrate(1.0f/par.getInt("fps"));
        }
    }

    /*for(int i = 0; i < objects; i++){
        if(cb != 0) (*cb)(50.f*i/objects, "Computing...");

        if(par.getBool("useRandomVertices"))
            addRandomObject(md, filler, getRandomOrigin(par), i);
        else
            addRandomObject(md, filler, inertiaContainer.CenterOfMass(), i);

        m_engine.registerTriMesh(*md.getMesh(fillOffset++));
    }

    for(int j = 0; j < par.getInt("fps"); j++){
        if(cb != 0) (*cb)(50 + 48.f*j/par.getInt("fps"), "Computing...");
        m_engine.integrate(1.0f / par.getInt("fps"));
    }*/

    m_engine.updateTransform();

    if(cb != 0) (*cb)(0, "Physics renderization of the scene completed...");

    return true;
}

vcg::Point3<float> RandomFillFilter::getRandomOrigin(RichParameterSet& par){
    //int randomVertex = static_cast<float>(rand())/RAND_MAX*(par.getMesh("randomVertices")->cm.vert.size() - 1);
    //return par.getMesh("randomVertices")->cm.vert[randomVertex].P();
    int randomFace = static_cast<float>(rand())/RAND_MAX*(par.getMesh("container")->cm.face.size() - 1);
    CFaceO& face = par.getMesh("container")->cm.face[randomFace];
    return face.P(0) + (face.N() * par.getMesh("filler")->cm.bbox.Diag());
}

void RandomFillFilter::addRandomObject(MeshDocument& md, MeshModel* filler, const vcg::Point3<float>& origin, int meshID){
    ostringstream meshName;
    meshName << "randomMesh" << meshID;
    MeshModel* meshCopy = md.addNewMesh(meshName.str().c_str());
    vcg::tri::Append<CMeshO,CMeshO>::Mesh(meshCopy->cm, filler->cm, false, true);
    meshCopy->cm.Tr = filler->cm.Tr;
    meshCopy->cm.Tr.SetColumn(3, origin);
}
