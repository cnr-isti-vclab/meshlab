#ifndef CONTAINER_H
#define CONTAINER_H

#include <stdio.h>
#include <QRect>
#include <QHash>
#include <meshlab/meshmodel.h>

using namespace std;

class Container
{
	// Class that stores info about the UV Vertex
	public:
		Container(int id = 0, float u = 0, float v = 0, QRect r = QRect(), CVertexO* p = 0, 
				  CFaceO* f = 0, unsigned wtindex = 0)
		{
			marca = false;
			cu = u; cv = v;
			idcomp = id;
			rect = r;
			punt = p;
			face.push_back(f);
			wt.push_back(wtindex);
			count = 1;
			imark.push_back(0);
		};
		~Container(){};
		void SetVertex(QRect r) {rect = r;};
		QRect GetVertex() {return rect;};
		// Precision lost caused by the conversion between u,v and screen space
		float GetU() {return cu;};
		float GetV() {return cv;};
		void SetU(float u) {cu = u;};
		void SetV(float v) {cv = v;};
		bool IsV() {return marca;};
		void SetV() {marca = true;};
		void ClearV() {marca = false;};
		void AddAdj(CVertexO* val) 
		{
			unsigned i = 0;
			for(; i < adj.size(); i++)
				if (adj[i] == val) break;
			if (i == adj.size())adj.push_back(val);
		};
		CVertexO* GetAdjAt(int index) {return adj[index];};
		int GetAdjSize() {return adj.size();};
		bool ContainAdj(CVertexO* val)
		{
			for(unsigned i = 0; i < adj.size(); i++)
				if (adj[i] == val) return true;
			return false;
		}
		void AddFace(CFaceO* f) {face.push_back(f); count++; imark.push_back(0);};
		CFaceO* GetFaceAt(int index) {return face[index];};
		unsigned GetFaceSize() {return face.size();};
		void SetCompID(int val) {idcomp = val;};
		int GetCompID() {return idcomp;};
		void SetPointer(CVertexO* p){punt = p;};
		CVertexO* GetPointer() {return punt;};
		void AddWT(unsigned wtindex) {wt.push_back(wtindex);};
		unsigned GetWTAt(int index) {return wt[index];};
		unsigned GetCount() {return count;}
		void Decrease() {--count;};
		void Reset() {count = adj.size()-1;};
		void Mark(unsigned i) {imark[i]++;};
		unsigned GetMark(unsigned i) {return imark[i];};

	private:
		QRect rect;				// Rectangle of the projected vertex
		float cu,cv;			// Values of the coord
		bool marca;				// Visit mark
		vector<CVertexO*> adj;		// Set of adjacent faces
		int idcomp;				// Id of the component of the vertex
		CVertexO* punt;			// Pointer to the real vertex
		vector<CFaceO*> face;	// Set of faces that refer to the vertex
		vector<unsigned> wt;	// Set of WT indexes of the faces
		unsigned count;
		vector<unsigned> imark;
};

#endif