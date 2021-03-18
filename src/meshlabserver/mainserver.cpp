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

#include <common/GLExtensionsManager.h>
#include <common/mlapplication.h>
#include <common/mlexception.h>
#include <common/plugins/plugin_manager.h>
#include <common/globals.h>
#include <common/filterscript.h>
#include <common/meshlabdocumentxml.h>
#include <common/meshlabdocumentbundler.h>
#include <common/mlexception.h>
#include <common/parameters/rich_parameter_list.h>
#include <wrap/qt/qt_thread_safe_memory_info.h>
#include <wrap/io_trimesh/alnParser.h>
#include <vcg/complex/algorithms/create/platonic.h>

#include <clocale>

#include <QDir>
#include <QGLFormat>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QSettings>


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
        PM.loadPlugins();

        //printf("Total %i filtering actions\n", PM.actionFilterMap.size());
        printf("Total %i io plugins\n", PM.numberIOPlugins());
    }

    void dumpPluginInfoWiki(FILE *fp)
    {
        if(!fp) return;
        for(FilterPlugin *iFilter: PM.filterPluginIterator())
            for(QAction *filterAction: iFilter->actions())
                fprintf(fp, "*<b><i>%s</i></b> <br>%s<br>\n", qUtf8Printable(filterAction->text()), qUtf8Printable(iFilter->filterInfo(filterAction)));
    }
	
	QMap<QString, RichParameterList> generateFilterParameterMap()
	{	
		QMap<QString, RichParameterList> FPM;
		MeshDocument md;	
		MeshModel* mm = md.addNewMesh("", "dummy", true);	
		vcg::tri::Tetrahedron(mm->cm);
		mm->updateDataMask(MeshModel::MM_ALL);
		QMap<QString, QAction*>::iterator ai;	
		for(FilterPlugin* fpi : PM.filterPluginIterator())
		//for (ai = this->actionFilterMap.begin(); ai != this->actionFilterMap.end(); ++ai)	
		{	
			for (QAction* ai: fpi->actions()) {
				QString filterName = fpi->filterName(ai);//  ->filterName();	
				//QAction act(filterName,NULL);	
				RichParameterList rp;	
				FPM[filterName] = rp;
			}
		}	
		return FPM;	
	}

    void dumpPluginInfoDoxygen(FILE *fp)
    {
        if(!fp) return;
        int i=0;
        QMap<QString, RichParameterList> FPM = generateFilterParameterMap();
        fprintf(fp,"/*! \\mainpage MeshLab Filter Documentation\n");
        //fprintf(fp,"\\AtBeginDocument{\\setcounter{tocdepth}{1}}");

        for(FilterPlugin *iFilter: PM.filterPluginIterator())
        {
            for(QAction *filterAction: iFilter->actions())
            {
                fprintf(fp,
                    "\n\\section f%i %s \n\n"
                    "%s\n"
                    ,i++, qUtf8Printable(filterAction->text()), qUtf8Printable(iFilter->filterInfo(filterAction)));

                fprintf(fp,  "<H2> Parameters </h2>\n");
                //            fprintf(fp,  "\\paragraph fp%i Parameters\n",i);

                if(! FPM[filterAction->text()].isEmpty())
                {
                    fprintf(fp,"<TABLE>\n");
					for(RichParameter& pp : FPM[filterAction->text()])
                    {
                        fprintf(fp,"<TR><TD> \\c %s  </TD> <TD> %s </TD> <TD><i> %s -- </i></TD> </TR>\n",
							qUtf8Printable(pp.value().typeName()), qUtf8Printable(pp.fieldDescription()), qUtf8Printable(pp.toolTip()));
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

        // HashTable storing all supported formats together with
        // the (1-based) index  of first plugin which is able to open it
        QHash<QString, IOMeshPlugin*> allKnownFormats;

        //PM.LoadFormats(filters, allKnownFormats,PluginManager::IMPORT);

        QFileInfo fi(fileName);
        // this change of dir is needed for subsequent textures/materials loading
        QDir curDir = QDir::current();
        QDir::setCurrent(fi.absolutePath());

        QString extension = fi.suffix();
        qDebug("Opening a file with extension %s", qUtf8Printable(extension));
        // retrieving corresponding IO plugin
        IOMeshPlugin* pCurrentIOPlugin = PM.inputMeshPlugin(extension);
        if (pCurrentIOPlugin == 0)
        {
            fprintf(fp,"Error encountered while opening file: ");
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }
        int mask = 0;

        RichParameterList prePar;
		pCurrentIOPlugin->initPreOpenParameter(extension,prePar);
        prePar.join(meshlab::defaultGlobalParameterList());

        try {
            pCurrentIOPlugin->open(extension, fileName, mm ,mask,prePar);
        }
        catch(const MLException& e){
            fprintf(fp,"MeshLabServer: Failed loading of %s from dir %s\n", qUtf8Printable(fileName), qUtf8Printable(QDir::currentPath()));
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

    bool exportMesh(MeshModel *mm, const int mask, const QString& fileName,bool writebinary,FILE* fp = stdout)
    {
        QFileInfo fi(fileName);
        // this change of dir is needed for subsequent textures/materials loading
        QDir curDir = QDir::current();
        QDir::setCurrent(fi.absolutePath());

        QString extension = fi.suffix();

        // retrieving corresponding IO plugin
        IOMeshPlugin* pCurrentIOPlugin = PM.outputMeshPlugin(extension);
        if (pCurrentIOPlugin == nullptr)
        {
            fprintf(fp,"Error encountered while opening file: ");
            //QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
            //QMessageBox::critical(this, tr("Opening Error"), errorMsgFormat.arg(fileName, extension));
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }

        // optional saving parameters (like ascii/binary encoding)
        RichParameterList savePar;
        pCurrentIOPlugin->initSaveParameter(extension, *mm, savePar);
        if(savePar.hasParameter("Binary")){
            savePar.setValue("Binary",BoolValue(writebinary));
        }

        int formatmask = 0;
        int defbits = 0;
        pCurrentIOPlugin->exportMaskCapability(extension,formatmask,defbits);
        try {
            pCurrentIOPlugin->save(extension, fileName, *mm ,mask & formatmask, savePar, nullptr);
            QDir::setCurrent(curDir.absolutePath());
            return true;
        }
        catch(const MLException& e)
        {
            fprintf(fp,"Failed saving\n");
            QDir::setCurrent(curDir.absolutePath());
            return false;
        }
    }

    bool loadMesh(const QString& fileName, IOMeshPlugin *pCurrentIOPlugin, MeshModel* mm, int& mask,RichParameterList* prePar, const Matrix44m &mtr, MeshDocument* md, FILE* fp = stdout)
    {
        if (mm == NULL)
            return false;

        QFileInfo fi(fileName);
        QString extension = fi.suffix();
        if(!fi.exists())
        {
            QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 does not exist.";
            fprintf(fp, "Meshlab Opening Error: %s", errorMsgFormat.arg(fileName).toStdString().c_str());
            return false;
        }
        if(!fi.isReadable())
        {
            QString errorMsgFormat = "Unable to open file:\n\"%1\"\n\nError details: file %1 is not readable.";
            fprintf(fp, "Meshlab Opening Error: %s", errorMsgFormat.arg(fileName).toStdString().c_str());
            return false;
        }

        // the original directory path before we switch it
        QString origDir = QDir::current().path();

        // this change of dir is needed for subsequent textures/materials loading
        QDir::setCurrent(fi.absoluteDir().absolutePath());

        // Adjust the file name after changing the directory
        QString fileNameSansDir = fi.fileName();

        // retrieving corresponding IO plugin
        if (pCurrentIOPlugin == 0)
        {
            QString errorMsgFormat = "Error encountered while opening file:\n\"%1\"\n\nError details: The \"%2\" file extension does not correspond to any supported format.";
            fprintf(fp, "Opening Error: %s", errorMsgFormat.arg(fileName, extension).toStdString().c_str());
            QDir::setCurrent(origDir); // undo the change of directory before leaving
            return false;
        }
        md->setBusy(true);
        pCurrentIOPlugin->setLog(&md->Log);

        try {
            pCurrentIOPlugin->open(extension, fileNameSansDir, *mm ,mask,*prePar);
        }
        catch (const MLException& e) {
            fprintf(fp, "Opening Failure: %s", (QString("While opening: '%1'\n\n").arg(fileName)+e.what()).toStdString().c_str()); // text+
            md->setBusy(false);
            QDir::setCurrent(origDir); // undo the change of directory before leaving
            return false;
        }


        //std::cout << "Opened mesh: in " << tm.elapsed() << " secs\n";
        // After opening the mesh lets ask to the io plugin if this format
        // requires some optional, or userdriven post-opening processing.
        // and in that case ask for the required parameters and then
        // ask to the plugin to perform that processing
        //RichParameterSet par;
        //pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
        //pCurrentIOPlugin->applyOpenParameter(extension, *mm, par);

        QString err = pCurrentIOPlugin->warningMessageString();
        if (!err.isEmpty())
        {
            fprintf(fp, "Opening Problems: %s", (QString("While opening: '%1'\n\n").arg(fileName)+err).toStdString().c_str());
        }

        //saveRecentFileList(fileName);

        //if (!(mm->cm.textures.empty()))
        //    updateTexture(mm->id());

        // In case of polygonal meshes the normal should be updated accordingly
        if( mask & vcg::tri::io::Mask::IOM_BITPOLYGONAL)
        {
            mm->updateDataMask(MeshModel::MM_POLYGONAL); // just to be sure. Hopefully it should be done in the plugin...
            int degNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
            if(degNum)
                fprintf(stdout, "Warning model contains %i degenerate faces. Removed them.",degNum);
            mm->updateDataMask(MeshModel::MM_FACEFACETOPO);
            vcg::tri::UpdateNormal<CMeshO>::PerBitQuadFaceNormalized(mm->cm);
            vcg::tri::UpdateNormal<CMeshO>::PerVertexFromCurrentFaceNormal(mm->cm);
        } // standard case
        else
        {
            vcg::tri::UpdateNormal<CMeshO>::PerFaceNormalized(mm->cm);
            if(!( mask & vcg::tri::io::Mask::IOM_VERTNORMAL) )
                vcg::tri::UpdateNormal<CMeshO>::PerVertexAngleWeighted(mm->cm);
        }

        vcg::tri::UpdateBounding<CMeshO>::Box(mm->cm);					// updates bounding box
        if(mm->cm.fn==0 && mm->cm.en==0)
        {
            if(mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
                mm->updateDataMask(MeshModel::MM_VERTNORMAL);
        }

        if(mm->cm.fn==0 && mm->cm.en>0)
        {
            if (mask & vcg::tri::io::Mask::IOM_VERTNORMAL)
                mm->updateDataMask(MeshModel::MM_VERTNORMAL);
        }

        //updateMenus();
        int delVertNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateVertex(mm->cm);
        int delFaceNum = vcg::tri::Clean<CMeshO>::RemoveDegenerateFace(mm->cm);
        vcg::tri::Allocator<CMeshO>::CompactEveryVector(mm->cm);
        if(delVertNum>0 || delFaceNum>0 )
            fprintf(fp, "MeshLab Warning: %s", (QString("Warning mesh contains %1 vertices with NAN coords and %2 degenerated faces.\nCorrected.").arg(delVertNum).arg(delFaceNum)).toStdString().c_str() );
        mm->cm.Tr = mtr;

        //computeRenderingDataOnLoading(mm,isareload, rendOpt);
        //updateLayerDialog();


        md->setBusy(false);

        QDir::setCurrent(origDir); // undo the change of directory before leaving

        return true;
    }

    bool loadMeshWithStandardParams(QString& fullPath, MeshModel* mm, const Matrix44m &mtr = Matrix44m::Identity(), MeshDocument* md = NULL)
    {
        if ((mm == NULL) || (md == NULL))
            return false;
        bool ret = false;
        if (!mm->isVisible())
        {
            mm->Clear();
            mm->visible = false;
        }
        else
            mm->Clear();
        QFileInfo fi(fullPath);
        QString extension = fi.suffix();
        IOMeshPlugin *pCurrentIOPlugin = PM.inputMeshPlugin(extension);

        if(pCurrentIOPlugin != NULL)
        {
            RichParameterList prePar;
			pCurrentIOPlugin->initPreOpenParameter(extension,prePar);
            //prePar = prePar.join(currentGlobalParams);
            int mask = 0;
            QElapsedTimer t;t.start();
            bool open = loadMesh(fullPath,pCurrentIOPlugin,mm,mask,&prePar,mtr, md, stdout);
            if(open)
            {
                RichParameterList par;
                pCurrentIOPlugin->initOpenParameter(extension, *mm, par);
                pCurrentIOPlugin->applyOpenParameter(extension,*mm,par);
                ret = true;
            }
        }
        return ret;
    }

    bool openProject(MeshDocument& md,const QString& fileName,FILE* fp = stdout)
    {
        //bool visiblelayer = layerDialog->isVisible();
        //showLayerDlg(false);
    	//globrendtoolbar->setEnabled(false);
        if (fileName.isEmpty()) return false;

        QFileInfo fi(fileName);
        //lastUsedDirectory = fi.absoluteDir();
        //TODO: move this to main()
        if((fi.suffix().toLower()!="aln") && (fi.suffix().toLower()!="mlp")  && (fi.suffix().toLower() != "mlb") && (fi.suffix().toLower()!="out") && (fi.suffix().toLower()!="nvm"))
        {
            //QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unknown project file extension");
            fprintf(fp, "Meshlab Opening Error: Unknown project file extension\n");
            return false;
        }

        // Common Part: init a Doc if necessary, and
        //bool activeDoc = (bool) !mdiarea->subWindowList().empty() && mdiarea->currentSubWindow();
        //bool activeEmpty = activeDoc && md.meshList.empty();

        //if (!activeEmpty)  newProject(fileName);

        md.setFileName(fileName);
        //mdiarea->currentSubWindow()->setWindowTitle(fileName);
        md.setDocLabel(fileName);

        md.setBusy(true);

        // this change of dir is needed for subsequent textures/materials loading
        QDir::setCurrent(fi.absoluteDir().absolutePath());
        //qb->show();

        if (QString(fi.suffix()).toLower() == "aln")
        {
            std::vector<RangeMap> rmv;
    		int retVal = ALNParser::ParseALN(rmv, qUtf8Printable(fileName));
            if(retVal != ALNParser::NoError)
            {
                //QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open ALN file");
                fprintf(fp,"Meshlab Opening Error: Unable to open ALN file\n");
                return false;
            }

            bool openRes=true;
            std::vector<RangeMap>::iterator ir;
            for(ir=rmv.begin();ir!=rmv.end() && openRes;++ir)
            {
                QString relativeToProj = fi.absoluteDir().absolutePath() + "/" + (*ir).filename.c_str();
                md.addNewMesh(relativeToProj,relativeToProj);
                openRes = loadMeshWithStandardParams(relativeToProj,md.mm(),ir->transformation, &md);
                if(!openRes)
                    md.delMesh(md.mm());
            }
        }

        if (QString(fi.suffix()).toLower() == "mlp" || QString(fi.suffix()).toLower() == "mlb")
        {
            std::map<int, MLRenderingData> rendOpt;
            if (!MeshDocumentFromXML(md, fileName, (QString(fi.suffix()).toLower() == "mlb"), rendOpt))
            {
              //QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open MeshLab Project file");
              fprintf(fp,"Meshlab Opening Error: Unable to open MeshLab Project file\n");
              return false;
            }
    		//GLA()->updateMeshSetVisibilities();
            for (int i=0; i<md.meshList.size(); i++)
            {
                QString fullPath = md.meshList[i]->fullName();
                //md.setBusy(true);
                Matrix44m trm = md.meshList[i]->cm.Tr; // save the matrix, because loadMeshClear it...
                if (!loadMeshWithStandardParams(fullPath, md.meshList[i], trm, &md))
                    md.delMesh(md.meshList[i]);
            }
        }

        ////// BUNDLER
        if (QString(fi.suffix()).toLower() == "out"){

            QString cameras_filename = fileName;
            QString image_list_filename;
            QString model_filename;

            image_list_filename = "list.txt";
            if(image_list_filename.isEmpty())
                return false;

            if(!MeshDocumentFromBundler(md,cameras_filename,image_list_filename,model_filename)){
                //QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open OUTs file");
                fprintf(fp,"Meshlab Opening Error: Unable to open OUTs file\n");
                return false;
            }


    //WARNING!!!!! i suppose it's not useful anymore but.......
    /*GLA()->setColorMode(GLW::CMPerVert);
    GLA()->setDrawMode(GLW::DMPoints);*/
    /////////////////////////////////////////////////////////
        }

        //////NVM
        if (QString(fi.suffix()).toLower() == "nvm"){

            QString cameras_filename = fileName;
            QString model_filename;

            if(!MeshDocumentFromNvm(md,cameras_filename,model_filename)){
                //QMessageBox::critical(this, tr("Meshlab Opening Error"), "Unable to open NVMs file");
                fprintf(fp,"Meshlab Opening Error: Unable to open NVMs file\n");
                return false;
            }
    //WARNING!!!!! i suppose it's not useful anymore but.......
    /*GLA()->setColorMode(GLW::CMPerVert);
    GLA()->setDrawMode(GLW::DMPoints);*/
    /////////////////////////////////////////////////////////
        }

        md.setBusy(false);
        //qb->reset();

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
                exportMesh(m,m->dataMask(),outfilename,true);
            }
        }

        QDir::setCurrent(curDir.absolutePath());
        return MeshDocumentToXMLFile(md, filename, false, false, outprojinfo.suffix().toLower() == "mlb");
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
            printf("File %s was not found.\n", qUtf8Printable(scriptfile));
            return false;
        }
        fprintf(fp,"Starting Script of %i actions",scriptPtr.size());
        GLLogStream log;
        for (FilterNameParameterValuesPair& pair : scriptPtr)
        {
            bool ret = true;
            //RichParameterSet &par = (*ii).second;
            QString fname = pair.filterName();
            fprintf(fp,"filter: %s\n", qUtf8Printable(fname));
            QAction *action = PM.filterAction(fname);
            if (action == NULL)
            {
                fprintf(fp,"filter %s not found", qUtf8Printable(fname));
                return false;
            }

            FilterPlugin *iFilter = qobject_cast<FilterPlugin *>(action->parent());
            iFilter->setLog(&log);
            int req = iFilter->getRequirements(action);
            if (mm != NULL)
                mm->updateDataMask(req);
            //make sure the PARMESH parameters are initialized

            //A filter in the script file couldn't have all the required parameter not defined (a script file not generated by MeshLab).
            //So we have to ask to the filter the default values for all the parameters and integrate them with the parameters' values
            //defined in the script file.
            RichParameterList required;
            iFilter->initParameterList(action,meshDocument,required);
            RichParameterList &parameterSet = pair.second;

            //The parameters in the script file are more than the required parameters of the filter. The script file is not correct.
            if (required.size() < parameterSet.size())
            {
                fprintf(fp,"The parameters in the script file are more than the filter %s requires.\n", qUtf8Printable(fname));
                return false;
            }

			int i = 0;
			for(RichParameter& rp : required) {
				if (!parameterSet.hasParameter(rp.name())) {
					parameterSet.addParam(rp);
				}
				assert(parameterSet.size() == required.size());
				RichParameter& parameter = parameterSet.at(i);
				//if this is a mesh parameter and the index is valid
				if(parameter.value().isMesh()) {
					RichMesh& md = reinterpret_cast<RichMesh&>(parameter);
					if(md.meshindex < meshDocument.size() && md.meshindex >= 0) {
						parameterSet.setValue(md.name(), MeshValue(&meshDocument, md.meshindex));
					}
					else {
						fprintf(fp,"Meshes loaded: %i, meshes asked for: %i \n", meshDocument.size(), md.meshindex );
						fprintf(fp,"One of the filters in the script needs more meshes than you have loaded.\n");
						exit(-1);
					}
				}
				i++;
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

                if (iFilter->filterArity(action) == FilterPlugin::SINGLE_MESH)
                {
                    MLRenderingData::PRIMITIVE_MODALITY pm = MLPoliciesStandAloneFunctions::bestPrimitiveModalityAccordingToMesh(meshDocument.mm());
                    if ((pm != MLRenderingData::PR_ARITY) && (meshDocument.mm() != NULL))
                    {
                        dt.set(pm, atts);
                        iFilter->glContext->initPerViewRenderingData(meshDocument.mm()->id(), dt);
                    }

                    if (meshDocument.mm() != NULL)
                    {
                        meshDocument.mm()->cm.svn = int(vcg::tri::UpdateSelection<CMeshO>::VertexCount(meshDocument.mm()->cm));
                        meshDocument.mm()->cm.sfn = int(vcg::tri::UpdateSelection<CMeshO>::FaceCount(meshDocument.mm()->cm));
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

                        if (mm != NULL)
                        {
                            mm->cm.svn = int(vcg::tri::UpdateSelection<CMeshO>::VertexCount(mm->cm));
                            mm->cm.sfn = int(vcg::tri::UpdateSelection<CMeshO>::FaceCount(mm->cm));
                        }
                    }
                }
            }
            meshDocument.setBusy(true);
            unsigned int postConditionMask = MeshModel::MM_UNKNOWN;
			try {
				iFilter->applyFilter( action, pair.second, meshDocument, postConditionMask, filterCallBack);
			}
			catch (MLException&) {
				ret = false;
			}
            meshDocument.setBusy(false);
            if (shared != NULL)
                delete iFilter->glContext;
            delete wid;
            QStringList logOutput;
            log.print(logOutput);
            foreach(QString logEntry, logOutput)
                fprintf(fp,"%s\n",qUtf8Printable(logEntry));
            if(!ret)
            {
                fprintf(fp,"Problem with filter: %s\n",qUtf8Printable(fname));
                return false;
            }
        }
        return true;
    }

private:
    PluginManager PM;
    MLSceneGLSharedDataContext* shared;
};

namespace commandline
{
    const char inproject('p');
    const char outproject('w');
    const char overwrite('x');
    const char inputmeshes('i');
    const char outputmesh('o');
    const char layer('l');
    const char lastlayer('x');
    const char currentlayer('c');
    const char mask('m');
    const char vertex('v');
    const char face('f');
    const char wedge('w');
    const char mesh('m');
    const char color('c');
    const char flags('f');
    const char normal('n');
    const char quality('q');
    const char radius('r');
    const char polygon('p');
    const char texture('t');
    const char log('l');
    const char dump('d');
    const char script('s');
    const char saveparam('s');
    const char ascii('a');

    void usage()
    {
        printf("MeshLabServer version: %s\n", qUtf8Printable(MeshLabApplication::appVer()));
        QFile docum(":/meshlabserver.txt");
        if (!docum.open(QIODevice::ReadOnly))
        {
            printf("MeshLabServer was not able to locate meshlabserver.txt file. The program will be closed\n");
            exit(-1);
        }
        QString help(docum.readAll());
        printf("\nUsage:\n%s",qUtf8Printable(help));
        docum.close();
    }

    QString optionValueExpression(const char cmdlineopt)
    {
        //Validate an option followed by spaces and a filepath
        return QString ("-" + QString(cmdlineopt) + "\\s+(.+)");
    }

    QString outputmeshExpression()
    {
		QString options("(" + QString(vertex) + "|" + QString(face) + "|" + QString(wedge) + "|" + QString(mesh) + "|" +QString(saveparam) + ")(" + QString(color) + "|" + QString(quality) + "|" + QString(flags) + "|" + QString(normal) + "|" + QString(radius) + "|" + QString(texture) + "|" + QString(polygon) + "|" + QString(ascii) + ")");
		QString optionslist(options + "(\\s+" + options + ")*");
		QString savingmask("-" + QString(mask) + "\\s+" + optionslist);
		QString layernumber("\\d+");
		QString layertosave("-" + QString(layer) + "\\s+(" + layernumber + "|" + currentlayer + "|" + lastlayer + ")");
		return optionValueExpression(outputmesh) + "(\\s+(" + savingmask + "|" + layertosave + "\\s+" + savingmask + "|" + layertosave + "))*";
    }

    bool validateCommandLine(const QString& str)
    {
        QString logstring("(" + optionValueExpression(log) + "\\s+" +  optionValueExpression(dump) + "|" + optionValueExpression(dump) + "\\s+" +  optionValueExpression(log) + "|" +  optionValueExpression(dump) + "|" + optionValueExpression(log) + ")");
        QString arg("(" + optionValueExpression(inproject) + "|" + optionValueExpression(inputmeshes) + "|" + optionValueExpression(outproject) + "(\\s+-" + overwrite + ")?" + "|" + optionValueExpression(script) + "|" + outputmeshExpression() + ")");
        QString args("(" + arg + ")(\\s+" + arg + ")*");
        QString completecommandline("(" + logstring + "|" + logstring + "\\s+" + args + "|" + args + ")");
        QRegExp completecommandlineexp(completecommandline);
        //completecommandlineexp.setMinimal(true);

        bool valid = completecommandlineexp.isValid();
        if (!valid)
            return false;
        completecommandlineexp.indexIn(str);
        QString rr = completecommandlineexp.cap();
        return (completecommandlineexp.matchedLength() == str.size());
    }

}

struct OutFileMesh
{
    OutFileMesh() : writebinary(true) {}
    QString filename;
    int mask;
    bool writebinary;
	/*WARNING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	/* We need these two constant values because when we parse the command line we don't know
   * yet how many layers will have the current document and which will be the current one.
   * Opening a project and/or importing a file happens after the parsing of the commandline
   * is completed */
	static const int lastlayerconst = -2;
	static const int currentlayerconst = -1;
	/******************************************************************************************/

	// Possible values can be:
	//	- lastlayerconst #the last layer of a document, DEFAULT value
	//	- currentlayerconst #the current layer of a document, sometimes it's different from the last layer of a document
	//	- a number between [0,inf) #identifying the correspondent layer position
  // WARNING!!!!! Please note that the layer position is DIFFERENT from the layer id
	int layerposition;
};

struct OutProject
{
    QString filename;
    bool overwrite;
};

int main(int argc, char *argv[])
{
    GLExtensionsManager::init();
    FILE* logfp = stdout;
    FILE* dumpfp = NULL;
    MeshLabApplication app(argc, argv);
    QStringList st = app.arguments();
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
    for (int ii = 1; ii < argc; ++ii)
    {
        QString argum(argv[ii]);
        argum = argum.trimmed();
        if (argum.contains(' '))
            argum = "\"" + argum + "\"";
        cmdline = cmdline + argum + " ";
    }
    if (!commandline::validateCommandLine(cmdline.trimmed()))
    {
        printf("CommandLine Syntax Error: please refer to the following documentation for a complete list of the MeshLabServer parameters.\n");
        commandline::usage();
        //system("pause");
        exit(-1);
    }

	QSettings settings(MeshLabApplication::organization(),MeshLabApplication::appArchitecturalName(MeshLabApplication::HW_64BIT));

	QVariant xmlgpupar = settings.value("MeshLab::System::maxGPUMemDedicatedToGeometry");

	QDomDocument doc;
	doc.setContent(xmlgpupar.toString(), false);

	QDomElement paramelem = doc.firstChild().toElement();
	int gpumemmb = paramelem.attribute("value").toInt();

	std::ptrdiff_t maxgpumem = (std::ptrdiff_t) gpumemmb * (float)(1024 * 1024);
	vcg::QtThreadSafeMemoryInfo gpumeminfo(maxgpumem);

    QGLFormat fmt = QGLFormat::defaultFormat();
    fmt.setAlphaBufferSize(8);
    QGLFormat::setDefaultFormat(fmt);

	MeshDocument meshDocument;

	MLSceneGLSharedDataContext shared(meshDocument, gpumeminfo, MeshLabScalarTest<MESHLAB_SCALAR>::doublePrecision(), 100000,100000);
	shared.makeCurrent();
	if (!GLExtensionsManager::initializeGLextensions_notThrowing())
	{
		printf("GLEW Init: failed!\n");
		//system("pause");
		exit(-1);
	}
	shared.doneCurrent();
    printf("Loading Plugins:\n");
	MeshLabServer server(&shared);
    server.loadPlugins();

    bool writebinary = true;
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
                    bool opened = server.openProject(meshDocument,inputproject, logfp);
                    if (!opened)
                    {
                        fprintf(logfp,"MeshLab Project %s has not been correctly opened. MeshLabServer application will exit.\n",qUtf8Printable(inputproject));
						//system("pause");
                        exit(-1);
                    }
                    else
                        fprintf(logfp,"MeshLab Project %s has been loaded.\n",qUtf8Printable(inputproject));
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
                        fprintf(logfp,"Project %s is not a valid \'mlp\' file format. Output file will be renamed as %s.mlp .\n",qUtf8Printable(pr.filename),qUtf8Printable(pr.filename + ".mlp"));
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
                        fprintf(logfp,"It was not possible to add new mesh %s to MeshLabServer. The program will exit\n",qUtf8Printable(info.absoluteFilePath()));
						//system("pause");
                        exit(-1);
                    }
                    bool opened = server.importMesh(*mmod, info.absoluteFilePath(),logfp);
                    if (!opened)
                    {
                        fprintf(logfp,"It was not possible to import mesh %s into MeshLabServer. The program will exit\n ",qUtf8Printable(info.absoluteFilePath()));
						//system("pause");
                        exit(-1);
                    }
                    fprintf(logfp,"Mesh %s loaded has %i vn %i fn\n", qUtf8Printable(info.absoluteFilePath()), mmod->cm.vn, mmod->cm.fn);
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

					/*WARNING! in order to maintain backward SYNTAX compatibility (not the SEMANTIC one!) by default the outputmesh saved is the one contained in the current layer*/
					outfl.layerposition = OutFileMesh::currentlayerconst;

                    fprintf(logfp,"output mesh  %s\n", qUtf8Printable(outfl.filename));
                    i++;
                }

				if (((i + 1) < argc) && (QString(argv[i + 1]) == (QString("-") + commandline::layer)))
				{
					i = i + 2;
					if (argv[i][0] == commandline::lastlayer)
						outfl.layerposition = OutFileMesh::lastlayerconst;
					else
					{
						if (argv[i][0] == commandline::currentlayer)
							outfl.layerposition = OutFileMesh::currentlayerconst;
						else
							outfl.layerposition = QString(argv[i]).toInt();
					}
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
                                case commandline::radius : i++; fprintf(logfp,"vertex radii, "   ); mask |= vcg::tri::io::Mask::IOM_VERTRADIUS;  break;
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

                        case commandline::saveparam :
                             {
                                switch( argv[i][1])
                                {
                                    case commandline::ascii:
                                        {
                                            writebinary = false;
                                            i++;
                                            break;
                                         }
                                }
                                break;
                             }
						case commandline::mesh :
							{
								switch (argv[i][1])
								{
								case commandline::polygon: i++; fprintf(logfp, "mesh polygon, "); mask |= vcg::tri::io::Mask::IOM_BITPOLYGONAL;   break;
								default:  i++; fprintf(logfp, "WARNING: unknowns per MESH attribute '%s'", argv[i + 1]); break;
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
                outfl.writebinary = writebinary;
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
                printf("Something bad happened parsing the document. String %s\n",qUtf8Printable(argv[i]));
				//system("pause");
                exit(-1);
            }
        }
    }

    for(int ii = 0; ii < scriptfiles.size();++ii)
    {
        fprintf(logfp,"Apply FilterScript: '%s'\n",qUtf8Printable(scriptfiles[ii]));
        bool returnValue = server.script(meshDocument, scriptfiles[ii],logfp);
        if(!returnValue)
        {
            fprintf(logfp,"Failed to apply script file %s\n",qUtf8Printable(scriptfiles[ii]));
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
            fprintf(logfp,"Output project has been saved in %s.\n",qUtf8Printable(outprojectfiles[ii].filename));
        else
        {
            fprintf(logfp,"Project %s has not been correctly saved in. MeshLabServer Application will exit.\n",qUtf8Printable(outprojectfiles[ii].filename));
			//system("pause");
            exit(-1);
        }
    }

	if (meshDocument.size() < outmeshlist.size())
		fprintf(logfp, "Error: trying to save %i meshes, but only %i available in the project\n", outmeshlist.size(), meshDocument.size());

	for (int ii = 0; ii < outmeshlist.size(); ++ii)
	{
		bool exported = false;
		if (outmeshlist[ii].layerposition < meshDocument.meshList.size())
		{
			int layertobesaved = outmeshlist[ii].layerposition;

			if (layertobesaved == OutFileMesh::lastlayerconst)
				layertobesaved = meshDocument.meshList.size() - 1;
			else
				if (layertobesaved == OutFileMesh::currentlayerconst)
					layertobesaved = meshDocument.meshList.indexOf(meshDocument.mm());

			if ((layertobesaved >= 0) && (layertobesaved < meshDocument.meshList.size()))
			{
				MeshModel* meshmod = meshDocument.meshList[layertobesaved];
				if (meshmod != NULL)
					exported = server.exportMesh(meshDocument.meshList[layertobesaved], outmeshlist[ii].mask, outmeshlist[ii].filename, outmeshlist[ii].writebinary, logfp);
				if (exported)
					fprintf(logfp, "Mesh %s saved as %s (%i vn %i fn)\n", qUtf8Printable(meshmod->fullName()), qUtf8Printable(outmeshlist[ii].filename), meshmod->cm.vn, meshmod->cm.fn);
				else
					fprintf(logfp, "Output mesh %s has NOT been saved\n", qUtf8Printable(outmeshlist[ii].filename));
			}
			else
				fprintf(logfp, "Output mesh %s has NOT been saved. A not existent layer has been requested to be saved\n", qUtf8Printable(outmeshlist[ii].filename));
		}
		else
			fprintf(logfp, "Invalid layer number %i. Last layer in the current document is the number %i. Output mesh %s will not be saved\n", outmeshlist[ii].layerposition, meshDocument.meshList.size() - 1, qUtf8Printable(outmeshlist[ii].filename));
	}//for(int ii


	if((logfp != NULL) && (logfp != stdout))
	{
		fclose(logfp);
	}

	shared.deAllocateGPUSharedData();
	//system("pause");
	return 0;
}//int main()
