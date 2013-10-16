#pragma once

#include <QString>
#include <QMap>
#include <QSet>
#include <QStringList>
#include <QFile>
#include <QDomDocument>
#include "Renderer.h"

#include "../../../SyntopiaCore/Math/Vector3.h"


namespace StructureSynth {
	namespace Model {	
		namespace Rendering {
			
			using namespace SyntopiaCore::Math;
			using namespace SyntopiaCore::GLEngine;


			/// A TemplatePrimitive is the definition for a single primitive (like Box or Sphere).
			/// It is a simple text string with placeholders for stuff like coordinates and color.
			class TemplatePrimitive {
			public:
				TemplatePrimitive() {};
				TemplatePrimitive(QString def) : def(def) {};
				TemplatePrimitive(const TemplatePrimitive& t) { this->def = t.def; };

				QString getText() { return def; }

				void substitute(QString before, QString after) {
					def.replace(before, after);
				};

				bool contains(QString input) {
					return def.contains(input);
				};

			private:
				QString def;
			};

			// A Template contains a number of TemplatePrimitives:
			// text definitions for each of the standard primitives (box, sphere, ...) with placeholders
			// for stuff like coordinates and color.
			class Template {
			public:
				Template() {};
				Template(QFile& file) { read(file); }
				Template(QString xmlString) { read(xmlString); }

				void read(QFile& file);
				void read(QString xmlString);
				void parse(QDomDocument& doc);

				QMap<QString, TemplatePrimitive>& getPrimitives() { return primitives; }
				TemplatePrimitive get(QString name) { return primitives[name]; }
				QString getDescription() { return description; }
				QString getFullText() { return fullText; }
				QString getName() { return name; }
				QString getDefaultExtension() { return defaultExtension; }
				QString getRunAfter() { return runAfter; }

			private:
				QMap<QString, TemplatePrimitive> primitives;
				QString description;
				QString name;
				QString defaultExtension;
				QString fullText;
				QString runAfter;
			};

			/// A renderer implementation based on the SyntopiaCore POV widget.
			class TemplateRenderer : public Renderer {
			public:
				TemplateRenderer();
				TemplateRenderer(QString xmlDefinitionFile);
				TemplateRenderer(Template myTemplate);
				
				virtual ~TemplateRenderer();

				virtual QString renderClass() { return "template"; }

				/// The primitives
				virtual void drawBox(SyntopiaCore::Math::Vector3f base, 
							  SyntopiaCore::Math::Vector3f dir1 , 
							  SyntopiaCore::Math::Vector3f dir2, 
							  SyntopiaCore::Math::Vector3f dir3,
								PrimitiveClass* classID);

				virtual void drawSphere(SyntopiaCore::Math::Vector3f center, float radius,
								PrimitiveClass* classID);

				
				virtual void drawMesh(  SyntopiaCore::Math::Vector3f startBase, 
										SyntopiaCore::Math::Vector3f startDir1, 
										SyntopiaCore::Math::Vector3f startDir2, 
										SyntopiaCore::Math::Vector3f endBase, 
										SyntopiaCore::Math::Vector3f endDir1, 
										SyntopiaCore::Math::Vector3f endDir2, 
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

				virtual void callGeneric(PrimitiveClass* classID);
				
				virtual void begin();
				virtual void end();
				
				virtual void setColor(SyntopiaCore::Math::Vector3f rgb) { this->rgb = rgb; }
				virtual void setBackgroundColor(SyntopiaCore::Math::Vector3f rgb);
				virtual void setAlpha(double alpha) { this->alpha = alpha; }

				virtual void setPreviousColor(SyntopiaCore::Math::Vector3f rgb) { this->oldRgb = rgb; }
				virtual void setPreviousAlpha(double alpha) { this->oldAlpha = alpha; }


				QString getOutput() ;
				
				// Issues a command for a specific renderclass such as 'template' or 'opengl'
				virtual void callCommand(const QString& renderClass, const QString& command);

				bool assertPrimitiveExists(QString templateName);
			
				void setCamera(Vector3f cameraPosition, Vector3f cameraUp, Vector3f cameraRight, Vector3f cameraTarget, int width, int height, double aspect, double fov) {
					this->cameraPosition = cameraPosition;
					this->cameraUp = cameraUp;
					this->cameraRight = cameraRight;
					this->cameraTarget = cameraTarget;
					this->width = width;
					this->height = height;
					this->aspect = aspect;
					this->fov = fov;
				}

				void doBeginEndSubstitutions(TemplatePrimitive& t);

				void doStandardSubstitutions(SyntopiaCore::Math::Vector3f base, 
					SyntopiaCore::Math::Vector3f dir1,
					SyntopiaCore::Math::Vector3f dir2, 
					SyntopiaCore::Math::Vector3f dir3, 
					TemplatePrimitive& t);

			private:
				
				SyntopiaCore::Math::Vector3f cameraPosition;
				SyntopiaCore::Math::Vector3f cameraUp;
				SyntopiaCore::Math::Vector3f cameraRight;
				SyntopiaCore::Math::Vector3f cameraTarget;


				SyntopiaCore::Math::Vector3f rgb;
				SyntopiaCore::Math::Vector3f backRgb;
				double alpha;
				Template workingTemplate;
				QStringList output;
				int counter;
				int width;
				int height;
				double aspect;
				double fov;
				QSet<QString> missingTypes;
				SyntopiaCore::Math::Vector3f oldRgb;
				double oldAlpha;
			};

		}
	}
}

