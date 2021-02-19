#ifndef ML_PLUGIN_GL_CONTEXT_H
#define ML_PLUGIN_GL_CONTEXT_H

#include "ml_shared_data_context.h"

class MLPluginGLContext : public QGLContext
{
public:
	MLPluginGLContext(
			const QGLFormat& frmt,
			QPaintDevice* dvc,
			MLSceneGLSharedDataContext& shared);
	~MLPluginGLContext();

	void initPerViewRenderingData(int meshid, MLRenderingData& dt);
	void removePerViewRenderindData();
	void setRenderingData(int meshid, MLRenderingData& dt);
	void drawMeshModel(int meshid) const;
	void meshAttributesUpdated(
			int meshid,
			bool conntectivitychanged,
			const MLRenderingData::RendAtts& dt);

	static void smoothModalitySuggestedRenderingData(MLRenderingData& dt);
	static void pointModalitySuggestedRenderingData(MLRenderingData& dt);
private:
	MLSceneGLSharedDataContext& _shared;
};

#endif // ML_PLUGIN_GL_CONTEXT_H
