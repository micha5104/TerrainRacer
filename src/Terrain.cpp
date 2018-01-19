#include "Terrain.h"
#include "utils/Jpeg.h"

#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <istream>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

// Enable multitexture blending across the terrain
#ifndef ENABLE_MULTITEXTURE
#define ENABLE_MULTITEXTURE 1
#endif

// Enable the blend constants based on the slope of the terrain
#ifndef ENABLE_SLOPE_BASED_BLEND
#define ENABLE_SLOPE_BASED_BLEND 1
#endif

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

inline float getPercentage(float value, const float min, const float max)
{
    value = glm::clamp( value, min, max );
    return ( value - min ) / ( max - min );
}

// Get's the size of the file in bytes.
inline int getFileLength(std::istream& file)
{
    int pos = file.tellg();
    file.seekg(0, std::ios::end );
    int length = file.tellg();
    // Restore the position of the get pointer
    file.seekg(pos);

    return length;
}

inline void deleteVertexBuffer(GLuint& vboID)
{
    if ( vboID != 0 )
    {
        glDeleteBuffersARB( 1, &vboID );
        vboID = 0;
    }
}

inline void createVertexBuffer(GLuint& vboID)
{
    // Make sure we don't loose the reference to the previous VBO if there is one
    deleteVertexBuffer(vboID);
    glGenBuffersARB( 1, &vboID );
}

inline void deleteTexture(GLuint& texID)
{
    if ( texID != 0 )
    {
        glDeleteTextures( 1, &texID );
        texID = 0;
    }
}

inline void createTexture(GLuint& texID)
{
    deleteTexture(texID);
    glGenTextures(1, &texID );
}

// Convert data from the char buffer to a floating point value between 0..1
// depending on the storage value of the original data
// NOTE: This only works with (LSB,MSB) data storage.
inline float getHeightValue(const unsigned char* data, unsigned char numBytes)
{
    switch ( numBytes )
    {
    case 1:
        {
            return (unsigned char)(data[0]) / (float)0xff;
        }
        break;
    case 2:
        {
            return (unsigned short)(data[1] << 8 | data[0] ) / (float)0xffff;
        }
        break;
    case 4:
        {
            return (unsigned int)(data[3] << 24 | data[2] << 16 | data[1] << 8 | data[0] ) / (float)0xffffffff;
        }
        break;
    default:
        {
            assert(false);  // Height field with non standard pixle sizes
        }
        break;
    }

    return 0.0f;
}

Terrain::Terrain(float heightScale /* = 500.0f */, float blockScale /* = 2.0f */) :
                mGLVertexBuffer(0),
                                mGLNormalBuffer(0),
                                mGLColorBuffer(0),
                                mGLTex0Buffer(0),
                                mGLTex1Buffer(0),
                                mGLTex2Buffer(0),
                                mGLIndexBuffer(0),
                                mLocalToWorldMatrix(1),
                                mInverseLocalToWorldMatrix(1),
                                mHeightmapDimensions(0, 0),
                                mHeightScale(heightScale),
                                mBlockScale(blockScale)
{
    memset(mGLTextures, 0, sizeof(mGLTextures));
}

Terrain::~Terrain()
{
    terminate();
}

void Terrain::terminate()
{
    deleteVertexBuffer(mGLVertexBuffer);
    deleteVertexBuffer(mGLNormalBuffer);
    deleteVertexBuffer(mGLVertexBuffer);
    deleteVertexBuffer(mGLTex0Buffer);
    deleteVertexBuffer(mGLTex1Buffer);
    deleteVertexBuffer(mGLTex2Buffer);
    deleteVertexBuffer(mGLIndexBuffer);

    for (unsigned int i = 0; i < mNumTextures; ++i)
    {
        deleteTexture(mGLTextures[i]);
    }
}

bool Terrain::loadTexture(const std::string& filename, unsigned int textureStage /*= 0*/)
{
    assert(textureStage < mNumTextures);
    deleteTexture(mGLTextures[textureStage]);

    Jpeg jpeg;
    int ret = jpeg.load(filename);
    if (ret != 0)
    {
        return false;
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLint internalFormat;
    if (jpeg.channels() == 3)
    {
        internalFormat = GL_RGB;
    }
    else
    {
        internalFormat = GL_RGBA;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, jpeg.width(), jpeg.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, jpeg.data());
    mGLTextures[textureStage] = texture;
    glBindTexture(GL_TEXTURE_2D, 0);

    if (mGLTextures[textureStage] != 0)
    {
        glBindTexture( GL_TEXTURE_2D, mGLTextures[textureStage]);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glBindTexture( GL_TEXTURE_2D, 0 );
    }

    return (mGLTextures[textureStage] != 0);
}

bool Terrain::loadHeightmap(const std::string& filename, unsigned char bitsPerPixel, unsigned int width, unsigned int height)
{
    struct stat buffer;
    if (stat(filename.c_str(), &buffer) != 0)
    {
        std::cerr << "Could not find file: " << filename << std::endl;
        return false;
    }

    std::ifstream ifs;
    ifs.open(filename, std::ifstream::binary);
    if ( ifs.fail() )
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    const unsigned int bytesPerPixel = bitsPerPixel / 8;
    const unsigned int expectedFileSize = ( bytesPerPixel * width * height );
    const unsigned int fileSize = getFileLength(ifs);

    if ( expectedFileSize != fileSize )
    {
        std::cerr << "Expected file size [" << expectedFileSize << " bytes] differs from actual file size [" << fileSize << " bytes]" << std::endl;
        return false;
    }

    unsigned char* heightMap = new unsigned char[fileSize];
    ifs.read( (char*)heightMap, fileSize);
    if ( ifs.fail() )
    {
        std::cerr << "An error occurred while reading from the height map file: " << filename << std::endl;
        ifs.close();
        delete [] heightMap;
        return false;
    }
    ifs.close();

    unsigned int numVerts = width * height;
    mPositionBuffer.resize(numVerts);
    mColorBuffer.resize(numVerts);
    mNormalBuffer.resize(numVerts);
    mTex0Buffer.resize(numVerts);

    mHeightmapDimensions = glm::uvec2(width, height);

    // Size of the terrain in world units
    float terrainWidth = (width - 1) * mBlockScale;
    float terrainHeight = (height - 1) * mBlockScale;

    float halfTerrainWidth = terrainWidth * 0.5f;
    float halfTerrainHeight = terrainHeight * 0.5f;

    for ( unsigned int j = 0; j < height; ++j )
    {
        for ( unsigned i = 0; i < width; ++i )
        {
            unsigned int index = ( j * width ) + i;
            assert( index * bytesPerPixel < fileSize );
            float heightValue = getHeightValue(&heightMap[index * bytesPerPixel], bytesPerPixel);

            float S = ( i / (float)(width - 1) );
            float T = ( j / (float)(height - 1) );

            float X = ( S * terrainWidth ) - halfTerrainWidth;
            float Y = heightValue * mHeightScale;
            float Z = (T * terrainHeight) - halfTerrainHeight;

            // Blend 3 textures depending on the height of the terrain
            float tex0Contribution = 1.0f - getPercentage(heightValue, 0.0f, 0.75f);
            float tex2Contribution = 1.0f - getPercentage(heightValue, 0.75f, 1.0f);

            mNormalBuffer[index] = glm::vec3(0);
            mPositionBuffer[index] = glm::vec3(X, Y, Z);
#if ENABLE_MULTITEXTURE
            mColorBuffer[index] = glm::vec4(tex0Contribution, tex0Contribution, tex0Contribution, tex2Contribution);
#else
            mColorBuffer[index] = glm::vec4(1.0f);
#endif
            mTex0Buffer[index] = glm::vec2(S, T);
        }
    }

    std::cout << "Terrain has been loaded!" << std::endl;
    delete [] heightMap;

    generateIndexBuffer();
    generateNormals();
    generateVertexBuffers();

    return true;
}

void Terrain::generateIndexBuffer()
{
    if (mHeightmapDimensions.x < 2 || mHeightmapDimensions.y < 2)
    {
        // Terrain hasn't been loaded, or is of an incorrect size
        return;
    }

    const unsigned int terrainWidth = mHeightmapDimensions.x;
    const unsigned int terrainHeight = mHeightmapDimensions.y;

    // 2 triangles for every quad of the terrain mesh
    const unsigned int numTriangles = ( terrainWidth - 1 ) * ( terrainHeight - 1 ) * 2;

    // 3 indices for each triangle in the terrain mesh
    mIndexBuffer.resize(numTriangles * 3);

    unsigned int index = 0; // Index in the index buffer
    for (unsigned int j = 0; j < (terrainHeight - 1); ++j )
    {
        for (unsigned int i = 0; i < (terrainWidth - 1); ++i )
        {
            int vertexIndex = ( j * terrainWidth ) + i;
            // Top triangle (T0)
            mIndexBuffer[index++] = vertexIndex;                           // V0
            mIndexBuffer[index++] = vertexIndex + terrainWidth + 1;        // V3
            mIndexBuffer[index++] = vertexIndex + 1;                       // V1
            // Bottom triangle (T1)
            mIndexBuffer[index++] = vertexIndex;                           // V0
            mIndexBuffer[index++] = vertexIndex + terrainWidth;            // V2
            mIndexBuffer[index++] = vertexIndex + terrainWidth + 1;        // V3
        }
    }
}

void Terrain::generateNormals()
{
    for (unsigned int i = 0; i < mIndexBuffer.size(); i += 3)
    {
        glm::vec3 v0 = mPositionBuffer[mIndexBuffer[i + 0]];
        glm::vec3 v1 = mPositionBuffer[mIndexBuffer[i + 1]];
        glm::vec3 v2 = mPositionBuffer[mIndexBuffer[i + 2]];

        glm::vec3 normal = glm::normalize( glm::cross( v1 - v0, v2 - v0 ) );

        mNormalBuffer[mIndexBuffer[i + 0]] += normal;
        mNormalBuffer[mIndexBuffer[i + 1]] += normal;
        mNormalBuffer[mIndexBuffer[i + 2]] += normal;
    }

    const glm::vec3 UP( 0.0f, 1.0f, 0.0f );
    for (unsigned int i = 0; i < mNormalBuffer.size(); ++i)
    {
        mNormalBuffer[i] = glm::normalize(mNormalBuffer[i]);

#if ENABLE_SLOPE_BASED_BLEND
        float val = glm::dot(mNormalBuffer[i], UP) - 0.1f;
        float fTexture0Contribution = glm::saturate<float, glm::highp>(val);
        mColorBuffer[i] = glm::vec4(fTexture0Contribution, fTexture0Contribution, fTexture0Contribution, mColorBuffer[i].w);
#endif

    }
}

void Terrain::generateVertexBuffers()
{
    // First generate the buffer object ID's
    createVertexBuffer(mGLVertexBuffer);
    createVertexBuffer(mGLNormalBuffer);
    createVertexBuffer(mGLVertexBuffer);
    createVertexBuffer(mGLTex0Buffer);
    createVertexBuffer(mGLTex1Buffer);
    createVertexBuffer(mGLTex2Buffer);
    createVertexBuffer(mGLIndexBuffer);

    // Copy the host data into the vertex buffer objects
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLVertexBuffer);
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(glm::vec3) * mPositionBuffer.size(), &(mPositionBuffer[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLVertexBuffer);
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(glm::vec4) * mColorBuffer.size(), &(mColorBuffer[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLNormalBuffer);
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(glm::vec3) * mNormalBuffer.size(), &(mNormalBuffer[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLTex0Buffer);
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(glm::vec2) * mTex0Buffer.size(), &(mTex0Buffer[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLTex1Buffer);
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(glm::vec2) * mTex0Buffer.size(), &(mTex0Buffer[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLTex2Buffer);
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, sizeof(glm::vec2) * mTex0Buffer.size(), &(mTex0Buffer[0]), GL_STATIC_DRAW_ARB);

    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mGLIndexBuffer);
    glBufferDataARB( GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint) * mIndexBuffer.size(), &(mIndexBuffer[0]), GL_STATIC_DRAW_ARB);
}

float Terrain::getHeightAt(const glm::vec3& position)
{
    float height = -FLT_MAX;
    // Check if the terrain dimensions are valid
    if (mHeightmapDimensions.x < 2 || mHeightmapDimensions.y < 2)
        return height;

    // Width and height of the terrain in world units
    float terrainWidth = (mHeightmapDimensions.x - 1) * mBlockScale;
    float terrainHeight = (mHeightmapDimensions.y - 1) * mBlockScale;
    float halfWidth = terrainWidth * 0.5f;
    float halfHeight = terrainHeight * 0.5f;

    // Multiple the position by the inverse of the terrain matrix
    // to get the position in terrain local space
    glm::vec3 terrainPos = glm::vec3(mInverseLocalToWorldMatrix * glm::vec4(position, 1.0f));
    glm::vec3 invBlockScale(1.0f / mBlockScale, 0.0f, 1.0f / mBlockScale);

    // Calculate an offset and scale to get the vertex indices
    glm::vec3 offset( halfWidth, 0.0f, halfHeight );

    // Get the 4 vertices that make up the triangle we're over
    glm::vec3 vertexIndices = ( terrainPos + offset ) * invBlockScale;

    int u0 = (int)floorf(vertexIndices.x);
    int u1 = u0 + 1;
    int v0 = (int)floorf(vertexIndices.z);
    int v1 = v0 + 1;

    if (u0 >= 0 && u1 < (int) mHeightmapDimensions.x && v0 >= 0 && v1 < (int) mHeightmapDimensions.y)
    {
        glm::vec3 p00 = mPositionBuffer[(v0 * mHeightmapDimensions.x) + u0];    // Top-left vertex
        glm::vec3 p10 = mPositionBuffer[(v0 * mHeightmapDimensions.x) + u1];    // Top-right vertex
        glm::vec3 p01 = mPositionBuffer[(v1 * mHeightmapDimensions.x) + u0];    // Bottom-left vertex
        glm::vec3 p11 = mPositionBuffer[(v1 * mHeightmapDimensions.x) + u1];    // Bottom-right vertex

        // Which triangle are we over?
        float percentU = vertexIndices.x - u0;
        float percentV = vertexIndices.z - v0;

        glm::vec3 dU, dV;
        if (percentU > percentV)
        {   // Top triangle
            dU = p10 - p00;
            dV = p11 - p10;
        }
        else
        {   // Bottom triangle
            dU = p11 - p01;
            dV = p01 - p00;
        }

        glm::vec3 heightPos = p00 + ( dU * percentU ) + ( dV * percentV );
        // Convert back to world-space by multiplying by the terrain's world matrix
        heightPos = glm::vec3(mLocalToWorldMatrix * glm::vec4(heightPos, 1));
        height = heightPos.y;
    }

    return height;
}

void Terrain::render()
{
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(mLocalToWorldMatrix));

    //
    // Texture Stage 0
    //
    // Simply output texture0 for stage 0.
    //
    glActiveTextureARB( GL_TEXTURE0_ARB );
    glMatrixMode( GL_TEXTURE );
    glPushMatrix();
    glScalef( 32.0f, 32.0f , 1.0f );

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, mGLTextures[0]);

    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLTex0Buffer);
    glTexCoordPointer( 2, GL_FLOAT, 0, BUFFER_OFFSET(0) );

#if ENABLE_MULTITEXTURE
    // Disable lighting because it changes the color of the vertices that are
    // used for the multitexture blending.
    glDisable( GL_LIGHTING );

    //
    // Texture Stage 1
    //
    // Perform a linear interpolation between the output of stage 0
    // (i.e texture0) and texture1 and use the RGB portion of the vertex's
    // color to mix the two.
    //
    glActiveTextureARB(GL_TEXTURE1_ARB );
    glMatrixMode( GL_TEXTURE );
    glPushMatrix();
    glScalef( 32.0f, 32.0f , 1.0f );

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, mGLTextures[1]);

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_COLOR );

    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLTex1Buffer);
    glTexCoordPointer( 2, GL_FLOAT, 0, BUFFER_OFFSET(0) );

    //
    // Texture Stage 2
    //
    // Perform a linear interpolation between the output of stage 1
    // (i.e texture0 mixed with texture1) and texture2 and use the ALPHA
    // portion of the vertex's color to mix the two.
    //
    glActiveTextureARB( GL_TEXTURE2_ARB );
    glMatrixMode( GL_TEXTURE );
    glPushMatrix();
    glScalef( 32.0f, 32.0f , 1.0f );

    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, mGLTextures[2]);

    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_INTERPOLATE_ARB );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_PREVIOUS_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_TEXTURE );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR );

    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, GL_PRIMARY_COLOR_ARB );
    glTexEnvi( GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, GL_SRC_ALPHA );

    glClientActiveTextureARB(GL_TEXTURE2_ARB);
    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLTex2Buffer);
    glTexCoordPointer( 2, GL_FLOAT, 0, BUFFER_OFFSET(0) );

#else
    glEnable( GL_TEXTURE );
    glEnable( GL_LIGHTING );
#endif

    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_NORMAL_ARRAY );

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLVertexBuffer);
    glVertexPointer( 3, GL_FLOAT, 0, BUFFER_OFFSET(0) );
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLVertexBuffer);
    glColorPointer( 4, GL_FLOAT, 0, BUFFER_OFFSET(0) );
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, mGLNormalBuffer);
    glNormalPointer( GL_FLOAT, 0, BUFFER_OFFSET(0) );

    glBindBufferARB( GL_ELEMENT_ARRAY_BUFFER_ARB, mGLIndexBuffer);
    glDrawElements( GL_TRIANGLES, mIndexBuffer.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    glDisableClientState( GL_NORMAL_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_VERTEX_ARRAY );

#if ENABLE_MULTITEXTURE
    glActiveTextureARB(GL_TEXTURE2_ARB);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glClientActiveTextureARB(GL_TEXTURE2_ARB);
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glActiveTextureARB(GL_TEXTURE1_ARB);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glClientActiveTextureARB(GL_TEXTURE1_ARB);
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );
#endif

    glActiveTextureARB(GL_TEXTURE0_ARB);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glClientActiveTextureARB(GL_TEXTURE0_ARB);
    glDisableClientState( GL_TEXTURE_COORD_ARRAY );

    glMatrixMode( GL_MODELVIEW );
    glPopMatrix();

#if _DEBUG
    debugRender();
#endif

}

void Terrain::renderNormals()
{
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(mLocalToWorldMatrix));

    glPushAttrib( GL_ENABLE_BIT );
    glDisable( GL_LIGHTING );

    glColor3f( 1.0f, 1.0f, 0.0f );// Yellow

    glBegin( GL_LINES );
    {
        for (unsigned int i = 0; i < mPositionBuffer.size(); ++i)
        {
            glm::vec3 p0 = mPositionBuffer[i];
            glm::vec3 p1 = (mPositionBuffer[i] + mNormalBuffer[i]);

            glVertex3fv( glm::value_ptr(p0) );
            glVertex3fv( glm::value_ptr(p1) );
        }
    }
    glEnd();

    glPopAttrib();

    glPopMatrix();
}

void Terrain::debugRender()
{
    renderNormals();
}
