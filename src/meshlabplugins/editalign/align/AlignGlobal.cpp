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

#include<vector>
#include<list>
#include<queue>
#include<stack>
#include<map>
#include<algorithm>
#include <stdarg.h>
//#include<vcg/Convergence.h>
#include "AlignPair.h"
#include "AlignGlobal.h"
#include <vcg/math/point_matching.h>

using namespace vcg;
using namespace std;

inline void LOG( FILE *fp, const char * f, ... )
{
	if(fp==0) return;

	va_list marker;
	va_start( marker, f );     
	vfprintf(fp,f,marker);
	va_end( marker );
	fflush(fp);
}


int AlignGlobal::ComputeConnectedComponents()
{
  printf("Building Connected Components on a graph with %i nodes and %i arcs\n",N.size(),A.size());
	
	CC.clear();
	list<AlignGlobal::Node>::iterator li;
	
	stack<AlignGlobal::Node *> ToReach; // nodi ancora da visitare
	stack<AlignGlobal::Node *> st;      // nodi che si stanno visitando

	for(li=N.begin();li!=N.end();++li)
	{
		(*li).sid=-1;
		ToReach.push(&*li);
	}
	
	int cnt=0;
	
	while(!ToReach.empty())
		{
			SubGraphInfo sg;
			st.push(&*ToReach.top());
			ToReach.pop();
			assert(st.top()->sid==-1);
			sg.root=st.top();
			sg.sid=cnt;
			sg.size=0;
			st.top()->sid=cnt;
			while(!st.empty())
				{
					AlignGlobal::Node *cur=st.top();
					st.pop();
					++sg.size;
					assert(cur->sid==cnt);
					printf("Visiting node %2i %2i\n",cur->id,cur->sid);

		 			list<VirtAlign *>::iterator li;
					for(li=cur->Adj.begin();li!=cur->Adj.end();++li)
						if((*li)->Adj(cur)->sid==-1) {
							(*li)->Adj(cur)->sid=cnt;
							st.push((*li)->Adj(cur));
						} else assert((*li)->Adj(cur)->sid==cnt); 
				}
			cnt++;
			CC.push_back(sg);
			while(!ToReach.empty() && ToReach.top()->sid!=-1) ToReach.pop();
		}					
	
	return cnt;
}

////////////////////////////////////////////////////////////////////////////////
// le due matrici M2F e F2M fanno coincidere i punti supposto che siano applicati ad essi 
// le transf proprie dei nodi a cui appartengono (Mov->M e Fix->M)
// Controlla che le le matrici di trasformazione portino effettivamente i punti scelti a coincidere.
// 
bool AlignGlobal::VirtAlign::Check()
{
	int i;
	if(FixP.size()!=MovP.size()) return false;
	Point3d mp,fp;
	double md=0,fd=0;
	double md2=0,fd2=0;
	Matrix44d &MovTr=Mov->M;
	Matrix44d &FixTr=Fix->M;
	for(i=0;i<FixP.size();++i)
		{
			mp=MovTr*MovP[i];
      fp=FixTr*FixP[i];

			md +=       Distance(fp,M2F*mp);
			md2+=SquaredDistance(fp,M2F*mp);
			
			fd +=       Distance(mp,F2M*fp);
			fd2+=SquaredDistance(mp,F2M*fp);
		}
  int nn=MovP.size();
  
	printf("Arc %3i -> %3i : %i pt\n",Fix->id,Mov->id,nn);
  printf("SquaredSum Distance %7.3f %7.3f Avg %7.3f %7.3f\n",fd2, md2, fd2/nn, md2/nn);
  printf("       Sum Distance %7.3f %7.3f Avg %7.3f %7.3f\n",fd , md ,  fd/nn, md/nn);
/*  printf("Fix->M:\n");print(Fix->M);
	printf("Mov->M:\n");print(Mov->M);
	printf("\nM2F->M:\n");print(M2F);
	printf("F2M->M:\n");print(F2M);
*/	return true;
}


AlignGlobal::Node *AlignGlobal::VirtAlign::Adj(AlignGlobal::Node *n)
{
	assert(n==Fix || n==Mov);
	if(n==Fix) return Mov;
	else return Fix;
}
void AlignGlobal::MakeAllDormant()
{
	list<AlignGlobal::Node>::iterator li;
	for(li=N.begin();li!=N.end();++li)
		(*li).Active=false;
}
void AlignGlobal::Dump(FILE *elfp)
{
	fprintf(elfp,"Alignment Graph of %i nodes and %i arcs\n",N.size(),A.size());
//  list<VirtAlign *>::iterator li;
//	for(li=A.begin();li!=A.end();++li)
//		printf("Arc : %3i ->%3i\n",(*li)->Fix->id,(*li)->Mov->id);

}
/*
	Controlla che ogni nodo sia raggiungibile a partire dalla root 
*/

bool AlignGlobal::CheckGraph()
{
	vector<bool> Visited(N.size(),false);
	stack<AlignGlobal::Node *> st;
	st.push(&(*N.begin()));
	while(!st.empty())
		{
			AlignGlobal::Node *cur=st.top();
			st.pop();
			//printf("Visiting node %i\n",cur->id);
		 	list<VirtAlign *>::iterator li;
			for(li=cur->Adj.begin();li!=cur->Adj.end();++li)
				if(!Visited[(*li)->Adj(cur)->id]) {
					Visited[(*li)->Adj(cur)->id]=true;
					st.push((*li)->Adj(cur));
				}
		}
	int cnt=0;
#ifdef __STL_CONFIG_H
count(Visited.begin(),Visited.end(),true,cnt);
#else
	cnt=count(Visited.begin(),Visited.end(),true);
#endif
	printf("Nodes that can be reached from root %i on %i \n",cnt,N.size());
	return cnt==N.size(); 
}


int AlignGlobal::DormantNum()
{
	int cnt=0;
	list<AlignGlobal::Node>::iterator li;
	for(li=N.begin();li!=N.end();++li)
		if(!(*li).Active) ++cnt;

	return cnt;
}

int AlignGlobal::ActiveNum()
{
	int cnt=0;
	list<AlignGlobal::Node>::iterator li;
	for(li=N.begin();li!=N.end();++li)
		if((*li).Active) ++cnt;

	return cnt;
}

AlignGlobal::Node *AlignGlobal::ChooseDormantWithMostDormantLink ()
{
	AlignGlobal::Node *BestNode=0;
	int MaxAdjNum=0;
	list<AlignGlobal::Node>::iterator li;
	for(li=N.begin();li!=N.end();++li)
		if(!(*li).Active)
		{
			int AdjNum = (*li).DormantAdjNum();
      if(AdjNum>MaxAdjNum) 
			{
				MaxAdjNum=AdjNum;
				BestNode=&(*li);
			}
		}
	assert(BestNode);
	assert(!BestNode->Queued);
	assert(!BestNode->Active);
	return BestNode;
}

AlignGlobal::Node *AlignGlobal::ChooseDormantWithMostActiveLink ()
{
	AlignGlobal::Node *BestNode=0;
	int MaxAdjNum=0;
	list<AlignGlobal::Node>::iterator li;
	for(li=N.begin();li!=N.end();++li)
		if(!(*li).Active)
		{
			int AdjNum = (*li).ActiveAdjNum();
      if(AdjNum>MaxAdjNum) 
			{
				MaxAdjNum=AdjNum;
				BestNode=&(*li);
			}
		}
	if(!BestNode) 
		{// Abbiamo finito di sistemare questa componente connessa. 
			printf("Warning! Unable to find a Node with at least an active link!!\n");
			return 0;
	}
	assert(BestNode);
  assert(!BestNode->Queued);
	assert(!BestNode->Active);
	return BestNode;
}

int AlignGlobal::Node::PushBackActiveAdj(queue<AlignGlobal::Node *>	&Q)
{
	int cnt=0;
	assert(Active);
  AlignGlobal::Node *pt;
	list<VirtAlign *>::iterator li;
	for(li=Adj.begin();li!=Adj.end();++li)
		{
			pt=(*li)->Adj(this);
			if(pt->Active && !pt->Queued) 
				{
					++cnt;
					pt->Queued=true;
					Q.push(pt);
				}
		}	
  return cnt;
}

int AlignGlobal::Node::ActiveAdjNum ()
{
	int cnt=0;
	list<VirtAlign *>::iterator li;
	for(li=Adj.begin();li!=Adj.end();++li)
		if((*li)->Adj(this)->Active) ++cnt;
  return cnt;
}

int AlignGlobal::Node::DormantAdjNum ()
{
	int cnt=0;
	list<VirtAlign *>::iterator li;
	for(li=Adj.begin();li!=Adj.end();++li)
		if(!(*li)->Adj(this)->Active) ++cnt;
  return cnt;
}


/******************************

Data una mesh con una posizione di base,  ne cambia la pos di base in maniera tale che
sia allineata con tutti i punti delle adiacenti attive;
In pratica il nodo stesso e' la mesh mov, mentre tutti quelli intorno franno una specie di fix virtuale

Alla fine e' cambiata la matrice M del nodo stesso e tutte le matrici degli allineamenti virtuali


******************************/
double AlignGlobal::Node::AlignWithActiveAdj(bool Rigid)
{
	list<VirtAlign *>::iterator li;

	printf("--- AlignWithActiveAdj --- \nMoving node %i with respect to nodes:",id);
	for(li=Adj.begin();li!=Adj.end();++li) if((*li)->Adj(this)->Active) printf(" %i,",(*li)->Adj(this)->id);
	printf("\n");

  //printf("Base Matrix of Node %i\n",id);print(M);
	
	// Step 1; Costruiamo le due liste di punti da allineare
  vector<Point3d> FixP,MovP, FixN,MovN;
	Box3d FixBox,MovBox;FixBox.SetNull();MovBox.SetNull();
	for(li=Adj.begin();li!=Adj.end();++li)
		if((*li)->Adj(this)->Active) // scorro tutti i nodi adiacenti attivi
		{
			//printf("Base Matrix of Node %i adj to %i\n",id,(*li)->Adj(this)->id);
			//print((*li)->Adj(this)->M);
			vector<Point3d> &AP=(*li)->AdjP(this);   // Punti sul nodo adiacente corrente;
			vector<Point3d> &AN=(*li)->AdjN(this);   // Normali sul nodo adiacente corrente;
		
			//printf("Transf that bring points of %i onto %i\n",id,(*li)->Adj(this)->id);
			//print((*li)->A2N(this));
			//printf("Transf that bring points of %i onto %i\n",(*li)->Adj(this)->id,id);
			//print((*li)->N2A(this));
			Point3d pf,nf;
			Point3d pm;
			for(int i=0;i<AP.size();++i)
			{
				pf=(*li)->Adj(this)->M*AP[i]; // i punti fissi sono quelli sulla sup degli adiacenti messi nella loro pos corrente 
				FixP.push_back(pf);
				FixBox.Add(pf);
				nf=(*li)->Adj(this)->M*Point3d(AP[i]+AN[i])-pf;
				nf.Normalize();
		    FixN.push_back(nf);
        
				pm=(*li)->A2N(this)*pf;
				MovP.push_back(pm); // i punti che si muovono sono quelli sul adj trasformati in modo tale da cascare sul nodo corr.
				MovBox.Add(pm);
			}
		}
	Matrix44d out;
	bool ret;
	//if(Rigid) ret=ComputeRigidMatchMatrix(out,FixP,MovP);
	//else ret=ComputeMatchMatrix2(out,FixP,FixN,MovP);
  ret=PointMatching<double>::ComputeRigidMatchMatrix(out,FixP,MovP);   
	Matrix44d outIn=out; Invert(outIn);
	//double maxdiff = MatrixNorm(out);
	double maxdiff = MatrixBoxNorm(out,FixBox);

	if(!ret) printf("Error!!\n\n");
//	printf("Computed Transformation:\n"); print(out);printf("--\n");
//	printf("Collected %i points . Err = %f\n",FixP.size(),maxdiff);
  
	// La matrice out calcolata e' quella che applicata ai punti MovP li porta su FixP, quindi i punti della mesh corrente 
	// La nuova posizione di base della mesh diventa quindi
	// M * out 
	// infatti se considero un punto della mesh originale applicarci la nuova matricie significa fare
	// p * M * out

  // M=M*out; //--Orig
	M=out*M;


 // come ultimo step occorre applicare la matrice trovata a tutti gli allineamenti in gioco.
	for(li=Adj.begin();li!=Adj.end();++li)// scorro tutti i nodi adiacenti attivi
		{
			//print((*li)->N2A(this));
			//print((*li)->A2N(this));printf("--\n");
   
			(*li)->N2A(this)=(*li)->N2A(this)*outIn;
			(*li)->A2N(this)=(*li)->A2N(this)*out  ;
			//print((*li)->N2A(this));
			//print((*li)->A2N(this));printf("--\n");
		}
	//getchar();
	return maxdiff;

}


// Nuova Norma per matrici:
// restituisce la max spostamento che si puo' ottenere se si 
// applica tale trasf a un insieme di punti contenuto nel box bb

double AlignGlobal::Node::MatrixBoxNorm(Matrix44d &NewM,Box3d &bb) const
{
		double maxdiff=0;
		Point3d pt;

		pt=Point3d(bb.min[0],bb.min[1],bb.min[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.max[0],bb.min[1],bb.min[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.min[0],bb.max[1],bb.min[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.max[0],bb.max[1],bb.min[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.min[0],bb.min[1],bb.max[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.max[0],bb.min[1],bb.max[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.min[0],bb.max[1],bb.max[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
		pt=Point3d(bb.max[0],bb.max[1],bb.max[2]); maxdiff=max(maxdiff,Distance(pt,NewM*pt));
return maxdiff;
}

double AlignGlobal::Node::MatrixNorm(Matrix44d &NewM) const
{
	double maxdiff=0;
	Matrix44d diff;
	diff.SetIdentity();
	diff=diff-NewM;
	for(int i=0;i<4;++i)
		for(int j=0;j<4;++j)
			maxdiff+=(diff[i][j]*diff[i][j]);
		return maxdiff;
}

void AlignGlobal::Clear()
{

	list<VirtAlign *>::iterator li;
	for(li=A.begin();li != A.end();++li)
		delete(*li);

	N.clear();
	A.clear();
	
}




/******************************
Per ogni componente connessa, 
  si inizia dal nodo con piu' adiacenti 
  Diventa attivo
  si sceglie tra i do

******************************/

bool AlignGlobal::GlobalAlign(const vector<string> &Names, 	const double epsilon, int maxiter, bool Rigid, FILE *elfp, CallBack* cb )
{
	double change;
  int step, localmaxiter;
	cb("Global Alignment...");
	LOG(elfp,"----------------\n----------------\nGlobalAlignment (eps %7.3f)\n",epsilon);
	
  queue<AlignGlobal::Node *>	Q; 
	MakeAllDormant();
	AlignGlobal::Node *curr=ChooseDormantWithMostDormantLink(); 
	curr->Active=true;  
	int cursid=curr->sid;
	LOG(elfp,"Root node %i '%s' with %i dormant link\n", curr->id, Names[curr->id].c_str(),curr->DormantAdjNum());
  
  while(DormantNum()>0)
	{
		LOG(elfp,"---------\nGlobalAlignment loop DormantNum = %i\n",DormantNum());
		
		curr=ChooseDormantWithMostActiveLink ();
		if(!curr) {
			// la componente connessa e' finita e si passa alla successiva cercando un dormant con tutti dormant.
			LOG(elfp,"\nCompleted Connected Component %i\n",cursid);
			LOG(elfp,"\nDormant Num: %i\n",DormantNum());
  	  
			curr=ChooseDormantWithMostDormantLink ();
			if(curr==0) {
										LOG(elfp,"\nFailed ChooseDormantWithMostDormantLink, choosen id:%i\n" ,0);
										break; // non ci sono piu' componenti connesse composte da piu' di una singola mesh.
									}
		           else LOG(elfp,"\nCompleted ChooseDormantWithMostDormantLink, choosen id:%i\n" ,curr->id);
  	  curr->Active=true;  
			cursid=curr->sid;
			curr=ChooseDormantWithMostActiveLink ();
			if(curr==0) LOG(elfp,"\nFailed    ChooseDormantWithMostActiveLink, choosen id:%i\n" ,0);
		        else  LOG(elfp,"\nCompleted ChooseDormantWithMostActiveLink, choosen id:%i\n" ,curr->id);  	 
		}  
		
    LOG(elfp,"\nAdded node %i '%s' with %i/%i Active link\n",curr->id,Names[curr->id].c_str(),curr->ActiveAdjNum(),curr->Adj.size());
  	curr->Active=true;
		curr->Queued=true;
		localmaxiter=ActiveNum()*10;  // Si suppone, ad occhio, che per risistemare un insieme di n mesh servano al piu' 10n passi;
		Q.push(curr);
		step=0;
    // ciclo interno di allineamento
    // 
		while(!Q.empty())
		{
		 curr=Q.front();
		 Q.pop();
		 curr->Queued=false;
		 change=curr->AlignWithActiveAdj(Rigid);
		 step++;
		 LOG(elfp,"     Step %5i Queue size %5i Moved %4i  err %10.4f\n",step,Q.size(),curr->id,change);
		 if(change>epsilon)
				{  
						curr->PushBackActiveAdj(Q);
						LOG(elfp,"         Large Change pushing back active nodes adj to %i to Q (new size %i)\n",curr->id,Q.size());
						if(change>epsilon*1000)  printf("Large Change Warning\n\n");
		 		}
     if(step>localmaxiter) return false;
		 if(step>maxiter) return false;
		}
	}
	if(!curr) 
		{
			LOG(elfp,"Alignment failed for %i meshes:\n",DormantNum());
			list<AlignGlobal::Node>::iterator li;
			
			for(li=N.begin();li!=N.end();++li)
				if(!(*li).Active){
					//(*li).M.SetIdentity();
					(*li).Discarded=true;
					LOG(elfp,"%5i\n",(*li).id);
				}
		}
	LOG(elfp,"Completed Alignment in %i steps with error %f\n",step,epsilon);
	return true;
}

// riempie un vettore di matrici con le matrici risultato dell'allineamento globale.
// il Vettore Id dice quali mesh prendere. 
bool AlignGlobal::GetMatrixVector(std::vector<Matrix44d> &Tr, std::vector<int> &Id)
{
  std::list<Node>::iterator li;
  Tr.clear();
  map<int,AlignGlobal::Node *> Id2N;
  for(li=N.begin();li!=N.end();++li)
	  Id2N[(*li).id]=&*li;
  
  Tr.resize(Id.size());
  for(int i=0;i<Id.size();++i)
  {
    if( Id2N[Id[i]] ==0 ) return false;
    Tr[i]=Id2N[Id[i]]->M;
  }
  return false;
}

/*
Build the Alignment Graphs starting from the vector of Results and from the vector of the matrix with the current starting positions.
*/
void AlignGlobal::BuildGraph(std::vector<AlignPair::Result *> &Res, vector<Matrix44d> &Tr, vector<int> &Id)
{
		Clear();
		// si suppone che la matrice Tr[i] sia relativa ad un nodo con id Id[i];
		int i,mn=Tr.size();

	//	printf("building graph\n");
		AlignGlobal::Node rgn;
		rgn.Active=false;
		rgn.Queued=false;
		rgn.Discarded=false;
		map<int,AlignGlobal::Node *> Id2N;
		map<int,int> Id2I;
		for(i=0;i<mn;++i)
		{
			rgn.id=Id[i];
			rgn.M=Tr[i];
			N.push_back(rgn);
			Id2N[rgn.id]=&(N.back());
			Id2I[rgn.id]=i;
		}

		printf("building %i graph arcs\n",Res.size());
		VirtAlign *tv;

    // Ciclo principale in cui si costruiscono i vari archi
    // Si assume che i result siano fatti nel sistema di riferimento della matrici fix.

		vector<AlignPair::Result *>::iterator rii;
		for(rii=Res.begin();rii!=Res.end();++rii)
		{
			AlignPair::Result *ri= *rii;
			tv = new VirtAlign();
			tv->Fix=Id2N[(*ri).FixName];
			tv->Mov=Id2N[(*ri).MovName];
			tv->Fix->Adj.push_back(tv);
			tv->Mov->Adj.push_back(tv);
			tv->FixP=(*ri).Pfix;
			tv->MovP=(*ri).Pmov;
			tv->FixN=(*ri).Nfix;
			tv->MovN=(*ri).Nmov;
			
		/*

			Siano:
			Pf e Pm   i punti sulle mesh fix e mov nei sist di rif originali
			Pft e Pmt i punti sulle mesh fix e mov dopo le trasf correnti; 
			Mf e Mm  le trasf che portano le  mesh fix e mov nelle posizioni correnti; 
			If e Im  le trasf inverse di cui sopra 
			Vale:
			Pft = Mf*Pf  e Pmt = Mm*Pm
			 Pf = If*Pft e Pm = Im*Pmt 

						 Res *   Pm     =         Pf;
						 Res * Im * Pmt =      If * Pft
				Mf * Res * Im * Pmt = Mf * If * Pft 
			(Mf * Res * Im) * Pmt = Pft 

		*/
			Matrix44d Mm=Tr[Id2I[(*ri).MovName]]; 
			Matrix44d Mf=Tr[Id2I[(*ri).FixName]]; 
			Matrix44d Im=Inverse(Mm);
			Matrix44d If=Inverse(Mf);
			
			Matrix44d NewTr = Mf * (*ri).Tr * Im; // --- orig

			tv->M2F=NewTr;
			tv->F2M=Inverse(NewTr);
			
      assert(tv->Check());
			A.push_back(tv);
		}
			
	ComputeConnectedComponents();
}
