#ifndef __MESHLABDOC_BUNDLER_H
#define __MESHLABDOC_BUNDLER_H

#include "ml_document/mesh_model.h"

bool MeshDocumentFromBundler(MeshDocument &md, QString filename_out, QString image_list_filename, QString model_filename);
bool MeshDocumentFromNvm(MeshDocument &md, QString filename_nvm, QString model_filename);

#endif // __MESHLABDOC_BUNDLER_H
