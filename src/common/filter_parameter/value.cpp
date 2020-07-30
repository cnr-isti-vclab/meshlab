#include "value.h"

#include "meshmodel.h"

MeshValue::MeshValue(MeshDocument* doc, int meshind)
{
    if (doc != nullptr)
        pval = doc->meshList.at(meshind);
    else
        pval = nullptr;
}
