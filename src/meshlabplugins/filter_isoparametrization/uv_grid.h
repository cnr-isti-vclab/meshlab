#ifndef UV_GRID
#define UV_GRID

template <class MeshType>
class UVGrid
{
	typedef typename MeshType::FaceType FaceType;
	typedef typename MeshType::VertexType VertexType;
	typedef typename MeshType::CoordType CoordType;
	typedef typename MeshType::ScalarType ScalarType;

	std::vector<std::vector<std::vector<FaceType*> > >  data;
	vcg::Point2<ScalarType> min;
	ScalarType edgeX,edgeY;
	int samples_edge;
public:
	vcg::Box2<ScalarType> bbox2;
private:
	
	vcg::Point2<int> Cell(const vcg::Point2<ScalarType> &UV)
	{
		vcg::Point2<ScalarType> swap=UV;
		swap-=min;
		swap.X()/=edgeX;
		swap.Y()/=edgeY;
		vcg::Point2<int> ret((int)floor(swap.X()),(int)floor(swap.Y()));
		return (ret);
	}

	void Add(const int &x,const int &y,FaceType* f)
	{
		data[x][y].push_back(f);
	}

public:
	/*void Init(MeshType& mesh,int &size_edge)
	{		
		if (size_edge<2)
			size_edge=2;

		samples_edge=size_edge;
		///allocate space
		data.resize(size_edge);
		for (int i=0;i<size_edge;i++)
			data[i].resize(size_edge);

		///get the bbox of UV
		
		for (unsigned int i=0;i<mesh.vert.size();i++)
			bbox2.Add(mesh.vert[i].T().P());

		///add borders
		ScalarType tempX=(bbox2.DimX()/(ScalarType)size_edge)*0.5;
		ScalarType tempY=(bbox2.DimY()/(ScalarType)size_edge)*0.5;
		bbox2.min-=vcg::Point2<ScalarType>(tempX,tempY);
		bbox2.max+=vcg::Point2<ScalarType>(tempX,tempY);

		///get edgesize
		min=bbox2.min;
		edgeX=bbox2.DimX()/(ScalarType)size_edge;
		edgeY=bbox2.DimY()/(ScalarType)size_edge;

		///add pointer to face
		for (unsigned int i=0;i<mesh.face.size();i++)
		{
			vcg::Box2<ScalarType> UVBox;
			UVBox.Add(mesh.face[i].V(0)->T().P());
			UVBox.Add(mesh.face[i].V(1)->T().P());
			UVBox.Add(mesh.face[i].V(2)->T().P());
			///get index of intersected cells
			vcg::Point2<int> minIndex=Cell(UVBox.min);
			vcg::Point2<int> maxIndex=Cell(UVBox.max);
			for (int x=minIndex.X();x<=maxIndex.X();x++)
				for (int y=minIndex.Y();y<=maxIndex.Y();y++)
					Add(x,y,&mesh.face[i]);
		}
	}*/
	
	void Init(MeshType &mesh,int size_edge=-1)
	{		
		if (size_edge==-1)
			size_edge=(int)sqrt((double)mesh.fn);

		if (size_edge<2)
			size_edge=2;

		samples_edge=size_edge;
		///allocate space
		data.resize(size_edge);
		for (int i=0;i<size_edge;i++)
			data[i].resize(size_edge);

		///get the bbox of UV
		
		for (unsigned int i=0;i<mesh.vert.size();i++)
			bbox2.Add(mesh.vert[i].T().P());

		///add borders
		ScalarType tempX=(bbox2.DimX()/(ScalarType)size_edge)*(ScalarType)0.5;
		ScalarType tempY=(bbox2.DimY()/(ScalarType)size_edge)*(ScalarType)0.5;
		bbox2.min-=vcg::Point2<ScalarType>(tempX,tempY);
		bbox2.max+=vcg::Point2<ScalarType>(tempX,tempY);

		///get edgesize
		min=bbox2.min;
		edgeX=bbox2.DimX()/(ScalarType)size_edge;
		edgeY=bbox2.DimY()/(ScalarType)size_edge;

		///add pointer to face
		for (unsigned int i=0;i<mesh.face.size();i++)
		{
			vcg::Box2<ScalarType> UVBox;
			UVBox.Add(mesh.face[i].V(0)->T().P());
			UVBox.Add(mesh.face[i].V(1)->T().P());
			UVBox.Add(mesh.face[i].V(2)->T().P());
			///get index of intersected cells
			vcg::Point2<int> minIndex=Cell(UVBox.min);
			vcg::Point2<int> maxIndex=Cell(UVBox.max);
			for (int x=minIndex.X();x<=maxIndex.X();x++)
				for (int y=minIndex.Y();y<=maxIndex.Y();y++)
					Add(x,y,&mesh.face[i]);
		}
	}

	///return true if exist a face in witch the point falls
	///and return the face and barycentric coordinates
	bool ProjectPoint(const vcg::Point2<ScalarType> &UV,
					std::vector<FaceType*> &face,
					std::vector<CoordType> &baryVal)
	{
    typedef typename MeshType::ScalarType ScalarType;
		if (!bbox2.IsIn(UV))
			return false;
		
		//const ScalarType _EPSILON = ScalarType(0.000000001);
		vcg::Point2i cell=Cell(UV);
		if (cell.X()>=(int)data.size())
			cell.X()-=1;
		if (cell.Y()>=(int)data.size())
			cell.Y()-=1;
		if (cell.X()<0)
			cell.X()=0;
		if (cell.Y()<0)
			cell.Y()=0;

		for (unsigned int i=0;i<data[cell.X()][cell.Y()].size();i++)
		{
			FaceType *f=data[cell.X()][cell.Y()][i];
			vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
			vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
			vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
			vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
			//ScalarType area=(tex1-tex0)^(tex2-tex0);
			CoordType bary;
			///then find if the point 2d falls inside
			//if ((area>_EPSILON)&&(t2d.InterpolationParameters(UV,bary.X(),bary.Y(),bary.Z())))
			if (t2d.InterpolationParameters(UV,bary.X(),bary.Y(),bary.Z()))
			{
				face.push_back(f);
				baryVal.push_back(bary);
				//return true;
			}
		}
		return (face.size()>0);
	}

	bool getClosest(const vcg::Point2<ScalarType> &UV,
		FaceType* &face,
		CoordType &baryVal)
	{

		ScalarType dist=100.0;

		if (!bbox2.IsIn(UV))
			return false;

		//const ScalarType _EPSILON = ScalarType(0.000000001);
		vcg::Point2i cell=Cell(UV);
		///test for each face
		for (unsigned int i=0;i<data.size();i++)
			for (unsigned int j=0;j<data[i].size();j++)
				for (unsigned int k=0;k<data[i][j].size();k++)
				{
					FaceType *f=data[i][j][k];
					vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
					vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
					vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
					vcg::Triangle2<ScalarType> t2d=vcg::Triangle2<ScalarType>(tex0,tex1,tex2);
					vcg::Point2<ScalarType> closest;
					ScalarType dist_test;
					t2d.PointDistance(UV,dist_test,closest);
					if (dist_test<dist)
					{
						dist=dist_test;
						face=f;
						#ifndef NDEBUG
						bool found=t2d.InterpolationParameters(closest,baryVal.X(),baryVal.Y(),baryVal.Z());
						assert(found);
						#else
						t2d.InterpolationParameters(closest,baryVal.X(),baryVal.Y(),baryVal.Z());
						#endif
					}
				}
				return (true);
	}

	bool CoordinatesPoint(const vcg::Point2<ScalarType> &UV,
						  CoordType &AbsCoord)
	{
		
		std::vector<FaceType*> faces;
		std::vector<CoordType> baryVal;
		bool found=ProjectPoint(UV,faces,baryVal);
		
		if (!found)
			return false;
		
		AbsCoord=CoordType(0,0,0);
		for (int i=0;i<faces.size();i++)
		{
			FaceType *f=faces[i];
			//if (!RestPos)
				AbsCoord+=InterpolatePos(f,baryVal[i]);
			/*else
				AbsCoord+=InterpolateRPos(f,baryVal[i]);*/
		}
		AbsCoord/=(ScalarType)faces.size();
		
		return true;
	}

	bool CoordinatesPointUnique(const vcg::Point2<ScalarType> &UV,
								CoordType &AbsCoord)
	{
		const ScalarType _EPSILON = ScalarType(0.00001);
		std::vector<FaceType*> faces;
		std::vector<CoordType> baryVal;
		bool found=ProjectPoint(UV,faces,baryVal);
		if (!found)
			return false;
		AbsCoord=CoordType(0,0,0);
		for (unsigned int i=0;i<faces.size();i++)
		{
			FaceType *f=faces[i];
			vcg::Point2<ScalarType> tex0=vcg::Point2<ScalarType>(f->V(0)->T().U(),f->V(0)->T().V());
			vcg::Point2<ScalarType> tex1=vcg::Point2<ScalarType>(f->V(1)->T().U(),f->V(1)->T().V());
			vcg::Point2<ScalarType> tex2=vcg::Point2<ScalarType>(f->V(2)->T().U(),f->V(2)->T().V());
			ScalarType area=(tex1-tex0)^(tex2-tex0);
			///then find if the point 2d falls inside
                        typename MeshType::CoordType bary;
			if (area>_EPSILON)
			{
				AbsCoord=InterpolateRPos(f,baryVal[i]);
				return true;
			}
			/*else
				AbsCoord+=InterpolateRPos(f,baryVal[i]);*/
		}
		//AbsCoord/=(ScalarType)faces.size();
		/*int num;
		num=faces.size();
		printf("NUmb %d\n",num);*/
		return false;
	}
};
#endif
