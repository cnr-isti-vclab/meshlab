#include "vcg_mesh.h"
#include "ocme_impostor.h"
#include "ocme_extract.h"

/* UGLY: this dependency is because of the templates..it should be removed*/
#ifdef SIMPLE_DB
#include "../ooc_vector/io/ooc_chains.hpp"
#else
#include "../ooc_vector/io/ooc_chains_kcdb.hpp"
#endif

#include <vcg/space/normal_extrapolation.h>

extern unsigned int  generic_bool;
int COff(const int & h);

void OCME::ComputeImpostors( ){
	for(CellsIterator ci  = cells.begin(); ci != cells.end(); ++ci){
		vcgMesh m;
		this->ClearImpostor((*ci).second->key);
		this->ExtractVerticesFromASingleCell((*ci).second->key,m);
		vcg::NormalExtrapolation<vcgMesh::VertContainer>::ExtrapolateNormals(m.vert.begin(),m.vert.end(),10);
		for(vcgMesh::VertexIterator vi = m.vert.begin(); vi != m.vert.end(); ++vi)
			(*ci).second->impostor->AddSample((*vi).P(),(*vi).cN(),vcg::Color4b::Gray);
	}
}
void OCME::ImpostorsToMesh(vcgMesh & m){

	//{
	//	vcg::Point3f p,n;
	//	vcg::Point3<unsigned char> c;

	//	m.Clear();
	//	for(CellsIterator ci = cells.begin(); ci != cells.end();++ci){
	//		Impostor * imp = (*ci).second->impostor;
	//		vcgMesh::VertexIterator vi = vcg::tri::Allocator<vcgMesh>::AddVertices(m,imp->proxies.size());
	//		for(	Impostor::PointCellIterator pi   = imp->proxies.begin(); pi != imp->proxies .end(); ++pi,++vi  ){
	//				imp->GetPointNormalColor(*pi,p,n,c);
	//				(*vi).N() =  n;
	//				(*vi).C() = vcg::Color4b(c[0],c[1],c[2],255);
	//				(*vi).P() = p;
	//		}
	//	}
	//}

	{
		vcg::Point3f p,n;
	 	vcg::Point3<unsigned char> c;

		m.Clear();
		for(CellsIterator cc = cells.begin(); cc != cells.end();++cc){
			Impostor * imp = (*cc).second->impostor;

			std::map<unsigned short,vcg::Point3<char> >::iterator pi,ni;
			std::map<unsigned short,vcg::Point3<unsigned char> >::iterator ci;
			
			pi = imp->centroids.data.begin();
			ni = imp->normals.data.begin();
			ci = imp->colors.data.begin();

			vcgMesh::VertexIterator vi = vcg::tri::Allocator<vcgMesh>::AddVertices(m,imp->centroids.data.size());

			for(; pi != imp->centroids.data.end(); ++pi,++ni,++ci,++vi){
					
					unsigned char i,j,k;
					stdMatrix3Sparse<vcg::Point3<char>,8>::Index((*pi).first,i,j,k);
					vcg::Point3<char> & a =  (*pi).second;
					p = vcg::Point3f(imp->C2F (a[0],i,0),imp->C2F (a[1],j,1),imp->C2F (a[2],k,2));
					vcg::Point3<char> & b = (*ni).second;
					n = vcg::Point3f(imp->C2F_01(b[0]),imp->C2F_01(b[1]),imp->C2F_01(b[2])).Normalize();

					c = (*ci).second;

					(*vi).N() =  n;
					(*vi).C() = vcg::Color4b(c[0],c[1],c[2],255);
					(*vi).P() = p;
			}
		}
	}
}

bool OCME::ClearImpostor(const CellKey & ck){

		std::vector<CellKey> chl;

		Cell * c = GetCell(ck,false), * c_c;
		if(!c)
				return false; // c is a root

		// first consider the data in this cell
		this->Children(ck,chl);
		float changed_occupancy = 0.f;

		for(unsigned int cc = 0; cc < 8; ++cc){
					if(c->impostor->non_empty_children & (1<<cc) ){
						c_c = GetCell(chl[cc],false);
						if(!c_c){
								/*	this cell was marked as a child at the last
										impostor updates. If it is not there anymore
										it means it has been deleted
								*/
								c->impostor->non_empty_children &= ~(1<<cc);   // update the children bit
								changed_occupancy+=0.125f;
						}else
						if(c_c->generic_bool())// it is one of the touched cells
								changed_occupancy+=0.125f;
				}
		}

		if(changed_occupancy >=  c->impostor->occupancy  * 0.5 ){  // if at half of the 8 children must be updated then the parent must be updated too
				if(!c->generic_bool()) c->generic_bool = FBool(&generic_bool);
				c->generic_bool = true;
				c->impostor->ClearDataCumulate();
				c->impostor->data_occupancy = 0;
				c->impostor->proxies.clear();
				RemoveImpostor(c->key);
		}

		return c->generic_bool();
	}

void OCME::ClearImpostors(std::vector<CellKey> & fromCells){
		++impostor_updated;
		++generic_bool;					// used to mark the touched cells

		std::vector<CellKey> cells_by_level [256];

		// phase 1. fill the array of level with the cells and reset their occupancy
		for(std::vector<CellKey>::iterator  ci  = fromCells.begin(); ci != fromCells.end(); ++ci){
			Cell *c = GetCell(*ci,false);
			c->generic_bool= FBool(&generic_bool);
			c->impostor->ClearDataCumulate();
			c->impostor->proxies.clear();
			c->impostor->data_occupancy = 0;
			RemoveImpostor(c->key);

			CellKey & ck = (*ci);
			cells_by_level[COff(ck.h)].push_back(ck);
	}

		unsigned int level = 0;
		while( (level  < 256) &&  cells_by_level[level].empty() ) {++level;} // find the lowest non empty level
		if(level == 256) return;																							// if the database is empty return

		// phase 2., bottom up clearing of the impostors
		for( ; level  < 255;++level ){
			::RemoveDuplicates(cells_by_level[level]);
			for(unsigned int i = 0; i <cells_by_level[level].size();++i){
				const CellKey &  k = cells_by_level[level][i];
				Cell* cell = GetCell( k,false);
				assert(cell);
				const CellKey & pk = Parent(k);
				if(ClearImpostor(pk))
						cells_by_level[level+1].push_back(pk);
				}
		}
}

void OCME::FillSamples(std::vector<CellKey> & cs){

		for(unsigned int i = 0; i < cs.size(); ++i){
				vcgMesh tmp;
				Cell *c = GetCell(cs[i],false);
				if(!c) continue;
				// *** actually there is no need to do all the work.
				// *** instead of ExtractContainedFacesFromASingleCell
				// *** we should have an ad hoc function FillSamplesFromASingleCell
				// *** TO DO.

                this->ExtractContainedFacesFromASingleCell(cs[i],tmp);

				/* clear the data that will be recomputed */
				c->impostor->ClearDataCumulate();

				bool hasColor = vcg::tri::HasPerVertexColor(tmp);

				for(vcgMesh::FaceIterator fi = tmp.face.begin(); fi != tmp.face.end(); ++fi){
						vcg::Point3f bary   = vcg::Barycenter(*fi);
						vcg::Point3f pp[3];
                        vcg::Point3f n = vcg::Normal(*fi).Normalize();
						vcg::Color4b color  =  (hasColor)? (*fi).V(0)->cC() : vcg::Color4b::Gray;
                        c->impostor->AddSample(bary,n,color);	// collect a sample for the impostor
				}
		}
}

bool OCME::UpdateImpostor(const CellKey & ck){
	/*
	This function assume that the samples are up-to-date and present
	in the cell ck and its 8 children
	*/
	std::vector<CellKey> chl;
	vcgMesh m;
	Cell * c = GetCell(ck,true), * c_c;
	// first consider the data in this cell
	std::vector<vcg::Point3f> smp;

	c->impostor->InitDataCumulate(ck.BBox3f());
	c->impostor->occupancy = 0;
	c->impostor->ComputeDataOccupancy();

	// then take the samples from all the children
	const  unsigned int & gridsize = c->impostor->Gridsize();
	this->Children(ck,chl);
	for(unsigned int cc = 0; cc < 8; ++cc){
		c_c = GetCell(chl[cc],false);
		if(c_c){

			c->impostor->AddSamplesFromImpostor(c_c->impostor);

			c->impostor->occupancy += c_c->impostor->occupancy * 0.125f; // == ../8
			c->impostor->non_empty_children |= 1<<cc; // raise the corresponding bit
			if(!c_c->rd->impostor_updated()){
				c_c->rd->impostor_updated = FBool(&impostor_updated);
				c_c->rd->impostor_updated	= true;
			}
		}
	}

	/*
	the occupancy of a cell as impostor is the maximum between the occupancy of its children and its own data occupancy
	*/
	if(c->impostor->occupancy < c->impostor->data_occupancy/float(gridsize*gridsize*gridsize)) 
		c->impostor->occupancy = c->impostor->data_occupancy/float(gridsize*gridsize*gridsize);

 
	c->impostor->SetCentroids(false);
	return (c->impostor->occupancy < 0.2f/float(gridsize*gridsize*gridsize));
}



void OCME::BuildImpostorsHierarchy(){BuildImpostorsHierarchy(this->cells);}
void OCME::BuildImpostorsHierarchy(std::vector<CellKey> & fromCells){
		std::vector<Cell*> cs;
		for(std::vector<CellKey>::iterator ci = fromCells.begin(); ci != fromCells.end(); ++ci) {
				Cell * c = GetCell(*ci,false);
				cs.push_back(c);
		}
		BuildImpostorsHierarchy(cs);
}
void OCME::BuildImpostorsHierarchy(CellsContainer & fromCells){
		std::vector<Cell*> cs;
		for(CellsIterator ci = fromCells.begin(); ci != fromCells.end(); ++ci) cs.push_back((*ci).second);
		BuildImpostorsHierarchy(cs);
}

void OCME::BuildImpostorsHierarchy(std::vector<Cell*> & fromCells){
	++impostor_updated;
	/*
	The impostors are built bottom up, starting from the smallest cells (lowest h)
	*/
	std::vector<CellKey> cells_by_level [256];

	std::vector<Cell*>::iterator ci;


	// phase 1. fill the array of level with the cells and compute centroid and data occupancy
	for(ci  = fromCells.begin(); ci != fromCells.end(); ++ci){
		(*ci)->impostor->SetCentroids();
		CellKey & ck = (*ci)->key;
		cells_by_level[COff(ck.h)].push_back(ck);
	}

	unsigned int level = 0;
	while( (level  < 256) &&  cells_by_level[level].empty() ) {++level;} // find the lowest non empty level
	if(level == 256) return;												// if the database is empty return

	// phase 2., bottom up updating of the impostors
	std::vector<vcg::Point3f> smp;
//	unsigned int tmpEnd = level+5;
	for( ; level  < 255;++level ){

			::RemoveDuplicates(cells_by_level[level]);
		// build of the impostors of this level
		for(unsigned int i = 0; i <cells_by_level[level].size();++i) 
				GetCell( cells_by_level[level][i],false)->impostor->Create( this,cells_by_level[level][i]);
		for(unsigned int i = 0; i <cells_by_level[level].size();++i) 
				GetCell( cells_by_level[level][i],false)->impostor->ClearDataCumulate();

		for(unsigned int i = 0; i <cells_by_level[level].size();++i){
			const CellKey &  k = cells_by_level[level][i];
			Cell* cell = GetCell( k,false);
			assert(cell);
			if(!cell->rd->impostor_updated()){
				const CellKey & pk = Parent(k);
			//	to_insert  = (GetCell( pk,false)==NULL);
				if(!UpdateImpostor(pk) /*&& to_insert*/)
					cells_by_level[level+1].push_back(pk);
				else
					octree_roots.push_back(GetCell(pk));
			}
		}
	}
}



bool OCME::UpdateImpostorPartial(const CellKey & ck){
	/*
	This function assume that the samples are up-to-date and present
	in the cell ck and its 8 children
	*/
	std::vector<CellKey> chl;
	vcgMesh m;
	Cell * c = GetCell(ck,true), * c_c;
	// first consider the data in this cell
	std::vector<vcg::Point3f> smp;

	c->impostor->InitDataCumulate(ck.BBox3f());
	c->impostor->occupancy = 0;
	c->impostor->ComputeDataOccupancy();

	// then take the samples from all the children
	const  unsigned int & gridsize = c->impostor->Gridsize();
	this->Children(ck,chl);
	for(unsigned int cc = 0; cc < 8; ++cc){
		c_c = GetCell(chl[cc],false);
		if(c_c){

			c->impostor->AddSamplesFromImpostor(c_c->impostor);

			c->impostor->occupancy += c_c->impostor->occupancy * 0.125f; // == ../8
			c->impostor->non_empty_children |= 1<<cc; // raise the corresponding bit
			if(!c_c->rd->impostor_updated()){
				c_c->rd->impostor_updated = FBool(&impostor_updated);
				c_c->rd->impostor_updated	= true;
			}
		}
	}

	/*
	the occupancy of a cell as impostor is the maximum between the occupancy of its children and its own data occupancy
	*/
	if(c->impostor->occupancy < c->impostor->data_occupancy/float(gridsize*gridsize*gridsize))
		c->impostor->occupancy = c->impostor->data_occupancy/float(gridsize*gridsize*gridsize);


	c->impostor->SetCentroids(false);
	return (c->impostor->occupancy < 0.2f/float(gridsize*gridsize*gridsize));
}
void OCME::BuildImpostorsHierarchyPartial(std::vector<CellKey> & fromCells){
		std::vector<Cell*> cs;
		for(std::vector<CellKey>::iterator ci = fromCells.begin(); ci != fromCells.end(); ++ci) {
				Cell * c = GetCell(*ci,false);
				cs.push_back(c);
		}
		BuildImpostorsHierarchyPartial(cs);
}
void OCME::BuildImpostorsHierarchyPartial(std::vector<Cell*> & fromCells){
	++impostor_updated;
	/*
	The impostors are built bottom up, starting from the smallest cells (lowest h)
	*/
	std::vector<CellKey> cells_by_level [256];
	std::vector<Cell*>::iterator ci;

	// phase 1. fill the array of level with the cells and compute centroid and data occupancy
	// This assume the samples per cells have already been collected
	for(ci  = fromCells.begin(); ci != fromCells.end(); ++ci){
		(*ci)->impostor->SetCentroids();
		CellKey & ck = (*ci)->key;
		cells_by_level[COff(ck.h)].push_back(ck);
	}

	unsigned int level = 0;
	while( (level  < 256) &&  cells_by_level[level].empty() ) {++level;}		// find the lowest non empty level
	if(level == 256) return;																								// if  empty return

	// phase 2., bottom up updating of the impostors
	//bool to_insert = false;

	for( ; level  < 255;++level ){
		::RemoveDuplicates(cells_by_level[level]);
		// build of the impostors of this level
		for(unsigned int i = 0; i <cells_by_level[level].size();++i)
				GetCell( cells_by_level[level][i],false)->impostor->Create( this,cells_by_level[level][i]);

		for(unsigned int i = 0; i <cells_by_level[level].size();++i){
			const CellKey &  k = cells_by_level[level][i];
			Cell* cell = GetCell( k,false);
			assert(cell);
			if(!cell->rd->impostor_updated()){
				const CellKey & pk = Parent(k);

				if(!UpdateImpostorPartial(pk) )
					cells_by_level[level+1].push_back(pk);
				else
					octree_roots.push_back(GetCell(pk)); // to be redone
			}
		}
	}
}
