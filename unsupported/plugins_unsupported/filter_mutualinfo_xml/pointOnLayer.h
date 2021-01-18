#ifndef POINTONLAYER_H
#define POINTONLAYER_H


#include <QString>	//String compare of class names
#include <QWidget>


/** <i></i> <b>Registration Point -- Contains 2d or 3d coordinates</b>

...
*/

enum PointType { _NO_POINT , _2D_POINT , _3D_POINT };

class PointOnLayer
{
public:

    ///Constructor
    PointOnLayer(int id=-1)
    { init(id); pType = _NO_POINT;}
    ///Constructor
    PointOnLayer(int id, double a, double b)
    { init(id); setPoint2D(a,b); }
    ///Constructor
    PointOnLayer(int id, double x, double y, double z)
    { init(id); setPoint3D(x,y,z); }
    ~PointOnLayer(){}
    ///
    void init(int id=-1){

        layerId=id;
    }
    ///
    void setPoint2D(double a, double b)
    {
        pX = a; pY = b; pZ = 0;
        pType = _2D_POINT;
    }
    ///
    void setPoint3D(double x, double y, double z)
    {

        pX = x; pY = y; pZ = z;
        pType = _3D_POINT;
    }
    ///
    int getLayerId(){return layerId;}
    PointType getType(){return pType;}


public:  //Members
    ///
    double pX,pY,pZ;
    ///
    int layerId;
    ///
    PointType pType;
};

#endif // POINTONLAYER_H
