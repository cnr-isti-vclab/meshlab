#ifndef VCG_IO_H
#define VCG_IO_H

#include <wrap/io_trimesh/import_ply.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_obj.h>
#include <vcg/complex/trimesh/update/bounding.h>
#include <vcg/complex/trimesh/update/normal.h>
#include <vcg/complex/trimesh/smooth.h>
#include <string>

using namespace std;

namespace agl
{
    namespace vcg_interface
    {
        /* this class loads and saves a mesh using vcg importers and exporters.
           Current supported formats: *ply */
        template< class vcg_mesh >
        class vcg_io
        {
        public:
            static bool load( vcg_mesh& m, const char* filename, string ext )
            {
                if( ext == "ply" )
                {
                    if( vcg::tri::io::ImporterPLY< vcg_mesh >::Open( m, filename ) != 0 )
                    {
                        return false;
                    }
                }
                else if( ext == "obj" )
                {

                    typename vcg::tri::io::ImporterOBJ< vcg_mesh >::Info oi;


                    if (!vcg::tri::io::ImporterOBJ<vcg_mesh>::LoadMask(filename, oi))
                    {
                        return false;
                    }

                    //m.Enable(oi.mask);
                    /*
                    if( vcg::tri::io::ImporterOBJ< vcg_mesh >::Open( m, filename, oi ) != 0 )
                    {
                        return false;
                    }
                    */

                    vcg::tri::io::ImporterOBJ< vcg_mesh >::Open( m, filename, oi );
                    if (m.fn <=0 || m.vn <= 0 )
                    {
                        return false;
                    }

                }

                vcg::tri::UpdateBounding< vcg_mesh >::Box( m );
                vcg::tri::UpdateNormals< vcg_mesh >::PerVertexNormalizedPerFaceNormalized( m );
                return true;
            }

            static bool save( vcg_mesh& m, const char* filename )
            {
                return ( vcg::tri::io::ExporterPLY< vcg_mesh >::Save( m, filename ) );
            }
        };
    }
}

#endif // VCG_IO_H
