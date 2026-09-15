#include "edge_color_test_support.h"
#include <common/GLExtensionsManager.h>
#include <QGuiApplication>
#include <QOffscreenSurface>
#include <QOpenGLContext>
#include <QOpenGLFramebufferObject>
#include <QImage>
#include <cmath>

// The production manager requires a subclass to supply its synchronization policy.
// These tests use one thread and one current GL context.
class Manager : public vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<CMeshO, int, MLPerViewGLOptions>
{
    using Base = vcg::NotThreadSafeGLMeshAttributesMultiViewerBOManager<CMeshO, int, MLPerViewGLOptions>;
public:
    Manager(CMeshO& mesh, vcg::MemoryInfo& memory, size_t batch) : Base(mesh, memory, batch) {}
    using Base::draw;
    using Base::setPerViewInfo;
    using Base::manageBuffers;
    using Base::isBORenderingAvailable;
    using Base::meshAttributesUpdated;
    using Base::removeAllViews;
};
constexpr int imageSize = 192;

static void expectPixel(float x, float y, const Color& expected, const std::string& description)
{
    const int px = int((x + .25f) / 1.5f * imageSize);
    const int py = int((y + .25f) / 1.5f * imageSize);
    unsigned char pixels[3 * 3 * 4];
    glReadPixels(px - 1, py - 1, 3, 3, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    bool matches = false;
    for (int i = 0; i < 9; ++i) {
        bool match = true;
        for (int c = 0; c < 3; ++c) match &= std::abs(int(pixels[i*4+c]) - int(expected[c])) <= 5;
        matches |= match;
    }
    require(matches, description + " (center RGB " + std::to_string(pixels[16]) + "," +
        std::to_string(pixels[17]) + "," + std::to_string(pixels[18]) + ")");
}

static void draw(Manager& manager, int view = 0, const std::vector<GLuint>& textures = {})
{
    glViewport(0, 0, imageSize, imageSize);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-.25, 1.25, -.25, 1.25, -2, 2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_DITHER);
    glDisable(GL_BLEND);
    glDisable(GL_MULTISAMPLE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LINE_SMOOTH);
    glShadeModel(GL_SMOOTH);
    manager.draw(view, textures);
    require(glGetError() == GL_NO_ERROR, "render without OpenGL errors");
}

static void runRenderer(bool gpu, QOpenGLFramebufferObject& framebuffer)
{
    MeshModel model(0, "", "");
    loadMesh(model);
    auto data = defaults(model);
    MLPerViewGLOptions options;
    data.get(options);
    options._perpoint_pointsize = 9;
    options._perpoint_pointattenuation_enabled = false;
    options._perwire_wirewidth = 3;
    data.set(options);
    vcg::NotThreadSafeMemoryInfo memory(gpu ? 64 * 1024 * 1024 : 0);
    Manager manager(model.cm, memory, 10000);
    manager.setPerViewInfo(0, data);
    manager.manageBuffers();
    require(manager.isBORenderingAvailable() == gpu, "exercise requested rendering backend");
    draw(manager);
    expectPixel(0, 0, Color(255,0,0,255), "shared vertex remains red");
    expectPixel(1, 0, Color(0,255,0,255), "green endpoint visible");
    expectPixel(0, 1, Color(0,0,255,255), "blue endpoint visible");
    for (float t : {.25f, .5f, .75f}) {
        expectPixel(t, 0, Color(255,255,0,255), "uniform yellow edge");
        expectPixel(0, t, Color(0,255,255,255), "uniform cyan edge");
    }
    require(framebuffer.toImage().save(gpu ? "edge-color-gpu.png" : "edge-color-fallback.png"), "save render evidence");

    RD::RendAtts changed;
    changed[Att::ATT_EDGECOLOR] = true;
    model.cm.edge[0].C() = Color(255,0,255,255);
    manager.meshAttributesUpdated(false, changed);
    manager.manageBuffers();
    draw(manager);
    expectPixel(.5f, 0, Color(255,0,255,255), "edge-color update reaches renderer");
    expectPixel(0, 0, Color(255,0,0,255), "edge-color edit leaves vertex color unchanged");

    changed.reset();
    changed[Att::ATT_VERTPOSITION] = true;
    model.cm.vert[1].P()[1] = .5f;
    manager.meshAttributesUpdated(false, changed);
    manager.manageBuffers();
    draw(manager);
    expectPixel(.5f, .25f, Color(255,0,255,255), "edge endpoint follows moved vertex");
    expectPixel(.5f, 0, Color(0,0,0,255), "old edge location cleared");

    // A second view can use vertex-interpolated wire colors without changing the first.
    auto vertexView = data;
    vertexView.set(RD::PR_WIREFRAME_EDGES, Att::ATT_EDGECOLOR, false);
    vertexView.set(RD::PR_WIREFRAME_EDGES, Att::ATT_VERTCOLOR, true);
    manager.setPerViewInfo(1, vertexView);
    manager.manageBuffers();
    draw(manager, 1);
    expectPixel(.5f, .25f, Color(128,128,0,255), "vertex-interpolated wire mode");
    draw(manager);
    expectPixel(.5f, .25f, Color(255,0,255,255), "independent edge-color viewer");
    changed.reset();
    changed[Att::ATT_VERTCOLOR] = true;
    model.cm.vert[1].C() = Color(0,0,255,255);
    manager.meshAttributesUpdated(false, changed);
    manager.manageBuffers();
    draw(manager, 1);
    expectPixel(.5f, .25f, Color(128,0,128,255), "vertex-color update reaches edge endpoints");
    draw(manager);
    expectPixel(.5f, .25f, Color(255,0,255,255), "vertex edit does not overwrite edge colors");

    auto constantView = vertexView;
    constantView.set(RD::PR_WIREFRAME_EDGES, Att::ATT_VERTCOLOR, false);
    auto constantOptions = options;
    constantOptions._perwire_mesh_color_enabled = true;
    model.cm.C() = Color(31,77,191,255);
    constantView.set(constantOptions);
    manager.setPerViewInfo(2, constantView);
    manager.manageBuffers();
    draw(manager, 2);
    expectPixel(.5f, .25f, model.cm.C(), "mesh-color wire mode");
    constantOptions._perwire_mesh_color_enabled = false;
    constantOptions._perwire_fixed_color_enabled = true;
    constantOptions._perwire_fixed_color = Color(180,90,45,255);
    constantView.set(constantOptions);
    manager.setPerViewInfo(2, constantView);
    manager.manageBuffers();
    draw(manager, 2);
    expectPixel(.5f, .25f, constantOptions._perwire_fixed_color, "user-defined wire mode");

    model.cm.edge[0].V(1) = &model.cm.vert[2];
    model.cm.edge[1].V(1) = &model.cm.vert[1];
    manager.meshAttributesUpdated(true, RD::RendAtts());
    manager.manageBuffers();
    draw(manager);
    expectPixel(0, .5f, Color(255,0,255,255), "same-count connectivity update");
    expectPixel(.5f, .25f, Color(0,255,255,255), "rewired cyan edge");

    vcg::tri::Allocator<CMeshO>::AddEdges(model.cm, 1);
    model.cm.edge.back().V(0) = &model.cm.vert[1];
    model.cm.edge.back().V(1) = &model.cm.vert[2];
    model.cm.edge.back().C() = Color(255,128,0,255);
    // Size changes must trigger allocation even without an attribute notification.
    manager.manageBuffers();
    draw(manager);
    expectPixel(.5f, .75f, Color(255,128,0,255), "edge-count growth");

    addFace(model.cm);
    model.updateDataMask(MeshModel::MM_FACECOLOR);
    model.cm.face[0].C() = Color(80,80,160,255);
    auto faceView = defaults(model);
    faceView.set(RD::PR_SOLID, Att::ATT_VERTCOLOR, false);
    faceView.set(RD::PR_SOLID, Att::ATT_FACECOLOR, true);
    faceView.set(RD::PR_SOLID, Att::ATT_FACENORMAL, false);
    faceView.set(RD::PR_SOLID, Att::ATT_VERTNORMAL, false);
    MLPerViewGLOptions faceOptions;
    faceView.get(faceOptions);
    faceOptions._persolid_noshading = true;
    faceOptions._perwire_wirewidth = 3;
    faceView.set(faceOptions);
    manager.setPerViewInfo(3, faceView);
    manager.meshAttributesUpdated(true, RD::RendAtts(true));
    manager.manageBuffers();
    draw(manager, 3);
    expectPixel(.3f, .45f, model.cm.face[0].C(), "solid face color in mixed mesh");
    expectPixel(.5f, .75f, Color(255,128,0,255), "explicit edge over solid face");
    draw(manager);
    expectPixel(0, 0, Color(255,0,0,255), "point colors with another face-color viewer");
    expectPixel(0, .5f, Color(255,0,255,255), "explicit stream independent of replicated face buffers");

    vcg::tri::Allocator<CMeshO>::DeleteEdge(model.cm, model.cm.edge.back());
    vcg::tri::Allocator<CMeshO>::CompactEveryVector(model.cm);
    manager.meshAttributesUpdated(true, RD::RendAtts());
    manager.manageBuffers();
    draw(manager);
    expectPixel(.5f, .75f, Color(0,0,0,255), "deleted edge disappears");
    manager.removeAllViews();
    manager.manageBuffers();
    require(memory.usedMemory() == 0, "release all GPU buffers");
}

static void runGeneratedWireAndTexture(bool gpu)
{
    MeshModel model(0, "", "");
    loadMesh(model);
    model.cm.edge.clear();
    model.cm.en = 0;
    model.clearDataMask(MeshModel::MM_EDGECOLOR);
    addFace(model.cm);
    model.updateDataMask(MeshModel::MM_POLYGONAL);
    auto data = defaults(model);
    data.set(RD::PR_SOLID, false);
    data.set(RD::PR_WIREFRAME_EDGES, Att::ATT_VERTNORMAL, false);
    MLPerViewGLOptions options;
    data.get(options);
    options._perwire_noshading = true;
    options._perwire_wirewidth = 3;
    data.set(options);
    vcg::NotThreadSafeMemoryInfo memory(gpu ? 64 * 1024 * 1024 : 0);
    Manager manager(model.cm, memory, 10000);
    manager.setPerViewInfo(0, data);
    manager.manageBuffers();
    draw(manager);
    expectPixel(.5f, 0, Color(128,128,0,255), "generated polygonal boundary retains vertex colors");

    model.clearDataMask(MeshModel::MM_VERTCOLOR | MeshModel::MM_POLYGONAL);
    model.updateDataMask(MeshModel::MM_WEDGTEXCOORD);
    model.cm.textures.push_back("test.png");
    for (int i = 0; i < 3; ++i) {
        model.cm.face[0].WT(i).P() = vcg::Point2f(float(i == 1), float(i == 2));
        model.cm.face[0].WT(i).N() = 0;
    }
    data = defaults(model);
    data.set(RD::PR_SOLID, Att::ATT_FACENORMAL, false);
    data.set(RD::PR_SOLID, Att::ATT_VERTNORMAL, false);
    data.get(options);
    options._persolid_noshading = true;
    options._persolid_fixed_color = Color(255,255,255,255);
    data.set(options);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    const unsigned char pixel[] = {20,140,220,255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    manager.setPerViewInfo(0, data);
    manager.meshAttributesUpdated(true, RD::RendAtts(true));
    manager.manageBuffers();
    draw(manager, 0, {texture});
    expectPixel(.25f, .25f, Color(20,140,220,255), "existing textured-face rendering");
    glDeleteTextures(1, &texture);
    manager.removeAllViews();
    manager.manageBuffers();
    require(memory.usedMemory() == 0, "release legacy rendering buffers");
}

int main(int argc, char** argv)
try {
    QGuiApplication app(argc, argv);
    QSurfaceFormat format;
    format.setVersion(2, 1);
    format.setProfile(QSurfaceFormat::NoProfile);
    format.setDepthBufferSize(24);
    QOpenGLContext context;
    context.setFormat(format);
    if (!context.create()) { std::cerr << "No desktop OpenGL context\n"; return 77; }
    QOffscreenSurface surface;
    surface.setFormat(context.format());
    surface.create();
    if (!context.makeCurrent(&surface)) { std::cerr << "Cannot make OpenGL context current\n"; return 77; }
    if (!GLExtensionsManager::initializeGLextensions_notThrowing()) return 77;
    while (glGetError() != GL_NO_ERROR) {}
    QOpenGLFramebufferObject framebuffer(imageSize, imageSize, QOpenGLFramebufferObject::CombinedDepthStencil);
    require(framebuffer.isValid() && framebuffer.bind(), "offscreen framebuffer");
    runRenderer(true, framebuffer);
    runRenderer(false, framebuffer);
    runGeneratedWireAndTexture(true);
    runGeneratedWireAndTexture(false);
    std::cout << "GPU and fallback pixel regressions passed\n";
    return 0;
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
