#pragma once

#include <QString>
#include "../../../SyntopiaCore/Math/Vector3.h"
#include "../../../SyntopiaCore/Math/Matrix4.h"
#include "../../../SyntopiaCore/GLEngine/Object3D.h"

namespace StructureSynth {
	namespace Model {	
		namespace Rendering {


		using namespace SyntopiaCore::GLEngine;

		/// Abstract base class for implementing a renderer
		class Renderer {
			public:
				Renderer() {};
				virtual ~Renderer() {};

				/// Flow
				virtual void begin() {};
				virtual void end() {};

				/// This defines the identifier for our renderer.
				virtual QString renderClass() { return ""; }

				/// The primitives
				virtual void drawBox(SyntopiaCore::Math::Vector3f base, 
								SyntopiaCore::Math::Vector3f dir1, 
								SyntopiaCore::Math::Vector3f dir2, 
								SyntopiaCore::Math::Vector3f dir3,
								PrimitiveClass* classID) = 0;

				virtual void drawMesh(  SyntopiaCore::Math::Vector3f startBase, 
										SyntopiaCore::Math::Vector3f startDir1, 
										SyntopiaCore::Math::Vector3f startDir2, 
										SyntopiaCore::Math::Vector3f endBase, 
										SyntopiaCore::Math::Vector3f endDir1, 
										SyntopiaCore::Math::Vector3f endDir2, 
										PrimitiveClass* classID) = 0;

				virtual void drawGrid(SyntopiaCore::Math::Vector3f base, 
								SyntopiaCore::Math::Vector3f dir1, 
								SyntopiaCore::Math::Vector3f dir2, 
								SyntopiaCore::Math::Vector3f dir3,
								PrimitiveClass* classID) = 0;

				virtual void drawLine(SyntopiaCore::Math::Vector3f from, 
										SyntopiaCore::Math::Vector3f to,
								PrimitiveClass* classID) = 0;

				virtual void drawDot(SyntopiaCore::Math::Vector3f pos,
								PrimitiveClass* classID) = 0;
				
				virtual void drawSphere(SyntopiaCore::Math::Vector3f center, float radius,
								PrimitiveClass* classID) = 0;

				virtual void drawTriangle(SyntopiaCore::Math::Vector3f p1,
										 SyntopiaCore::Math::Vector3f p2,
									     SyntopiaCore::Math::Vector3f p3,
											PrimitiveClass* classID) = 0;

				virtual void callGeneric(PrimitiveClass* ) {};
				
				// Color
				// RGB in [0;1] intervals.
				virtual void setColor(SyntopiaCore::Math::Vector3f rgb) = 0;
				virtual void setBackgroundColor(SyntopiaCore::Math::Vector3f rgb) = 0;
				virtual void setAlpha(double alpha) = 0;

				virtual void setPreviousColor(SyntopiaCore::Math::Vector3f rgb) = 0;
				virtual void setPreviousAlpha(double alpha) = 0;


				// Camera settings
				virtual void setTranslation(SyntopiaCore::Math::Vector3f /*translation*/) {};
				virtual void setScale(double /*scale*/) {};
				virtual void setRotation(SyntopiaCore::Math::Matrix4f /*rotation*/) {};
				virtual void setPivot(SyntopiaCore::Math::Vector3f /*pivot*/) {};
				virtual void setPerspectiveAngle(double /*angle*/) {};

				// Issues a command for a specific renderclass such as 'template' or 'opengl'
				virtual void callCommand(const QString& /*renderClass*/, const QString& /*command*/) {};
		};

		}
	}
}

