#include "pointCorrespondence.h"

PointCorrespondence::PointCorrespondence()
{
    pointList = new QList<PointOnLayer>();
    numofItems=0;
}

PointCorrespondence::~PointCorrespondence()
{
    delete pointList;
}

void PointCorrespondence::addPoint(PointOnLayer &newPt)
{
    pointList->append(newPt);
    numofItems++;
}

PointOnLayer PointCorrespondence::getPointAt(int i){
    return pointList->at(i);
}
