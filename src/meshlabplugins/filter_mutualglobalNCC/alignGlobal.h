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
/****************************************************************************
  History
$Log: sampleplugins.h,v $
Revision 1.2  2006/11/29 00:59:21  cignoni
Cleaned plugins interface; changed useless help class into a plain string

Revision 1.1  2006/09/25 09:24:39  e_cerisoli
add sampleplugins

****************************************************************************/



#include <common/interfaces.h>


class AlignPair
{
public: 

	AlignPair() {imageId=0; projId=0, meshId=0, mutual=0.0; area=0.0; valid=false; }
	~AlignPair() {};

	int imageId;
	int projId;
	int meshId;

	float mutual;
	float area;
	float weight;

	bool valid;

	bool CreatePair(int imId, int prId, int mId, float mut, float ar, bool val) {imageId=imId; projId=prId, meshId=mId, mutual=mut; area=ar; valid=val; }

	


};

struct ordering {
    bool operator ()(AlignPair a, AlignPair b) {
		return a.area > b.area;
    }
};

struct orderingW {
    bool operator ()(AlignPair a, AlignPair b) {
		return (a.area*a.mutual) > (b.area*b.mutual);
    }
};


class Node
{
public: 

	Node() {active=false; id=0; }
	~Node() {}


	bool active;
	bool assigned;
	int id;
	int grNum;
	double avMut;
	std::vector<AlignPair> arcs;

};

class SubGraph
{
public: 

	int id;
	std::vector<Node> nodes;

};

//std::vector<SubGraph*> graphs;

