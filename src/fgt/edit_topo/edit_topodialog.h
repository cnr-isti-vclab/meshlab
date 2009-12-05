/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2008                                                \/)\/    *
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

#ifndef edit_topoDIALOG_H
#define edit_topoDIALOG_H



#include <QWidget>

#include <QtGui/QApplication>
#include <common/interfaces.h>

#include "ui_edit_topodialog.h"

using namespace std;
using namespace vcg;


//**************************************************************
//	struct Vtx
//		this simple struct is used to manage user defined
//		vertices stack
//
struct Vtx
{
	Point3f V;
	QString vName;

    inline bool operator == (const Vtx &b) const
    { return ((V==b.V) && (vName==b.vName)); }
    inline bool operator != (const Vtx &b) const
    { return ((V!=b.V) || (vName!=b.vName)); }
};


//**************************************************************
//	struct Edg
//		this simple struct is used to manage user defined
//		edges stack
//
struct Edg
{
	Vtx v[2];

    inline bool operator == (const Edg &b) const
    { return (((v[0]==b.v[0]) && (v[1]==b.v[1]))||((v[1]==b.v[0]) && (v[0]==b.v[1]))); }
    inline bool operator != (const Edg &b) const
    { return ( ((v[0] != b.v[0])&&(v[0] != b.v[1]))||((v[1] != b.v[0])&&(v[1] != b.v[1])) ); }

	inline bool containsVtx(const Vtx &vt) const
	{
		bool toRet = false;
			for(int j=0; j<2; j++)
				if(v[j] == vt)
					toRet = true;
		return toRet;
	}
};

//**************************************************************
//	struct Fce
//		this simple struct is used to manage user defined
//		faces stack
//
struct Fce
{
	Edg e[3];
	bool selected;

    inline bool operator == (const Fce &f) const
    { return (containsEdg(f.e[0]) && containsEdg(f.e[1]) && containsEdg(f.e[2])); }
    inline bool operator != (const Fce &f) const
    { return (!(containsEdg(f.e[0]) && containsEdg(f.e[1]) && containsEdg(f.e[2]))); }

	inline bool containsEdg(const Edg &ed) const
	{
		bool toRet = false;
		for(int i=0; i<3; i++)
			if(e[i]==ed)
				toRet=true;

		return toRet;
	}
	inline bool containsVtx(const Vtx &vt) const
	{
		bool toRet = false;
		for(int i=0; i<3; i++)
			for(int j=0; j<2; j++)
				if(e[i].v[j] == vt)
					toRet = true;

		return toRet;
	}
};

// All ui edit modes
typedef enum 
	{
		U_NONE,
		U_VTX_SEL,
		U_VTX_SEL_FREE,
		U_VTX_DEL,
		U_VTX_CONNECT,
		U_VTX_DE_CONNECT,
		U_EDG_SPLIT,
		U_EDG_COLLAPSE,
		U_FCE_SEL,
		U_DND
	} UtensType;


//**************************************************************
//	class edit_topodialog
//		Qt Ui class
//
class edit_topodialog : public QWidget
{
	Q_OBJECT
		
	public:
		edit_topodialog(QWidget *parent = 0);
		~edit_topodialog();
			
		UtensType utensil;

		// Vertices, edges and faces table update
		void updateVtxTable(QList<Vtx> list);
		void updateEdgTable(QList<Edg> list);
		void updateFceTable(QList<Fce> list);
		
		// Text 
		int getIterations();
		float dist();

		// Checkbox
		bool isDEBUG();
		bool drawLabels();
		bool drawEdges();

		// Status bar
		void setBarMax(int val);
		void setBarVal(int val);
		void setStatusLabel(QString txt);

	private:
		Ui::edit_topodialog ui;

	private slots:
	void on_checkBox_2_stateChanged(int);
	void on_checkBox_3_stateChanged(int);
	void on_checkBox_stateChanged(int);
		void on_ButtonConnectVertex_5_clicked();
		void on_ButtonConnectVertex_4_clicked();
		void on_ButtonConnectVertex_2_clicked();

		void on_ButtonSelectVertex_clicked();
		void on_ButtonSelectVertexFree_clicked();
		void on_ButtonDeSelectVertex_clicked();
		void on_ButtonConnectVertex_clicked();
		void on_ButtonDeConnectVertex_clicked();
		void on_toolBox_currentChanged(int i);

		void on_ButtonSelectFace_clicked();	
		void on_ButtonMeshCreate_clicked();
		void on_ButtonClose_clicked();

	signals:
		void mesh_create();
		void update_request();
};

#endif

