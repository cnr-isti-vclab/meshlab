/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <GL/glew.h>
#include <common/mlapplication.h>
#include <common/mlexception.h>
#include <common/interfaces.h>
#include <common/pluginmanager.h>
#include <common/filterscript.h>
#include <common/meshlabdocumentxml.h>
#include <common/mlexception.h>
#include <common/filterparameter.h>
#include <wrap/qt/qt_thread_safe_memory_info.h>
#include "../meshlab/mainwindow.h"
#include <clocale>

#include <QFileInfo>


class FilterData
{
public:
    FilterData();
    QString name;
    QString info;
    int filterClass;
    bool operator <(const FilterData &d) const {return name<d.name;}
};

class MeshLabServer
{
public:
    MeshLabServer(MLSceneGLSharedDataContext* shar) 
		:shared(shar)
	{
	}

    ~MeshLabServer() 
	{
	}

    static bool filterCallBack(const int pos, const char * str)
    {
        int static lastPos=-1;
        if(pos==lastPos) return true;
        lastPos=pos;
        printf("%s",str);
        return true;
    }

    // Here we need a better way to find the plugins directory.
    // To be implemented:
    // use the QSettings togeter with MeshLab.
    // When meshlab starts if he find the plugins write the absolute path of that directory in a persistent qsetting place.
    // Here we use that QSetting. If it is not set we remember to run meshlab first once.
    // in this way it works safely on mac too and allows the user to put the small meshlabserver binary wherever they desire (/usr/local/bin).

    void loadPlugins()
    {
        PM.loadPlugins(defaultGlobal);

        printf("Total %i filtering actions\n", PM.actionFilterMap.size());
        printf("Total %i io plugins\n", PM.meshIOPlugins().size());
    }

    void dumpPluginInfoWiki(FILE *fp)
    {
        if(!fp) return;
        foreach(MeshFilterInterface *iFilter, PM.meshFilterPlugins())
            foreach(QAction *filterAction, iFilter->actions())
            fprintf(fp, "*<b><i>%s</i></b> <br>%s<br>\n",qPrintable(filterAction->text()), qPrintable(iFilter->filterInfo(filterAction)));
    }

    void dumpPluginInfoDoxygen(FILE *fp)
    {
        if(!fp) return;
        int i=0;
        QMap<QString, RichParameterSet> FPM = PM.generateFilterParameterMap();
        fprintf(fp,"/*! \\mainpage MeshLab Filter Documentation\n");
        //fprintf(fp,"\\AtBeginDocument{\\setcounter{tocdepth}{1}}");

        foreach(MeshFilterInterface *iFilter, PM.meshFilterPlugins())
        {
            foreach(QAction *filterAction, iFilter->actions())
            {
                fprintf(fp,
                    "\n\\section f%i %s \n\n"
                    "%s\n"
                    ,i++,qPrintable(filterAction->text()),qPrintable(iFilter->filterInfo(filterAction)));

                fprintf(fp,  "<H2> Parameters </h2>\n");
                //            fprintf(fp,  "\\paragraph fp%i Parameters\n",i);

                if(! FPM[filterAction->text()].paramList.empty())
                {
                    fprintf(fp,"<TABLE>\n");
                    foreach(RichParameter* pp, FPM[filterAction->text()].paramList)
                    {
                        fprintf(fp,"<TR><TD> \\c %s  </TD> <TD> %s </TD> <TD><i> %s -- </i></TD> </TR>\n",
                            qPrintable(pp->val->typeName()),qPrintable(pp->pd->fieldDesc),qPrintable(pp->pd->tooltip));
                    }
                    fprintf(fp,"</TABLE>\n");
                }
                else fprintf(fp,"No parameters.<br>");

            }
        }
        fprintf(fp,"*/");
    }

    bool importMesh(MeshModel &mm, const QString& fileName,FILE* fp = stdout)
    {
        // Opening files in a transparent form (IO plugins contribution is hidden to user)
        QStringList filters;

        // HashTable storing all supported formats togheter with
        // the (1-based) index  of first plugin which is able to open it
        QHash<QString, MeshIOInterface*> allKnownFormats;

        //PM.LoadFormats(filters, allKnownFormats,PluginManager::IMPORT);

        QFileInfo fi(fileName);
        // this change of dir is needed for subsequent textures/materials loading
        QDir curDir = QDir::current();
        QDir::setCurrent(fi.absolutePath());

        QString extension = fi.suffix();
        qDebug("Opening a file with extention %s",qPrintable(extension));
        // retrieving corresponding IO plugin
        MeshIOInterface* pCurrentIOPlugin = PM.allKnowInputFormats[extension.toLower()];
        if (pCurrentIOPlugin == 0)
        {
            fprintf(fp,"Error encountered while opening file: ");
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }
        int mask = 0;

        RichParameterSet prePar;
        pCurrentIOPlugin->initPreOpenParameter(extension, fileName,prePar);

        if (!pCurrentIOPlugin->open(extension, fileName, mm ,mask,prePar))
        {
            fprintf(fp,"MeshLabServer: Failed loading of %s from dir %s\n",qPrintable(fileName),qPrintable(QDir::currentPath()));
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }

        // In case of polygonal meshes the normal should be updated accordingly
        if( mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL)
        {
            mm.updateDataMask(MeshModel::MM_POLYGONAL); // just to be sure. Hopefully it should be done in the plugin...
            int degNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm.cm);
            if(degNum)
                fprintf(fp,"Warning model contains %i degenerate faces. Removed them.",degNum);
            mm.updateDataMask(MeshModel::MM_FACEFACETOPO);
            vcg::tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(mm.cm);
            vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mm.cm);
        } // standard case
        else {
            if( mask & vcg::tri::io::Mask::IOM_VERTNORMAL) // the mesh already has its per vertex normals (point clouds)
            {
                vcg::tri::UpdateNormal<CMeshO>::PerFace(mm.cm);
                vcg::tri::UpdateBounding<CMeshO>::Box(mm.cm);					// updates bounding box
            }
            else mm.UpdateBoxAndNormals(); // the very standard case
        }

        if(mm.cm.fn==0)
        {
            if (mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
                mm.updateDataMask(MeshModel::MM_VERTNORMAL);
        }
        else
            mm.updateDataMask(MeshModel::MM_VERTNORMAL);

		if (shared != NULL)
			shared->meshInserted(mm.id());
        //vcg::tri::UpdateBounding<CMeshO>::Box(mm.cm);
        QDir::setCurrent(curDir.absolutePath());
        return true;
    }

    bool exportMesh(MeshModel *mm, const int mask, const QString& fileName,FILE* fp = stdout)
    {
        QFileInfo fi(fileName);
        // this change of dir is needed for subsequent textures/materials loading
        QDir curDir = QDir::current();
        QDir::setCurrent(fi.absolutePath());

        QString extension = fi.suffix();

        // retrieving corresponding IO plugin
        MeshIOInterface* pCurrentIOPlugin = PM.allKnowOutputFormats[extension.toLower()];
        if (pCurrentIOPlugin == 0)
        {
            fprintf(fp,"Error encountered while opening file: ");
            //QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
            //QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }

        // optional saving parameters (like ascii/binary encoding)
        RichParameterSet savePar;
        pCurrentIOPlugin->initSaveParameter(extension, *mm, savePar);
        int formatmask = 0;
        int defbits = 0;
        pCurrentIOPlugin->GetExportMaskCapability(extension,formatmask,defbits);
        if (!pCurrentIOPlugin->save(extension, fileName, *mm ,mask & formatmask, savePar))
        {
            fprintf(fp,"Failed saving\n");
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }
        QDir::setCurrent(curDir.absolutePath());
        return true;
    }

    bool openProject(MeshDocument& md,const QString& filename)
    {
      QDir curDir = QDir::current();
        QFileInfo fi(filename);
        bool opened = MeshDocumentFromXML(md,fi.absoluteFilePath());
        if (!opened)
            return false;
        QDir::setCurrent(fi.absolutePath());
        //WARNING! I'm not putting inside MeshDocumentFromXML function because I'm too scared of what can happen inside MeshLab code....
        md.setFileName(fi.absoluteFilePath());
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i=0; i<md.meshList.size(); i++)
        {
            if (md.meshList[i] != NULL)
            {
                QString fullPath = md.meshList[i]->fullName();
                md.setBusy(true);
                Matrix44m trm = md.meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
                if (!importMesh(*md.meshList[i],fullPath))
                {
                    md.delMesh(md.meshList[i]);
                    md.setBusy(false);
                    QDir::setCurrent(curDir.absolutePath());
                    return false;
                }
                else
                    md.meshList[i]->cm.Tr=trm;
                md.setCurrentMesh(md.meshList[i]->id());
                md.setBusy(false);
            }
        }
        QDir::setCurrent(curDir.absolutePath());
        return true;
    }

    bool saveProject(MeshDocument& md,const QString& filename,const QString& outfilemeshmiddlename = QString())
    {
        QFileInfo outprojinfo(filename);
        QString outdir = outprojinfo.absolutePath();

        QDir curDir = QDir::current();
        QDir::setCurrent(outprojinfo.absolutePath());
        foreach(MeshModel* m,md.meshList)
        {
            if (m != NULL)
            {
                QString outfilename;
                QFileInfo fi(m->fullName());
                if (!fi.exists())
                    outfilename = outdir + "/" + m->label().remove(" ") + outfilemeshmiddlename + ".ply";
                else
                    outfilename =  fi.absolutePath() + "/" + fi.completeBaseName() + outfilemeshmiddlename + "." + fi.completeSuffix();
                m->setFileName(outfilename);
                QFileInfo of(outfilename);
                m->setLabel(of.fileName());
                exportMesh(m,m->dataMask(),outfilename);
            }
        }

        QDir::setCurrent(curDir.absolutePath());
        return MeshDocumentToXMLFile(md,filename,false);
    }

    bool script(MeshDocument &meshDocument,const QString& scriptfile,FILE* fp)
    {
        MeshModel* mm = meshDocument.mm();

        FilterScript scriptPtr;

        //Open/Load FilterScript

        if (scriptfile.isEmpty())
        {
            printf("No script specified\n");
            return false;
        }
        if (!scriptPtr.open(scriptfile))
        {
            printf("File %s was not found.\n",qPrintable(scriptfile));
            return false;
        }
        fprintf(fp,"Starting Script of %i actions",scriptPtr.filtparlist.size());
        GLLogStream log;
        for(FilterScript::iterator ii = scriptPtr.filtparlist.begin();ii!= scriptPtr.filtparlist.end();++ii)
        {
            bool ret = false;
            //RichParameterSet &par = (*ii).second;
            QString fname = (*ii)->filterName();
            fprintf(fp,"filter: %s\n",qPrintable(fname));
            if (!(*ii)->isXMLFilter())
            {
                QAction *action = PM.actionFilterMap[ fname];
                if (action == NULL)
                {
                    fprintf(fp,"filter %s not found",qPrintable(fname));
                    return false;
                }

                MeshFilterInterface *iFilter = qobject_cast<MeshFilterInterface *>(action->parent());
                iFilter->setLog(&log);
                int req = iFilter->getRequirements(action);
                mm->updateDataMask(req);
                //make sure the PARMESH parameters are initialized

                //A filter in the script file couldn't have all the required parameter not defined (a script file not generated by MeshLab).
                //So we have to ask to the filter the default values for all the parameters and integrate them with the parameters' values
                //defined in the script file.
                RichParameterSet required;
                iFilter->initParameterSet(action,meshDocument,required);
                OldFilterNameParameterValuesPair* pairold = reinterpret_cast<OldFilterNameParameterValuesPair*>(*ii);
                RichParameterSet &parameterSet = pairold->pair.second;

                //The parameters in the script file are more than the required parameters of the filter. The script file is not correct.
                if (required.paramList.size() < parameterSet.paramList.size())
                {
                    fprintf(fp,"The parameters in the script file are more than the filter %s requires.\n",qPrintable(fname));
                    return false;
                }

                for(int i = 0; i < required.paramList.size(); i++)
                {
                    RichParameterCopyConstructor v;
                    if (!parameterSet.hasParameter(required.paramList[i]->name))
                    {
                        required.paramList[i]->accept(v);
                        parameterSet.addParam(v.lastCreated);
                    }
                    assert(parameterSet.paramList.size() == required.paramList.size());
                    RichParameter* parameter = parameterSet.paramList[i];
                    //if this is a mesh paramter and the index is valid
                    if(parameter->val->isMesh())
                    {
                        RichMesh* md = reinterpret_cast<RichMesh*>(parameter);
                        if(	md->meshindex < meshDocument.size() &&
                            md->meshindex >= 0  )
                        {
                            RichMesh* rmesh = new RichMesh(parameter->name,meshDocument.getMesh(md->meshindex),&meshDocument);
                            parameterSet.paramList.replace(i,rmesh);
                        } else
                        {
                            fprintf(fp,"Meshes loaded: %i, meshes asked for: %i \n", meshDocument.size(), md->meshindex );
                            fprintf(fp,"One of the filters in the script needs more meshes than you have loaded.\n");
                            exit(-1);
                        }
                        delete parameter;
                    }
                }

				QGLWidget* wid = NULL;
				if (shared != NULL)
				{
					wid = new QGLWidget(NULL,shared);
					iFilter->glContext = new MLPluginGLContext(QGLFormat::defaultFormat(), wid->context()->device(),*shared);
					bool created = iFilter->glContext->create(wid->context());
					if ((!created) || (!iFilter->glContext->isValid()))
					{
						fprintf(fp, "A valid GLContext is required by the filter to work.\n");
						return false;
					}
					MLRenderingData dt;
					MLRenderingData::RendAtts atts;
					atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
					atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;

					if (iFilter->filterArity(action) == MeshFilterInterface::SINGLE_MESH)
					{
						MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDocument.mm());
						if ((pm != MLRenderingData::PR_ARITY) && (meshDocument.mm() != NULL))
						{
							dt.set(pm, atts);
							iFilter->glContext->initPerViewRenderingData(meshDocument.mm()->id(), dt);
						}
					}
					else
					{
						for (int ii = 0; ii < meshDocument.meshList.size(); ++ii)
						{
							MeshModel* mm = meshDocument.meshList[ii];
							MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(mm);
							if ((pm != MLRenderingData::PR_ARITY) && (mm != NULL))
							{
								dt.set(pm, atts);
								iFilter->glContext->initPerViewRenderingData(mm->id(), dt);
							}
						}
					}
				}
                meshDocument.setBusy(true);
                ret = iFilter->applyFilter( action, meshDocument, pairold->pair.second, filterCallBack);
                meshDocument.setBusy(false);
				if (shared != NULL)
					delete iFilter->glContext;
				delete wid;
				
            }
            else
            {

                MeshLabXMLFilterContainer cont = PM.stringXMLFilterMap[ fname];
                MLXMLPluginInfo* info = cont.xmlInfo;
                MeshLabFilterInterface* cppfilt = cont.filterInterface;
                try
                {
                    if (cppfilt != NULL)
                    {
                        cppfilt->setLog(&log);

                        Env env;
                        env.loadMLScriptEnv(meshDocument,PM);
                        XMLFilterNameParameterValuesPair* xmlfilt = reinterpret_cast<XMLFilterNameParameterValuesPair*>(*ii);
                        QMap<QString,QString>& parmap = xmlfilt->pair.second;
                        for(QMap<QString,QString>::const_iterator it = parmap.constBegin();it != parmap.constEnd();++it)
                            env.insertExpressionBinding(it.key(),it.value());

                        EnvWrap envwrap(env);
                        MLXMLPluginInfo::XMLMapList params = info->filterParameters(fname);
                        for(int i = 0; i < params.size(); ++i)
                        {
                            MLXMLPluginInfo::XMLMap& parinfo = params[i];

                            //if this is a mesh parameter and the index is valid
                            if(parinfo[MLXMLElNames::paramType]  == MLXMLElNames::meshType)
                            {
                                QString& parnm = parinfo[MLXMLElNames::paramName];
                                MeshModel* meshmdl = envwrap.evalMesh(parnm);
                                if( meshmdl == NULL)
                                {
                                    //parnm is associated with ,
                                    printf("Meshes loaded: %i, meshes asked for: %i \n", meshDocument.size(), envwrap.evalInt(parnm) );
                                    printf("One of the filters in the script needs more meshes than you have loaded.\n");
                                    return false;
                                }
                            }
                        }
						QGLWidget* wid = NULL;
						if (shared != NULL)
						{
							wid = new QGLWidget(NULL, shared);
							cppfilt->glContext = new MLPluginGLContext(QGLFormat::defaultFormat(), wid->context()->device(), *shared);
							bool created = cppfilt->glContext->create(wid->context());
							if ((!created) || (!cppfilt->glContext->isValid()))
							{
								fprintf(fp, "A valid GLContext is required by the filter to work.\n");
								return false;
							}

							MLRenderingData dt;
							MLRenderingData::RendAtts atts;
							atts[MLRenderingData::ATT_NAMES::ATT_VERTPOSITION] = true;
							atts[MLRenderingData::ATT_NAMES::ATT_VERTNORMAL] = true;

							if (info->filterAttribute(fname, MLXMLElNames::filterArity) == MLXMLElNames::singleMeshArity)
							{
								MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDocument.mm());
								if ((pm != MLRenderingData::PR_ARITY) && (meshDocument.mm() != NULL))
								{
									dt.set(pm, atts);
									cppfilt->glContext->initPerViewRenderingData(meshDocument.mm()->id(), dt);
								}
							}
							else
							{
								for (int ii = 0; ii < meshDocument.meshList.size(); ++ii)
								{
									MeshModel* mm = meshDocument.meshList[ii];
									MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(mm);
									if ((pm != MLRenderingData::PR_ARITY) && (mm != NULL))
									{
										dt.set(pm, atts);
										cppfilt->glContext->initPerViewRenderingData(mm->id(), dt);
									}
								}
							}
						}

                        //WARNING!!!!!!!!!!!!
                        /* IT SHOULD INVOKE executeFilter function. Unfortunately this function create a different thread for each invoked filter, and the MeshLab synchronization mechanisms are quite naive. Better to invoke the filters list in the same thread*/
                        meshDocument.setBusy(true);
                        ret = cppfilt->applyFilter( fname, meshDocument, envwrap, filterCallBack );
                        meshDocument.setBusy(false);
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        if (shared != NULL)
							delete cppfilt->glContext;
						delete wid;
                    }
                    else
                        throw MLException("WARNING! The MeshLab Script System is able to manage just the C++ XML filters.");
                }
                catch (MLException& e)
                {
                    meshDocument.Log.Log(GLLogStream::WARNING,e.what());
                }
            }
            QStringList logOutput;
            log.print(logOutput);
            foreach(QString logEntry, logOutput)
                fprintf(fp,"%s\n",qPrintable(logEntry));
            if(!ret)
            {
                fprintf(fp,"Problem with filter: %s\n",qPrintable(fname));
                return false;
            }
        }
        return true;
    }

private:
    PluginManager PM;
    RichParameterSet defaultGlobal;
	MLSceneGLSharedDataContext* shared;
};

namespace commandline
{
    const char inproject('p');
    const char outproject('w');
    const char overwrite('x');
    const char inputmeshes('i');
    const char outputmesh('o');
    const char mask('m');
    const char vertex('v');
    const char face('f');
    const char wedge('w');
    const char color('c');
    const char flags('f');
    const char normal('n');
    const char quality('q');
    const char texture('t');
    const char log('l');
    const char dump('d');
    const char script('s');

    void usage()
    {
        QFile docum(":/meshlabserver.txt");
        if (!docum.open(QIODevice::ReadOnly))
        {
            printf("MeshLabServer was not able to locate meshlabserver.txt file. The program will be closed\n");
            exit(-1);
        }
        QString help(docum.readAll());
        printf("\nUsage:\n%s",qPrintable(help));
        docum.close();
    }

    QString optionValueExpression(const char cmdlineopt)
    {
        return QString ("-" + QString(cmdlineopt) + "\\s+\\S+");
    }

    QString outputmeshExpression()
    {
        return optionValueExpression(outputmesh) + "(\\s+-" + QString(mask) + "(\\s+(" + QString(vertex) + "|" + QString(face) + "|" + QString(wedge) + ")+(" + QString(color) + "|" + QString(quality) + "|" + QString(flags) + "|" + QString(normal) + "|" + QString(texture) + ")+)+)?";
    }

    bool validateCommandLine(const QString& str)
    {
        QString logstring("(" + optionValueExpression(log) + "\\s+" +  optionValueExpression(dump) + "|" + optionValueExpression(dump) + "\\s+" +  optionValueExpression(log) + "|" +  optionValueExpression(dump) + "|" + optionValueExpression(log) + ")");
        //QString remainstring("(" + optionValueExpression(inproject) + "|" + optionValueExpression(inputmeshes,true) + ")" + "(\\s+" + optionValueExpression(inproject) + "|\\s+" + optionValueExpression(inputmeshes,true) + ")*(\\s+" + optionValueExpression(outproject) + "|\\s+" + optionValueExpression(script) + "|\\s+" + outputmeshExpression() + ")*");
        QString arg("(" + optionValueExpression(inproject) + "|" + optionValueExpression(inputmeshes) + "|" + optionValueExpression(outproject) + "(\\s+-" + overwrite + ")?" + "|" + optionValueExpression(script) + "|" + outputmeshExpression() + ")");
        QString args("(" + arg + ")(\\s+" + arg + ")*");
        QString completecommandline("(" + logstring + "|" + logstring + "\\s+" + args + "|" + args + ")");
        QRegExp completecommandlineexp(completecommandline);
        completecommandlineexp.indexIn(str);
        QString rr = completecommandlineexp.cap();
        return (completecommandlineexp.matchedLength() == str.size());
    }

}

struct OutFileMesh
{
    QString filename;
    int mask;
};

struct OutProject
{
    QString filename;
    bool overwrite;
};

int main(int argc, char *argv[])
{
    FILE* logfp = stdout;
    FILE* dumpfp = NULL;
    MeshLabApplication app(argc, argv);
	std::setlocale(LC_ALL, "C");
	QLocale::setDefault(QLocale::C);
    if(argc == 1)
    {
        commandline::usage();
		//system("pause");
        exit(-1);
    }
    QStringList scriptfiles;
    QList<OutFileMesh> outmeshlist;
    QList<OutProject> outprojectfiles;

    QString cmdline;
    for(int ii = 1;ii < argc;++ii)
        cmdline = cmdline + argv[ii] + " ";
    if (!commandline::validateCommandLine(cmdline.trimmed()))
    {
        printf("CommandLine Syntax Error: please refer to the following documentation for a complete list of the MeshLabServer parameters.\n");
        commandline::usage();
		//system("pause");
        exit(-1);
    }

	QSettings settings(MeshLabApplication::organization(),MeshLabApplication::appArchitecturalName(MeshLabApplication::HW_64BIT));

	QVariant xmlgpupar = settings.value(MainWindowSetting::maximumDedicatedGPUMem());

	QDomDocument doc;
	doc.setContent(xmlgpupar.toString(), false);

	QDomElement paramelem = doc.firstChild().toElement();
	int gpumemmb = paramelem.attribute("value").toInt();

	std::ptrdiff_t maxgpumem = (std::ptrdiff_t) gpumemmb * (float)(1024 * 1024);
	vcg::QtThreadSafeMemoryInfo gpumeminfo(maxgpumem);

	MeshDocument meshDocument;

	MLSceneGLSharedDataContext shared(meshDocument, gpumeminfo, MeshLabScalarTest<MESHLAB_SCALAR>::doublePrecision(), 100000,100000);
	shared.makeCurrent();
	GLenum err = glewInit();
	if (err != GLEW_NO_ERROR)
	{
		printf("GLEW Init: failed!\n");
		//system("pause");
		exit(-1);
	}
	shared.doneCurrent();
    printf("Loading Plugins:\n");
	MeshLabServer server(&shared);
    server.loadPlugins();

    
    int i = 1;
    while(i < argc)
    {
        QString tmp = argv[i];
        switch(argv[i][1])
        {
        case commandline::inproject :
            {
                if (((i+1) < argc) && (argv[i+1][0] != '-'))
                {
                    QFileInfo finfo(argv[i+1]);
                    QString inputproject = finfo.absoluteFilePath();
                    if (finfo.completeSuffix().toLower() != "mlp")
                    {
                        fprintf(logfp,"Project %s is not a valid \'mlp\' file format. MeshLabServer application will exit.\n",qPrintable(inputproject));
						//system("pause");
                        exit(-1);
                    }
                    bool opened = server.openProject(meshDocument,inputproject);
                    if (!opened)
                    {
                        fprintf(logfp,"MeshLab Project %s has not been correctly opened. MeshLabServer application will exit.\n",qPrintable(inputproject));
						//system("pause");
                        exit(-1);
                    }
                    else
                        fprintf(logfp,"MeshLab Project %s has been loaded.\n",qPrintable(inputproject));
                    ++i;
                }
                else
                {
                    fprintf(logfp,"Missing project name. MeshLabServer application will exit.\n");
					//system("pause");
                    exit(-1);
                }
                ++i;
                break;
            }
        case commandline::outproject :
            {
                if (((i+1) < argc) && (argv[i+1][0] != '-'))
                {
                    QFileInfo finfo(argv[i+1]);
                    OutProject pr;
                    pr.overwrite = false;
                    pr.filename = finfo.absoluteFilePath();
                    if (finfo.completeSuffix().toLower() != "mlp")
                    {
                        fprintf(logfp,"Project %s is not a valid \'mlp\' file format. Output file will be renamed as %s.mlp .\n",qPrintable(pr.filename),qPrintable(pr.filename + ".mlp"));
                        pr.filename += ".mlp";
                    }
                    ++i;
					QString overtmp('-'); 
					overtmp += commandline::overwrite;
                    if (((i + 1) < argc) && (QString(argv[i+1]) == overtmp))
                    {
                        pr.overwrite = true;
                        ++i;
                    }
                    outprojectfiles << pr;
                }
                ++i;
                break;
            }
        case commandline::inputmeshes :
            {
                while( ((i+1) < argc) && argv[i+1][0] != '-')
                {
                    QFileInfo info(argv[i+1]);
                    //now add it to the document
                    MeshModel* mmod = meshDocument.addNewMesh(info.absoluteFilePath(),"");
                    if (mmod == NULL)
                    {
                        fprintf(logfp,"It was not possible to add new mesh %s to MeshLabServer. The program will exit\n",qPrintable(info.absoluteFilePath()));
						//system("pause");
                        exit(-1);
                    }
                    bool opened = server.importMesh(*mmod, info.absoluteFilePath(),logfp);
                    if (!opened)
                    {
                        fprintf(logfp,"It was not possible to import mesh %s into MeshLabServer. The program will exit\n ",qPrintable(info.absoluteFilePath()));
						//system("pause");
                        exit(-1);
                    }
                    fprintf(logfp,"Mesh %s loaded has %i vn %i fn\n", qPrintable(info.absoluteFilePath()), mmod->cm.vn, mmod->cm.fn);
                    i++;
                }
                i++;
                break;
            }
        case commandline::outputmesh :
            {
                QString fileout;
                int mask = 0;
                OutFileMesh outfl;
                if( ((i+1) < argc) && argv[i+1][0] != '-')
                {
                    QFileInfo info(argv[i+1]);
                    outfl.filename = info.absoluteFilePath();
                    fprintf(logfp,"output mesh  %s\n", qPrintable(outfl.filename));
                    i++;
                }
                if (((i + 1) < argc) && (QString(argv[i+1]) == (QString("-") + commandline::mask)))
                {
                    i = i + 2;
                    do
                    {
                        switch (argv[i][0])
                        {
                        case commandline::vertex :
                            {
                                switch (argv[i][1])
                                {
                                case commandline::color : i++; fprintf(logfp,"vertex color, "     ); mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;    break;
                                case commandline::flags : i++; fprintf(logfp,"vertex flags, "     ); mask |= vcg::tri::io::Mask::IOM_VERTFLAGS;    break;
                                case commandline::normal : i++; fprintf(logfp,"vertex normals, "   ); mask |= vcg::tri::io::Mask::IOM_VERTNORMAL;   break;
                                case commandline::quality : i++; fprintf(logfp,"vertex quality, "   ); mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;  break;
                                case commandline::texture : i++; fprintf(logfp,"vertex tex coords, "); mask |= vcg::tri::io::Mask::IOM_VERTTEXCOORD; break;
                                default :  i++; fprintf(logfp,"WARNING: unknowns per VERTEX attribute '%s'",argv[i+1]);break;
                                }
                                break;
                            }
                        case commandline::face :
                            {
                                switch (argv[i][1])
                                {
                                case commandline::color : i++; fprintf(logfp,"face color, "  ); mask |= vcg::tri::io::Mask::IOM_FACECOLOR;   break;
                                case commandline::flags : i++; fprintf(logfp,"face flags, "  ); mask |= vcg::tri::io::Mask::IOM_FACEFLAGS;   break;
                                case commandline::normal : i++; fprintf(logfp,"face normals, "); mask |= vcg::tri::io::Mask::IOM_FACENORMAL;  break;
                                case commandline::quality : i++; fprintf(logfp,"face quality, "); mask |= vcg::tri::io::Mask::IOM_FACEQUALITY; break;
                                default :  i++; fprintf(logfp,"WARNING: unknowns per FACE attribute '%s'",argv[i+1]);break;
                                }
                                break;
                            }
                        case commandline::wedge :
                            {
                                switch (argv[i][1])
                                {
                                case commandline::color : i++; fprintf(logfp,"wedge color, "     ); mask |= vcg::tri::io::Mask::IOM_WEDGCOLOR;   break;
                                case commandline::normal : i++; fprintf(logfp,"wedge normals, "   ); mask |= vcg::tri::io::Mask::IOM_WEDGNORMAL;  break;
                                case commandline::texture : i++; fprintf(logfp,"wedge tex coords, "); mask |= vcg::tri::io::Mask::IOM_WEDGTEXCOORD;break;
                                default :  i++; fprintf(logfp,"WARNING: unknowns per WEDGE attribute '%s'",argv[i+1]);break;
                                }
                                break;
                            }
                        default :  i++; fprintf(logfp,"WARNING: unknowns attribute '%s'",argv[i]);break;
                        }
                    }while (((i) < argc) && (argv[i][0] != '-'));
                }
                else
                    ++i;
                outfl.mask = mask;
                outmeshlist << outfl;
                break;
            }
        case commandline::script :
            {
                QFileInfo fi(argv[i+1]);
                QString scriptName = fi.absoluteFilePath();
                scriptfiles << scriptName;
                i += 2;
                break;
            }
        case commandline::log :
            {
                //freopen redirect both std::cout and printf. Now I'm quite sure i will get everything the plugins will print in the standard output (i hope no one used std::cerr...)
                logfp = fopen(argv[i+1],"a");
                if (logfp == NULL)
                    printf("Error occurred opening file %s. It's not possible to redirect the output from the stdout\n",argv[i+1]);
                else
                    printf("Log is saved in %s\n", argv[i+1]);
                i += 2;
                break;
            }
        case commandline::dump :
            {
                dumpfp = fopen(argv[i+1],"w");
                if (dumpfp == NULL)
                    fprintf(logfp,"Error occurred opening file %s. It's not possible to redirect the output from the stdout\n",argv[i+1]);
                else
                {
                    server.dumpPluginInfoDoxygen(dumpfp);
                    fclose(dumpfp);
                    fprintf(logfp,"Dump file is saved in %s\n", argv[i+1]);
                }
                i+=2;
                break;
            }
        default:
            {
                printf("Something bad happened parsing the document. String %s\n",qPrintable(argv[i]));
				//system("pause");
                exit(-1);
            }
        }
    }

    for(int ii = 0; ii < scriptfiles.size();++ii)
    {
        fprintf(logfp,"Apply FilterScript: '%s'\n",qPrintable(scriptfiles[ii]));
        bool returnValue = server.script(meshDocument, scriptfiles[ii],logfp);
        if(!returnValue)
        {
            fprintf(logfp,"Failed to apply script file %s\n",qPrintable(scriptfiles[ii]));
			//system("pause");
            exit(-1);
        }
    }

    for(int ii = 0;ii < outprojectfiles.size();++ii)
    {
        QString outfilemiddlename = "";
        if (!outprojectfiles[ii].overwrite)
        {
            outfilemiddlename = "_out";
            if (ii >= 1)
                outfilemiddlename += QString::number(ii);
        }
        bool saved = server.saveProject(meshDocument,outprojectfiles[ii].filename,outfilemiddlename);
        if (saved)
            fprintf(logfp,"Output project has been saved in %s.\n",qPrintable(outprojectfiles[ii].filename));
        else
        {
            fprintf(logfp,"Project %s has not been correctly saved in. MeshLabServer Application will exit.\n",qPrintable(outprojectfiles[ii].filename));
			//system("pause");
            exit(-1);
        }
    }

	if (meshDocument.size() < outmeshlist.size())
		fprintf(logfp, "Error: trying to save %i meshes, but only %i available in the project\n", qPrintable(outmeshlist.size()), qPrintable(meshDocument.size()));
	else
	{
		for (int ii = 0; ii < outmeshlist.size(); ++ii)
		{
			if (meshDocument.meshList[ii] != NULL)
			{
				bool exported = server.exportMesh(meshDocument.meshList[ii], outmeshlist[ii].mask, outmeshlist[ii].filename, logfp);
				if (exported)
					fprintf(logfp, "Mesh %s saved as %s (%i vn %i fn)\n", qPrintable(meshDocument.mm()->fullName()), qPrintable(outmeshlist[ii].filename), meshDocument.mm()->cm.vn, meshDocument.mm()->cm.fn);
				else
					fprintf(logfp, "Output mesh %s has NOT been saved\n", qPrintable(outmeshlist[ii].filename));
			}
			else
				fprintf(logfp, "Invalid layer %i. Output mesh %s will not be saved\n", qPrintable(ii), qPrintable(outmeshlist[ii].filename));
		}

	}


    if((logfp != NULL) && (logfp != stdout))
        fclose(logfp);

	shared.deAllocateGPUSharedData();
	//system("pause");
    return 0;
}

