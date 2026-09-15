#include "edge_color_test_support.h"
#include <meshlab/ml_render_gui.h>
#include <meshlab/dialogs/save_mesh_attributes_dialog.h>
#include <QApplication>
#include <QCheckBox>
#include <QPluginLoader>
#include <QTemporaryDir>
#include <common/plugins/interfaces/io_plugin.h>

int main(int argc, char** argv)
try {
    QApplication app(argc, argv);
    MeshModel model(0, "", "");
    loadMesh(model);
    const auto data = defaults(model);
    MLRenderingWireParametersFrame frame(0, nullptr);
    frame.updateVisibility(&model);
    frame.setPrimitiveButtonStatesAccordingToRenderingData(data);
    auto* edge = frame.findChild<MLRenderingPerEdgeColorAction*>();
    auto* vertex = frame.findChild<MLRenderingPerVertexColorAction*>();
    auto* mesh = frame.findChild<MLRenderingPerMeshColorAction*>();
    auto* user = frame.findChild<MLRenderingUserDefinedColorAction*>();
    require(edge && vertex && mesh && user, "wire color controls");
    require(edge->isVisible() && edge->isEnabled() && edge->isChecked(), "edge-color default shown in UI");
    for (auto* choice : {static_cast<MLRenderingAction*>(vertex), static_cast<MLRenderingAction*>(mesh),
                        static_cast<MLRenderingAction*>(user), static_cast<MLRenderingAction*>(edge)}) {
        choice->setChecked(true);
        auto next = data;
        frame.getCurrentRenderingDataAccordingToGUI(next);
        require(attribute(next, RD::PR_POINTS, Att::ATT_VERTCOLOR), "wire UI leaves point colors unchanged");
        require(attribute(next, RD::PR_WIREFRAME_EDGES, Att::ATT_EDGECOLOR) == (choice == edge), "edge color is mutually exclusive");
        require(attribute(next, RD::PR_WIREFRAME_EDGES, Att::ATT_VERTCOLOR) == (choice == vertex), "vertex color is mutually exclusive");
        MLPerViewGLOptions nextOptions;
        next.get(nextOptions);
        require(nextOptions._peredge_fauxwire_enabled, "color changes preserve explicit-edge source");
    }
    addFace(model.cm);
    frame.updateVisibility(&model);
    auto* source = frame.findChild<MLRenderingFauxEdgeWireAction*>();
    auto* sourceToolbar = frame.findChild<MLRenderingOnOffToolbar*>();
    require(source && sourceToolbar && sourceToolbar->actions().first()->isVisible(), "mixed mesh exposes explicit-edge mode");
    source->setChecked(false);
    frame.switchWireModality(0, source);
    require(!edge->isEnabled() && vertex->isChecked(), "triangle mode disables edge colors and selects vertex colors");
    source->setChecked(true);
    frame.switchWireModality(0, source);
    require(edge->isEnabled(), "return to explicit-edge mode");

    const int capability = Exporter::GetExportMaskCapability();
    SaveMeshAttributesDialog save(nullptr, &model, capability, capability, RichParameterList());
    auto* checkbox = save.findChild<QCheckBox*>("check_iom_edgecolor");
    require(checkbox && checkbox->isEnabled() && checkbox->isChecked(), "export edge colors enabled by default");
    require(QMetaObject::invokeMethod(&save, "on_okButton_clicked"), "accept default export options");
    require((save.getNewMask() & Mask::IOM_EDGEINDEX) && (save.getNewMask() & Mask::IOM_EDGECOLOR), "default export preserves edges and colors");
    checkbox->setChecked(false);
    require(QMetaObject::invokeMethod(&save, "on_okButton_clicked"), "apply export options");
    require((save.getNewMask() & Mask::IOM_EDGEINDEX) && !(save.getNewMask() & Mask::IOM_EDGECOLOR), "disable export colors without dropping edges");
    model.clearDataMask(MeshModel::MM_EDGECOLOR);
    frame.updateVisibility(&model);
    require(!edge->isVisible(), "hide unavailable edge colors");
    SaveMeshAttributesDialog plain(nullptr, &model, capability, capability, RichParameterList());
    require(!plain.findChild<QCheckBox*>("check_iom_edgecolor")->isEnabled(), "disable export colors when absent");
#ifdef EDGE_COLOR_IO_PLUGIN
    QPluginLoader loader(EDGE_COLOR_IO_PLUGIN);
    auto* plugin = qobject_cast<IOPlugin*>(loader.instance());
    require(plugin != nullptr, loader.errorString().toStdString());
    QTemporaryDir temporary;
    MeshModel imported(4, "", "");
    int importedMask = 0;
    plugin->open("PLY", EDGE_COLOR_FIXTURE, imported, importedMask, RichParameterList(), nullptr);
    require(imported.hasPerEdgeColor(), "IO plugin enables edge-color data");
    int pluginCapability = 0, pluginDefaults = 0;
    plugin->exportMaskCapability("PLY", pluginCapability, pluginDefaults);
    require((pluginDefaults & Mask::IOM_EDGECOLOR) && (pluginDefaults & Mask::IOM_EDGEINDEX), "plugin export defaults");
    // Match the application's save path: filter defaults through the mesh's
    // available attributes before passing the mask to the plugin.
    SaveMeshAttributesDialog pluginSave(nullptr, &imported, pluginCapability, pluginDefaults, RichParameterList());
    require(QMetaObject::invokeMethod(&pluginSave, "on_okButton_clicked"), "accept plugin export options");
    const int pluginSaveMask = pluginSave.getNewMask();
    require((pluginSaveMask & Mask::IOM_EDGECOLOR) && (pluginSaveMask & Mask::IOM_EDGEINDEX), "graph export includes edges and colors");
    require(!(pluginSaveMask & Mask::IOM_BITPOLYGONAL), "graph export does not request polygon topology");
    const auto filename = temporary.filePath("plugin.ply");
    plugin->save("PLY", filename, imported, pluginSaveMask, plugin->initSaveParameter("PLY", imported), nullptr);
    MeshModel roundtrip(5, "", "");
    loadMesh(roundtrip, filename);
    require(roundtrip.hasPerEdgeColor() && roundtrip.cm.edge[1].C() == Color(0,255,255,255), "plugin save/reload");
    imported.clearDataMask(MeshModel::MM_EDGECOLOR);
    plugin->save("PLY", filename, imported, pluginSaveMask, plugin->initSaveParameter("PLY", imported), nullptr);
    MeshModel uncolored(6, "", "");
    loadMesh(uncolored, filename);
    require(!uncolored.hasPerEdgeColor() && uncolored.cm.EN() == 2, "plugin defaults do not invent edge-color data");
#endif
    std::cout << "Rendering and export UI regressions passed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
