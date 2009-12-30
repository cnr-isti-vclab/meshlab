#include <Qt>
#include <QtGui>
#include "filter_ssynth.h"
#include <meshlabplugins/io_x3d/import_x3d.h>
#include <common/meshmodel.h>
#include <common/interfaces.h>
/*#include <meshlabplugins/io_base/baseio.h>
#include "structuresynth/StructureSynth/Model/Rendering/TemplateRenderer.h"
#include "structuresynth/StructureSynth/Model/RandomStreams.h"
#include "structuresynth/StructureSynth/Parser/Preprocessor.h"
#include "structuresynth/StructureSynth/Parser/Tokenizer.h"
#include "structuresynth/StructureSynth/Parser/EisenParser.h"*/
using namespace std;
using namespace vcg;
using namespace vcg::tri::io;
/*using namespace StructureSynth::Parser;
using namespace StructureSynth::Model;
using namespace StructureSynth::Model::Rendering;*/
FilterSSynth::FilterSSynth(){
    typeList<< CR_SSYNTH;
    //FilterSSynth::mask=0;
    FilterIDType tt;
    foreach(tt , types())
        actionList << new QAction(filterName(tt), this);
}
//FilterSSynth::~FilterSSynth(){}
QString FilterSSynth::filterName(FilterIDType filter) const
{
    switch(filter)
    {
    case CR_SSYNTH:
            return QString("Structure Synth mesh creation");
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
    par.addParam(new RichString("grammar","","Eisen Script grammar","Write a grammar according to Eisen Script specification and using the primitives box, sphere, mesh, dot "));
    par.addParam(new RichInt("seed",0,"seed for random construction",""));
    return;
}
bool FilterSSynth::openX3D(const QString &formatName, const QString &fileName, MeshModel &m, int& mask, const RichParameterSet &, vcg::CallBackPos *cb, QWidget* parent)
{
    mask = 0;

    // initializing progress bar status
    if (cb != NULL)		(*cb)(0, "Loading...");

    QString errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nError details: %3";
    string filename = QFile::encodeName(fileName).constData ();
    bool normalsUpdated = false;
    vcg::tri::io::AdditionalInfoX3D* info = NULL;
    if(formatName.toUpper() == tr("X3D") || formatName.toUpper() == tr("X3DV") || formatName.toUpper() == tr("WRL"))
    {
            int result;
            if (formatName.toUpper() == tr("X3D"))
                    result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMask(filename.c_str(), info);
            else
                    result = vcg::tri::io::ImporterX3D<CMeshO>::LoadMaskVrml(filename.c_str(), info);
            if ( result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
            {
                    QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, info->filenameStack[info->filenameStack.size()-1], vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
                    return false;
            }
            if (info->mask & vcg::tri::io::Mask::IOM_VERTTEXCOORD)
            {
                    info->mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;
                    info->mask &=(~vcg::tri::io::Mask::IOM_VERTTEXCOORD);
            }
            if (info->mask & vcg::tri::io::Mask::IOM_WEDGCOLOR)
                    info->mask &=(~vcg::tri::io::Mask::IOM_WEDGCOLOR);
            if (info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
                    info->mask &=(~vcg::tri::io::Mask::IOM_WEDGNORMAL);
            m.Enable(info->mask);

            errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nFile: %2\nLine number: %3\nError details: %4";
            result = vcg::tri::io::ImporterX3D<CMeshO>::Open(m.cm, filename.c_str(), info, cb);
            if (result != vcg::tri::io::ImporterX3D<CMeshO>::E_NOERROR)
            {
                    QString fileError = info->filenameStack[info->filenameStack.size()-1];
                    QString lineError;
                    lineError.setNum(info->lineNumberError);
                    QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName, fileError, lineError, vcg::tri::io::ImporterX3D<CMeshO>::ErrorMsg(result)));
                    return false;
            }
            if (m.cm.vert.size() == 0)
            {
                    errorMsgFormat = "Error encountered while loading file:\n\"%1\"\n\nError details: File without a geometry";
                    QMessageBox::critical(parent, tr("X3D Opening Error"), errorMsgFormat.arg(fileName));
                    return false;
            }
            if(info->mask & vcg::tri::io::Mask::IOM_WEDGNORMAL)
                    normalsUpdated = true;
            mask = info->mask;
    }
    // verify if texture files are present
    QString missingTextureFilesMsg = "The following texture files were not found:\n";
    bool someTextureNotFound = false;
    for(unsigned int tx = 0; tx < info->textureFile.size(); ++tx)
    {
            FILE* pFile = fopen (info->textureFile[tx].toStdString().c_str(), "r");
            if (pFile == NULL)
            {
                    missingTextureFilesMsg.append("\n");
                    missingTextureFilesMsg.append(info->textureFile[tx].toStdString().c_str());
                    someTextureNotFound = true;
            }
            else
            {
                    m.cm.textures.push_back(info->textureFile[tx].toStdString());
                    fclose (pFile);
            }
    }


    if (someTextureNotFound)
    {
            QMessageBox::warning(parent, tr("Missing texture files"), missingTextureFilesMsg);
    }

    vcg::tri::UpdateBounding<CMeshO>::Box(m.cm);					// updates bounding box
    if (!normalsUpdated)
            vcg::tri::UpdateNormals<CMeshO>::PerVertex(m.cm);		// updates normals

    if (cb != NULL)	(*cb)(99, "Done");

    delete(info);
    return true;
}
bool FilterSSynth::applyFilter(QAction*  filter, MeshDocument &md, RichParameterSet & par, vcg::CallBackPos *cb)
{
    RichParameter* grammar=par.findParameter(QString("grammar"));
    RichParameter* seed=par.findParameter(QString("seed"));
    QString path=ssynth(grammar->val->getString(),seed->val->getInt());
    QFile file(path);
    int mask;
    const QString ext("X3D");
    const QString name(file.fileName());
    MeshModel* mm=md.mm();
    openX3D(ext,name,*(mm),mask,par,cb);
    file.remove();
    return true;
}
int FilterSSynth::getRequirements(QAction *){
    return MeshModel::MM_NONE;
}
QString FilterSSynth::ssynth(QString grammar,int seed){
    QString path("");
    /*      QFile filer(QString(x3d.rendertemplate"));
            Template templ(filer);
            TemplateRenderer renderer(templ);
            renderer.begin();
            Preprocessor pp;
            QString out = pp.Process(grammar);
            Tokenizer token(out);
            EisenParser parser(&token);
            RuleSet* rs=parser.parseRuleset();
            rs->resolveNames();
            rs->dumpInfo();
            RandomStreams::SetSeed(seed);
            Builder b(&renderer,rs,false);
            b.buildMeshlab();
            renderer.end();
            QString output=renderer.getOutput();
            QFile file("output.x3d");
            if(!file.open(QFile::WriteOnly | QFile::Text)){assert(0); return;}
            QTextStream outp(&file);
            outp << output;
            file.close();
            QString path(file.fileName());
*/
    return path;
}
 MeshFilterInterface::FilterClass FilterSSynth::getClass(QAction *filter)
{
            return MeshFilterInterface::MeshCreation;
}
 Q_EXPORT_PLUGIN(FilterSSynth)
