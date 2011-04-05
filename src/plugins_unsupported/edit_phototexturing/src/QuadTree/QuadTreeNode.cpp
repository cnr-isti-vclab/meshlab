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
#include <QtGui>
#include "QuadTreeNode.h"

#define QTN_EPSILON 1e-6
//#define QTN_EPSILON 0.0
QuadTreeNode::QuadTreeNode(double x, double y,double w, double h){
	qx=x;
	qy=y;
	qw=w;
	qh=h;
	endOfTree = false;
	qchildren[0] = NULL;
	qchildren[1] = NULL;
	qchildren[2] = NULL;
	qchildren[3] = NULL;
	qleafs = NULL;
}


QuadTreeNode::~QuadTreeNode(){
	//qDebug() << "QuadTreeNode::~QuadTreeNode()";
	if(qchildren[0]!= NULL){
		delete qchildren[0];
	}
	if(qchildren[1]!= NULL){
		delete qchildren[1];
	}
	if(qchildren[2]!= NULL){
		delete qchildren[2];
	}
	if(qchildren[3]!= NULL){
		delete qchildren[3];
	}

	if (qleafs!=NULL){
		//qleafs->clear();
		//delete qleafs;
	}
}

void QuadTreeNode::buildQuadTree(QList<QuadTreeLeaf*> *list, double min_width, double min_height){
	//qDebug()<< "list->size():"<< list->size() <<"min_width: "<<min_width << "min_height:" <<min_height;
	buildQuadTree(list,min_width, min_height,MAX_LEAFS,MAX_DEPTH);
}
void QuadTreeNode::buildQuadTree(QList<QuadTreeLeaf*> *list, double min_width, double min_height, int max_leafs, int max_depth){
	qleafs = list;
	if(list->size()> max_leafs && max_depth>=0 && qw/2.0 > min_width && qh/2.0>min_height){
		//qDebug()<<"split leafs: "<< list->size() << qx << qy <<qw <<qh;
		endOfTree = false;
		qchildren[0] = new QuadTreeNode(qx,			qy,			qw/2.0,	qh/2.0);
		qchildren[1] = new QuadTreeNode(qx+qw/2.0,	qy,			qw/2.0,	qh/2.0);
		qchildren[2] = new QuadTreeNode(qx+qw/2.0,	qy+qh/2.0,	qw/2.0,	qh/2.0);
		qchildren[3] = new QuadTreeNode(qx,			qy+qh/2.0,	qw/2.0,	qh/2.0);
		//| Q0 | Q1 |
		//-----------
		//| Q3 | Q2 |
		QList<QuadTreeLeaf*> *q0list = new QList<QuadTreeLeaf*>();
		QList<QuadTreeLeaf*> *q1list = new QList<QuadTreeLeaf*>();
		QList<QuadTreeLeaf*> *q2list = new QList<QuadTreeLeaf*>();
		QList<QuadTreeLeaf*> *q3list = new QList<QuadTreeLeaf*>();
		int i;
		QuadTreeLeaf* tmp;
		for (i=0;i<list->size();i++){
			tmp=list->at(i);

			if(tmp->isInside(qchildren[0]->qx,qchildren[0]->qy,qchildren[0]->qw,qchildren[0]->qh)){
				q0list->push_back(tmp);
			}
			if(tmp->isInside(qchildren[1]->qx,qchildren[1]->qy,qchildren[1]->qw,qchildren[1]->qh)){
				q1list->push_back(tmp);
			}
			if(tmp->isInside(qchildren[2]->qx,qchildren[2]->qy,qchildren[2]->qw,qchildren[2]->qh)){
				q2list->push_back(tmp);
			}
			if(tmp->isInside(qchildren[3]->qx,qchildren[3]->qy,qchildren[3]->qw,qchildren[3]->qh)){
				q3list->push_back(tmp);
			}
		}
		qchildren[0]->buildQuadTree(q0list, min_width, min_height, max_leafs,max_depth-1);
		qchildren[1]->buildQuadTree(q1list, min_width, min_height, max_leafs,max_depth-1);
		qchildren[2]->buildQuadTree(q2list, min_width, min_height, max_leafs,max_depth-1);
		qchildren[3]->buildQuadTree(q3list, min_width, min_height, max_leafs,max_depth-1);
	}else{
		endOfTree=true;
		qleafs = list;
		//qDebug() << "leafs: "<< qleafs->size()<< "depth: " << max_depth;
	}
}


void QuadTreeNode::getLeafs(double x, double y,QList <QuadTreeLeaf*> &list){

	if (endOfTree){

		int i;
		QuadTreeLeaf* tmp;
		for(i=0;i<qleafs->size();i++){

			tmp = qleafs->at(i);
			if(tmp->isInside(x,y)){
				list.push_back(tmp);
			}
		}
		//qDebug()<< "qleafs" << qleafs->size() << "list" << list->size();
	}else{
		int i;
		for (i=0;i<4;i++){
			if ((x>=(qchildren[i]->qx) && x<= (qchildren[i]->qx + qchildren[i]->qw) && y>=(qchildren[i]->qy) && y<= (qchildren[i]->qy + qchildren[i]->qh))){
				qchildren[i]->getLeafs(x,y,list);
			}
		}
	}
}

