/*  A class representing a set of points and the format
 * that we will be storing them
 * 
 * 
 * @author Oscar Barney
 */


#ifndef PICKED_POINTS_H
#define PICKED_POINTS_H

#include <QString>
#include <QMap>

#include <meshlab/meshmodel.h>

//our points
class PickedPoint
{
public:
	PickedPoint(QString _name, vcg::Point3f _point){
		name = _name;
		point = _point;
	}
	
	//name of point
	QString name;  
	
	//tells us whether to use this or not in a calculation
	//bool active;

	//point
	vcg::Point3f point;
};

class PickedPoints
{
public:
	
	PickedPoints();
	
	~PickedPoints();
	
	//opens a file containing the picked points
	bool open(QString filename);
	
	//save 
	bool save(QString filename);

	//add a point to the map
	void addPoint(QString name, vcg::Point3f point);
	
	std::vector<PickedPoint*> * getPickedPointVector();
	
	std::vector<vcg::Point3f> * getPoint3fVector();
	
	//translate each point using the matrix
	//if the mesh moves you can then translate the points useing this function
	void translatePoints(vcg::Matrix44f &translation);
	
	//get the suggested filename for the points.  will be based on the mesh's filename
	static QString getSuggestedPickedPointsFileName(const MeshModel &meshModel);
	
	//extension of the filetype for Picked Points
	static const QString fileExtension;
	
	//for use with per mesh attributes
	static const std::string Key;
	
private:
	
	//data
	std::vector<PickedPoint*> *pointVector;
	
	//Declare Constants
	
	//root name of the xml document
	static const QString rootName;
	
	//point element's name
	static const QString pointElementName;
	
	//point's name
	static const QString pointName;
	
	//point's x coord
	static const QString xCoordinate;
	
	//point's y coord
	static const QString yCoordinate;
	
	//points z coord
	static const QString zCoordinate;
	

};

#endif
