#ifndef edit_topoDIALOG_H
#define edit_topoDIALOG_H



#include <QWidget>

#include <QtGui/QApplication>
#include <meshlab/interfaces.h>

#include "ui_edit_topodialog.h"

using namespace std;
using namespace vcg;

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


struct Vtx
{
	Point3f V;
	QString vName;

    inline bool operator == (const Vtx &b) const
    {
		return ((V==b.V) && (vName==b.vName));
    }

    inline bool operator != (const Vtx &b) const
    {
		return ((V!=b.V) || (vName!=b.vName));
    }
};

struct Edg
{
	Vtx v[2];

    inline bool operator == (const Edg &b) const
    {
		return (((v[0]==b.v[0]) && (v[1]==b.v[1]))
				||((v[1]==b.v[0]) && (v[0]==b.v[1])));
    }

    inline bool operator != (const Edg &b) const
    {
		return ( ((v[0] != b.v[0])&&(v[0] != b.v[1])) 
					||	((v[1] != b.v[0])&&(v[1] != b.v[1])) );
	}

	inline bool containsVtx(const Vtx &vt) const
	{
		bool toRet = false;
			for(int j=0; j<2; j++)
				if(v[j] == vt)
					toRet = true;

		return toRet;
	}
};

struct Fce
{
	Edg e[3];

	bool selected;

    inline bool operator == (const Fce &f) const
    {
		return (containsEdg(f.e[0]) && containsEdg(f.e[1]) && containsEdg(f.e[2]));
	}

    inline bool operator != (const Fce &f) const
    {
		return (!(containsEdg(f.e[0]) && containsEdg(f.e[1]) && containsEdg(f.e[2])));
	}

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




class edit_topodialog : public QWidget
{
	Q_OBJECT
		
	public:
		edit_topodialog(QWidget *parent = 0);
		~edit_topodialog();
			
		UtensType utensil;

		void updateVtxTable(QList<Vtx> list);
		void updateEdgTable(QList<Edg> list);
		void updateFceTable(QList<Fce> list);

		int getIterations();
		float dist();

		bool isDEBUG();

		void setBarMax(int val);
		void setBarVal(int val);

	private:
		Ui::edit_topodialog ui;
	//	GLArea* parent;
	
	// Events
	private slots:
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

