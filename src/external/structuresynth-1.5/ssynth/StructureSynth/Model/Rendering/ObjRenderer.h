#pragma once

#include <QString>
#include "../../../SyntopiaCore/Math/Vector3.h"
#include "../../../SyntopiaCore/Math/Matrix4.h"
#include "../../../SyntopiaCore/GLEngine/Object3D.h"
#include "Renderer.h"

namespace StructureSynth {
	namespace Model {	
		namespace Rendering {

			using namespace SyntopiaCore::Math;
			using namespace SyntopiaCore::GLEngine;

			struct VertexNormal {
				VertexNormal() {};
				VertexNormal(int vID, int nID) : vID(vID), nID(nID) {};
				int vID;
				int nID;
			};

			struct ObjGroup {
				QString groupName;
				QVector<Vector3f> vertices;
				QVector<Vector3f> normals;
				QVector<QVector<VertexNormal> > faces;

				void addGroup(ObjGroup g);
				void reduceVertices();
			};

			/// Obj file renderer
			class ObjRenderer : public Renderer {
			public:
				ObjRenderer(int sphereDT, int sphereDP, bool groupByTagging, bool groupByColor) 
					: sphereDT(sphereDT), sphereDP(sphereDP), groupByTagging(groupByTagging), groupByColor(groupByColor) {};
				virtual ~ObjRenderer() {};

				/// Flow
				virtual void begin();
				virtual void end();

				/// This defines the identifier for our renderer.
				virtual QString renderClass() { return "ObjRenderer"; }

				/// The primitives
				virtual void drawBox(Vector3f base, 
					Vector3f dir1, 
					Vector3f dir2, 
					Vector3f dir3,
					PrimitiveClass* classID);

				virtual void drawMesh(  Vector3f startBase, 
					Vector3f startDir1, 
					Vector3f startDir2, 
					Vector3f endBase, 
					Vector3f endDir1, 
					Vector3f endDir2, 
					PrimitiveClass* classID);

				virtual void drawGrid(Vector3f base, 
					Vector3f dir1, 
					Vector3f dir2, 
					Vector3f dir3,
					PrimitiveClass* classID);

				virtual void drawLine(Vector3f from, 
					Vector3f to,
					PrimitiveClass* classID);

				virtual void drawDot(Vector3f pos,
					PrimitiveClass* classID);

				virtual void drawSphere(Vector3f center, float radius,
					PrimitiveClass* classID);

				virtual void drawTriangle(Vector3f p1,
					Vector3f p2,
					Vector3f p3,
					PrimitiveClass* classID);

				virtual void callGeneric(PrimitiveClass* ) {};

				// Color
				// RGB in [0;1] intervals.
				virtual void setColor(Vector3f rgb) { this->rgb = rgb; }
				virtual void setBackgroundColor(Vector3f /*rgb*/) {};
				virtual void setAlpha(double alpha) { this->alpha = alpha; }

				virtual void setPreviousColor(Vector3f /*rgb*/) {};
				virtual void setPreviousAlpha(double /*alpha*/) {};


				// Camera settings
				virtual void setTranslation(Vector3f /*translation*/) {};
				virtual void setScale(double /*scale*/) {};
				virtual void setRotation(Matrix4f /*rotation*/) {};
				virtual void setPivot(Vector3f /*pivot*/) {};
				virtual void setPerspectiveAngle(double /*angle*/) {};

				// Issues a command for a specific renderclass such as 'template' or 'opengl'
				virtual void callCommand(const QString& /*renderClass*/, const QString& /*command*/) {};

				void addQuad(ObjGroup& group, Vector3f v1,Vector3f v2,Vector3f v3,Vector3f v4);
				void addLineQuad(ObjGroup& group, Vector3f v1,Vector3f v2,Vector3f v3,Vector3f v4);
				void setClass(QString classID, Vector3f rgb, double alpha);

				void writeToStream(QTextStream& ts);

			private:
				QMap<QString, ObjGroup> groups;
				QString currentGroup;
				SyntopiaCore::Math::Vector3f rgb;
				double alpha;
				int sphereDT;
				int sphereDP;
				bool groupByTagging; 
				bool groupByColor;

			};

		}
	}
}

