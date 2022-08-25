#ifndef __OCMECELL__
#define __OCMECELL__

/*
 Cell store the data of the cell that will be permanently stored in memory with 
 the chain, such as the dependences from/to other cells
 */
#include "vcg_mesh.h"
#include <map>
#include <vector>
#include <vcg/space/box3.h>
#include "../utils/logging.h"
#include "../utils/name_access_function_bounds.h"
#include "../ooc_vector/ooc_chains.h"
#include "FBool.h"
#include "boolvector.h"

#include <GL/glew.h>
#include <wrap/gl/gl_geometry.h>
#include <wrap/gl/trimesh.h>

extern Logging * olgn;

struct CellToken;
struct Impostor ;
/*
implementation of a cell
*/
struct CellKey{
	CellKey(){}
	CellKey(const int & _x, const int & _y, const int &  _z, const int & _h): x(_x),y(_y),z(_z),h(_h){}
	CellKey(const vcg::Point3f p, const int & _h): x(static_cast<int>(p[0])),y(static_cast<int>(p[1])),z(static_cast<int>(p[2])),h(_h){}
	int x,y,z,h;
	vcg::Point3i P3i() const {return vcg::Point3i(x,y,z);}
	vcg::Point3f P3f() const {return vcg::Point3f(static_cast<float>(x),static_cast<float>(y),static_cast<float>(z));}
	vcg::Point3f GP3f() const { float lev = (h>=0)?(float) (1<<h): 1.f/(1<<-h);
								return vcg::Point3f(static_cast<float>(x*lev),static_cast<float>(y*lev),static_cast<float>(z*lev));}
	vcg::Box3f   BBox3f() const {vcg::Box3f b; b.Add(GP3f()); b.Add(CellKey(x+1,y+1,z+1,h).GP3f()); return b;}
        bool  operator == (const CellKey & o) const {return (x==o.x)&&(y==o.y)&&(z==o.z)&&(h==o.h);}
        bool  operator < (const CellKey & o) const {return (x==o.x)?(y==o.y)?(z==o.z)?(h<o.h):(z<o.z):(y<o.y):(x < o.x);}
	// h = i -> dx == dy == dz == 2^i
};

/* GIndex is a global index on the hashed multigrid  data structure*/
struct GIndex{
	GIndex() {SetUnassigned();}
	GIndex(const CellKey & c, const int & ii):ck(c),i(ii){}
	CellKey ck; // cell
	int i;		// order inside cell
        bool operator ==(const GIndex & b) const {return ( ck==b.ck) &&( i == b.i);}
        bool operator < (const GIndex & b) const {return ( (ck == b.ck)? (i<b.i) : (ck < b.ck));}

	void SetUnassigned(){ i = -1; }
	bool IsUnassigned()	{return  (i==-1);}

};


/* GISet is the set of GIndex corresponding to the same vertex*/
struct GISet{
    std::map<CellKey,unsigned int > giset;
	unsigned int bi;
	typedef std::map<CellKey,unsigned int >::iterator CopiesIterator;
    typedef std::map<CellKey,unsigned int >::iterator iterator;

    void Add(GIndex gi){ giset.insert(std::make_pair(gi.ck,gi.i));}
	void Add(std::pair <CellKey,unsigned int > p){giset.insert(p);}
    void Add(GISet &gis){ giset.insert(gis.giset.begin(),gis.giset.end());}
    void Clear(){giset.clear();}
    bool IsUnassigned(){return giset.empty();}

    iterator begin(){return giset.begin();}
    iterator end(){return giset.end();}

    const  bool   operator < (const GISet & o) const {
        return giset<o.giset;
    }

    void sub(GISet & o){
        for(iterator i = o.begin(); i!=o.end(); ++i)
            giset.erase((*i).first);
    }

	unsigned int &  BI(){return bi;}

    int Index(CellKey ck){
        iterator gi = giset.find(ck);
        if(gi==giset.end()) return -1;
        else
            return (*gi).second;
    }
};

/*
 *** Dependence Property ***
	
	For each face if holds that the bounding boxes of the cells that intersect the bounding box of the face
	include the face.
	In other terms: if cell A is crossed by the BB of face F, than the BB of cell A must contain the face F.

	This must be true for all the levels BUT we use ScaleRange to limit the property to the cells that possibly
	contain element in common with f (references to vertex of f)

*/

struct ScaleRange{
	ScaleRange():min(1),max(-1){}
	ScaleRange( const int & _min, const int &  _max):min(_min),max(_max){}
	ScaleRange Add(const int & m, const  int &  M){ 
		if(IsNull()){min=m;max=M;} else {this->min = std::min(this->min,m); this->max = std::max(this->max,M);}  return (*this);}
	ScaleRange Add(const ScaleRange &  sr){return Add(sr.min,sr.max);}
	ScaleRange Add(const int &  h){ if(this->IsNull()) {this->min = this->max = h;}else{
												if(h>this->max) this->max = h; else
												if(h<this->min) this->min = h; 
												}
									return (*this);
								}

	bool Include(const int &  h){return ( (h<=this->max) && (h >= this->min) );} 
	bool IsNull(){return min>max;}
	bool operator ==(const ScaleRange & sr)const {return (min == sr.min) && (max==sr.max);}
	int min,max;
};

struct Box4{
	Box4(){bbox3.SetNull();};
	Box4(vcg::Box3f _bbox3, ScaleRange _sr):bbox3(_bbox3),sr(_sr){}
	Box4 Add(vcg::Box3f b3, ScaleRange _sr ){ bbox3.Add(b3); sr.Add(_sr); return (*this);}
	Box4 Add(vcg::Point3f p, int h ){ bbox3.Add(p); sr.Add(h); return (*this);}
	vcg::Box3f bbox3;
	ScaleRange sr;
	bool operator ==(const Box4 & b) const {return (bbox3==b.bbox3) && (sr==b.sr);}
};

struct BorderIndex{
	BorderIndex(){}
	BorderIndex(unsigned int _vi,unsigned int _bi):vi(_vi),bi(_bi){}
	unsigned int 
		vi,		// pointer to a border vertex in the cell
		bi;		// its incremental mark
};

/* per cell auxiliary data structure for edit& commit */
struct EditCommitAuxData{
	BoolVector deleted_face;
	BoolVector deleted_vertex;
	BoolVector deleted_border;

	FBool is_in_kernel;
	FBool locked;
};

/* per cell auxiliary data structure for rendering */
struct RenderAuxData{
	RenderAuxData():renderCacheToken(0),_mesh(0){}
	~RenderAuxData(){if(_mesh) delete _mesh;}
	FBool impostor_updated;
	FBool to_render;

	CellToken * renderCacheToken;

	float priority;

	vcgMesh & Mesh(){ if(_mesh==0) _mesh = new vcgMesh(); return *_mesh;}
	void ClearMesh(){if(_mesh==0) return; _mesh->Clear(); delete _mesh; _mesh = 0;}
private:
	vcgMesh * _mesh;

};

struct Cell{

	Cell ():ecd(0),impostor(0),rd(0) {}
	Cell(CellKey ck);
	~Cell();

	// position of the cell
	CellKey key; 

	typedef std::map<std::string, ChainBase *  > StringChainMap;

	// the list of all the elements that are stored in this cell
	// faces and vertices will be always among the elements, oher
	// can be added (edge, string...whatever)
	std::map<std::string, ChainBase *  > elements;

	// the attributes are special element that are bound to those in elements
	// The normal per vertex, the normal per face and so on.
	std::map<std::string, ChainBase *  > perVertex_attributes;
	std::map<std::string, ChainBase *  > perFace_attributes;


	/* Get / Set attribute values by name
	   TODO: to cache the pointer to the chain. In this implementation each single access to an attributes
		   requires a find on a map
	*/

	/// Vertex attributes
	void GetVertexAttribute(std::string name, const unsigned int & pos, void * dst);
	void SetVertexAttribute(std::string name, const unsigned int & pos, void * src);

	/// Face attributes
	void GetFaceAttribute(std::string name, const unsigned int & pos, void * dst);
	void SetFaceAttribute(std::string name, const unsigned int & pos, void * src);

	/// query available attributes
	void GetPerVertexAttributeList(std::vector<std::string> & attr_list);
	void GetPerFaceAttributeList(std::vector<std::string> & attr_list);

	// remove specified faces
	void RemoveFaces(std::vector<unsigned int> toDelete);

	// remove specified vertices
	void RemoveVertices(std::vector<unsigned int> toDelete);

	// basic  elements
	/* all the faces assigned to this cell. NOTE: this is a shortcut, "face" is also in this->elements */
	Chain<OFace>    *face;

	/* all the vertices assigned to this cell. NOTE: this is a shortcut, "vert" is also in this->elements */
	Chain<OVertex>  *vert;

	/* border vertices */
	Chain<BorderIndex> *border;

	// auxiliary data needed for edit/Commit
	EditCommitAuxData	*	ecd;

	// init/clear auxiliary data needed for edit/Commit
	void InitEditCommitAuxData();
	void ClearEditCommitAuxData();


	// auxiliary data needed for rendering
	RenderAuxData		*	rd;

	// init/clear auxiliary data needed for rendering
	void InitRenderAuxData();
	void ClearRenderAuxData();

	// return the size in RAM one the cell is loaded
	int SizeInRAM();

	/* generic fast bool value*/
	FBool generic_bool;

	// check if the attribute named attr_name among the elements
	//bool HasAttribute( std::string attr_name);

	// check if it is empty
	bool IsEmpty();
 
	// bounding box of all the triangles that cross the cell
	Box4 bbox;

	// dependence set. All the cells that must be loaded when this one has to be edited
	std::set<CellKey> dependence_set;

	// add a face
	int AddFace(OFace );

	// add a vertex
	unsigned int AddVertex(OVertex );


	template <class VertexType>
	void ExportVertexAttribute(std::string attr_name, const unsigned int & i, VertexType & v );

	template <class FaceType>
	void ExportFaceAttribute(std::string attr_name, const unsigned int & i, FaceType & f );

	template <class VertexType>
	void ImportVertexAttribute(std::string attr_name, const unsigned int & i, VertexType & v );

	template <class FaceType>
	void ImportFaceAttribute(std::string attr_name, const unsigned int & i, FaceType & f );

//	template <class VertexType>
//	void GetVertexAttribute(std::string name, const unsigned int & pos, VertexType & dst){
//		std::map<std::string, ChainBase *  > ::iterator ii = perVertex_attributes.find(name); // find the chain
//		RAssert(ii != perVertex_attributes.end());
//		void * dstPtr;
//		(*ii).second->GetValue(pos,dstPtr);
//		nafb::Get(name,dst,(*ii))
//		/*nafb::Call(name,dst,)
//		(*ii).second->GetValue(pos,dst)*/;
//	}

	Impostor  * impostor;
	

	/* serialization */
	int SizeOf();
	char * Serialize (char * );
	char * DeSerialize (char * );
};

#endif
