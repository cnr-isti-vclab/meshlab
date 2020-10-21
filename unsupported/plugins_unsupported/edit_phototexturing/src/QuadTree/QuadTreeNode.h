/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
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
	
//private: 
	void buildQuadTree(QList<QuadTreeLeaf*> *list, double min_width, double min_height, int max_leafs, int max_depth);
};

#endif /*QUADTREENODE_H_*/
