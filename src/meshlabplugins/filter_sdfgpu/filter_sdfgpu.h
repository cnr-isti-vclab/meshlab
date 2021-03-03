#ifndef FILTER_SDFGPU_H
#define FILTER_SDFGPU_H

#include <QObject>

#include <common/plugins/interfaces/filter_plugin.h>

#include <gpuProgram.h>
#include <framebufferObject.h>
#include <texture2D.h>

enum ONPRIMITIVE{ON_VERTICES=0, ON_FACES=1};

class SdfGpuPlugin : public QObject, public FilterPlugin
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(FILTER_PLUGIN_IID)
	Q_INTERFACES(FilterPlugin)
	
	public:
		
		enum{ SDF_SDF, SDF_DEPTH_COMPLEXITY, SDF_OBSCURANCE };
	
	SdfGpuPlugin();
	
	QString pluginName() const;
	
	QString filterName(FilterIDType filterId) const;
	
	QString filterInfo(FilterIDType filterId) const;
	
	FilterClass getClass(const QAction *) const
	{
		return FilterPlugin::VertexColoring;
	}
	
	FILTER_ARITY filterArity(const QAction* act) const;
	
	bool requiresGLContext(const QAction* action) const;
	
	//Main plugin function
	bool applyFilter(const QAction* filter, MeshDocument &md, std::map<std::string, QVariant>& outputValues, unsigned int& postConditionMask, const RichParameterList & par, vcg::CallBackPos *cb);
	
	//Parameters init for user interface
	virtual void initParameterList(const QAction* action, MeshModel &m, RichParameterList &parlst);
	
	//Draw the mesh
	void fillFrameBuffer(bool front,  MeshModel* mm);
	
	//Mesh setup
	void setupMesh(MeshDocument& md, ONPRIMITIVE onprim );
	
	//Init OpenGL context
	bool initGL(MeshModel& mm);
	
	//OpenGL clean up
	void releaseGL(MeshModel &m);
	
	//Setup camera orientation
	void setCamera(vcg::Point3f camDir, vcg::Box3f meshBBox);
	
	//Calculate sdf or obscurance along a ray
	void TraceRay(const QAction* action, int peelingIteration, const vcg::Point3f& dir, MeshModel* mm );
	
	//Enable depth peeling shader
	void useDepthPeelingShader(FramebufferObject* fbo);
	
	//Position and normal of each vertex are copied to two separate texture, to be used in sdf and obscurance GPU calculation
	void vertexDataToTexture(MeshModel &m);
	
	void faceDataToTexture(MeshModel &m);
	
	//Sdf calculation for each depth peeling iteration
	void calculateSdfHW(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* fboPrevBack, const vcg::Point3f& cameraDir );
	
	//Copy sdf values from result texture to the mesh (vertex quality)
	void applySdfPerVertex(MeshModel &m);
	
	//Copy sdf values from result texture to the mesh (face quality)
	void applySdfPerFace(MeshModel &m);
	
	
	//Obscurance calculation for each depth peeling iteration
	void calculateObscurance(FramebufferObject* fboFront, FramebufferObject* fboBack, FramebufferObject* nextFront, const vcg::Point3f& cameraDir, float bbDiag );
	
	//Copy obscurance values from result texture to the mesh (vertex color)
	void applyObscurancePerVertex(MeshModel &m, float numberOfRays);
	
	//Copy obscurance values from result texture to the mesh (face color)
	void applyObscurancePerFace(MeshModel &m, float numberOfRays);
	
	void preRender(unsigned int peelingIteration);
	
	bool postRender(unsigned int peelingIteration);
	
protected:
	
	ONPRIMITIVE        mOnPrimitive;
	unsigned int       mResTextureDim;
	unsigned int       mNumberOfTexRows; //the number of rows of a texture actually used for the result texture
	FloatTexture2D*    mVertexCoordsTexture;
	FloatTexture2D*    mVertexNormalsTexture;
	FramebufferObject* mFboResult;    //Fbo and texture storing the result computation
	FloatTexture2D*    mResultTexture;
	FloatTexture2D*    mDirsResultTexture;
	FramebufferObject* mFboArray[3];  //Fbos and textures for depth peeling
	FloatTexture2D*    mDepthTextureArray[3];
	FloatTexture2D*    mColorTextureArray[3];
	unsigned int       mPeelingTextureSize;
	float              mTolerance;
	float              mMinCos;
	float              mTau;      //obscurance exponent
	float              mMinDist;  //min dist between vertices to check too thin parts
	float              mScale;  //Scaling factor used to setup camera
	GPUProgram*        mDeepthPeelingProgram;
	GPUProgram*        mSDFProgram;
	GPUProgram*        mObscuranceProgram;
	bool               mRemoveFalse;
	bool               mRemoveOutliers;
	GLuint             mOcclusionQuery;
	GLuint             mPixelCount;
	unsigned int       mTempDepthComplexity;
	unsigned int       mDepthComplexity;
	bool               mDepthComplexityWarning;
	
	CMeshO::PerFaceAttributeHandle<vcg::Point3f>   mMaxQualityDirPerFace;
	CMeshO::PerVertexAttributeHandle<vcg::Point3f> mMaxQualityDirPerVertex;
	
};

#endif // FILTER_SDFGPU_H
