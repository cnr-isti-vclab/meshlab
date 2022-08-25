#ifndef MYTRENDERER_H
#define MYTRENDERER_H
#include <StructureSynth/Model/Rendering/TemplateRenderer.h>
#include <SyntopiaCore/Math/Vector3.h>
class MyTrenderer:public StructureSynth::Model::Rendering::TemplateRenderer{
public:
        MyTrenderer(StructureSynth::Model::Rendering::Template templ):StructureSynth::Model::Rendering::TemplateRenderer(templ){ workingTempl=templ;}
        ~MyTrenderer(){output.clear();};
        void drawBox(SyntopiaCore::Math::Vector3f base,
                                SyntopiaCore::Math::Vector3f dir1 ,
                                SyntopiaCore::Math::Vector3f dir2,
                                SyntopiaCore::Math::Vector3f dir3,
                                SyntopiaCore::GLEngine::PrimitiveClass* classID);
        void drawSphere(SyntopiaCore::Math::Vector3f center, float radius,SyntopiaCore::GLEngine::PrimitiveClass* classID);
        void drawMesh(SyntopiaCore::Math::Vector3f startBase, SyntopiaCore::Math::Vector3f startDir1, SyntopiaCore::Math::Vector3f startDir2, SyntopiaCore::Math::Vector3f endBase, SyntopiaCore::Math::Vector3f endDir1, SyntopiaCore::Math::Vector3f endDir2, SyntopiaCore::GLEngine::PrimitiveClass* classID);
        void drawDot(SyntopiaCore::Math::Vector3f v,SyntopiaCore::GLEngine::PrimitiveClass* classID);
        void drawTriangle(SyntopiaCore::Math::Vector3f p1,
                                SyntopiaCore::Math::Vector3f p2,
                                SyntopiaCore::Math::Vector3f p3,
                                SyntopiaCore::GLEngine::PrimitiveClass* classID);
        QString getOutput();
        void begin();
        void end();
        void setColor(SyntopiaCore::Math::Vector3f rgb) { this->rgb = rgb; }
        void setAlpha(double alpha) { this->alpha = alpha; }
private:
        StructureSynth::Model::Rendering::Template workingTempl;
        QStringList output;
        SyntopiaCore::Math::Vector3f rgb;
        double alpha;
};

#endif // MYTRENDERER_H
