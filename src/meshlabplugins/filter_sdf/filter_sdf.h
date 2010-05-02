#ifndef COMPUTESDFPLUGIN_H
#define COMPUTESDFPLUGIN_H
#include <filterinterface.h>
#include "calculatesdf.h"

class SdfPlugin : public SingleMeshFilterInterface{
    Q_OBJECT
    Q_INTERFACES(MeshFilterInterface)

public:
    SdfPlugin();
    bool applyFilter(MeshDocument&, RichParameterSet &, vcg::CallBackPos*);
};
#endif
