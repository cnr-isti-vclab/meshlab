// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2018 Jérémie Dumas <jeremie.dumas@ens-lyon.org>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.
////////////////////////////////////////////////////////////////////////////////
#include "ImGuiMenu.h"
#include "ImGuiHelpers.h"
#include <igl/project.h>
#include <imgui/imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_fonts_droid_sans.h>
#include <GLFW/glfw3.h>
#include <iostream>
////////////////////////////////////////////////////////////////////////////////

namespace igl
{
namespace opengl
{
namespace glfw
{
namespace imgui
{

IGL_INLINE void ImGuiMenu::init(igl::opengl::glfw::Viewer *_viewer)
{
  ViewerPlugin::init(_viewer);
  // Setup ImGui binding
  if (_viewer)
  {
    IMGUI_CHECKVERSION();
    if (!context_)
    {
      // Single global context by default, but can be overridden by the user
      static ImGuiContext * __global_context = ImGui::CreateContext();
      context_ = __global_context;
    }
    const char* glsl_version = "#version 150";
    ImGui_ImplGlfw_InitForOpenGL(viewer->window, false);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 5.0f;
    reload_font();
  }
}

IGL_INLINE void ImGuiMenu::reload_font(int font_size)
{
  hidpi_scaling_ = hidpi_scaling();
  pixel_ratio_ = pixel_ratio();
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->Clear();
  io.Fonts->AddFontFromMemoryCompressedTTF(droid_sans_compressed_data,
    droid_sans_compressed_size, font_size * hidpi_scaling_);
  io.FontGlobalScale = 1.0 / pixel_ratio_;
}

IGL_INLINE void ImGuiMenu::shutdown()
{
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  // User is responsible for destroying context if a custom context is given
  // ImGui::DestroyContext(*context_);
}

IGL_INLINE bool ImGuiMenu::pre_draw()
{
  glfwPollEvents();

  // Check whether window dpi has changed
  float scaling = hidpi_scaling();
  if (std::abs(scaling - hidpi_scaling_) > 1e-5)
  {
    reload_font();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  return false;
}

IGL_INLINE bool ImGuiMenu::post_draw()
{
  draw_menu();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  return false;
}

IGL_INLINE void ImGuiMenu::post_resize(int width, int height)
{
  if (context_)
  {
    ImGui::GetIO().DisplaySize.x = float(width);
    ImGui::GetIO().DisplaySize.y = float(height);
  }
}

// Mouse IO
IGL_INLINE bool ImGuiMenu::mouse_down(int button, int modifier)
{
  ImGui_ImplGlfw_MouseButtonCallback(viewer->window, button, GLFW_PRESS, modifier);
  return ImGui::GetIO().WantCaptureMouse;
}

IGL_INLINE bool ImGuiMenu::mouse_up(int button, int modifier)
{
  //return ImGui::GetIO().WantCaptureMouse;
  // !! Should not steal mouse up
  return false;
}

IGL_INLINE bool ImGuiMenu::mouse_move(int mouse_x, int mouse_y)
{
  return ImGui::GetIO().WantCaptureMouse;
}

IGL_INLINE bool ImGuiMenu::mouse_scroll(float delta_y)
{
  ImGui_ImplGlfw_ScrollCallback(viewer->window, 0.f, delta_y);
  return ImGui::GetIO().WantCaptureMouse;
}

// Keyboard IO
IGL_INLINE bool ImGuiMenu::key_pressed(unsigned int key, int modifiers)
{
  ImGui_ImplGlfw_CharCallback(nullptr, key);
  return ImGui::GetIO().WantCaptureKeyboard;
}

IGL_INLINE bool ImGuiMenu::key_down(int key, int modifiers)
{
  ImGui_ImplGlfw_KeyCallback(viewer->window, key, 0, GLFW_PRESS, modifiers);
  return ImGui::GetIO().WantCaptureKeyboard;
}

IGL_INLINE bool ImGuiMenu::key_up(int key, int modifiers)
{
  ImGui_ImplGlfw_KeyCallback(viewer->window, key, 0, GLFW_RELEASE, modifiers);
  return ImGui::GetIO().WantCaptureKeyboard;
}

// Draw menu
IGL_INLINE void ImGuiMenu::draw_menu()
{
  // Viewer settings
  if (callback_draw_viewer_window) { callback_draw_viewer_window(); }
  else { draw_viewer_window(); }

  // Other windows
  if (callback_draw_custom_window) { callback_draw_custom_window(); }
  else { draw_custom_window(); }
}

IGL_INLINE void ImGuiMenu::draw_viewer_window()
{
  float menu_width = 180.f * menu_scaling();
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
  ImGui::SetNextWindowSizeConstraints(ImVec2(menu_width, -1.0f), ImVec2(menu_width, -1.0f));
  bool _viewer_menu_visible = true;
  ImGui::Begin(
      "Viewer", &_viewer_menu_visible,
      ImGuiWindowFlags_NoSavedSettings
      | ImGuiWindowFlags_AlwaysAutoResize
  );
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
  if (callback_draw_viewer_menu) { callback_draw_viewer_menu(); }
  else { draw_viewer_menu(); }
  ImGui::PopItemWidth();
  ImGui::End();
}

IGL_INLINE void ImGuiMenu::draw_viewer_menu()
{
  // Workspace
  if (ImGui::CollapsingHeader("Workspace", ImGuiTreeNodeFlags_DefaultOpen))
  {
    float w = ImGui::GetContentRegionAvailWidth();
    float p = ImGui::GetStyle().FramePadding.x;
    if (ImGui::Button("Load##Workspace", ImVec2((w-p)/2.f, 0)))
    {
      viewer->load_scene();
    }
    ImGui::SameLine(0, p);
    if (ImGui::Button("Save##Workspace", ImVec2((w-p)/2.f, 0)))
    {
      viewer->save_scene();
    }
  }

  // Mesh
  if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
  {
    float w = ImGui::GetContentRegionAvailWidth();
    float p = ImGui::GetStyle().FramePadding.x;
    if (ImGui::Button("Load##Mesh", ImVec2((w-p)/2.f, 0)))
    {
      viewer->open_dialog_load_mesh();
    }
    ImGui::SameLine(0, p);
    if (ImGui::Button("Save##Mesh", ImVec2((w-p)/2.f, 0)))
    {
      viewer->open_dialog_save_mesh();
    }
  }

  // Viewing options
  if (ImGui::CollapsingHeader("Viewing Options", ImGuiTreeNodeFlags_DefaultOpen))
  {
    if (ImGui::Button("Center object", ImVec2(-1, 0)))
    {
      viewer->core().align_camera_center(viewer->data().V, viewer->data().F);
    }
    if (ImGui::Button("Snap canonical view", ImVec2(-1, 0)))
    {
      viewer->snap_to_canonical_quaternion();
    }

    // Zoom
    ImGui::PushItemWidth(80 * menu_scaling());
    ImGui::DragFloat("Zoom", &(viewer->core().camera_zoom), 0.05f, 0.1f, 20.0f);

    // Select rotation type
    int rotation_type = static_cast<int>(viewer->core().rotation_type);
    static Eigen::Quaternionf trackball_angle = Eigen::Quaternionf::Identity();
    static bool orthographic = true;
    if (ImGui::Combo("Camera Type", &rotation_type, "Trackball\0Two Axes\0002D Mode\0\0"))
    {
      using RT = igl::opengl::ViewerCore::RotationType;
      auto new_type = static_cast<RT>(rotation_type);
      if (new_type != viewer->core().rotation_type)
      {
        if (new_type == RT::ROTATION_TYPE_NO_ROTATION)
        {
          trackball_angle = viewer->core().trackball_angle;
          orthographic = viewer->core().orthographic;
          viewer->core().trackball_angle = Eigen::Quaternionf::Identity();
          viewer->core().orthographic = true;
        }
        else if (viewer->core().rotation_type == RT::ROTATION_TYPE_NO_ROTATION)
        {
          viewer->core().trackball_angle = trackball_angle;
          viewer->core().orthographic = orthographic;
        }
        viewer->core().set_rotation_type(new_type);
      }
    }

    // Orthographic view
    ImGui::Checkbox("Orthographic view", &(viewer->core().orthographic));
    ImGui::PopItemWidth();
  }

  // Helper for setting viewport specific mesh options
  auto make_checkbox = [&](const char *label, unsigned int &option)
  {
    return ImGui::Checkbox(label,
      [&]() { return viewer->core().is_set(option); },
      [&](bool value) { return viewer->core().set(option, value); }
    );
  };

  // Draw options
  if (ImGui::CollapsingHeader("Draw Options", ImGuiTreeNodeFlags_DefaultOpen))
  {
    if (ImGui::Checkbox("Face-based", &(viewer->data().face_based)))
    {
      viewer->data().dirty = MeshGL::DIRTY_ALL;
    }
    make_checkbox("Show texture", viewer->data().show_texture);
    if (ImGui::Checkbox("Invert normals", &(viewer->data().invert_normals)))
    {
      viewer->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
    }
    make_checkbox("Show overlay", viewer->data().show_overlay);
    make_checkbox("Show overlay depth", viewer->data().show_overlay_depth);
    ImGui::ColorEdit4("Background", viewer->core().background_color.data(),
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
    ImGui::ColorEdit4("Line color", viewer->data().line_color.data(),
        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_PickerHueWheel);
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.3f);
    ImGui::DragFloat("Shininess", &(viewer->data().shininess), 0.05f, 0.0f, 100.0f);
    ImGui::PopItemWidth();
  }

  // Overlays
  if (ImGui::CollapsingHeader("Overlays", ImGuiTreeNodeFlags_DefaultOpen))
  {
    make_checkbox("Wireframe", viewer->data().show_lines);
    make_checkbox("Fill", viewer->data().show_faces);
    make_checkbox("Show vertex labels", viewer->data().show_vertex_labels);
    make_checkbox("Show faces labels", viewer->data().show_face_labels);
    make_checkbox("Show extra labels", viewer->data().show_custom_labels);
  }
}

IGL_INLINE float ImGuiMenu::pixel_ratio()
{
  // Computes pixel ratio for hidpi devices
  int buf_size[2];
  int win_size[2];
  GLFWwindow* window = glfwGetCurrentContext();
  glfwGetFramebufferSize(window, &buf_size[0], &buf_size[1]);
  glfwGetWindowSize(window, &win_size[0], &win_size[1]);
  return (float) buf_size[0] / (float) win_size[0];
}

IGL_INLINE float ImGuiMenu::hidpi_scaling()
{
  // Computes scaling factor for hidpi devices
  float xscale, yscale;
  GLFWwindow* window = glfwGetCurrentContext();
  glfwGetWindowContentScale(window, &xscale, &yscale);
  return 0.5 * (xscale + yscale);
}

} // end namespace
} // end namespace
} // end namespace
} // end namespace
