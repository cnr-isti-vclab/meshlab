#ifndef QUADTREENODE_H_
#define QUADTREENODE_H_

#include <QList>

#include "QuadTreeLeaf.h"

#define MAX_LEAFS 20
#define MAX_DEPTH 20
class QuadTreeNode{
	
private:
	double qx,qy,qw,qh;
	QList<QuadTreeLeaf*> *qleafs;
	QuadTreeNode* qchildren[4];	
	bool endOfTree;
public:
	QuadTreeNode(double x, double y,double w, double h);
	
	~QuadTreeNode();
	
	void buildQuadTree(QList<QuadTreeLeaf*> *list, double min_width, double min_height);
	
	void getLeafs(double x, double y, QList<QuadTreeLeaf*> &list);
	
private: 
	void buildQuadTree(QList<QuadTreeLeaf*> *list, double min_width, double min_height, int max_leafs, int max_depth);
};

#endif /*QUADTREENODE_H_*/
