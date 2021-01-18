#ifndef POINTCORRESPONDENCE_H
#define POINTCORRESPONDENCE_H

#include <QList>
#include "pointOnLayer.h"

class PointCorrespondence
{

public:

    PointCorrespondence();
    ~PointCorrespondence();

    int	numofItems;

    void addPoint(PointOnLayer &newPt);
    PointOnLayer getPointAt(int i);

private:

    QList<PointOnLayer> *pointList;
};


#endif // POINTCORRESPONDENCE_H
