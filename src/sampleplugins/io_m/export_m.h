#ifndef EXPORT_M_H
#define EXPORT_M_H

namespace vcg {
    namespace tri {
        namespace io {
            template <class SaveMeshType>
            class ExporterM
            {

            public:
                typedef typename SaveMeshType::VertexPointer VertexPointer;
                typedef typename SaveMeshType::FaceIterator FaceIterator;

                static int Save(SaveMeshType &m, const char * filename, int mask=0 )
                {
                    FILE *fp= fopen(filename,"w");
                    if(!fp) return -1;
                    FaceIterator fi;
                    fprintf(fp,"Graphics3D[\n\t{\n");

                    if (mask &io::Mask::IOM_BITPOLYGONAL) {
                        assert(tri::HasFFAdjacency(m));
                        std::vector<VertexPointer> polygon;
                        for(fi=m.face.begin();fi!=m.face.end();++fi) if (!fi->IsD()) fi->ClearV();
                        for(fi=m.face.begin();fi!=m.face.end();++fi) if (!fi->IsD()) if (!fi->IsV()) {
                            vcg::tri::PolygonSupport<SaveMeshType,SaveMeshType>::ExtractPolygon(&*fi,polygon);

                            fprintf(fp,"Polygon[{");
                            for(int i=0;i<int(polygon.size()) ;++i)
                            {
                                if(i!=0) fprintf(fp,",");
                                fprintf(fp,"{%f, %f, %f}",polygon[i]->P()[0],polygon[i]->P()[1],polygon[i]->P()[2]);
                            }
                            fprintf(fp,"}],\n");
                        }
                    }
                    else {
                        for(fi =m.face.begin();fi!=m.face.end();++fi)
                        {
                            fprintf(fp,"Polygon[{");
                            fprintf(fp,"{%f, %f, %f},",(*fi).P(0)[0],(*fi).P(0)[1],(*fi).P(0)[2]);
                            fprintf(fp,"{%f, %f, %f},",(*fi).P(1)[0],(*fi).P(1)[1],(*fi).P(1)[2]);
                            fprintf(fp,"{%f, %f, %f}" ,(*fi).P(2)[0],(*fi).P(2)[1],(*fi).P(2)[2]);
                            fprintf(fp,"}],\n");
                        }
                        fprintf(fp,"},\n	{		Boxed -> False	 	}\n]");
                    }
                    fprintf(fp,"},\n	{		Boxed -> False	 	}\n]");

                    fclose(fp);
                    return 0;
                }
                /*
            returns mask of capability one define with what are the saveable information of the format.
        */
                static int GetExportMaskCapability()
                {
                    int capability = 0;

                    //vert
                    capability |= vcg::tri::io::Mask::IOM_VERTCOORD;
                    capability |= vcg::tri::io::Mask::IOM_BITPOLYGONAL;

                    return capability;
                }

                static const char *ErrorMsg(int error)
                {
                    if(error==-1) return "unable to open file";
                    return "Ok, no errors";
                }
                static bool WriteHtmlSnippet(const char *meshfilename, const char *htmlfilename)
                {
                    FILE *fp=fopen(htmlfilename,"w");
                    if(!fp) return false;
                    fprintf(fp, "<applet code=\"Live.class\" codebase=\".\" archive=\"live.jar\" align=\"middle\" width=\"600\" height=\"600\" alt=\"%s\"><param name=\"input_file\" value=\"%s\" />",meshfilename,meshfilename);
                    fclose(fp);
                    return true;
                }
            };
        } // end namespace io
    } // end namespace tri
} // end namespace vcg
#endif // EXPORT_M_H
