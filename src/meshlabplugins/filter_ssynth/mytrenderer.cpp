#include "mytrenderer.h"
using namespace StructureSynth::Model::Rendering;
using namespace SyntopiaCore::GLEngine;
using namespace SyntopiaCore::Math;
using namespace StructureSynth::Model;
void MyTrenderer::drawBox(SyntopiaCore::Math::Vector3f base,
                                SyntopiaCore::Math::Vector3f dir1 ,
                                SyntopiaCore::Math::Vector3f dir2,
                                SyntopiaCore::Math::Vector3f dir3,
                                PrimitiveClass* classID)
{
        TemplatePrimitive t(workingTempl.get("box"));
                                        if(t.contains("{x3dvectors}")){
                                                QString mat = QString("translation=\"%1 %2 %3\"")
                                                        .arg(base.x()).arg(base.y()).arg(base.z());
                                                QString coord=QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10 %11 %12 %13 %14 %15 %16 %17 %18 %19 %20 %21 %22 %23 %24")
                                                        .arg(0).arg(0).arg(0)
                                                        .arg(dir3.x()).arg(dir3.y()).arg(dir3.z())
                                                        .arg(dir2.x()).arg(dir2.y()).arg(dir2.z())
                                                        .arg((dir2+dir3).x()).arg((dir2+dir3).y()).arg((dir2+dir3).z())
                                                        .arg(dir1.x()).arg(dir1.y()).arg(dir1.z())
                                                        .arg((dir1+dir3).x()).arg((dir1+dir3).y()).arg((dir1+dir3).z())
                                                        .arg((dir1+dir2).x()).arg((dir1+dir2).y()).arg((dir1+dir2).z())
                                                        .arg((dir1+dir2+dir3).x()).arg((dir1+dir2+dir3).y()).arg((dir1+dir2+dir3).z());
                                                t.substitute("{coords}",coord);
                                                t.substitute("{x3dvectors}",mat);
                                        }
                                        t.substitute("{r}", QString::number(rgb.x()));
                                        t.substitute("{g}", QString::number(rgb.y()));
                                        t.substitute("{b}", QString::number(rgb.z()));
                                        t.substitute("{alpha}", QString::number(alpha));
                                        output.append(t.getText());
}
void MyTrenderer::drawSphere(SyntopiaCore::Math::Vector3f center, float radius, SyntopiaCore::GLEngine::PrimitiveClass *classID){
                                TemplatePrimitive t(workingTempl.get("sphere"));
                                t.substitute("{cx}", QString::number(center.x()));
                                t.substitute("{cy}", QString::number(center.y()));
                                t.substitute("{cz}", QString::number(center.z()));
                                if(t.contains("{x3dsphscale}")){
                                        float diff=radius;
                                        QString scale=QString("%1 %2 %3")
                                                .arg(diff).arg(diff).arg(diff);
                                        t.substitute("{x3dsphscale}",scale);
                                        
                                }
								t.substitute("{r}", QString::number(rgb.x()));
                                        t.substitute("{g}", QString::number(rgb.y()));
                                        t.substitute("{b}", QString::number(rgb.z()));
                                        t.substitute("{alpha}", QString::number(alpha));
                                t.substitute("{rad}", QString::number(radius));
                                output.append(t.getText());
}
void MyTrenderer::drawMesh(SyntopiaCore::Math::Vector3f startBase,
                                                   SyntopiaCore::Math::Vector3f startDir1,
                                                   SyntopiaCore::Math::Vector3f startDir2,
                                                   SyntopiaCore::Math::Vector3f endBase,
                                                   SyntopiaCore::Math::Vector3f endDir1,
                                                   SyntopiaCore::Math::Vector3f endDir2,
                                                   SyntopiaCore::GLEngine::PrimitiveClass* classID)
{
                TemplatePrimitive t(workingTempl.get("mesh"));
                if(t.contains("{x3dtranslate}")){
                        Vector3f end=endBase - startBase;
                        Vector3f O(0,0,0);
                        QString tslate=QString("%1 %2 %3")
                        .arg(startBase.x()).arg(startBase.y()).arg(startBase.z());
                        Vector3f coords[10]={O,startDir1,startDir2,endDir1,endDir2,end,(end+endDir1),(end+endDir2),(startDir1+startDir2),(end+endDir1+endDir2)};
                        QString coord("");
                        for(int i=0;i<10;i++)
                                coord.append(QString::number(coords[i].x())).append(" ").append(QString::number(coords[i].y())).append(" ").append(QString::number(coords[i].z())).append(" ");
                        t.substitute("{x3dtranslate}",tslate);
                        t.substitute("{x3dcoord}",coord);
                }
                t.substitute("{r}", QString::number(rgb.x()));
                t.substitute("{g}", QString::number(rgb.y()));
                t.substitute("{b}", QString::number(rgb.z()));
                t.substitute("{alpha}", QString::number(alpha));
                output.append(t.getText());
}
        QString MyTrenderer::getOutput() {
                                QString out = output.join("");

                                // Normalize output (seems the '\n' converts to CR+LF on windows while saving
                                // whereas '\r\n' converts to CR+CR+LF? so we remove the \r's).
                                out = out.replace("\r","");
                                return out;
                        }
        void MyTrenderer::begin(){
                TemplatePrimitive t(workingTempl.get("begin"));
                output.append(t.getText());
        }
        void MyTrenderer::end(){
                TemplatePrimitive t(workingTempl.get("end"));
                output.append(t.getText());
        }
        void MyTrenderer::drawDot(SyntopiaCore::Math::Vector3f v,PrimitiveClass* classID) {
                                TemplatePrimitive t(workingTempl.get("dot"));
                                t.substitute("{x}", QString::number(v.x()));
                                t.substitute("{y}", QString::number(v.y()));
                                t.substitute("{z}", QString::number(v.z()));

                                t.substitute("{r}", QString::number(rgb.x()));
                                t.substitute("{g}", QString::number(rgb.y()));
                                t.substitute("{b}", QString::number(rgb.z()));

                                t.substitute("{alpha}", QString::number(alpha));
                                output.append(t.getText());
                        }
        void MyTrenderer::drawTriangle(SyntopiaCore::Math::Vector3f p1,
                                SyntopiaCore::Math::Vector3f p2,
                                SyntopiaCore::Math::Vector3f p3,
                                PrimitiveClass* classID) {
                                        TemplatePrimitive t(workingTempl.get("triangle"));
                                        t.substitute("{p1x}", QString::number(p1.x()));
                                        t.substitute("{p1y}", QString::number(p1.y()));
                                        t.substitute("{p1z}", QString::number(p1.z()));
                                        t.substitute("{p2x}", QString::number(p2.x()));
                                        t.substitute("{p2y}", QString::number(p2.y()));
                                        t.substitute("{p2z}", QString::number(p2.z()));
                                        t.substitute("{p3x}", QString::number(p3.x()));
                                        t.substitute("{p3y}", QString::number(p3.y()));
                                        t.substitute("{p3z}", QString::number(p3.z()));
                                        t.substitute("{r}", QString::number(rgb.x()));
                                        t.substitute("{g}", QString::number(rgb.y()));
                                        t.substitute("{b}", QString::number(rgb.z()));
                                        t.substitute("{alpha}", QString::number(alpha));
                                        output.append(t.getText());
}
