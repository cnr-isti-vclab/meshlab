#include "TemplateRenderer.h"
#include "../../../SyntopiaCore/Math/Vector3.h"
#include "../../../SyntopiaCore/Logging/Logging.h"
#include "../../../SyntopiaCore/Exceptions/Exception.h"
#include "../PrimitiveClass.h"

#include <QDomDocument>
#include <QIODevice>
#include <QFile>
#include <QFileInfo>
#include <QMap>

using namespace SyntopiaCore::Math;
using namespace SyntopiaCore::Logging;
using namespace SyntopiaCore::Exceptions;

namespace StructureSynth {
	namespace Model {	
		namespace Rendering {


			void Template::read(QString xml) {
				QDomDocument doc;

				QString errorMessage;
				int errorLine = 0;
				int errorColumn = 0; 

				if (!doc.setContent(xml, false, &errorMessage, &errorLine, &errorColumn )) {
					QString error = QString("[Line %1, Col %2] %3").arg(errorLine).arg(errorColumn).arg(errorMessage);
					WARNING("Unable to parse xml: " + error);

					throw Exception("Unable to parse xml from string: " + error );					
		

					return;
				}

				fullText = doc.toString();

				parse(doc);
			}

			void Template::read(QFile& file) {
				QDomDocument doc;
				if (!file.open(QIODevice::ReadOnly)) {
					throw Exception("Unable to open file: " + QFileInfo(file).absoluteFilePath());					
				}
				QString errorMessage;
				int errorLine = 0;
				int errorColumn = 0; 

				if (!doc.setContent(&file, false, &errorMessage, &errorLine, &errorColumn)) {
					file.close();
					QString error = QString("[Line %1, Col %2] %3").arg(errorLine).arg(errorColumn).arg(errorMessage);
					
					throw Exception("Unable to parse file: " + error + " in file: " + QFileInfo(file).absoluteFilePath());					
				}
				file.close();

				fullText = doc.toString();

				parse(doc);
			}

			void Template::parse(QDomDocument& doc) {
				QDomElement docElem = doc.documentElement();

				QDomNode n = docElem.firstChild();

				QDomElement ne = docElem.toElement(); // try to convert the node to an element.
				if(!ne.isNull()) {
					if (ne.hasAttribute("name")) {
						this->name = ne.attribute("name");
					} else {
						this->name = "NONAME";
					}

					if (ne.hasAttribute("defaultExtension")) {
						this->defaultExtension = ne.attribute("defaultExtension");
					} else {
						this->defaultExtension = "Unknown file type (*.txt)";
					}

					if (ne.hasAttribute("runAfter")) {
						this->runAfter = ne.attribute("runAfter");
					} else {
						this->runAfter = "";
					}
				}

				while(!n.isNull()) {
					QDomElement e = n.toElement(); // try to convert the node to an element.
					if(!e.isNull()) {
						if (e.tagName() == "primitive" || e.tagName() == "substitution") {
							if (e.tagName() == "substitution") {
								WARNING("Element-name 'substitution' is a deprecated name. Please rename to 'primitive'.");
							}

							if (!e.hasAttribute("name")) {
								WARNING("Primitive without name attribute found!");
								n = n.nextSibling();
								continue;
							}


							QString type = "";
							if (e.hasAttribute("type")) {
								type = "::" + e.attribute("type");
							}


							QString name = e.attribute("name") + type;
							primitives[name] = TemplatePrimitive(e.text());
						} else if (e.tagName() == "description") {
							description = e.text();
						} else {

							WARNING("Expected 'primitive' or 'description' element, found: " + e.tagName());
							
						}
					}
					n = n.nextSibling();
				}


			}

			TemplateRenderer::TemplateRenderer(QString xmlDefinitionFile) {
				counter = 0;
				QFile file(xmlDefinitionFile);

				workingTemplate.read(file);
			}

			TemplateRenderer::TemplateRenderer(Template myTemplate) {
				counter = 0;
				workingTemplate = myTemplate;
			}
				


			TemplateRenderer::TemplateRenderer()  
			{
				counter = 0;
			}

			TemplateRenderer::~TemplateRenderer() {
			}

			bool TemplateRenderer::assertPrimitiveExists(QString templateName) {
				if (!workingTemplate.getPrimitives().contains(templateName)) {
					QString error = 
						QString("Template error: the primitive '%1' is not defined.").arg(templateName);

					if (!missingTypes.contains(error)) {
						// Only show each error once.
						WARNING(error);
						INFO("(A template may not support all drawing primitives. Either update the template or choose another primitive)");
						missingTypes.insert(error);
					} 
					return false;
				}
				return true;

			} 

			void TemplateRenderer::doBeginEndSubstitutions(TemplatePrimitive& t)
			{
				t.substitute("{CamPosX}", QString::number(cameraPosition.x()));
				t.substitute("{CamPosY}", QString::number(cameraPosition.y()));
				t.substitute("{CamPosZ}", QString::number(cameraPosition.z()));

				t.substitute("{CamUpX}", QString::number(cameraUp.x()));
				t.substitute("{CamUpY}", QString::number(cameraUp.y()));
				t.substitute("{CamUpZ}", QString::number(cameraUp.z()));

				Vector3f cameraDir = cameraTarget-cameraPosition;
				cameraDir.normalize();

				t.substitute("{CamDirX}", QString::number(cameraDir.x()));
				t.substitute("{CamDirY}", QString::number(cameraDir.y()));
				t.substitute("{CamDirZ}", QString::number(cameraDir.z()));

				t.substitute("{CamRightX}", QString::number(cameraRight.x()));
				t.substitute("{CamRightY}", QString::number(cameraRight.y()));
				t.substitute("{CamRightZ}", QString::number(cameraRight.z()));

				t.substitute("{CamTargetX}", QString::number(cameraTarget.x()));
				t.substitute("{CamTargetY}", QString::number(cameraTarget.y()));
				t.substitute("{CamTargetZ}", QString::number(cameraTarget.z()));

				if (t.contains("{CamColumnMatrix}")) {
					const Vector3f u = -cameraRight;
					const Vector3f v = cameraUp;
					const Vector3f w = -cameraDir;

					QString mat = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 0.0 0.0 0.0 1.0")
						.arg(u.x()).arg(v.x()).arg(w.x()).arg(cameraPosition.x())
						.arg(u.y()).arg(v.y()).arg(w.y()).arg(cameraPosition.y())
						.arg(u.z()).arg(v.z()).arg(w.z()).arg(cameraPosition.z());

					t.substitute("{CamColumnMatrix}", mat);
				}

				t.substitute("{aspect}", QString::number(aspect));
				t.substitute("{width}", QString::number(width));
				t.substitute("{height}", QString::number(height));
				t.substitute("{fov}", QString::number(fov));

				t.substitute("{BR}", QString::number(backRgb.x()));
				t.substitute("{BG}", QString::number(backRgb.y()));
				t.substitute("{BB}", QString::number(backRgb.z()));

				t.substitute("{BR256}", QString::number(backRgb.x()*255));
				t.substitute("{BG256}", QString::number(backRgb.y()*255));
				t.substitute("{BB256}", QString::number(backRgb.z()*255));
			}

			void TemplateRenderer::doStandardSubstitutions(SyntopiaCore::Math::Vector3f base, 
				SyntopiaCore::Math::Vector3f dir1 , 
				SyntopiaCore::Math::Vector3f dir2, 
				SyntopiaCore::Math::Vector3f dir3, TemplatePrimitive& t) {
					if (t.contains("{matrix}")) {
						QString mat = QString("%1 %2 %3 0 %4 %5 %6 0 %7 %8 %9 0 %10 %11 %12 1")
							.arg(dir1.x()).arg(dir1.y()).arg(dir1.z())
							.arg(dir2.x()).arg(dir2.y()).arg(dir2.z())
							.arg(dir3.x()).arg(dir3.y()).arg(dir3.z())
							.arg(base.x()).arg(base.y()).arg(base.z());

						t.substitute("{matrix}", mat);
					}

					if (t.contains("{columnmatrix}")) {
						QString mat = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 0 0 0 1")
							.arg(dir1.x()).arg(dir2.x()).arg(dir3.x()).arg(base.x())
							.arg(dir1.y()).arg(dir2.y()).arg(dir3.y()).arg(base.y())
							.arg(dir1.z()).arg(dir2.z()).arg(dir3.z()).arg(base.z());

						t.substitute("{columnmatrix}", mat);
					}

					if (t.contains("{povmatrix}")) {
						QString mat = QString("%1, %2, %3, %4, %5, %6, %7, %8, %9, %10, %11, %12")
							.arg(dir1.x()).arg(dir1.y()).arg(dir1.z())
							.arg(dir2.x()).arg(dir2.y()).arg(dir2.z())
							.arg(dir3.x()).arg(dir3.y()).arg(dir3.z())
							.arg(base.x()).arg(base.y()).arg(base.z());

						t.substitute("{povmatrix}", mat);
					}

					t.substitute("{r}", QString::number(rgb.x()));
					t.substitute("{g}", QString::number(rgb.y()));
					t.substitute("{b}", QString::number(rgb.z()));
					t.substitute("{alpha}", QString::number(alpha));
					t.substitute("{oneminusalpha}", QString::number(1-alpha));

			}

			void TemplateRenderer::drawBox(SyntopiaCore::Math::Vector3f base, 
				SyntopiaCore::Math::Vector3f dir1 , 
				SyntopiaCore::Math::Vector3f dir2, 
				SyntopiaCore::Math::Vector3f dir3,
				PrimitiveClass* classID) 
			{
				QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
				if (!assertPrimitiveExists("box"+alternateID)) return;
				TemplatePrimitive t(workingTemplate.get("box"+alternateID)); 

				doStandardSubstitutions(base, dir1, dir2, dir3, t);

				if (t.contains("{uid}")) {
					t.substitute("{uid}", QString("Box%1").arg(counter++));
				}

				output.append(t.getText());
			};

			void TemplateRenderer::drawTriangle(SyntopiaCore::Math::Vector3f p1,
				SyntopiaCore::Math::Vector3f p2,
				SyntopiaCore::Math::Vector3f p3,
				PrimitiveClass* classID) {

					QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
					if (!assertPrimitiveExists("triangle"+alternateID)) return;
					TemplatePrimitive t(workingTemplate.get("triangle"+alternateID)); 

					if (t.contains("{uid}")) {
						t.substitute("{uid}", QString("Triangle%1").arg(counter++));
					}

					t.substitute("{p1x}", QString::number(p1.x()));
					t.substitute("{p1y}", QString::number(p1.y()));
					t.substitute("{p1z}", QString::number(p1.z()));
					t.substitute("{p2x}", QString::number(p2.x()));
					t.substitute("{p2y}", QString::number(p2.y()));
					t.substitute("{p2z}", QString::number(p2.z()));
					t.substitute("{p3x}", QString::number(p3.x()));
					t.substitute("{p3y}", QString::number(p3.y()));
					t.substitute("{p3z}", QString::number(p3.z()));

					t.substitute("{alpha}", QString::number(alpha));
					t.substitute("{oneminusalpha}", QString::number(1-alpha));


					output.append(t.getText());

			}


			void TemplateRenderer::drawGrid(SyntopiaCore::Math::Vector3f base, 
				SyntopiaCore::Math::Vector3f dir1, 
				SyntopiaCore::Math::Vector3f dir2, 
				SyntopiaCore::Math::Vector3f dir3,
				PrimitiveClass* classID) {

					QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
					if (!assertPrimitiveExists("grid"+alternateID)) return;
					TemplatePrimitive t(workingTemplate.get("grid"+alternateID)); 


					doStandardSubstitutions(base, dir1, dir2, dir3, t);

					if (t.contains("{uid}")) {
						t.substitute("{uid}", QString("Grid%1").arg(counter++));
					}



					output.append(t.getText());
			};

			void TemplateRenderer::drawLine(SyntopiaCore::Math::Vector3f from, SyntopiaCore::Math::Vector3f to,PrimitiveClass* classID) {
				QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
				if (!assertPrimitiveExists("line"+alternateID)) return;
				TemplatePrimitive t(workingTemplate.get("line"+alternateID)); 
				t.substitute("{x1}", QString::number(from.x()));
				t.substitute("{y1}", QString::number(from.y()));
				t.substitute("{z1}", QString::number(from.z()));

				t.substitute("{x2}", QString::number(to.x()));
				t.substitute("{y2}", QString::number(to.y()));
				t.substitute("{z2}", QString::number(to.z()));

				t.substitute("{alpha}", QString::number(alpha));
				t.substitute("{oneminusalpha}", QString::number(1-alpha));

				if (t.contains("{uid}")) {
					t.substitute("{uid}", QString("Line%1").arg(counter++));
				}

				output.append(t.getText());
			};

			void TemplateRenderer::drawDot(SyntopiaCore::Math::Vector3f v,PrimitiveClass* classID) {
				QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
				if (!assertPrimitiveExists("dot"+alternateID)) return;
				TemplatePrimitive t(workingTemplate.get("dot"+alternateID)); 
				t.substitute("{x}", QString::number(v.x()));
				t.substitute("{y}", QString::number(v.y()));
				t.substitute("{z}", QString::number(v.z()));

				t.substitute("{r}", QString::number(rgb.x()));
				t.substitute("{g}", QString::number(rgb.y()));
				t.substitute("{b}", QString::number(rgb.z()));

				t.substitute("{alpha}", QString::number(alpha));
				t.substitute("{oneminusalpha}", QString::number(1-alpha));

				if (t.contains("{uid}")) {
					t.substitute("{uid}", QString("Dot%1").arg(counter++));
				}

				output.append(t.getText());
			};

			void TemplateRenderer::drawSphere(SyntopiaCore::Math::Vector3f center, float radius,PrimitiveClass* classID) {
				QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
				if (!assertPrimitiveExists("sphere"+alternateID)) return;
				TemplatePrimitive t(workingTemplate.get("sphere"+alternateID)); 
				t.substitute("{cx}", QString::number(center.x()));
				t.substitute("{cy}", QString::number(center.y()));
				t.substitute("{cz}", QString::number(center.z()));

				t.substitute("{rad}", QString::number(radius));

				t.substitute("{r}", QString::number(rgb.x()));
				t.substitute("{g}", QString::number(rgb.y()));
				t.substitute("{b}", QString::number(rgb.z()));

				t.substitute("{alpha}", QString::number(alpha));
				t.substitute("{oneminusalpha}", QString::number(1-alpha));

				if (t.contains("{uid}")) {
					t.substitute("{uid}", QString("Sphere%1").arg(counter++));
				}

				output.append(t.getText());
			};

			void TemplateRenderer::begin() {
				if (!assertPrimitiveExists("begin")) return;
				TemplatePrimitive t(workingTemplate.get("begin")); 

				doBeginEndSubstitutions(t);

				output.append(t.getText());
			};

			void TemplateRenderer::end() {
				if (!assertPrimitiveExists("end")) return;
				TemplatePrimitive t(workingTemplate.get("end")); 

				doBeginEndSubstitutions(t);

				output.append(t.getText());
			};

			void TemplateRenderer::callGeneric(PrimitiveClass* classID) {
				QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
				if (!assertPrimitiveExists("template"+alternateID)) return;
				TemplatePrimitive t(workingTemplate.get("template"+alternateID)); 
				output.append(t.getText());
			}
				

			void TemplateRenderer::setBackgroundColor(SyntopiaCore::Math::Vector3f rgb) {
				backRgb = rgb;
			}

			void TemplateRenderer::drawMesh(  SyntopiaCore::Math::Vector3f startBase, 
				SyntopiaCore::Math::Vector3f startDir1, 
				SyntopiaCore::Math::Vector3f startDir2, 
				SyntopiaCore::Math::Vector3f endBase, 
				SyntopiaCore::Math::Vector3f endDir1, 
				SyntopiaCore::Math::Vector3f endDir2, 
				PrimitiveClass* classID) {
					QString alternateID = ((classID->name).isEmpty() ? "" : "::" + (classID->name));
					if (!assertPrimitiveExists("mesh"+alternateID)) return;
					TemplatePrimitive t(workingTemplate.get("mesh"));
					if (t.contains("{uid}")) {
						t.substitute("{uid}", QString("Box%1").arg(counter++));
					}

					// TODO: This really isn't a matrix, we need to find a better way to export the mesh.
					if (t.contains("{matrix}")) {
						QString mat = QString("%1 %2 %3 0 %4 %5 %6 0 %7 %8 %9 0 %10 %11 %12 0 %13 %14 %15 0 %16 %17 %18 1")
							.arg(startBase.x()).arg(startBase.y()).arg(startBase.z())
							.arg(startDir1.x()).arg(startDir1.y()).arg(startDir1.z())
							.arg(startDir2.x()).arg(startDir2.y()).arg(startDir2.z())
							.arg(endBase.x()).arg(endBase.y()).arg(endBase.z())
							.arg(endDir1.x()).arg(endDir1.y()).arg(endDir1.z())
							.arg(endDir2.x()).arg(endDir2.y()).arg(endDir2.z());

							t.substitute("{matrix}", mat);
					}



					t.substitute("{r}", QString::number(rgb.x()));
					t.substitute("{g}", QString::number(rgb.y()));
					t.substitute("{b}", QString::number(rgb.z()));
					t.substitute("{alpha}", QString::number(alpha));
					t.substitute("{oneminusalpha}", QString::number(1-alpha));

					output.append(t.getText());
			};

			void TemplateRenderer::callCommand(const QString& renderClass, const QString& /*command*/) {
				if (renderClass != this->renderClass()) return;

			}

			QString TemplateRenderer::getOutput() { 
				QString out = output.join(""); 

				// Normalize output (seems the '\n' converts to CR+LF on windows while saving
				// whereas '\r\n' converts to CR+CR+LF? so we remove the \r's).
				out = out.replace("\r","");
				return out;
			}


		}
	}
}

