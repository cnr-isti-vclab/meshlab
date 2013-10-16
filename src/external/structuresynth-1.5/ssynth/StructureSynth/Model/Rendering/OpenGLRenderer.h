#pragma once

#include <QString>
#include "../../../SyntopiaCore/GLEngine/EngineWidget.h"
#include "Renderer.h"

#include "../../../SyntopiaCore/Math/Vector3.h"


namespace StructureSynth {
	namespace Model {	
		namespace Rendering {

			using namespace SyntopiaCore::GLEngine;


			/// A renderer implementation based on the SyntopiaCore openGL widget.
			class OpenGLRenderer : public Renderer {
			public:
				OpenGLRenderer(SyntopiaCore::GLEngine::EngineWidget* engine) : engine(engine) {};
				virtual ~OpenGLRenderer() {};

				/// The primitives
				virtual void drawBox(SyntopiaCore::Math::Vector3f base, 
					          SyntopiaCore::Math::Vector3f dir1 , 
							  SyntopiaCore::Math::Vector3f dir2, 
							  SyntopiaCore::Math::Vector3f dir3,
								PrimitiveClass* classID);

				
				virtual void drawMesh(  SyntopiaCore::Math::Vector3f startBase, 
										SyntopiaCore::Math::Vector3f startDir1, 
										SyntopiaCore::Math::Vector3f startDir2, 
										SyntopiaCore::Math::Vector3f endBase, 
										SyntopiaCore::Math::Vector3f endDir1, 
										SyntopiaCore::Math::Vector3f endDir2, 
										PrimitiveClass* classID);

				virtual void drawSphere(SyntopiaCore::Math::Vector3f center, float radius,
								PrimitiveClass* classID);

				virtual void drawGrid(SyntopiaCore::Math::Vector3f base, 
								SyntopiaCore::Math::Vector3f dir1, 
								SyntopiaCore::Math::Vector3f dir2, 
								SyntopiaCore::Math::Vector3f dir3,
								PrimitiveClass* classID);

				virtual void drawLine(SyntopiaCore::Math::Vector3f from, 
										SyntopiaCore::Math::Vector3f to,
								PrimitiveClass* classID);

				virtual void drawDot(SyntopiaCore::Math::Vector3f pos,
								PrimitiveClass* classID);

				virtual void drawTriangle(SyntopiaCore::Math::Vector3f p1,
										 SyntopiaCore::Math::Vector3f p2,
									     SyntopiaCore::Math::Vector3f p3,
											PrimitiveClass* classID);

				virtual void begin();
				virtual void end();
				
				virtual void setColor(SyntopiaCore::Math::Vector3f rgb) { this->rgb = rgb; }
				virtual void setBackgroundColor(SyntopiaCore::Math::Vector3f rgb);
				virtual void setAlpha(double alpha) { this->alpha = alpha; }

				virtual void setPreviousColor(SyntopiaCore::Math::Vector3f rgb) { this->oldRgb = rgb; }
				virtual void setPreviousAlpha(double alpha) { this->oldAlpha = alpha; }


				virtual void setTranslation(SyntopiaCore::Math::Vector3f /*translation*/);
				virtual void setScale(double /*scale*/);
				virtual void setRotation(SyntopiaCore::Math::Matrix4f /*rotation*/);
				virtual void setPivot(SyntopiaCore::Math::Vector3f /*pivot*/);
				virtual void setPerspectiveAngle(double /*angle*/);

				// Issues a command for a specific renderclass such as 'template' or 'opengl'
				virtual void callCommand(const QString& renderClass, const QString& command);
			private:
				SyntopiaCore::GLEngine::EngineWidget* engine;
				SyntopiaCore::Math::Vector3f rgb;
				double alpha;
				SyntopiaCore::Math::Vector3f oldRgb;
				double oldAlpha;
			};

		}
	}
}

