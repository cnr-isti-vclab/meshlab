#ifndef __MESHLABDOC_BUNDLER_H
#define __MESHLABDOC_BUNDLER_H

#include <wrap/io_trimesh/import_out.h>
#include <wrap/io_trimesh/import_nvm.h>

bool MeshDocumentFromBundler(MeshDocument &md,QString filename_out,QString image_list_filename, QString model_filename);
bool MeshDocumentFromNvm(MeshDocument &md,QString filename_nvm, QString model_filename);
#endif // __MESHLABDOC_XML_H
