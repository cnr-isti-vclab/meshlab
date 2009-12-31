#include "RandomDropFilter.h"

RandomDropFilter::RandomDropFilter(){
}

void RandomDropFilter::initParameterSet(QAction* action,MeshDocument& md, RichParameterSet & par){
    MeshSubFilter::initParameterSet(action, md, par);

    QStringList layers;
    for(int i = 1; i <= md.size(); i++){
        layers.push_back(QString::number(i));
    }
    par.addParam(new RichEnum("layers", 0, layers, "Random object layer", "Select the layer that contains the object that will spawn randomly"));

    par.addParam(new RichFloat("distance", 1.0f, "Random radius interval", "The object will spawn in a random position contained in the specified radius"));
}

bool RandomDropFilter::applyFilter(QAction* filter, MeshDocument &md, RichParameterSet& par, vcg::CallBackPos* cb){
    MeshSubFilter::applyFilter(filter, md, par, cb);

    int currentStep  = (par.getDynamicFloat("timeline") * m_steps) / 100;

    for(int i = 0; i < md.size(); i++)
        md.getMesh(i)->cm.Tr = m_layersTrans[i][currentStep];

    return true;
}

void RandomDropFilter::initialize(MeshDocument&, RichParameterSet&){
}

bool RandomDropFilter::configurationHasChanged(RichParameterSet& par){
    return MeshSubFilter::configurationHasChanged(par);
}
