/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "vgMeshRenderer.h"


namespace Vitelotte
{


VGMeshRendererResources::VGMeshRendererResources()
    : m_initialized(false)
{
}


VGMeshRendererResources::~VGMeshRendererResources()
{
    if(m_initialized) releaseGLResources();
}


bool VGMeshRendererResources::initialize()
{
    bool ok = true;
    ok &= initSolidShader(m_solidLinearShader, m_solidLinearUniforms,
                          VGMeshRendererShaders::frag_linear_glsl);
    ok &= initSolidShader(m_solidQuadraticShader, m_solidQuadraticUniforms,
                          VGMeshRendererShaders::frag_quadratic_glsl);
    ok &= initWireframeShader();
    m_initialized = ok;
    return ok;
}


void VGMeshRendererResources::releaseGLResources()
{
    m_initialized = false;

    m_solidLinearShader.destroy();
    m_solidQuadraticShader.destroy();
    m_wireframeShader.destroy();
}


bool VGMeshRendererResources::initSolidShader(
        PatateCommon::Shader& shader, SolidUniforms& unif, const char* fragCode)
{
    shader.create();

    bool ok = true;
    ok &= shader.addShader(GL_VERTEX_SHADER,
                           VGMeshRendererShaders::vert_common_glsl);
    ok &= shader.addShader(GL_TESS_CONTROL_SHADER,
                           VGMeshRendererShaders::ctrl_common_glsl);
    ok &= shader.addShader(GL_TESS_EVALUATION_SHADER,
                           VGMeshRendererShaders::eval_common_glsl);
    ok &= shader.addShader(GL_FRAGMENT_SHADER,
                           VGMeshRendererShaders::frag_common_glsl);
    ok &= shader.addShader(GL_FRAGMENT_SHADER, fragCode);

    shader.bindAttributeLocation("vx_position", VG_MESH_POSITION_ATTR_LOC);
    shader.bindAttributeLocation("vx_normal",   VG_MESH_NORMAL_ATTR_LOC);
    ok &= shader.finalize();

    if(!ok)
        return false;

    unif.viewMatrixLoc        = shader.getUniformLocation("viewMatrix");
    unif.normalMatrixLoc      = shader.getUniformLocation("normalMatrix");
    unif.nodesLoc             = shader.getUniformLocation("nodes");
    unif.baseNodeIndexLoc     = shader.getUniformLocation("baseNodeIndex");
    unif.singularTrianglesLoc = shader.getUniformLocation("singularTriangles");
    unif.smoothnessLoc        = shader.getUniformLocation("smoothness");
    unif.enableShadingLoc     = shader.getUniformLocation("enableShading");
    unif.meshColorSpaceLoc    = shader.getUniformLocation("meshColorSpace");
    unif.screenColorSpaceLoc  = shader.getUniformLocation("screenColorSpace");

    return true;
}


bool VGMeshRendererResources::initWireframeShader()
{
    PatateCommon::Shader& shader = m_wireframeShader;
    WireframeUniforms& unif = m_wireframeUniforms;
    shader.create();

    bool ok = true;
    ok &= shader.addShader(GL_VERTEX_SHADER,
                           VGMeshRendererShaders::vert_common_glsl);
    ok &= shader.addShader(GL_TESS_CONTROL_SHADER,
                           VGMeshRendererShaders::ctrl_common_glsl);
    ok &= shader.addShader(GL_TESS_EVALUATION_SHADER,
                           VGMeshRendererShaders::eval_common_glsl);
    // Uncomment this to display tesselated wireframe.
//    ok &= shader.addShader(GL_GEOMETRY_SHADER,
//                           VGMeshRendererShaders::geom_common_glsl);
    ok &= shader.addShader(GL_FRAGMENT_SHADER,
                           VGMeshRendererShaders::frag_common_glsl);
    ok &= shader.addShader(GL_FRAGMENT_SHADER,
                           VGMeshRendererShaders::frag_wireframe_glsl);

    shader.bindAttributeLocation("vx_position", VG_MESH_POSITION_ATTR_LOC);
    ok &= shader.finalize();

    if(!ok)
        return false;

    unif.viewMatrixLoc        = shader.getUniformLocation("viewMatrix");
    unif.viewportSizeLoc      = shader.getUniformLocation("viewportSize");
    unif.smoothnessLoc        = shader.getUniformLocation("smoothness");
    unif.lineWidthLoc         = shader.getUniformLocation("lineWidth");
    unif.wireframeColorLoc    = shader.getUniformLocation("wireframeColor");

    return true;
}


template < typename Vector >
Eigen::Vector4f DefaultPosProj<Vector>::operator()(const Vector& position) const {
    Eigen::Vector4f p = Eigen::Vector4f::Unit(3);
    unsigned size = std::min(unsigned(position.size()), 3u);
    p.head(size) = position.head(size);
    return p;
}


template < typename Value >
Eigen::Vector4f DefaultValueProj<Value>::operator()(const Value& value) const {
    typedef Eigen::Vector3f Vector3;
    Eigen::Vector4f c = Eigen::Vector4f::Unit(3);
    switch(value.size())
    {
    case 2:  c(3)        = value(1);                     // fall-through
    case 1:  c.head<3>() = Vector3::Constant(value(0));  break;
    case 3:  c.head<3>() = value.template cast<float>(); break;
    default: c           = value.template head<4>()
                                .template cast<float>(); break;
    }
    return c;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::VGMeshRenderer(
        Resources* resources, const PosProj& posProj, const ValueProj& valueProj) :
    m_useVao(true),
    m_ownResources(false),

    m_positionProjection(posProj),
    m_valueProjection(valueProj),
    m_invalidNodeColor(Vector4::Unit(3)),
    m_meshColorSpace(PatateCommon::COLOR_NONE),
    m_screenColorSpace(PatateCommon::COLOR_SRGB),

    m_resources(resources),

    m_verticesBuffer(0),
    m_indicesBuffer(0),
    m_nodesBuffer(0),
    m_nodesTexture(0),

    m_vao(0),

    m_vertices(),
    m_indices(),
    m_nodes(),

    m_quadratic(false),
    m_nTriangles(0),
    m_nSingulars(0)
{}


template < class _Mesh, typename _PosProj, typename _ValueProj >
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::~VGMeshRenderer()
{
    releaseGLResources();
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
const typename VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::PosProj&
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::positionProjection() const
{
    return m_positionProjection;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
typename VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::PosProj&
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::positionProjection()
{
    return m_positionProjection;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
const typename VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::ValueProj&
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::valueProjection() const
{
    return m_valueProjection;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
typename VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::ValueProj&
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::valueProjection()
{
    return m_valueProjection;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
ColorSpace VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::screenColorSpace() const
{
    return m_screenColorSpace;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::setScreenColorSpace(ColorSpace cs) const
{
    m_screenColorSpace = cs;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::setResources(
        Resources* resources)
{
    assert(!m_resources);
    m_resources = resources;
    m_ownResources = false;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::releaseGLResources()
{
    if(m_ownResources)
    {
        delete m_resources;
        m_resources = 0;
    }

    if(m_verticesBuffer)
    {
        glDeleteBuffers(1, &m_verticesBuffer);
        m_verticesBuffer = 0;
    }
    if(m_indicesBuffer)
    {
        glDeleteBuffers(1, &m_indicesBuffer);
        m_indicesBuffer = 0;
    }
    if(m_nodesBuffer)
    {
        glDeleteBuffers(1, &m_nodesBuffer);
        m_nodesBuffer = 0;
    }

    if(m_nodesTexture)
    {
        glDeleteTextures(1, &m_nodesTexture);
        m_nodesTexture = 0;
    }

    if(m_vao)
    {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::updateBuffers(const Mesh& mesh)
{
    PATATE_ASSERT_NO_GL_ERROR();

    m_quadratic = mesh.hasEdgeValue();
    m_3d        = mesh.nDims() == 3;
    m_meshColorSpace = mesh.colorSpace();

    int nodePerTriangle = m_quadratic? 6: 3;

    // Compute number of singular and normal triangles
    m_nSingulars = mesh.nSingularFaces();
    m_nTriangles = mesh.nFaces() - m_nSingulars;

    if(m_nSingulars + m_nTriangles == 0) {
        return;
    }

    // Reserve buffers
    m_vertices.resize(mesh.verticesSize() + 2 * mesh.edgesSize());
    m_indices.resize((m_nTriangles * 3 + m_nSingulars * 3) * 3);
    m_nodes.resize(m_nTriangles * nodePerTriangle +
                   m_nSingulars * (nodePerTriangle + 1));

    // Push vertices positions
    for(typename Mesh::VertexIterator vit = mesh.verticesBegin();
        vit != mesh.verticesEnd(); ++vit)
    {
        m_vertices[(*vit).idx()].position = position(mesh, *vit);
        m_vertices[(*vit).idx()].normal.setZero();
    }

    // Subdivide curved edges
    unsigned nVertices = mesh.verticesSize();
    for(typename Mesh::EdgeIterator eit = mesh.edgesBegin();
        eit != mesh.edgesEnd(); ++eit)
    {
        unsigned i1 = nVertices + 2 * (*eit).idx();
        unsigned i2 = i1 + 1;

        typename Mesh::Halfedge h = mesh.halfedge(*eit, 0);
        bool orient = mesh.halfedgeOrientation(h);
        const Vector& v0 = mesh.position(orient? mesh.toVertex(h): mesh.fromVertex(h));
        const Vector& v3 = mesh.position(orient? mesh.fromVertex(h): mesh.toVertex(h));
        if(!mesh.isCurved(*eit) || mesh.edgeCurve(*eit).type() == BEZIER_LINEAR) {
            m_vertices[i1].position = m_positionProjection(
                        (2 * v0 + v3) / 3);
            m_vertices[i2].position = m_positionProjection(
                        (v0 + 2 * v3) / 3);
        } else if(mesh.edgeCurve(*eit).type() == BEZIER_QUADRATIC) {
            m_vertices[i1].position = m_positionProjection(
                        (v0 + 2 * mesh.edgeCurve(*eit).point(1)) / 3);
            m_vertices[i2].position = m_positionProjection(
                        (2 * mesh.edgeCurve(*eit).point(1) + v3) / 3);
        } else {
            m_vertices[i1].position = m_positionProjection(
                        mesh.edgeCurve(*eit).point(1));
            m_vertices[i2].position = m_positionProjection(
                        mesh.edgeCurve(*eit).point(2));
        }
        // Normals are interpolated later.
    }

    // Push faces indices and nodes
    unsigned triIndex = 0;
    unsigned singIndex = m_nTriangles * 3;
    unsigned triNodeIndex = 0;
    unsigned singNodeIndex = m_nTriangles * nodePerTriangle;
    for(typename Mesh::FaceIterator fit = mesh.facesBegin();
        fit != mesh.facesEnd(); ++fit)
    {
        // Ensure we work with triangles
        assert(mesh.valence(*fit) == 3);
        typename Mesh::Halfedge h = mesh.halfedge(*fit);

        bool isSingular = mesh.nSingulars(*fit);
        if(isSingular)
        {
            // The first vertex must be the singular one
            while(!mesh.isSingular(h)) { h = mesh.nextHalfedge(h); }
        }

        unsigned& index = isSingular? singIndex: triIndex;
        unsigned& nodeIndex = isSingular? singNodeIndex: triNodeIndex;
        Vector3 pts[3];
        // Push vertices nodes
        for(int ei = 0; ei < 3; ++ei)
        {
            if(m_3d) pts[ei] = mesh.position(mesh.toVertex(h));
            m_indices[3 * (index + ei)] = mesh.toVertex(h).idx();
            h = mesh.nextHalfedge(h);
            m_indices[3 * (index + ei) + 1] = nVertices + 2 * mesh.edge(h).idx()
                    + mesh.halfedgeOrientation(h);
            m_indices[3 * (index + ei) + 2] = nVertices + 2 * mesh.edge(h).idx()
                    + 1 - mesh.halfedgeOrientation(h);
            m_nodes[nodeIndex + ei] = mesh.hasToVertexValue()?
                        color(mesh, mesh.fromVertexValueNode(h)):
                        Eigen::Vector4f(.8, .8, .8, 1.);
        }
        // Singular node is the last one
        if(isSingular)
            m_nodes[nodeIndex + nodePerTriangle] = color(mesh, mesh.toVertexValueNode(h));

        if(m_quadratic)
        {
            // Push edge nodes
            h = mesh.prevHalfedge(h);
            for(int ei = 0; ei < 3; ++ei)
            {
                m_nodes[nodeIndex + 3 + ei] = color(mesh, mesh.edgeValueNode(h));
                h = mesh.nextHalfedge(h);
            }
        }

        if(m_3d) {
            // Compute normals
            Vector3 fn = (pts[1] - pts[0]).cross(pts[2] - pts[0]);
            for(int ei = 0; ei < 3; ++ei) {
                m_vertices[mesh.toVertex(h).idx()].normal += fn;
                h = mesh.nextHalfedge(h);
            }
        }

        index += 3;
        nodeIndex += nodePerTriangle + isSingular;
    }
//    assert(triIndex == m_nTriangles * 3 && singIndex == m_indices.size());
//    assert(triNodeIndex == m_nTriangles * nodePerTriangle && singNodeIndex == m_nodes.size());

    if(m_3d) {
        // Normalize normals
        for(typename Mesh::VertexIterator vit = mesh.verticesBegin();
            vit != mesh.verticesEnd(); ++vit) {
            m_vertices[(*vit).idx()].normal.normalize();
        }
    }

    // Create and upload buffers
    createAndUploadBuffer(m_verticesBuffer, GL_ARRAY_BUFFER,
                          m_vertices);

    createAndUploadBuffer(m_indicesBuffer, GL_ELEMENT_ARRAY_BUFFER,
                          m_indices);

    createAndUploadBuffer(m_nodesBuffer, GL_ARRAY_BUFFER,
                          m_nodes);

    // Create and setup texture buffer
    if(!m_nodesTexture)
    {
        glGenTextures(1, &m_nodesTexture);
    }
    glBindTexture(GL_TEXTURE_BUFFER, m_nodesTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_nodesBuffer);

    PATATE_ASSERT_NO_GL_ERROR();
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::drawGeometry(unsigned geomFlags)
{
    assert((geomFlags & ~ALL_TRIANGLES) == 0);
    PATATE_ASSERT_NO_GL_ERROR();

    unsigned nPrimitives = 0;
    unsigned firstPrimitive = 0;
    if(geomFlags & SINGULAR_TRIANGLES)
    {
        nPrimitives += m_nSingulars;
        firstPrimitive = m_nTriangles;
    }
    if(geomFlags & NORMAL_TRIANGLES)
    {
        nPrimitives += m_nTriangles;
        firstPrimitive = 0;
    }

    if(nPrimitives == 0)
    {
        return;
    }

    bool setupBuffers = !m_useVao || !m_vao;
    if(m_useVao)
    {
        if(!m_vao)
            glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
    }

    if(setupBuffers)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_verticesBuffer);

        glEnableVertexAttribArray(Resources::VG_MESH_POSITION_ATTR_LOC);
        glVertexAttribPointer(Resources::VG_MESH_POSITION_ATTR_LOC,
                              4, GL_FLOAT,
                              false, sizeof(GlVertex),
                              PATATE_FIELD_OFFSET(GlVertex, position));

        glEnableVertexAttribArray(Resources::VG_MESH_NORMAL_ATTR_LOC);

        glVertexAttribPointer(Resources::VG_MESH_NORMAL_ATTR_LOC,
                              3, GL_FLOAT,
                              false, sizeof(GlVertex),
                              PATATE_FIELD_OFFSET(GlVertex, normal));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
    }

    glPatchParameteri(GL_PATCH_VERTICES, 9);
    glDrawElements(GL_PATCHES, nPrimitives * 9, GL_UNSIGNED_INT,
                  (const void*)(firstPrimitive * 9 * sizeof(unsigned)));

    if(m_useVao) {
        glBindVertexArray(0);
    }
    else {
        glDisableVertexAttribArray(Resources::VG_MESH_POSITION_ATTR_LOC);
        glDisableVertexAttribArray(Resources::VG_MESH_NORMAL_ATTR_LOC);
    }

    PATATE_ASSERT_NO_GL_ERROR();
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::render(
        const Eigen::Matrix4f& viewMatrix, float smoothness)
{
    PATATE_ASSERT_NO_GL_ERROR();

    if(!m_resources) initResources();

    PatateCommon::Shader& shader = m_quadratic?
                m_resources->solidQuadraticShader():
                m_resources->solidLinearShader();

    const Resources::SolidUniforms& unif = m_quadratic?
                m_resources->solidQuadraticUniforms():
                m_resources->solidLinearUniforms();

    shader.use();

    Eigen::Matrix3f normalMatrix = viewMatrix.topLeftCorner<3, 3>();
    glUniformMatrix4fv(unif.viewMatrixLoc, 1, false, viewMatrix.data());
    glUniformMatrix3fv(unif.normalMatrixLoc, 1, false, normalMatrix.data());
    glUniform1i(unif.nodesLoc, NODES_TEXTURE_UNIT);
    glUniform1f(unif.smoothnessLoc, smoothness);
    glUniform1i(unif.enableShadingLoc, m_3d);
    glUniform1i(unif.meshColorSpaceLoc, m_meshColorSpace);
    glUniform1i(unif.screenColorSpaceLoc, m_screenColorSpace);

    glActiveTexture(GL_TEXTURE0 + NODES_TEXTURE_UNIT);
    glBindTexture(GL_TEXTURE_BUFFER, m_nodesTexture);

    // Normal triangles
    glUniform1i(unif.baseNodeIndexLoc, 0);
    glUniform1i(unif.singularTrianglesLoc, false);

    drawGeometry(NORMAL_TRIANGLES);

    // Singular triangles
    int nodePerTriangle = m_quadratic? 6: 3;
    glUniform1i(unif.baseNodeIndexLoc, m_nTriangles * nodePerTriangle);
    glUniform1i(unif.singularTrianglesLoc, true);

    drawGeometry(SINGULAR_TRIANGLES);

    PATATE_ASSERT_NO_GL_ERROR();
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::renderWireframe(
        const Eigen::Matrix4f& viewMatrix, const Eigen::Vector2f& viewportSize,
        float lineWidth, const Eigen::Vector4f& color, float smoothness)
{
    PATATE_ASSERT_NO_GL_ERROR();

    if(!m_resources) initResources();

    m_resources->wireframeShader().use();
    const Resources::WireframeUniforms& unif = m_resources->wireframeUniforms();

    glUniformMatrix4fv(unif.viewMatrixLoc, 1, false, viewMatrix.data());
    glUniform2fv(unif.viewportSizeLoc, 1, viewportSize.data());
    glUniform1f(unif.smoothnessLoc, smoothness);
    glUniform1f(unif.lineWidthLoc, lineWidth);
    glUniform4fv(unif.wireframeColorLoc, 1, color.data());

    drawGeometry(ALL_TRIANGLES);

    PATATE_ASSERT_NO_GL_ERROR();
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
inline typename VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::Vector4
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::position(const Mesh& mesh, Vertex vx) const
{
    return m_positionProjection(mesh.position(vx));
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
inline typename VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::Vector4
VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::color(const Mesh& mesh, Node node) const
{
    Vector4 c = (mesh.isValid(node) && mesh.isConstraint(node))?
                m_valueProjection(mesh.value(node)):
                m_invalidNodeColor;
    return c;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::initResources()
{
    assert(!m_resources);

    m_resources = new Resources();
    bool ok = m_resources->initialize();
    if(!ok) std::abort();
    m_ownResources = true;
}


template < class _Mesh, typename _PosProj, typename _ValueProj >
template < typename Vec >
void VGMeshRenderer<_Mesh, _PosProj, _ValueProj>::createAndUploadBuffer(
        GLuint& glId, GLenum type, const Vec& data, GLenum usage)
{
    if(!glId)
    {
        glGenBuffers(1, &glId);
    }
    glBindBuffer(type, glId);
    glBufferData(type, data.size() * sizeof(typename Vec::value_type),
                 &(data[0]), usage);
}

}
