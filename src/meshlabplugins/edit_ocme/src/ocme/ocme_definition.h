#ifndef __OCME_DEFINITION_
#define __OCME_DEFINITION_

/* stdlib includes */
#include <map>
#include <cmath>

/* ocme includes */
#include "ocme_disk_loader.h"
#include "cell.h"
#include "cell_attributes.h"

#include "type_traits.h"
#include "vcg_mesh.h"
#include "../utils/logging.h"

/* vcglib includes */
#include <vcg/complex/algorithms/stat.h>
#include <GL/glew.h>
#include <wrap/gl/space.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gl/splatting_apss/splatrenderer.h>


extern unsigned int kernelSetMark;
extern unsigned int lockedMark;
extern unsigned int impostor_updated;
extern unsigned int to_render_fbool;
extern double cellsizes[256];
extern vcg::Point4f corners[8];

void InitCellSizes();
double CS(const int & h);

template <class CoordType >
CellKey ComputeCellKey( const CoordType & p, const int & h){
		const int x =  (int)floor(p[0] / CS(h) );
		const int y =  (int)floor(p[1] / CS(h) );
		const int z =  (int)floor(p[2] / CS(h) );
		return CellKey(x,y,z,h);
}
std::string ToString(CellKey key);


extern Logging * lgn;


struct OCME{
	
	OCME();
	~OCME();

	struct Params{
		unsigned int side_factor;
	};  
 	Params params;

	struct Statistics{
	Statistics():	n_cells(0),n_chains(0),n_chunks_faces(0),n_chunks_vertex(0),
                                        size_faces(0),size_vertex(0),
					size_dependences(0),size_lcm_allocation_table(0),size_ocme_table(0),size_impostors(0),n_getcell(0),
					n_files(0),n_chunks_faces_avg_per_cell(0),n_chunks_vertex_avg_per_cell(0),
                                        n_triangles(0),  n_vertices(0),n_proxies(0),
					input_file_size(0),time_disk_write(0.0),
					time_disk_read(0.0),time_total(0.0)
	{}
		unsigned long 
			n_triangles,
			n_vertices,
			n_proxies,
			input_file_size,
			n_cells ,
			n_chains,
			n_chunks_faces,
			n_chunks_vertex,

			size_faces,
			size_vertex,

			size_dependences,
			size_lcm_allocation_table,
			size_ocme_table,
			size_impostors,
			n_getcell;
		
		unsigned int 
			n_files;

		float	n_chunks_faces_avg_per_cell,
				n_chunks_vertex_avg_per_cell,

				time_for_loading,
				time_disk_write,
				time_disk_read,
				time_total
				;
		
		unsigned int cells_to_render;

		unsigned long TotalSize(){
			return 	size_faces	+
					size_vertex	+

					size_lcm_allocation_table +
					size_dependences +
					size_ocme_table +
					size_impostors ;
		}
	};
	Statistics stat;
	OOCEnv  oce;
	
	struct HashFunctor : public std::unary_function<CellKey, size_t>
	{
		enum
		  { // parameters for hash table
				bucket_size = 4, // 0 < bucket_size
				min_buckets = 8
			};

		size_t operator()(const CellKey &p) const
		{
			const size_t _HASH_P0 = 73856093u;
			const size_t _HASH_P1 = 19349663u;
			const size_t _HASH_P2 = 83492791u;
			const size_t _HASH_P3 = 83492791u;

			return size_t(p.x)*_HASH_P0 ^  size_t(p.y)*_HASH_P1 ^  size_t(p.z)*_HASH_P2 ^size_t(p.h) *_HASH_P3;
		}

		bool operator()(const CellKey &s1, const CellKey &s2) const
		{ // test if s1 ordered before s2
			return (s1 < s2);
		}
	};

//	typedef	CachedMap CellsContainer;
//	typedef CachedMap::CellsContainer::iterator CellsIterator;

	typedef std::map<CellKey,Cell* /*,HashFunctor*/> CellsContainer;
	typedef CellsContainer::iterator CellsIterator;

	CellsContainer cells;

	// true if written data will not be changed (when a mesh is added)
	bool streaming_mode;

	// true if modification to the set of cells are recorded
	bool record_cells_set_modification;

	// progressive mark to identify copies of the same vertex in different cells
	unsigned int gbi;

	// cell added since  record_cells_set_modification was set to true
	std::vector<CellKey> touched_cells;

	// cell added since  record_cells_set_modification was set to true
	std::vector<CellKey> added_cells;

	// cell removed since  record_cells_set_modification was set to true
	std::vector<CellKey> removed_cells;

	// start recording  cells_set_modification
	void RecordCellsSetModification();

	// stop recording  cells_set_modification
	void StopRecordCellsSetModification();

	// if record_cells_set_modification is recording record key ahs been touched
	void MarkTouched(const CellKey & key );

	// if record_cells_set_modification is recording record key ahs been touched and removed
	void MarkRemoved(const CellKey & key );

	// if record_cells_set_modification is recording record key ahs been touched and removed
	void MarkTouchedAdded(const CellKey & key );

	// Return the parent of a CellKey
	CellKey Parent(const CellKey & ck);

	// Return the children of a CellKey
	void Children(const CellKey & ck, std::vector<CellKey> & children);

	// Return the 0..7 order  of a CellKey with respect to the father
	unsigned char  ChildrenBit(const CellKey & ck);

	// return the cells overlapped by a given Box4 (still unused)
	void OverlappingBBoxes4(Cell * cl, const Box4 & b, const int & h,std::vector<CellKey> & overlapping);

	// return the cells overlapped by a given vcg::Box3f
	void OverlappingBBoxes( const vcg::Box3f & b, const int & h,std::vector<CellKey> & overlapping);

	// return the cells overlapped by a given vcg::Box3f
	void CreateDependence(Cell * & c1, Cell * & c2);

	// compute at which level should be put something that is long "l"
	int ComputeLevel(const float & l);

	// compute at which level should be put the face f (clamped to sr)	
	template <class MeshType> 
	inline int ComputeLevel( typename MeshType::FaceType & f );

	// compute at which level should be put the face f (clamped to sr)	
	template <class MeshType> 
	inline int ComputeLevel( typename MeshType::FaceType & f, ScaleRange & sr );

	// create a new cell
	Cell* NewCell(const CellKey & key);

	// get a cell (optionally create it)
	Cell* GetCell(const CellKey & key, bool ifnot_create = true);

	// same as GetCell but also initialize the data structure to cumulate 
	// samples for impostor  
	Cell* GetCellC(const CellKey & key, bool ifnot_create = true);

	// remove am empty cell (it must be empty)
	void RemoveCell(const CellKey & key);

	// reset an impostor
	void RemoveImpostor(const CellKey & key);

	// given a cellkey and the name of the atribute it returns the name
	// of the chain in the OOCEnv
	std::string NameOfChain(const CellKey & key,const std::string  & chain_name);

	// add an attribute
	template <class ATTR_TYPE>
	Chain <ATTR_TYPE>  * AddStringChain( Cell::StringChainMap *sce, std::string name, Cell * );

	template <class ATTR_TYPE>
	Chain <ATTR_TYPE>  * AddElement( std::string name, Cell * );

	template <class ATTR_TYPE>
	Chain <ATTR_TYPE>  * AddPerVertexAttribute( std::string name, Cell * );

	template <class ATTR_TYPE>
	Chain <ATTR_TYPE>  * AddPerFaceAttribute( std::string name, Cell * );

	void GetCellsAttributes(std::vector<Cell*> cells, AttributeMapper & am);


//	template <class ATTR_TYPE>
//	Chain <ATTR_TYPE>  * AddPerUserElementAttribute( std::string name, Cell * );

	/*
	Compute the ScaleRange of a mesh
	*/
	template <class MeshType>
	ScaleRange ScaleRangeOfMesh( MeshType & m);

	/*
	Compute the ScaleRange of a vector of triangles
	*/
	template <class S>
	ScaleRange ScaleRangeOfTris( std::vector<vcg::Point3<vcg::Point3<S> > > & tris);

	/*
	Add a mesh to the database
	*/
	template <class MeshType>
	void AddMesh( MeshType & m, AttributeMapper  = AttributeMapper() );

	/*
	  Make sure the cell c contains all the attributes in attr_map
	*/
	void UpdateCellsAttributes(Cell * , const AttributeMapper & attr_map);

	/*
	Compute the impostor for iven cell
	*/
	void ComputeImpostors();

	/*
	make a mesh with all the impostors
	*/
	void ImpostorsToMesh(vcgMesh & m);

	/*
	clear the impostor for the passed cells and reflect the change in the hierarchy
	*/
	void ClearImpostors(std::vector<CellKey> & cks);

	/*
	Given a cell, update its children bit by zeroing the bit corresponding to deleted cells.
	if more than half the children (>=4) have been changed return true (it means its impostor
	must be recomputed/
	*/
	bool ClearImpostor(const CellKey & ck);

	/*
	Compute the samples for a specified set of cells
	*/
	void FillSamples(std::vector<CellKey> & cs);

	/*
	Given a cell, compute its impostor taking into account its faces and the impostors of its
	children. It ASSUMES that the impostor of the children are up to date.
	Return true if the occupancy is lower than a given threshold (stop the bottom up constuction)
	*/
	bool UpdateImpostor(const CellKey & ck);

	/*
	Build impostor hierarchy (and useful calls)
	*/
	void BuildImpostorsHierarchy(std::vector<CellKey> & fromCells);

	void BuildImpostorsHierarchy(std::vector<Cell*> & fromCells);
	void BuildImpostorsHierarchy(CellsContainer & fromCells);
	void BuildImpostorsHierarchy();

	/*
	Partial rebuild of the hierarchy
	*/
	bool UpdateImpostorPartial(const CellKey & ck);
	void BuildImpostorsHierarchyPartial(std::vector<CellKey> & fromCells);
	void BuildImpostorsHierarchyPartial(std::vector<Cell*> & fromCells);


	/* move the face from GIndex from to cell with CellKey to new_c and update GIndex gposv
	  Update from with the new GINdex
	*/
	void MoveFace(GIndex & from, const CellKey &  to);

	/* Recompute the scale range of a cell 
		All the operations are done to guarantee conservatively the Dependence Property (see comment of ScaleRance in file cell.h),
		which bring to big scale ranges and to poor performance.
		This function takes a cell (first version, then a set of cells) and recompute the scale range
	*/
	void ComputeScaleRange(Cell * c);

	/*
	++++++++++++++++++++++++++++++++ Rendering ++++++++++++++++++++++++++++++
	*/

	RenderCachesController renderCache;

	struct RenderParams{
		bool one_level,only_impostors,render_subcells,visitOn;
		unsigned int level;
		unsigned int memory_limit_in_core;
		unsigned int memory_limit_video;

	};
	RenderParams renderParams;

	void InitRender();

	/* 
	this is the default renderer, i.e. which renders the vcgMesh type of mesh. This is used to
	show the part of the mesh that are interactively loaded. Note that a user may edit a mesh
	with other component that, when updated, will be rendered by another class. This is just 
	a placeholder for a more sophisticated OcmeRenderer class that use 
	impostors       to produce a proxy of the real data
	*/
	vcg::GlTrimesh<vcgMesh> default_renderer;

	// modelview and projection matrix
	vcg::Matrix44f mm,pm;
	int vp[4];

	// frustum in world space
	vcg::Point3f frustum[8];

	// selection frustum in world space
	vcg::Point3f sel_frustum[8];
	// selection frustum in view space
	vcg::Point4f sel_corners[8];

	// returns the cells selected by the user and lock them (do not allow to be drop)
	void Select(std::vector<Cell*> & selected);
	// deselect previously locked cells
	void DeSelect(std::vector<Cell*> & selected);

	/* update the frustum in world space */
	void ConvertFrustumView2World(vcg::Point4f * fv,vcg::Point3f * fw);

	// visit the hierarchy of impostor

	/* check if the cell c must be refined and with which priority */
	bool  IsToRefineScreenErr(Cell * & c,float & pri);
	/* check if the cells is in the view frustum */
	bool IsInFrustum(Cell * c, vcg::Point3f * f, float &d, bool bsphere_or_bbox = true);
	/* perform the hierarchical visit*/
	void  Visit(CellKey root, std::vector<Cell*> & to_render);
	void  Visit(std::vector<Cell*> & to_render);



	// DiskLoader
	// DiskLoader * disk_loader;
	void NeedCell(Cell*c);

	// we stop the rendering before the committ and flush the cache.
	// It is a bit rough but we avoid at once the problems with cells
	// in the cells_to_render queue that are deleted by the commit.

	QMutex render_mutex;
	void StopAndFlushRendering();
	void StartRendering();

	// render the dataset
	std::vector<Cell*> cells_to_render;
	void Render(int);

	// splatting apss rendering for the impostors
	SplatRenderer<vcgMesh> splat_renderer;

	// roots of the octree forest
	std::list<Cell*> octree_roots;

	/*
	++++++++++++++++++++++++++++++ END  Rendering ++++++++++++++++++++++++++++++
	*/

	/*
	++++++++++++++++++++++++++++++++ Edit/Commit ++++++++++++++++++++++++++++++
	*/

	/*
		keep trace of the faces and vertices taken for editing.
		These containers are used to find out which element have been deleted by difference
	*/
	std::vector<GIndex> edited_faces;
        std::vector<GISet>  edited_vertices;
        std::vector<Cell*>  toCleanUpCells;					// cells that contain deleted elements

	// Build a mesh with all the faces of a single cell whose vertices are also
	// in the same cell (only debug purpose)
	template <class MeshType>
	void ExtractContainedFacesFromASingleCell(CellKey ck , MeshType & m);

	// Build a mesh with all the vertices in the cells passed as argument
	template <class MeshType>
	void ExtractVerticesFromASingleCell( CellKey ck , MeshType & m);

	// Build a mesh with all the faces and vertices in the cells passed as argument
	template <class MeshType>
	void Extract(  std::vector<Cell*> & cells, MeshType & m,  AttributeMapper attr_map = AttributeMapper() );

	// Build a mesh where the elements contained in cells may be edited
	// If the element sum up to more than max_size do nothing and return false
	template <class MeshType>
	bool Edit(  std::vector<Cell*> & cells, MeshType & m, unsigned int max_size,  AttributeMapper attr_map = AttributeMapper());

	// Undo edit
	void 	DropEdited();

	// find the removed elements
	template <class MeshType>
	void FindRemovedElements( MeshType & m,
                                typename MeshType::template PerVertexAttributeHandle<GISet> &gPosV,
                                typename MeshType::template PerFaceAttributeHandle<GIndex> & gPosF );

	// commit a mesh that was previously built with edit
	template <class MeshType>
	void Commit( MeshType & m, AttributeMapper attr_map = AttributeMapper() );

	// given a vector of cells, compute all the cells that must be loaded to be able
	// to edit the given cells (obviously the two sets will not be disjoint)
	void ComputeDependentCells( const std::vector<Cell*> & cells, std::vector<CellKey> & dep_cells);
	void ComputeDependentCells( const std::vector<Cell*> & cells, std::vector<Cell*>   & dep_cells);

	/*	compact vectors of cell elements [in the first version only faces and vertices]
		for all the cells passed as argument
		NOTE: this will imply to load and modify other cells  */

	/* not much to do, simply compact the relative chain 
	Note: this is because currently faces are not pointed by anyone
	*/
	void RemoveDeletedFaces(  std::vector<Cell*> & cells);
	void RemoveDeletedBorder(std::vector<Cell*> & cells);
 	void RemoveDeletedVertices(    std::vector<Cell*>   cells);


	/*
	Remove empty cells. This is clearup stuff. In a final release of the code
	it should never be necessary to call it.
	*/
	void RemoveEmptyCells(  );

	/* Build a mesh with all the faces and vertices in the database (meaningless in any real case,
		if you can do this you do not need OCME)
	*/
	template <class MeshType>
	void ExtractMesh( MeshType & m);




	// create a new OCME
	void Create(const char * name, unsigned int pagesize = 1024);

	// Open an existing OCME
	void Open(const char * name);

	// Close a previously opened OCME
	void Close(const bool & savebeforeclosing);

	// save to file
	void Save();

	// load from file
	void Load();

	void RecStats();

	/* serialization */
	int SizeOf();
	char * Serialize ( char * const);
	char * DeSerialize ( char * const);

	// save the impostor of cell "c"
	void SaveImpostor(Cell * c);

	// load the impostor of cell "c"
	void LoadImpostor(Cell * c);

	/*	UGLY
		see the comment in chain_mem.h
	*/
	void  * extMemHnd;

	/* --------- DATA VERIFY AND REPAIR ----------- */
	void Verify();

	/* --------- DATA VERIFY AND REPAIR ----------- */
	void ComputeStatistics();

	/* --------- DEBUG ------------ */
	/* here a number of functions to check the consistency of the dataset */

	// verify that vertex referred by not deleted faces are non deleted
	bool CheckFaceVertDeletions(Cell *);

	// verify if all the undeleted faces point to undeleted vertices 
	bool CheckFaceVertexAdj(Cell *);

	// verify that dep is really the dependent set of kn
	bool CheckDependentSet( std::vector<Cell*> &  dep);

	bool BorderExists(Cell* c,unsigned int vi);

};

#endif
