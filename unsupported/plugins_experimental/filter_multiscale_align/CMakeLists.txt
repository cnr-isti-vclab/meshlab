# Copyright 2019-2020, Collabora, Ltd.
# SPDX-License-Identifier: BSL-1.0

### Generated file! Edit the templates in src/templates,
### specifically src/templates/CMakeLists.template.cmake (shared with all other directories),
### or create a derived template in src/templates/filter_multiscale_align.cmake,
### then re-run ./make-cmake.py

set(SOURCES filter_multiscale_align.cpp generic_align.cpp multiscale_align.cpp)

set(HEADERS
    filter_multiscale_align.h
    generic_align.h
    lib/Dynamic/basics.h
    lib/Dynamic/dynamicSolver.h
    lib/Dynamic/needlemanWunsch.h
    lib/Dynamic/scaleEstimation.h
    lib/Patate/Grenaille/Core/algebraicSphere.h
    lib/Patate/Grenaille/Core/basket.h
    lib/Patate/Grenaille/Core/covariancePlaneFit.h
    lib/Patate/Grenaille/Core/curvature.h
    lib/Patate/Grenaille/Core/enums.h
    lib/Patate/Grenaille/Core/gls.h
    lib/Patate/Grenaille/Core/orientedSphereFit.h
    lib/Patate/Grenaille/Core/plane.h
    lib/Patate/Grenaille/Core/primitive.h
    lib/Patate/Grenaille/Core/unorientedSphereFit.h
    lib/Patate/Grenaille/Core/weightFunc.h
    lib/Patate/Grenaille/Core/weightKernel.h
    lib/Patate/Vitelotte/Core/bezierPath.h
    lib/Patate/Vitelotte/Core/elementBuilderBase.h
    lib/Patate/Vitelotte/Core/femSolver.h
    lib/Patate/Vitelotte/Core/femUtils.h
    lib/Patate/Vitelotte/Core/fvElement.h
    lib/Patate/Vitelotte/Core/fvElementBuilder.h
    lib/Patate/Vitelotte/Core/linearElement.h
    lib/Patate/Vitelotte/Core/linearElementBuilder.h
    lib/Patate/Vitelotte/Core/morleyElement.h
    lib/Patate/Vitelotte/Core/morleyElementBuilder.h
    lib/Patate/Vitelotte/Core/quadraticElement.h
    lib/Patate/Vitelotte/Core/quadraticElementBuilder.h
    lib/Patate/Vitelotte/Core/singularElementDecorator.h
    lib/Patate/Vitelotte/Core/solverError.h
    lib/Patate/Vitelotte/Core/vgMesh.h
    lib/Patate/Vitelotte/Utils/dcMesh.h
    lib/Patate/Vitelotte/Utils/mvgReader.h
    lib/Patate/Vitelotte/Utils/mvgWithCurvesReader.h
    lib/Patate/Vitelotte/Utils/mvgWithCurvesWriter.h
    lib/Patate/Vitelotte/Utils/mvgWriter.h
    lib/Patate/Vitelotte/Utils/vgMeshRenderer.h
    lib/Patate/common/defines.h
    lib/Patate/common/gl_utils/color.h
    lib/Patate/common/gl_utils/macros.h
    lib/Patate/common/gl_utils/shader.h
    lib/Patate/common/surface_mesh/objReader.h
    lib/Patate/common/surface_mesh/objWriter.h
    lib/Patate/common/surface_mesh/properties.h
    lib/Patate/common/surface_mesh/surfaceMesh.h
    lib/Patate/grenaille.h
    lib/Patate/vitelotte.h
    lib/Patate/vitelotte_gl.h
    multiscale_align.h
    struct.h
    utils.h)

add_library(filter_multiscale_align MODULE ${SOURCES} ${HEADERS})

target_include_directories(filter_multiscale_align
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(filter_multiscale_align PUBLIC common)

set_property(TARGET filter_multiscale_align PROPERTY FOLDER Plugins)

set_property(TARGET filter_multiscale_align
             PROPERTY RUNTIME_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

set_property(TARGET filter_multiscale_align
             PROPERTY LIBRARY_OUTPUT_DIRECTORY ${MESHLAB_PLUGIN_OUTPUT_DIR})

install(
    TARGETS filter_multiscale_align
    DESTINATION ${MESHLAB_PLUGIN_INSTALL_DIR}
    COMPONENT Plugins)
