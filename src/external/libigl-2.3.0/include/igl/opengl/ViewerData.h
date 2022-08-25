// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_VIEWERDATA_H
#define IGL_VIEWERDATA_H

#include "MeshGL.h"
#include <igl/igl_inline.h>
#include <igl/colormap.h>
#include <cassert>
#include <cstdint>
#include <Eigen/Core>
#include <memory>
#include <vector>

// Alec: This is a mesh class containing a variety of data types (normals,
// overlays, material colors, etc.)
//
// WARNING: Eigen data members (such as Eigen::Vector4f) should explicitly
// disable alignment (e.g. use `Eigen::Matrix<float, 4, 1, Eigen::DontAlign>`),
// in order to avoid alignment issues further down the line (esp. if the
// structure are stored in a std::vector).
//
// See this thread for a more detailed discussion:
// https://github.com/libigl/libigl/pull/1029
//
namespace igl
{

// TODO: write documentation
namespace opengl
{

// Forward declaration
class ViewerCore;

class ViewerData
{
public:
  ViewerData();

  // Empty all fields
  IGL_INLINE void clear();

  // Change the visualization mode, invalidating the cache if necessary
  IGL_INLINE void set_face_based(bool newvalue);

  // Helpers that can draw the most common meshes
  IGL_INLINE void set_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);
  IGL_INLINE void set_vertices(const Eigen::MatrixXd& V);
  IGL_INLINE void set_normals(const Eigen::MatrixXd& N);

  IGL_INLINE void set_visible(bool value, unsigned int core_id = 1);

  // Set the color of the mesh
  //
  // Inputs:
  //   C  #V|#F|1 by 3 list of colors
  IGL_INLINE void set_colors(const Eigen::MatrixXd &C);

  // Set per-vertex UV coordinates
  //
  // Inputs:
  //   UV  #V by 2 list of UV coordinates (indexed by F)
  IGL_INLINE void set_uv(const Eigen::MatrixXd& UV);

  // Set per-corner UV coordinates
  //
  // Inputs:
  //   UV_V  #UV by 2 list of UV coordinates
  //   UV_F  #F by 3 list of UV indices into UV_V
  IGL_INLINE void set_uv(const Eigen::MatrixXd& UV_V, const Eigen::MatrixXi& UV_F);

  // Set the texture associated with the mesh.
  //
  // Inputs:
  //   R  width by height image matrix of red channel
  //   G  width by height image matrix of green channel
  //   B  width by height image matrix of blue channel
  //
  IGL_INLINE void set_texture(
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& R,
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& G,
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& B);

  // Set the texture associated with the mesh.
  //
  // Inputs:
  //   R  width by height image matrix of red channel
  //   G  width by height image matrix of green channel
  //   B  width by height image matrix of blue channel
  //   A  width by height image matrix of alpha channel
  //
  IGL_INLINE void set_texture(
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& R,
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& G,
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& B,
    const Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic>& A);

  // Set pseudo-colorable scalar data associated with the mesh.
  //
  // Inputs:
  //   caxis_min  caxis minimum bound
  //   caxis_max  caxis maximum bound
  //   D  #V by 1 list of scalar values
  //   cmap colormap type
  //   num_steps number of intervals to discretize the colormap
  //
  // To-do: support #F by 1 per-face data
  IGL_INLINE void set_data(
    const Eigen::VectorXd & D,
    double caxis_min,
    double caxis_max,
    igl::ColorMapType cmap = igl::COLOR_MAP_TYPE_VIRIDIS,
    int num_steps = 21);

  // Use min(D) and max(D) to set caxis.
  IGL_INLINE void set_data(const Eigen::VectorXd & D,
    igl::ColorMapType cmap = igl::COLOR_MAP_TYPE_VIRIDIS,
    int num_steps = 21);

  // Not to be confused with set_colors, this creates a _texture_ that will be
  // referenced to pseudocolor according to the scalar field passed to set_data.
  //
  // Inputs:
  //   CM  #CM by 3 list of colors
  IGL_INLINE void set_colormap(const Eigen::MatrixXd & CM);

  // Sets points given a list of point vertices. In constrast to `add_points`
  // this will (purposefully) clober existing points.
  //
  // Inputs:
  //   P  #P by 3 list of vertex positions
  //   C  #P|1 by 3 color(s)
  IGL_INLINE void set_points(
    const Eigen::MatrixXd& P,
    const Eigen::MatrixXd& C);
  IGL_INLINE void add_points(const Eigen::MatrixXd& P,  const Eigen::MatrixXd& C);

  // Clear the point data
  IGL_INLINE void clear_points();

  // Sets edges given a list of edge vertices and edge indices. In constrast
  // to `add_edges` this will (purposefully) clober existing edges.
  //
  // Inputs:
  //   P  #P by 3 list of vertex positions
  //   E  #E by 2 list of edge indices into P
  //   C  #E|1 by 3 color(s)

  IGL_INLINE void set_edges (const Eigen::MatrixXd& P, const Eigen::MatrixXi& E, const Eigen::MatrixXd& C);
  // Alec: This is very confusing. Why does add_edges have a different API from
  // set_edges?
  IGL_INLINE void add_edges (const Eigen::MatrixXd& P1, const Eigen::MatrixXd& P2, const Eigen::MatrixXd& C);
  // Sets edges given a list of points and eminating vectors
  IGL_INLINE void set_edges_from_vector_field(
    const Eigen::MatrixXd& P, 
    const Eigen::MatrixXd& V, 
    const Eigen::MatrixXd& C);

  // Clear the edge data
  IGL_INLINE void clear_edges();

  // Sets / Adds text labels at the given positions in 3D.
  // Note: This requires the ImGui viewer plugin to display text labels.
  IGL_INLINE void add_label (const Eigen::VectorXd& P,  const std::string& str);
  IGL_INLINE void set_labels (const Eigen::MatrixXd& P,  const std::vector<std::string>& str);

  // Clear the label data
  IGL_INLINE void clear_labels ();

  // Computes the normals of the mesh
  IGL_INLINE void compute_normals();

  // Assigns uniform colors to all faces/vertices
  IGL_INLINE void uniform_colors(
    const Eigen::Vector3d& diffuse,
    const Eigen::Vector3d& ambient,
    const Eigen::Vector3d& specular);

  // Assigns uniform colors to all faces/vertices
  IGL_INLINE void uniform_colors(
    const Eigen::Vector4d& ambient,
    const Eigen::Vector4d& diffuse,
    const Eigen::Vector4d& specular);

  // Generate a normal image matcap
  IGL_INLINE void normal_matcap();

  // Generates a default grid texture (without uvs)
  IGL_INLINE void grid_texture();

  // Copy visualization options from one viewport to another
  IGL_INLINE void copy_options(const ViewerCore &from, const ViewerCore &to);

  Eigen::MatrixXd V; // Vertices of the current mesh (#V x 3)
  Eigen::MatrixXi F; // Faces of the mesh (#F x 3)

  // Per face attributes
  Eigen::MatrixXd F_normals; // One normal per face

  Eigen::MatrixXd F_material_ambient; // Per face ambient color
  Eigen::MatrixXd F_material_diffuse; // Per face diffuse color
  Eigen::MatrixXd F_material_specular; // Per face specular color

  // Per vertex attributes
  Eigen::MatrixXd V_normals; // One normal per vertex

  Eigen::MatrixXd V_material_ambient; // Per vertex ambient color
  Eigen::MatrixXd V_material_diffuse; // Per vertex diffuse color
  Eigen::MatrixXd V_material_specular; // Per vertex specular color

  // UV parametrization
  Eigen::MatrixXd V_uv; // UV vertices
  Eigen::MatrixXi F_uv; // optional faces for UVs

  // Texture
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> texture_R;
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> texture_G;
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> texture_B;
  Eigen::Matrix<unsigned char,Eigen::Dynamic,Eigen::Dynamic> texture_A;

  // Overlays

  // Lines plotted over the scene
  // (Every row contains 9 doubles in the following format S_x, S_y, S_z, T_x, T_y, T_z, C_r, C_g, C_b),
  // with S and T the coordinates of the two vertices of the line in global coordinates, and C the color in floating point rgb format
  Eigen::MatrixXd lines;

  // Points plotted over the scene
  // (Every row contains 6 doubles in the following format P_x, P_y, P_z, C_r, C_g, C_b),
  // with P the position in global coordinates of the center of the point, and C the color in floating point rgb format
  Eigen::MatrixXd points;

  // Text labels plotted over the scene
  // Textp contains, in the i-th row, the position in global coordinates where the i-th label should be anchored
  // Texts contains in the i-th position the text of the i-th label
  Eigen::MatrixXd           vertex_labels_positions;
  Eigen::MatrixXd           face_labels_positions;
  Eigen::MatrixXd           labels_positions;
  std::vector<std::string>  vertex_labels_strings;
  std::vector<std::string>  face_labels_strings;
  std::vector<std::string>  labels_strings;

  // Marks dirty buffers that need to be uploaded to OpenGL
  uint32_t dirty;

  // Enable per-face or per-vertex properties
  bool face_based;

  // Enable double-sided lighting on faces
  bool double_sided;

  // Invert mesh normals
  bool invert_normals;

  // Visualization options
  // Each option is a binary mask specifying on which viewport each option is set.
  // When using a single viewport, standard boolean can still be used for simplicity.
  unsigned int is_visible;
  unsigned int show_overlay;
  unsigned int show_overlay_depth;
  unsigned int show_texture;
  unsigned int use_matcap;
  unsigned int show_faces;
  unsigned int show_lines;
  unsigned int show_vertex_labels;
  unsigned int show_face_labels;
  unsigned int show_custom_labels;

  // Point size / line width
  float point_size;
  // line_width is NOT SUPPORTED on Mac OS and Windows
  float line_width;
  Eigen::Matrix<float, 4, 1, Eigen::DontAlign> line_color;
  Eigen::Matrix<float, 4, 1, Eigen::DontAlign> label_color;

  // Shape material
  float shininess;

  // Unique identifier
  int id;

  // OpenGL representation of the mesh
  igl::opengl::MeshGL meshgl;

  // Update contents from a 'Data' instance
  IGL_INLINE void update_labels(
    igl::opengl::MeshGL& meshgl,
    igl::opengl::MeshGL::TextGL& GL_labels,
    const Eigen::MatrixXd& positions,
    const std::vector<std::string>& strings
  );
  IGL_INLINE void updateGL(
    const igl::opengl::ViewerData& data,
    const bool invert_normals,
    igl::opengl::MeshGL& meshgl);
  };

} // namespace opengl
} // namespace igl

////////////////////////////////////////////////////////////////////////////////

#include <igl/serialize.h>
namespace igl
{
  namespace serialization
  {
    inline void serialization(bool s, igl::opengl::ViewerData& obj, std::vector<char>& buffer)
    {
      SERIALIZE_MEMBER(V);
      SERIALIZE_MEMBER(F);
      SERIALIZE_MEMBER(F_normals);
      SERIALIZE_MEMBER(F_material_ambient);
      SERIALIZE_MEMBER(F_material_diffuse);
      SERIALIZE_MEMBER(F_material_specular);
      SERIALIZE_MEMBER(V_normals);
      SERIALIZE_MEMBER(V_material_ambient);
      SERIALIZE_MEMBER(V_material_diffuse);
      SERIALIZE_MEMBER(V_material_specular);
      SERIALIZE_MEMBER(V_uv);
      SERIALIZE_MEMBER(F_uv);
      SERIALIZE_MEMBER(texture_R);
      SERIALIZE_MEMBER(texture_G);
      SERIALIZE_MEMBER(texture_B);
      SERIALIZE_MEMBER(texture_A);
      SERIALIZE_MEMBER(lines);
      SERIALIZE_MEMBER(points);
      SERIALIZE_MEMBER(labels_positions);
      SERIALIZE_MEMBER(labels_strings);
      SERIALIZE_MEMBER(dirty);
      SERIALIZE_MEMBER(face_based);
      SERIALIZE_MEMBER(show_faces);
      SERIALIZE_MEMBER(show_lines);
      SERIALIZE_MEMBER(invert_normals);
      SERIALIZE_MEMBER(show_overlay);
      SERIALIZE_MEMBER(show_overlay_depth);
      SERIALIZE_MEMBER(show_vertex_labels);
      SERIALIZE_MEMBER(show_face_labels);
      SERIALIZE_MEMBER(show_custom_labels);
      SERIALIZE_MEMBER(show_texture);
      SERIALIZE_MEMBER(double_sided);
      SERIALIZE_MEMBER(point_size);
      SERIALIZE_MEMBER(line_width);
      SERIALIZE_MEMBER(line_color);
      SERIALIZE_MEMBER(shininess);
      SERIALIZE_MEMBER(id);
    }
    template<>
    inline void serialize(const igl::opengl::ViewerData& obj, std::vector<char>& buffer)
    {
      serialization(true, const_cast<igl::opengl::ViewerData&>(obj), buffer);
    }
    template<>
    inline void deserialize(igl::opengl::ViewerData& obj, const std::vector<char>& buffer)
    {
      serialization(false, obj, const_cast<std::vector<char>&>(buffer));
      obj.dirty = igl::opengl::MeshGL::DIRTY_ALL;
    }
  }
}

#ifndef IGL_STATIC_LIBRARY
#  include "ViewerData.cpp"
#endif

#endif
