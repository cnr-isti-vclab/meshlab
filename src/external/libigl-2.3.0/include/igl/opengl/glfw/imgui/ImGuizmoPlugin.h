#ifndef IGL_OPENGL_GFLW_IMGUI_IMGUIZMOPLUGIN_H
#define IGL_OPENGL_GFLW_IMGUI_IMGUIZMOPLUGIN_H
#include "../../../igl_inline.h"
#include "ImGuiMenu.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imguizmo/ImGuizmo.h>
#include <Eigen/Dense>

namespace igl{ namespace opengl{ namespace glfw{ namespace imgui{

class ImGuizmoPlugin : public igl::opengl::glfw::imgui::ImGuiMenu
{
public:
  // callback(T) called when the stored transform T changes
  std::function<void(const Eigen::Matrix4f &)> callback;
  // Whether to display
  bool visible = true;
  // whether rotating, translating or scaling
  ImGuizmo::OPERATION operation;
  // stored transformation
  Eigen::Matrix4f T;
  // Initilize with rotate operation on an identity transform (at origin)
  ImGuizmoPlugin():operation(ImGuizmo::ROTATE),T(Eigen::Matrix4f::Identity()){};
  /////////////////////////////////////////////////////////////////////////////
  // Boilerplate
  virtual void init(igl::opengl::glfw::Viewer *_viewer) override;
  virtual bool pre_draw() override;
  /////////////////////////////////////////////////////////////////////////////
  virtual bool post_draw() override;
};

}}}}

#ifndef IGL_STATIC_LIBRARY
#  include "ImGuizmoPlugin.cpp"
#endif

#endif
