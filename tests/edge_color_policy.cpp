#include "edge_color_test_support.h"

int main()
try {
    MeshModel model(0, "", "");
    loadMesh(model);
    auto data = defaults(model);
    require(data.isPrimitiveActive(RD::PR_POINTS) && data.isPrimitiveActive(RD::PR_WIREFRAME_EDGES), "show points and edges");
    require(attribute(data, RD::PR_POINTS, Att::ATT_VERTCOLOR), "points use vertex colors");
    require(attribute(data, RD::PR_WIREFRAME_EDGES, Att::ATT_EDGECOLOR), "edges use edge colors");
    require(!attribute(data, RD::PR_WIREFRAME_EDGES, Att::ATT_VERTCOLOR), "edge color takes precedence");
    MLPerViewGLOptions options;
    data.get(options);
    require(options._perpoint_noshading && options._perwire_noshading, "unlit graph colors");
    require(!options._perwire_fixed_color_enabled && !options._perwire_mesh_color_enabled, "no wire color override");

    std::string encoded;
    data.serialize(encoded);
    require(encoded.substr(0, 3) == "v2:", "versioned rendering settings");
    RD decoded;
    require(decoded.deserialize(encoded), "new settings roundtrip");
    require(attribute(decoded, RD::PR_WIREFRAME_EDGES, Att::ATT_EDGECOLOR), "edge color mode survives roundtrip");
    for (int pm = 0; pm < RD::PR_ARITY; ++pm)
        require(data.isPrimitiveActive(RD::PRIMITIVE_MODALITY(pm)) == decoded.isPrimitiveActive(RD::PRIMITIVE_MODALITY(pm)), "visibility survives roundtrip");
    // Legacy layout: modality mask, four blocks of seven public bits + two indices, 32 GL option bits.
    const std::string legacy = "0011" "100100000" "100100001" "000000000" "000000000" + std::string(32, '0');
    require(decoded.deserialize(legacy), "read legacy project settings");
    require(decoded.isPrimitiveActive(RD::PR_POINTS) && decoded.isPrimitiveActive(RD::PR_WIREFRAME_EDGES), "legacy visibility");
    require(attribute(decoded, RD::PR_POINTS, Att::ATT_VERTCOLOR) && attribute(decoded, RD::PR_WIREFRAME_EDGES, Att::ATT_VERTCOLOR), "legacy vertex-color mapping");
    require(!attribute(decoded, RD::PR_WIREFRAME_EDGES, Att::ATT_EDGECOLOR), "legacy edge colors default off");
    require(!decoded.deserialize("v2:101") && !decoded.deserialize("v9:invalid"), "malformed settings rejected");

    RD::RendAtts changed;
    Policies::fromMeshModelMaskToMLRenderingAtts(MeshModel::MM_EDGECOLOR, changed);
    require(changed[Att::ATT_EDGECOLOR] && !changed[Att::ATT_VERTCOLOR], "independent change notification");
    addFace(model.cm);
    data = defaults(model);
    require(data.isPrimitiveActive(RD::PR_SOLID) && data.isPrimitiveActive(RD::PR_WIREFRAME_EDGES), "mixed meshes show solid plus explicit edges");
    require(!data.isPrimitiveActive(RD::PR_POINTS), "mixed mesh retains point visibility default");
    require(Policies::isPrimitiveModalityCompatibleWithMesh(&model, RD::PR_WIREFRAME_EDGES), "mixed explicit-edge compatibility");
    require(!attribute(data, RD::PR_SOLID, Att::ATT_EDGECOLOR), "solid colors remain independent");
    data.set(RD::PR_WIREFRAME_TRIANGLES, Att::ATT_EDGECOLOR, true);
    require(!attribute(data, RD::PR_WIREFRAME_TRIANGLES, Att::ATT_EDGECOLOR), "generated wireframes cannot use edge colors");

    model.clearDataMask(MeshModel::MM_EDGECOLOR | MeshModel::MM_VERTCOLOR);
    model.updateDataMask(MeshModel::MM_FACECOLOR);
    data = defaults(model);
    require(attribute(data, RD::PR_SOLID, Att::ATT_FACECOLOR), "existing face-color default");
    model.updateDataMask(MeshModel::MM_WEDGTEXCOORD | MeshModel::MM_POLYGONAL);
    data = defaults(model);
    require(attribute(data, RD::PR_SOLID, Att::ATT_WEDGETEXTURE), "texture attribute remains available");
    require(data.isPrimitiveActive(RD::PR_WIREFRAME_EDGES), "polygonal wire default");
    MeshModel cloud(1, "", "");
    vcg::tri::Allocator<CMeshO>::AddVertices(cloud.cm, 1);
    cloud.updateDataMask(MeshModel::MM_VERTCOLOR);
    data = defaults(cloud);
    require(data.isPrimitiveActive(RD::PR_POINTS) && !data.isPrimitiveActive(RD::PR_WIREFRAME_EDGES), "point-cloud default unchanged");
    std::cout << "Rendering policy and project compatibility regressions passed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
