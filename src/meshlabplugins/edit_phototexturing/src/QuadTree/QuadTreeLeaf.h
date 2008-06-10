#ifndef QUADTREELEAF_H_
#define QUADTREELEAF_H_

class QuadTreeLeaf{
public:
	virtual bool isInside(double x, double y, double w, double h){
		return false;
	};
	
	virtual bool isInside(double x, double y){
			return false;
	};
};

#endif /*QUADTREELEAF_H_*/
