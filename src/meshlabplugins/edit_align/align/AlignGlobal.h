/****************************************************************************
* VCGLib                                                            o o     *
* Visual and Computer Graphics Library                            o     o   *
*                                                                _   O  _   *
* Copyright(C) 2004                                                \/)\/    *
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

#ifndef __VCG_ALIGNGLOBAL
#define __VCG_ALIGNGLOBAL
#include <list>
#include <queue>
#include <wrap/callback.h>
namespace vcg 
{

class AlignGlobal
{
public:
class Node;

// allineamento virtuale tra due mesh (estratto da un alignresult)
// Nota Importante: la trasformazione e i punti qui memorizzati si intendono al netto delle trasf di base delle due mesh in gioco.
// Quindi se qualcuno sposta una mesh le pos dei punti sono ancora valide ma non la trasf da applicarvi.
class VirtAlign
{
public:
 Node *Fix, *Mov; // allineamento tra i e j
 std::vector<Point3d> FixP; // punti su Fix
 std::vector<Point3d> MovP; // punti su Mov
 std::vector<Point3d> FixN; // Normali su Fix
 std::vector<Point3d> MovN; // Normali su Mov
 Matrix44d M2F; //la matrice da applicare ai punti di Mov per ottenere quelli su Fix
 Matrix44d F2M; //la matrice da applicare ai punti di Fix per ottenere quelli su Mov
/* 
 
  Nel caso semplificato che le mesh avessero come trasf di base l'identita' deve valere: 

                     N2A(N).Apply(   P(N)) ~= AdjP(N)
                     A2N(N).Apply(AdjP(N)) ~=    P(N)

In generale un nodo N qualsiasi dell'VirtAlign vale che:

    N2A(N).Apply(       N->M.Apply(   P(N)) ) ~= AdjN(N)->M.Apply( AdjP(N) );
    A2M(N).Apply( AdjN(N)->M.Apply(AdjP(N)) ) ~=       N->M.Apply(    P(N) );

in cui il ~= significa uguale al netto dell'errore di allineamento.

Per ottenere i virtualmate relativi ad un nodo n:

 */

 Node *Adj(Node *n);

 inline Matrix44d       &N2A(Node *n) {if(n==Fix) return F2M; else return M2F;}
 inline Matrix44d       &A2N(Node *n) {if(n==Fix) return M2F; else return F2M;} 

 inline std::vector<Point3d>    &P(Node *n) {if(n==Fix) return FixP; else return MovP;}
 inline std::vector<Point3d>    &N(Node *n) {if(n==Fix) return FixN; else return MovN;}

 inline std::vector<Point3d> &AdjP(Node *n) {if(n==Fix) return MovP; else return FixP;}
 inline std::vector<Point3d> &AdjN(Node *n) {if(n==Fix) return MovN; else return FixN;}
 bool Check();
};


class Node 
{
public:	
	Node(){id=-1;Active=false;Discarded=false;Queued=false;}
	
	int id;  // id della mesh a cui corrisponde il nodo
	int sid; // Subgraph id;
	Matrix44d M; // La matrice che mette la mesh nella sua posizione di base;
  std::list<VirtAlign *> Adj;

	bool Active;  // true se un nodo e' in Active
		            // false se e' dormant; 
	bool Queued;
	bool Discarded;
	// Allinea un nodo con tutti i suoi vicini
	double AlignWithActiveAdj(bool Rigid);
	double MatrixNorm(Matrix44d &NewM) const;
	double MatrixBoxNorm(Matrix44d &NewM,Box3d &bb) const;
  int PushBackActiveAdj(std::queue<Node *>	&Q);
	int DormantAdjNum();
	int ActiveAdjNum();
};
// classe ausiliaria per memorizzare le componenti connesse del grafo
class SubGraphInfo
{
public:
	int sid;
	int size;
	Node *root;
};

	Node *ChooseDormantWithMostDormantLink ();
	Node *ChooseDormantWithMostActiveLink  ();
  void MakeAllDormant();
  void Clear();
  bool GlobalAlign(const std::vector<std::string> &Names, 	const double epsilon, int maxiter, bool Rigid, FILE *elfp=0, CallBack * cb=DummyCallBack );

	bool CheckGraph();

// Data members:
	
  std::list<Node> N;
	std::list<VirtAlign *> A;
	std::list<SubGraphInfo> CC; // Descrittori delle componenti connesse, riempito dalla ComputeConnectedComponents


	int DormantNum();
	int ActiveNum();
	void Dump(FILE *fp);

	int ComputeConnectedComponents();

/******************************/
/******************************/
  void BuildGraph(std::vector<AlignPair::Result *> &Res, std::vector<Matrix44d> &Tr, std::vector<int> &Id);
  bool GetMatrixVector(std::vector<Matrix44d> &Tr, std::vector<int> &Id);
	};
} // end namespace
#endif
