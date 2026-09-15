#include "edge_color_test_support.h"
#include <common/ml_document/mesh_model_state.h>
#include <QTemporaryDir>
#include <QDataStream>

int main()
try {
    QTemporaryDir temporary;
    require(temporary.isValid(), "temporary directory");
    MeshModel model(0, "", "");
    require(!model.hasPerEdgeColor(), "color storage does not imply color data");
    const int mask = loadMesh(model);
    require((mask & (Mask::IOM_EDGECOLOR | Mask::IOM_EDGEINDEX)) ==
            (Mask::IOM_EDGECOLOR | Mask::IOM_EDGEINDEX), "edge import mask");
    require(model.hasPerEdgeColor() && model.hasPerVertexColor(), "model color presence");
    require(model.cm.VN() == 3 && model.cm.EN() == 2 && model.cm.FN() == 0, "example counts");
    const Color vertexColors[] = {Color(255,0,0,255), Color(0,255,0,255), Color(0,0,255,255)};
    for (int i = 0; i < 3; ++i) require(model.cm.vert[i].C() == vertexColors[i], "vertex RGB");
    require(model.cm.edge[0].C() == Color(255,255,0,255), "yellow edge, default alpha");
    require(model.cm.edge[1].C() == Color(0,255,255,255), "cyan edge");
    for (int i = 0; i < 2; ++i) {
        require(model.cm.edge[i].V(0) == &model.cm.vert[0], "shared edge endpoint");
        require(model.cm.edge[i].V(1) == &model.cm.vert[i+1], "second endpoint");
    }

    model.updateDataMask();
    require(model.hasPerEdgeColor(), "mask refresh preserves actual edge colors");
    MeshModelState snapshot;
    snapshot.create(MeshModel::MM_EDGECOLOR, &model);
    model.cm.edge[0].C() = Color(1,2,3,4);
    require(snapshot.apply(&model), "restore edge colors");
    require(model.cm.edge[0].C() == Color(255,255,0,255), "restored edge color");
    CMeshO copied(model.cm);
    require(copied.EN() == 2 && copied.edge[1].C() == model.cm.edge[1].C(), "copy retains edge colors");
    require(copied.edge[0].V(0) == &copied.vert[0], "copy remaps endpoint pointers");

    // Distinct records with the same endpoints must not be merged by IO.
    vcg::tri::Allocator<CMeshO>::AddEdges(model.cm, 1);
    model.cm.edge.back().V(0) = &model.cm.vert[0];
    model.cm.edge.back().V(1) = &model.cm.vert[1];
    model.cm.edge.back().C() = Color(18,42,93,117);
    addFace(model.cm);
    const int capability = Exporter::GetExportMaskCapability();
    require((capability & Mask::IOM_EDGECOLOR) && (capability & Mask::IOM_EDGEINDEX), "export capabilities");
    for (bool binary : {false, true}) {
        const auto file = temporary.filePath(binary ? "binary.ply" : "ascii.ply");
        // Request color without EDGEINDEX: colors imply connectivity.
        require(Exporter::Save(model.cm, QFile::encodeName(file).constData(),
            Mask::IOM_VERTCOLOR | Mask::IOM_EDGECOLOR, binary) == 0, "save colored edges");
        MeshModel restored(1, "", "");
        loadMesh(restored, file);
        require(restored.cm.EN() == 3 && restored.cm.FN() == 1, "mixed mesh roundtrip counts");
        for (int i = 0; i < 3; ++i) {
            require(restored.cm.edge[i].C() == model.cm.edge[i].C(), "RGBA roundtrip");
            require(restored.cm.vert[i].C() == model.cm.vert[i].C(), "vertex colors remain independent");
        }
        require(restored.cm.edge[2].V(1) == &restored.cm.vert[1], "duplicate endpoint roundtrip");
        require(Exporter::Save(model.cm, QFile::encodeName(file).constData(),
            Mask::IOM_VERTCOLOR | Mask::IOM_EDGEINDEX, binary) == 0, "save without edge color");
        MeshModel plain(2, "", "");
        loadMesh(plain, file);
        require(plain.cm.EN() == 3 && !plain.hasPerEdgeColor(), "disable color, retain geometry");
    }

    vcg::tri::Allocator<CMeshO>::DeleteEdge(model.cm, model.cm.edge[1]);
    vcg::tri::Allocator<CMeshO>::CompactEveryVector(model.cm);
    require(model.cm.EN() == 2 && model.cm.edge[1].C() == Color(18,42,93,117), "edge compaction preserves records");

    const auto path = temporary.filePath("variant.ply");
    const auto original = fixtureText();
    for (auto byteOrder : {QDataStream::LittleEndian, QDataStream::BigEndian}) {
        auto contents = original.left(original.indexOf("end_header\n") + 11);
        contents.replace("format ascii", byteOrder == QDataStream::BigEndian ?
            "format binary_big_endian" : "format binary_little_endian");
        QDataStream stream(&contents, QIODevice::WriteOnly | QIODevice::Append);
        stream.setByteOrder(byteOrder);
        stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
        for (int i = 0; i < 3; ++i) {
            stream << float(i == 1) << float(i == 2) << float(0);
            for (int c = 0; c < 3; ++c) stream << quint8(vertexColors[i][c]);
        }
        stream << qint32(0) << qint32(1) << quint8(255) << quint8(255) << quint8(0);
        stream << qint32(0) << qint32(2) << quint8(0) << quint8(255) << quint8(255);
        saveText(path, contents);
        MeshModel binary(3, "", "");
        loadMesh(binary, path);
        require(binary.cm.edge[0].C() == Color(255,255,0,255) &&
            binary.cm.edge[1].C() == Color(0,255,255,255), "both binary byte orders import edge RGB");
    }
    auto edgeOnly = original.left(original.indexOf("element edge"));
    edgeOnly.replace("property uchar red\nproperty uchar green\nproperty uchar blue\n", "");
    edgeOnly += original.mid(original.indexOf("element edge"));
    edgeOnly.replace("0 0 0 255 0 0\n1 0 0 0 255 0\n0 1 0 0 0 255\n", "0 0 0\n1 0 0\n0 1 0\n");
    saveText(path, edgeOnly);
    MeshModel edgeOnlyModel(3, "", "");
    loadMesh(edgeOnlyModel, path);
    require(edgeOnlyModel.hasPerEdgeColor() && !edgeOnlyModel.hasPerVertexColor(), "edge colors without vertex colors");
    require(attribute(defaults(edgeOnlyModel), RD::PR_WIREFRAME_EDGES, Att::ATT_EDGECOLOR), "edge-only colors selected by default");
    for (int channels : {0, 1, 2}) {
        auto input = original;
        const auto edgeStart = input.indexOf("element edge");
        auto header = input.left(edgeStart);
        auto tail = input.mid(edgeStart);
        if (channels < 3) tail.replace("property uchar blue\n", "");
        if (channels < 2) tail.replace("property uchar green\n", "");
        if (channels < 1) tail.replace("property uchar red\n", "");
        tail.replace("0 1 255 255 0\n", channels == 0 ? "0 1\n" : channels == 1 ? "0 1 255\n" : "0 1 255 255\n");
        tail.replace("0 2 0 255 255\n", channels == 0 ? "0 2\n" : channels == 1 ? "0 2 0\n" : "0 2 0 255\n");
        saveText(path, header + tail);
        MeshModel partial(3, "", "");
        loadMesh(partial, path);
        require(!partial.hasPerEdgeColor() && partial.cm.EN() == 2, "incomplete RGB is not a color attribute");
    }
    auto invalid = original;
    invalid.replace("0 1 255 255 0", "0 3 255 255 0");
    saveText(path, invalid);
    CMeshO bad;
    int badMask = 0;
    require(Importer::Open(bad, QFile::encodeName(path).constData(), badMask) ==
            vcg::tri::io::PlyInfo::E_BAD_VERT_INDEX_EDGE, "invalid endpoint rejected");
    invalid = original;
    invalid.replace("0 1 255 255 0", "-1 1 255 255 0");
    saveText(path, invalid);
    require(Importer::Open(bad, QFile::encodeName(path).constData(), badMask) ==
            vcg::tri::io::PlyInfo::E_BAD_VERT_INDEX_EDGE, "negative endpoint rejected");
    saveText(path, original.left(original.lastIndexOf("0 2 0 255 255")));
    require(Importer::Open(bad, QFile::encodeName(path).constData(), badMask) ==
            vcg::tri::io::PlyInfo::E_SHORTFILE, "truncated edge data rejected");
    auto empty = original;
    empty.replace("element edge 2", "element edge 0");
    empty.replace("0 1 255 255 0\n0 2 0 255 255\n", "");
    saveText(path, empty);
    MeshModel noEdges(4, "", "");
    loadMesh(noEdges, path);
    require(noEdges.cm.EN() == 0 && !defaults(noEdges).isPrimitiveActive(RD::PR_WIREFRAME_EDGES), "empty edge collection");
    require(Exporter::Save(noEdges.cm, QFile::encodeName(path).constData(),
        Mask::IOM_VERTCOLOR | Mask::IOM_EDGECOLOR, false) == 0, "save empty edge collection");
    MeshModel emptyRoundtrip(5, "", "");
    loadMesh(emptyRoundtrip, path);
    require(emptyRoundtrip.cm.EN() == 0 && !emptyRoundtrip.hasPerEdgeColor() &&
        emptyRoundtrip.cm.vert[0].C() == vertexColors[0], "no edge properties emitted without an edge element");
    model.clearDataMask(MeshModel::MM_EDGECOLOR);
    model.updateDataMask();
    require(!model.hasPerEdgeColor(), "mask refresh does not invent colors");
    std::cout << "PLY IO and mesh-state regressions passed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
