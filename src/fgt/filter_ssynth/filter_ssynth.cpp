#include <Qt>
#include <QtGui>
#include "filter_ssynth.h"
#include <meshlabplugins/io_x3d/import_x3d.h>
#include <common/meshmodel.h>
#include <common/interfaces.h>
#include "mytrenderer.h"
#include <StructureSynth/Model/RandomStreams.h>
#include <StructureSynth/Parser/Preprocessor.h>
#include <StructureSynth/Parser/Tokenizer.h>
#include <StructureSynth/Parser/EisenParser.h>
#include <StructureSynth/Model/Builder.h>
#include <QMessageBox>
using namespace std;
using namespace vcg;
using namespace vcg::tri::io;
using namespace StructureSynth::Parser;
using namespace StructureSynth::Model;
using namespace StructureSynth::Model::Rendering;
using namespace SyntopiaCore::Exceptions;
FilterSSynth::FilterSSynth(){
    typeList<< CR_SSYNTH;
    FilterIDType tt;
    this->renderTemplate= QString("%1 %2 %3 %4 %5 %6 %7 %8 %9 %10")
            .arg("<template defaultExtension=\"X3D file format (*.x3d)\" name=\"x3d\" ><description>Custom x3d export template</description><primitive name=\"begin\"><![CDATA[<?xml version=\"1.0\" encoding=\"UTF-8\"?>")
            .arg("<!DOCTYPE X3D PUBLIC \"ISO//Web3D//DTD X3D 3.1//EN\" \"http://www.web3d.org/specifications/x3d-3.1.dtd\"><X3D profile=\"Immersive\" version=\"3.1\" xsd:noNamespaceSchemaLocation=\"http://www.web3d.org/specifications/x3d-3.1.xsd\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema-instance\" ><head></head><Scene>]]></primitive>")
            .arg("<primitive name=\"end\"><![CDATA[</Scene></X3D>]]></primitive>")
            .arg("<primitive name=\"sphere\"><![CDATA[<Transform translation=\"{cx} {cy} {cz}\" scale=\"{x3dsphscale}\"><Shape><IndexedFaceSet solid=\"true\" coordIndex=\"14 12 1 -1, 12 14 13 -1, 2 13 14 -1, 13 0 12 -1, 16 1 12 -1, 12 15 16 -1, 5 16 15 -1, 12 0 15 -1, 18 13 2 -1, 13 18 17 -1, 3 17 18 -1, 17 0 13 -1, 20 17 3 -1, 17 20 19 -1, 4 19 20 -1, 19 0 17 -1, 21 19 4 -1, 19 21 15 -1, 5 15 21 -1, 15 0 19 -1, 23 1 16 -1, 16 22 23 -1, 10 23 22 -1, 22 16 5 -1, 25 2 14 -1, 14 24 25 -1, 6 25 24 -1, 24 14 1 -1, 27 3 18 -1, 18 26 27 -1, 7 27 26 -1, 26 18 2 -1, 29 4 20 -1, 20 28 29 -1, 8 29 28 -1, 28 20 3 -1, 31 5 21 -1, 21 30 31 -1, 9 31 30 -1, 30 21 4 -1, 32 23 10 -1, 23 32 24 -1, 6 24 32 -1, 24 1 23 -1, 33 25 6 -1, 25 33 26 -1, 7 26 33 -1, 26 2 25 -1, 34 27 7 -1, 27 34 28 -1, 8 28 34 -1, 28 3 27 -1, 35 29 8 -1, 29 35 30 -1, 9 30 35 -1, 30 4 29 -1, 36 31 9 -1, 31 36 22 -1, 10 22 36 -1, 22 5 31 -1, 38 6 32 -1, 32 37 38 -1, 11 38 37 -1, 37 32 10 -1, 39 7 33 -1, 33 38 39 -1, 11 39 38 -1, 38 33 6 -1, 40 8 34 -1, 34 39 40 -1, 11 40 39 -1, 39 34 7 -1, 41 9 35 -1, 35 40 41 -1, 11 41 40 -1, 40 35 8 -1, 37 10 36 -1, 36 41 37 -1, 11 37 41 -1, 41 36 9 -1 \">")
            .arg("<Coordinate point=\"0 0 -1 0.7236 -0.52572 -0.447215 -0.276385 -0.85064 -0.447215 -0.894425 0 -0.447215 -0.276385 0.85064 -0.447215 0.7236 0.52572 -0.447215 0.276385 -0.85064 0.447215 -0.7236 -0.52572 0.447215 -0.7236 0.52572 0.447215 0.276385 0.85064 0.447215 0.894425 0 0.447215 0 0 1 0.425323 -0.309011 -0.850654 -0.162456 -0.499995 -0.850654 0.262869 -0.809012 -0.525738 0.425323 0.309011 -0.850654 0.850648 0 -0.525736 -0.52573 0 -0.850652 -0.688189 -0.499997 -0.525736 -0.162456 0.499995 -0.850654 -0.688189 0.499997 -0.525736 0.262869 0.809012 -0.525738 0.951058 0.309013 0 0.951058 -0.309013 0 0.587786 -0.809017 0 0 -1 0 -0.587786 -0.809017 0 -0.951058 -0.309013 0 -0.951058 0.309013 0 -0.587786 0.809017 0 0 1 0 0.587786 0.809017 0 0.688189 -0.499997 0.525736 -0.262869 -0.809012 0.525738 -0.850648 0 0.525736 -0.262869 0.809012 0.525738 0.688189 0.499997 0.525736 0.52573 0 0.850652 0.162456 -0.499995 0.850654 -0.425323 -0.309011 0.850654 -0.425323 0.309011 0.850654 0.162456 0.499995 0.850654\"/><ColorRGBA color=\"{x3dspherecol}\" /></IndexedFaceSet></Shape></Transform>]]></primitive>")
            .arg("<primitive name=\"box\"><![CDATA[<Transform {x3dvectors}><Shape><IndexedFaceSet coordIndex=\"0 1 2 -1 0 2 4 -1 4 2 6 -1 4 7 5 -1 4 6 7 -1 1 3 2 -1 1 0 4 -1 1 4 5 -1 3 7 2 -1 2 7 6 -1 3 1 7 -1 7 1 5 -1\" solid=\"false\"><Coordinate point=\"{coords}\" /><ColorRGBA color=\"{r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha}\" /></IndexedFaceSet></Shape></Transform>]]></primitive>")
            .arg("<primitive name=\"triangle\"><![CDATA[<Shape><IndexedFaceSet coordIndex=\"0 1 2 -1\" solid=\"false\"><Coordinate point=\"{p1x} {p1y} {p1z} {p2x} {p2y} {p2z} {p3x} {p3y} {p3z}\" /><ColorRGBA color=\"{r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha}\" /></IndexedFaceSet></Shape>]]></primitive>")
            .arg("<primitive name=\"dot\"><![CDATA[<Shape><PointSet><Coordinate point=\"{x} {y} {z}\" /><ColorRGBA color=\"{r} {g} {b} {alpha}\"/></PointSet></Shape>]]></primitive>")
            .arg("<primitive name=\"mesh\"><![CDATA[<Transform translation=\"{x3dtranslate}\"><Shape><IndexedFaceSet coordIndex=\"1 6 0 -1 0 6 5 -1 5 7 0 -1 0 7 2 -1 6 1 9 -1 1 8 9 -1 7 9 2 -1 2 9 8 -1\"><Coordinate point=\"{x3dcoord}\" /><ColorRGBA color=\"{r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha} {r} {g} {b} {alpha}\" /></IndexedFaceSet></Shape></Transform>]]></primitive> ")
            .arg("</template>");
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}
//FilterSSynth::~FilterSSynth(){}
QString FilterSSynth::filterName(FilterIDType filter) const
{
    switch(filter)
    {
    case CR_SSYNTH:
            return QString("Structure Synth Mesh Creation");
            break;
    default:
            assert(0); return QString("error");
            break;
    }
}

QString FilterSSynth::filterInfo(FilterIDType filterId) const
{
    switch(filterId)
    {
    case CR_SSYNTH:
            return QString("Structure Synth mesh creation based on Eisen Script.\n For further instruction visit http://structuresynth.sourceforge.net/reference.php");
           break;
    default:
            assert(0); return QString("error");
    }
}
void FilterSSynth::initParameterSet(QAction* filter,MeshDocument &md, RichParameterSet &par)
{
        //par.addParam(new RichOpenFile(QString("openf"),QString(""),QString(".es")));
    par.addParam(new RichString("grammar","set maxdepth 40 R1 R2 rule R1 { { x 1 rz 6 ry 6 s 0.99 } R1 { s 2 } sphere } rule R2 {{ x -1 rz 6 ry 6 s 0.99 } R2 { s 2 } sphere} ","Eisen Script grammar","Write a grammar according to Eisen Script specification and using the primitives box, sphere, mesh, dot "));
    par.addParam(new RichInt("seed",1,"seed for random construction",""));
   // par.addParam(new RichOpenFile("openg","",".ES","Open grammar",""));
    //par.addParam(new RichSaveFile("saveg",par.findParameter("grammar")->val,NULL));
    return;
}
void FilterSSynth::openX3D(const QString &fileName, MeshModel &m, int& mask, vcg::CallBackPos *cb, QWidget* parent)
{
        vcg::tri::io::AdditionalInfoX3D* info = NULL;
        int result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMask(fileName.toStdString().c_str(), info);
        m.Enable(info->mask);
        result = vcg::tri::io::ImporterX3D<CMeshO>::Open(m.cm, fileName.toStdString().c_str(), info, cb);
         vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);
         vcg::tri::UpdateNormals<CMeshO>::PerVertexNormalizedPerFaceNormalized(m.cm);
         mask=info->mask;
        delete(info);
}
bool FilterSSynth::applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    RichParameter* grammar=par.findParameter(QString("grammar"));
    RichParameter* seed=par.findParameter(QString("seed"));
    QString path=ssynth(grammar->val->getString(),seed->val->getInt(),cb);
    if(QFile::exists(path)){
    QFile file(path);
    int mask;
    const QString name(file.fileName());

        openX3D(name,*(md.mm()),mask,cb);
    file.remove();
    return true;
        }
        else{
                QWidget *  parent=(QWidget*)this->parent();
                QMessageBox::critical(parent,tr("Error"),tr("An error occurred during the mesh generation: ").append(path));
                return false;
        }
}
int FilterSSynth::getRequirements(QAction *)
{
    return MeshModel::MM_NONE;
}
 QString FilterSSynth::ssynth(QString grammar,int seed,CallBackPos *cb){
    QString path("");
        QString tdir = QApplication::applicationDirPath();
       // QFile filer(tdir.append(tr("/x3d.rendertemplate")));//il file per ora va messo nella cartella dove c'è l'eseguibile di meshlab
          //if(filer.exists()){
                        if (cb != NULL)		(*cb)(0, "Loading...");
            //Template templ(filer);
            Template templ(this->renderTemplate);
            MyTrenderer renderer(templ);
            renderer.begin();
            Preprocessor pp;
            QString out = pp.Process(grammar);
            Tokenizer token(out);
            EisenParser parser(&token);
                        try
                        {
            RuleSet* rs=parser.parseRuleset();
            rs->resolveNames();
            rs->dumpInfo();

            RandomStreams::SetSeed(seed);
            Builder b(&renderer,rs,false);
            b.build();
            renderer.end();
            QString output=renderer.getOutput();
            QFile file("output.x3d");
            if(!file.open(QFile::WriteOnly | QFile::Text)){assert(0); return QString("");}
            QTextStream outp(&file);
            outp << output;
            file.close();
            path=file.fileName();
                        if (cb != NULL){	(*cb)(99, "Done");}
                        }
                        catch(Exception& ex){
                                return ex.getMessage();
                        }
                        catch(ParseError& ex){
                                return ex.getMessage();
                        }
        /*}
          else path=QString("");*/
    return path;
}
int FilterSSynth::postCondition(QAction* filter) const
{
        return MeshModel::MM_VERTCOLOR;
}
 MeshFilterInterface::FilterClass FilterSSynth::getClass(QAction *filter)
{
            return MeshFilterInterface::MeshCreation;
}
QList<MeshIOInterface::Format> FilterSSynth::importFormats() const
 {
         QList<MeshIOInterface::Format> formats;
         formats<< MeshIOInterface::Format("Eisen Script File", tr("ES"));
         return formats;
 }
 QList<MeshIOInterface::Format> FilterSSynth::exportFormats() const
 {
         QList<MeshIOInterface::Format> formats;
         formats<< MeshIOInterface::Format("Eisen Script File", tr("ES"));
         return formats;
 }
 bool FilterSSynth::open(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet & par, CallBackPos *cb, QWidget *parent)
{
        //if (cb != NULL)		(*cb)(0, "Loading...");

        this->seed=par.findParameter("seed")->val->getInt();
        QFile grammar(fileName);
        grammar.open(QFile::ReadOnly|QFile::Text);
        QString gcontent(grammar.readAll());
        grammar.close();
        this->grammar=gcontent;
        QString x3dfile(FilterSSynth::ssynth(gcontent,this->seed,cb));
        if(QFile::exists(x3dfile)){
        openX3D(x3dfile,m,mask,cb);
        QFile x3df(x3dfile);
        x3df.remove();
        return true;
        }
        else{
            QMessageBox::critical(parent,tr("Error"),tr("An error occurred during the mesh generation: ").append(x3dfile));
                return false;
        }
 }
 bool FilterSSynth::save(const QString &formatName, const QString &fileName, MeshModel &m, const int mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget *parent)
 {
         if(formatName.toUpper()==QString("ES")){
                 if(grammar!=tr("")){
                        if (cb !=NULL) (*cb)(99, "Saving ES File...");
                        QFile tosave(fileName);
                        tosave.open(QFile::WriteOnly|QFile::Text);
                        QTextStream outp(&tosave);
                        outp << grammar;
                        tosave.close();
                        return true;
                 }
                 else QMessageBox::critical(parent,tr("Error"),tr("Nothing to save"));
         }
         else {
                 QMessageBox::critical(parent,tr("Error"),tr("Wrong format"));
                return false;
         }
 }
 void FilterSSynth::GetExportMaskCapability(QString &format, int &capability, int &defaultBits) const{}
 void FilterSSynth::initPreOpenParameter(const QString &formatName, const QString &filename, RichParameterSet &parlst){
         parlst.addParam(new RichInt(tr("seed"),1,tr("Seed for random mesh generation")));
 }
 Q_EXPORT_PLUGIN(FilterSSynth)



