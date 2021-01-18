/*
 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef _VITELOTTE_VG_MESH_RENDERER_
#define _VITELOTTE_VG_MESH_RENDERER_


#include <Eigen/Dense>
#include <Eigen/StdVector>

#include "../../common/gl_utils/shader.h"
#include "../../common/gl_utils/color.h"

#include "../Core/femUtils.h"
#include "vgMeshRendererShaders.hpp"


namespace Vitelotte {


using PatateCommon::ColorSpace;

/**
 * \brief The VGMeshRendererResources encapsulate resources required by
 * VGMeshRenderer for sharing.
 */
class VGMeshRendererResources {
public:
    struct SolidUniforms
    {
        GLint viewMatrixLoc;
        GLint normalMatrixLoc;
        GLint nodesLoc;
        GLint baseNodeIndexLoc;
        GLint singularTrianglesLoc;
        GLfloat smoothnessLoc;
        GLint enableShadingLoc;
        GLint meshColorSpaceLoc;
        GLint screenColorSpaceLoc;
    };

    struct WireframeUniforms
    {
        GLint viewMatrixLoc;
        GLint viewportSizeLoc;
        GLfloat smoothnessLoc;
        GLint lineWidthLoc;
        GLint wireframeColorLoc;
    };

    enum
    {
        VG_MESH_POSITION_ATTR_LOC,
        VG_MESH_NORMAL_ATTR_LOC
    };

public:
    inline VGMeshRendererResources();
    inline ~VGMeshRendererResources();

    inline bool initialize();
    inline void releaseGLResources();

    // private
    inline PatateCommon::Shader& solidLinearShader()    { return m_solidLinearShader; }
    inline PatateCommon::Shader& solidQuadraticShader() { return m_solidQuadraticShader; }
    inline PatateCommon::Shader& wireframeShader()      { return m_wireframeShader; }

    inline const SolidUniforms&     solidLinearUniforms()    const { return m_solidLinearUniforms; }
    inline const SolidUniforms&     solidQuadraticUniforms() const { return m_solidQuadraticUniforms; }
    inline const WireframeUniforms& wireframeUniforms()      const { return m_wireframeUniforms; }

protected:
    inline bool initSolidShader(PatateCommon::Shader& shader, SolidUniforms& unif,
                         const char *fragCode);
    inline bool initWireframeShader();

protected:
    bool m_initialized;
    PatateCommon::Shader m_solidLinearShader;
    PatateCommon::Shader m_solidQuadraticShader;
    PatateCommon::Shader m_wireframeShader;
    SolidUniforms     m_solidLinearUniforms;
    SolidUniforms     m_solidQuadraticUniforms;
    WireframeUniforms m_wireframeUniforms;
};


template < typename Vector >
struct DefaultPosProj {
    inline Eigen::Vector4f operator()(const Vector& position) const;
};


template < typename Value >
struct DefaultValueProj {
    inline Eigen::Vector4f operator()(const Value& value) const;
};


/**
 * \brief An OpenGL 3 renderer to display VGMesh objects.
 */
template < class _Mesh,
           typename _PosProj   = DefaultPosProj  <typename _Mesh::Vector>,
           typename _ValueProj = DefaultValueProj<typename _Mesh::Value> >
class VGMeshRenderer
{
public:
    typedef _Mesh      Mesh;
    typedef _PosProj   PosProj;
    typedef _ValueProj ValueProj;

    typedef typename Mesh::Node Node;
    typedef typename Mesh::Vector Vector;
    typedef typename Mesh::Value Value;

    typedef typename Mesh::Vertex Vertex;
    typedef typename Mesh::Face Face;

    typedef VGMeshRendererResources Resources;

    typedef Eigen::Vector3f Vector3;
    typedef Eigen::Vector4f Vector4;

    enum
    {
        NORMAL_TRIANGLES = 0x01,
        SINGULAR_TRIANGLES = 0x02,

        ALL_TRIANGLES = NORMAL_TRIANGLES | SINGULAR_TRIANGLES
    };

    enum
    {
        NODES_TEXTURE_UNIT
    };

public:
    VGMeshRenderer(Resources* resources = 0,
                   const PosProj&   posProj   = PosProj(),
                   const ValueProj& valueProj = ValueProj());
    ~VGMeshRenderer();

    const PosProj&   positionProjection() const;
          PosProj&   positionProjection();
    const ValueProj& valueProjection()    const;
          ValueProj& valueProjection();

    ColorSpace screenColorSpace() const;
    void setScreenColorSpace(ColorSpace cs) const;

    void setResources(Resources* resources);
    void releaseGLResources();
    void updateBuffers(const Mesh& mesh);

    void drawGeometry(unsigned geomFlags);
    void render(const Eigen::Matrix4f& viewMatrix, float smoothness = 8);
    void renderWireframe(const Eigen::Matrix4f& viewMatrix,
                         const Eigen::Vector2f& viewportSize, float lineWidth = 1,
                         const Eigen::Vector4f& color = Eigen::Vector4f(0, 0, 0, 1),
                         float smoothness = 8);

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
    typedef std::vector<unsigned> IndicesVector;
    typedef std::vector<Vector4, Eigen::aligned_allocator<Vector4> > Vector4Vector;

    struct GlVertex {
        Vector4 position;
        Vector3 normal;
    };
    typedef std::vector<GlVertex, Eigen::aligned_allocator<GlVertex> > VertexVector;

protected:
    Vector4 position(const Mesh& mesh, Vertex vx) const;
    Vector4 color(const Mesh& mesh, Node node) const;

    void initResources();

    template < typename Vec >
    void createAndUploadBuffer(GLuint& glId, GLenum type,
                               const Vec& data,
                               GLenum usage = GL_DYNAMIC_DRAW);

private:
    bool m_useVao;
    bool m_convertSrgbToLinear;
    bool m_ownResources;

    PosProj    m_positionProjection;
    ValueProj  m_valueProjection;
    Vector4    m_invalidNodeColor;
    ColorSpace m_meshColorSpace;
    ColorSpace m_screenColorSpace;

    Resources* m_resources;

    GLuint m_verticesBuffer;
    GLuint m_indicesBuffer;
    GLuint m_nodesBuffer;
    GLuint m_nodesTexture;

    GLuint m_vao;

    VertexVector  m_vertices;
    IndicesVector m_indices;
    Vector4Vector m_nodes;

    bool     m_quadratic;
    bool     m_3d;
    unsigned m_nTriangles;
    unsigned m_nSingulars;
};


}

#include "vgMeshRenderer.hpp"


#endif
