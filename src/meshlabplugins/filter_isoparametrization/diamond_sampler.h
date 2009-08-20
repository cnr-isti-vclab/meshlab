#ifndef _DIAMSAMPLER
#define _DIAMSAMPLER
#include <stat_remeshing.h>
#include <vcg/complex/trimesh/clean.h>

class DiamSampler{
	typedef IsoParametrization::CoordType CoordType;
	typedef IsoParametrization::ScalarType ScalarType;

	std::vector<std::vector<std::vector<CoordType> > > SampledPos;
	IsoParametrization *isoParam;
	unsigned int sampleSize;

	void DeAllocatePos()
	{
		///positions
		for (unsigned int i=0;i<SampledPos.size();i++)
		{
			for (unsigned int j=0;j<SampledPos[i].size();j++)
				SampledPos[i][j].clear();
			SampledPos[i].clear();
		}
		SampledPos.clear();
	}

	void AllocatePos(const int & sizeSampl)
	{
		///positions
		AbstractMesh *domain=isoParam->AbsMesh();
		int num_edges=0;
		for (unsigned int i=0;i<domain->face.size();i++)
		{
			AbstractFace *f=&domain->face[i];
			for (int j=0;j<3;j++)
				if (f->FFp(j)>f)
					num_edges++;
		}

		SampledPos.resize(num_edges);
		for (unsigned int i=0;i<SampledPos.size();i++)
		{
			SampledPos[i].resize(sizeSampl);
			for (unsigned int j=0;j<SampledPos[i].size();j++)
				SampledPos[i][j].resize(sizeSampl);
		}
	}

	CoordType AveragePos(const std::vector<ParamFace*> &faces,
		std::vector<CoordType> &barys)
	{
		CoordType pos=CoordType(0,0,0);
		for (unsigned int i=0;i<faces.size();i++)
		{
			pos+=(faces[i]->V(0)->P()*barys[i].X()+
				faces[i]->V(1)->P()*barys[i].Y()+
				faces[i]->V(2)->P()*barys[i].Z());
		}
		pos/=(ScalarType)faces.size();
		return pos;
	}

	int n_diamonds;
	int inFace;
	int inEdge;
	int inStar;
	int n_merged;
public:

	///initialize the sampler 
	void Init(IsoParametrization *_isoParam)
	{
		isoParam=_isoParam;
	}

	template <class OutputMesh>
	void GetMesh(OutputMesh &SaveMesh)
	{

		typedef typename OutputMesh::FaceType MyFace;
		typedef typename OutputMesh::VertexType MyVertex;

		SaveMesh.Clear();

		SaveMesh.vert.reserve(SampledPos.size()*
			sampleSize*
			sampleSize);

		SaveMesh.face.reserve(SampledPos.size()*
			(sampleSize-1)*
			(sampleSize-1)*2);

		SaveMesh.vn=0;
		SaveMesh.fn=0;

		///suposed to be the same everywhere
		///allocate space
		std::vector<std::vector<MyVertex*> > vertMatrix;
		vertMatrix.resize(sampleSize);
		for (unsigned int i=0;i<sampleSize;i++)
			vertMatrix[i].resize(sampleSize);

		///add vertices
		for (unsigned int i=0;i<SampledPos.size();i++)
		//for (unsigned int i=11;i<12;i++)
		{		
			///add vertices & positions
			for (unsigned int j=0;j<sampleSize;j++)
				for (unsigned int k=0;k<sampleSize;k++)
				{

					SaveMesh.vert.push_back(MyVertex());
					SaveMesh.vert.back().P()=SampledPos[i][j][k];
					vertMatrix[j][k]=&SaveMesh.vert.back();
					SaveMesh.vn++;
				}
				/*printf("samppling %d\n",i);*/
				///add faces
				for (unsigned int j=0;j<sampleSize-1;j++)
					for (unsigned int k=0;k<sampleSize-1;k++)
					{


						MyVertex *v0=vertMatrix[j][k];
						MyVertex *v1=vertMatrix[j+1][k];
						MyVertex *v2=vertMatrix[j+1][k+1];
						MyVertex *v3=vertMatrix[j][k+1];

						SaveMesh.face.push_back(MyFace());
						SaveMesh.face.back().V(0)=v0;
						SaveMesh.face.back().V(1)=v1;
						SaveMesh.face.back().V(2)=v3;

						SaveMesh.face.push_back(MyFace());
						SaveMesh.face.back().V(0)=v1;
						SaveMesh.face.back().V(1)=v2;
						SaveMesh.face.back().V(2)=v3;

						SaveMesh.fn+=2;
					}

		}
		///get minimum edge size
		ScalarType minE,maxE;
		MaxMinEdge<OutputMesh>(SaveMesh,minE,maxE);
		/*int num_tri=SampledPos.size()*sampleSize*sampleSize*2;
		ScalarType Area_mesh=Area<OutputMesh>(SaveMesh);
		ScalarType Edge=sqrt((((Area_mesh/(ScalarType)num_tri)*4.0)/(ScalarType)sqrt(3.0)));*/
		n_merged=vcg::tri::Clean<OutputMesh>::MergeCloseVertex(SaveMesh,(ScalarType)minE*(ScalarType)0.9);
		vcg::tri::UpdateNormals<OutputMesh>::PerVertexNormalized(SaveMesh);
		/*Log("Merged %d vertices\n",merged);*/
	}

	//typedef enum SampleAttr{SMNormal, SMColor, SMPosition};

	///sample the parametrization
	void SamplePos(const int &size)
	{
		sampleSize=size;
		DeAllocatePos();//clear old data
		AllocatePos(size);	///allocate for new one
		inFace=0;
		inEdge=0;
		inStar=0;
		int global=0;

		///start sampling values
		/*Log("Num Diamonds: %d \n",SampledPos.size());*/
		
		
		for (unsigned int diam=0;diam<SampledPos.size();diam++)
			for (unsigned int j=0;j<sampleSize;j++)
				for (unsigned int k=0;k<sampleSize;k++)
				{
					vcg::Point2<ScalarType> UVQuad,UV;
					UVQuad.X()=(ScalarType)j/(ScalarType)(sampleSize-1);
					UVQuad.Y()=(ScalarType)k/(ScalarType)(sampleSize-1);
					int I;
					//printf("Quad: %d,%f,%f \n",diam,UV.X(),UV.Y());
					///get coordinate in parametric space
					isoParam->inv_GE1Quad(diam,UVQuad,I,UV);
					//printf("Alfabeta: %d,%f,%f \n",I,UV.X(),UV.Y());
					///and sample
					std::vector<ParamFace*> faces;
					std::vector<CoordType> barys;
					int domain=isoParam->Theta(I,UV,faces,barys);

					if (domain==0)
						inFace++;
					else
						if (domain==1)
							inEdge++;
						else
							if (domain==2)
								inStar++;

					global++;
					//printf("Find in domain: %d \n",domain);
					///store value
					CoordType val=AveragePos(faces,barys);
					/*if (domain==2)
					val=CoordType(0,0,0);*/
					SampledPos[diam][j][k]=val;
				}
				/*#ifndef _MESHLAB
				printf("In Face: %f \n",(ScalarType)inFace/(ScalarType)global);
				printf("In Diamond: %f \n",(ScalarType)inEdge/(ScalarType)global);
				printf("In Star: %f \n",(ScalarType)inStar/(ScalarType)global);
				#endif*/
				/*Log("In Face: %f \n",(ScalarType)inFace/(ScalarType)global);
				Log("In Diamond: %f \n",(ScalarType)inEdge/(ScalarType)global);
				Log("In Star: %f \n",(ScalarType)inStar/(ScalarType)global);*/
	}

	void getResData(int &_n_diamonds,int &_inFace,
					int &_inEdge,int &_inStar,int &_n_merged)
	{
		_n_diamonds=n_diamonds;
		_inFace=inFace;
		_inEdge=inEdge;
		_inStar=inStar;
		_n_merged=n_merged;
	}

	void GetCoords(std::vector<CoordType> &positions)
	{
		for (unsigned int diam=0;diam<SampledPos.size();diam++)
			for (unsigned int j=0;j<sampleSize;j++)
				for (unsigned int k=0;k<sampleSize;k++)
					positions.push_back(SampledPos[diam][j][k]);
	}


};
#endif