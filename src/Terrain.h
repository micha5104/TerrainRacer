#ifndef TERRAIN_H
#define TERRAIN_H

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include <GL/glut.h>
#include <glu.h>
#include <vector>

class Terrain
{
public:

    Terrain( float heightScale = 500.0f, float blockScale = 2.0f );
    virtual ~Terrain();

    void terminate();
    bool loadHeightmap(const std::string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height);
    bool loadTexture(const std::string& filename, unsigned int textureStage = 0);

    // Get the height of the terrain at a position in world space
    float getHeightAt(const glm::vec3& position);

    void render();
    // In debug builds, the terrain normals will be rendered.
    void debugRender();

protected:
    void generateIndexBuffer();
    void generateNormals();

    // Generates the vertex buffer objects from the
    // position, normal, texture, and color buffers
    void generateVertexBuffers();

    void renderNormals();

private:
    typedef std::vector<glm::vec3>  PositionBuffer;
    typedef std::vector<glm::vec4>  ColorBuffer;
    typedef std::vector<glm::vec3>  NormalBuffer;
    typedef std::vector<glm::vec2>  TexCoordBuffer;
    typedef std::vector<GLuint>     IndexBuffer;

    PositionBuffer mPositionBuffer;
    ColorBuffer mColorBuffer;
    NormalBuffer mNormalBuffer;
    TexCoordBuffer mTex0Buffer;
    IndexBuffer mIndexBuffer;

    // ID's for the VBO's
    GLuint mGLVertexBuffer;
    GLuint mGLNormalBuffer;
    GLuint mGLColorBuffer;
    GLuint mGLTex0Buffer;
    GLuint mGLTex1Buffer;
    GLuint mGLTex2Buffer;
    GLuint mGLIndexBuffer;

    static const unsigned int mNumTextures = 3;
    GLuint mGLTextures[mNumTextures];

    glm::mat4x4 mLocalToWorldMatrix;
    glm::mat4x4 mInverseLocalToWorldMatrix;

    // The dimensions of the heightmap texture
    glm::uvec2 mHeightmapDimensions;

    // The height-map value will be multiplied by this value
    // before it is assigned to the vertex's Y-coordinate
    float mHeightScale;

    // The vertex's X and Z coordinates will be multiplied by this
    // for each step when building the terrain
    float mBlockScale;
};

#endif
