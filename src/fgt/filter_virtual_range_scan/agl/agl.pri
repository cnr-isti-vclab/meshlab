# the .pro file that includes this .pri must define the following
# variables:
# AGL_PATH: path to the agl library
# VCG_PATH: path to the vcg library
SOURCES += $$AGL_PATH/shaders/shader_set.cpp \
    $$VCG_PATH/wrap/ply/plylib.cpp \
    $$VCG_PATH/wrap/gui/trackball.cpp \
    $$AGL_PATH/shaders/shaders_manager.cpp \
    $$AGL_PATH/vcg_interface/gl_trackball_widget.cpp \
    $$VCG_PATH/wrap/gui/trackmode.cpp \
    $$AGL_PATH/textures/texture2d.cpp \
    $$AGL_PATH/utils/error_bin.cpp \
    $$AGL_PATH/buffers/fbo.cpp \
    $$AGL_PATH/utils/file_rw.cpp \
    $$AGL_PATH/shaders/shader_object.cpp \
    $$AGL_PATH/renderers/sobel_renderer.cpp \
    $$AGL_PATH/vrs/shared_resources.cpp
HEADERS += $$AGL_PATH/vcg_interface/vcg_type.h \
    $$AGL_PATH/vcg_interface/vcg_io.h \
    $$AGL_PATH/renderers/base_renderer.h \
    $$AGL_PATH/shaders/shader_set.h \
    $$AGL_PATH/renderers/one_shader_renderer.h \
    $$AGL_PATH/shaders/shader.h \
    $$AGL_PATH/shaders/shaders_manager.h \
    $$AGL_PATH/renderers/renderer.h \
    $$AGL_PATH/vcg_interface/gl_trackball_widget.h \
    $$AGL_PATH/textures/texture2d.h \
    $$AGL_PATH/utils/error_bin.h \
    $$AGL_PATH/buffers/fbo.h \
    $$AGL_PATH/renderers/shadow_map_renderer.h \
    $$AGL_PATH/utils/file_rw.h \
    $$AGL_PATH/shaders/shader_object.h \
    $$AGL_PATH/vcg_interface/vcg_renderer.h \
    $$AGL_PATH/renderers/composite_renderer.h \
    $$AGL_PATH/renderers/sobel_renderer.h \
    $$AGL_PATH/utils/viewing_info.h \
    $$AGL_PATH/renderers/mesh_renderer.h \
    $$AGL_PATH/agl.h \
    $$AGL_PATH/textures/texture_pixel.h \
    $$AGL_PATH/textures/client_texture2d.h \
    $$AGL_PATH/agl_global.h \
    $$AGL_PATH/utils/drawing_utils.h \
    $$AGL_PATH/shaders/shader_variables.h \
    $$AGL_PATH/shaders/simple_shader_variables.h \
    $$AGL_PATH/vrs/vrs_parameters.h \
    $$AGL_PATH/vrs/sampler.h \
    $$AGL_PATH/vrs/povs_generator.h \
    $$AGL_PATH/textures/textures_manager.h \
    $$AGL_PATH/shaders/shader_builder.h \
    $$AGL_PATH/vrs/pixel_data.h \
    $$AGL_PATH/vrs/pixel_data_manager.h \
    $$AGL_PATH/vrs/shared_resources.h \
    $$AGL_PATH/vrs/stages/stage.h \
    $$AGL_PATH/vrs/stages/start.h \
    $$AGL_PATH/vrs/stages/compactor.h \
    $$AGL_PATH/vrs/stages/killer.h \
    $$AGL_PATH/vrs/stages/mask_updater.h \
    $$AGL_PATH/vrs/stages/dead_masker.h \
    $$AGL_PATH/vrs/stages/final_compactor.h \
    $$AGL_PATH/vrs/detectors/feature_detector.h \
    $$AGL_PATH/vrs/detectors/sobel_detector.h \
    $$AGL_PATH/vrs/stages/feature_detection.h \
    $$AGL_PATH/vrs/stages/discriminator.h
OTHER_FILES += $$AGL_PATH/shaders/glsl/position.vert \
    $$AGL_PATH/shaders/glsl/position.frag \
    $$AGL_PATH/shaders/glsl/normal.vert \
    $$AGL_PATH/shaders/glsl/normal.frag \
    $$AGL_PATH/shaders/glsl/lambert.vert \
    $$AGL_PATH/shaders/glsl/lambert.frag \
    $$AGL_PATH/shaders/glsl/shadow_map.vert \
    $$AGL_PATH/shaders/glsl/shadow_map.frag \
    $$AGL_PATH/shaders/glsl/depth.vert \
    $$AGL_PATH/shaders/glsl/depth.frag \
    $$AGL_PATH/shaders/glsl/sobel.vert \
    $$AGL_PATH/shaders/glsl/sobel.frag \
    $$AGL_PATH/vrs/glsl/compactor.template \
    $$AGL_PATH/vrs/glsl/copier.vert \
    $$AGL_PATH/vrs/glsl/copier.frag \
    $$AGL_PATH/vrs/glsl/dead_masker.vert \
    $$AGL_PATH/vrs/glsl/dead_masker.frag \
    $$AGL_PATH/vrs/glsl/dead_masker_negation.vert \
    $$AGL_PATH/vrs/glsl/dead_masker_negation.frag \
    $$AGL_PATH/vrs/glsl/discriminator.vert \
    $$AGL_PATH/vrs/glsl/discriminator.frag \
    $$AGL_PATH/vrs/glsl/final_compactor.template \
    $$AGL_PATH/vrs/glsl/killer.vert \
    $$AGL_PATH/vrs/glsl/killer.frag \
    $$AGL_PATH/vrs/glsl/level_builder.vert \
    $$AGL_PATH/vrs/glsl/level_builder.frag \
    $$AGL_PATH/vrs/glsl/mask_updater.vert \
    $$AGL_PATH/vrs/glsl/mask_updater.frag \
    $$AGL_PATH/vrs/glsl/depth_fixer.vert \
    $$AGL_PATH/vrs/glsl/depth_fixer.frag \
    $$AGL_PATH/vrs/glsl/detectors/sobel.detector \
    $$AGL_PATH/vrs/glsl/detectors/depth_range_detector.vert \
    $$AGL_PATH/vrs/glsl/detectors/depth_range_detector.frag \
    $$AGL_PATH/vrs/glsl/detectors/detector.vert \
    $$AGL_PATH/vrs/glsl/detectors/detector.frag

RESOURCES += $$AGL_PATH/agl.qrc

