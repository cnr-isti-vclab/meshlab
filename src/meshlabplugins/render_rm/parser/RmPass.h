#ifndef __RMPASS_H__
#define __RMPASS_H__

#include <QList>
#include <QString>
#include <QStringList>
#include <qDebug>


#include "GlState.h"
#include "UniformVar.h"

class RenderTarget {
	public:
		QString name;
		bool renderToScreen;
		bool colorClear;
		bool depthClear;
		float clearColorValue;
		float depthClearValue;

		RenderTarget(QString _name = QString(), bool _renderToScreen = false, bool _colorClear = false, bool _depthClear = false, float _clearColorValue = 0, float _depthClearValue = 0) {
			name = _name;
			renderToScreen = _renderToScreen;
			colorClear = _colorClear;
			depthClear = _depthClear;
			clearColorValue = _clearColorValue;
			depthClearValue = _depthClearValue;
		}
};


class RmPass
{
	public:
		enum CodeType {
			FRAGMENT,
			VERTEX
		};
	
	private:
	QString name;
	int index;

	QString fragment;
	QString vertex;
	
	QList<UniformVar> fragmentUniform;
	QList<UniformVar> vertexUniform;

	QString modelReference;
	QString modelReferenceFileName;
	QList<GlState> states;

	RenderTarget renderTarget;

	// * we look for a variable declared as uniform in a specific 
	// * source code (fragment or vertex's one) and check for its type
	UniformVar searchUniformVariable( QString & name, enum CodeType codetype);

	public:
		RmPass( QString _name = QString(), int _index = -1) { name = _name; index = _index; }
		virtual ~RmPass( ){}
		

		UniformVar searchFragmentUniformVariable( QString & name);
		UniformVar searchVertexUniformVariable( QString & name );

		void addOpenGLState( GlState & state ) { states.append(state); }

		// * Setter and Getter
		bool hasIndex() { return index != -1; }
		int getIndex() { return index; }


		void setModelReference( QString _modelReference ) { modelReference = _modelReference; }
		QString & getModelReference( ) { return modelReference; }
		void setModelReferenceFileName( QString _modelReferenceFileName ) { modelReferenceFileName = _modelReferenceFileName; }
		QString & getModelReferenceFileName( ) { return modelReferenceFileName; }

		void setFragment( QString _fragment ) { fragment = _fragment; }
		QString & getFragment( ) { return fragment; }

		void setVertex( QString _vertex ) { vertex = _vertex; }
		QString & getVertex( ) { return vertex; }

		QString & getName() { return name; }

		void setRenderTarget( RenderTarget rt ) { renderTarget = rt; }
		RenderTarget & getRenderTarget() { return renderTarget; }
		bool hasRenderTarget() { return !renderTarget.name.isNull(); }


		void addFragmentUniformVariable( UniformVar & var ) { fragmentUniform.append(var); }
		void addVertexUniformVariable( UniformVar & var ) { vertexUniform.append(var); }

		int fragmentUniformVariableSize() { return fragmentUniform.size(); }
		int vertexUniformVariableSize() { return vertexUniform.size(); }

		UniformVar & getFragmentUniformVariable( int idx ) { return getUniformVariable( idx, RmPass::FRAGMENT ); }
		UniformVar & getVertexUniformVariable( int idx ) { return getUniformVariable( idx, RmPass::VERTEX ); }
		UniformVar & getUniformVariable( int idx, enum CodeType codetype ) {
			if( codetype == RmPass::FRAGMENT ) return fragmentUniform[idx];
			return vertexUniform[idx];
		}

		int openGLStatesSize() { return states.size(); }
		GlState & getOpenGLState( int idx ) { return states[idx]; }


		bool operator< (const RmPass & p) const { return index < p.index; }
};

#endif

