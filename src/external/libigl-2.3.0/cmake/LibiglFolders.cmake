# Sort projects inside the solution
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(igl_folder_targets FOLDER_NAME)
    foreach(target IN ITEMS ${ARGN})
        if(TARGET ${target})
            get_target_property(TYPE ${target} TYPE)
            if(NOT (TYPE STREQUAL "INTERFACE_LIBRARY"))
                set_target_properties(${target} PROPERTIES FOLDER "${FOLDER_NAME}")
            endif()
        endif()
    endforeach()
endfunction()

function(igl_set_folders)

igl_folder_targets("ThirdParty/Embree"
    algorithms
    embree
    lexers
    math
    simd
    sys
    tasking
)

igl_folder_targets("ThirdParty"
    CoMISo
    glad
    glfw
    imgui
    predicates
    tetgen
    tinyxml2
    triangle
)

igl_folder_targets("Libigl"
    igl
    igl_comiso
    igl_embree
    igl_opengl
    igl_opengl_glfw
    igl_opengl_glfw_imgui
    igl_png
    igl_predicates
    igl_stb_image
    igl_tetgen
    igl_triangle
    igl_xml
)

igl_folder_targets("Unit Tests"
    libigl_tests
)

igl_folder_targets("Tutorials"
    101_FileIO_bin
    102_DrawMesh_bin
    103_Events_bin
    104_Colors_bin
    105_Overlays_bin
    106_ViewerMenu_bin
    107_MultipleMeshes_bin
    108_MultipleViews_bin
    201_Normals_bin
    202_GaussianCurvature_bin
    203_CurvatureDirections_bin
    204_Gradient_bin
    205_Laplacian_bin
    206_GeodesicDistance_bin
    301_Slice_bin
    302_Sort_bin
    303_LaplaceEquation_bin
    304_LinearEqualityConstraints_bin
    305_QuadraticProgramming_bin
    306_EigenDecomposition_bin
    401_BiharmonicDeformation_bin
    402_PolyharmonicDeformation_bin
    403_BoundedBiharmonicWeights_bin
    404_DualQuaternionSkinning_bin
    405_AsRigidAsPossible_bin
    406_FastAutomaticSkinningTransformations_bin
    407_BiharmonicCoordinates_bin
    408_DirectDeltaMush_bin
    501_HarmonicParam_bin
    502_LSCMParam_bin
    503_ARAPParam_bin
    504_NRosyDesign_bin
    505_MIQ_bin
    506_FrameField_bin
    507_Planarization_bin
    601_Serialization_bin
    604_Triangle_bin
    605_Tetgen_bin
    606_AmbientOcclusion_bin
    607_ScreenCapture_bin
    701_Statistics_bin
    702_WindingNumber_bin
    703_Decimation_bin
    704_SignedDistance_bin
    705_MarchingCubes_bin
    706_FacetOrientation_bin
    707_SweptVolume_bin
    708_Picking_bin
    709_SLIM_bin
    710_SCAF_bin
    711_Subdivision_bin
    712_DataSmoothing_bin
    713_ShapeUp_bin
    714_MarchingTets_bin
    715_MeshImplicitFunction_bin
    716_HeatGeodesics_bin
    718_IterativeClosestPoint_bin
    719_ExplodedView_bin
)

endfunction()
