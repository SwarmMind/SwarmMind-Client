#pragma once

#include <glbinding/gl41core/gl.h>
using namespace gl41core;

class StaticRenderData
{
public:
	StaticRenderData(class Texture* texture);
    StaticRenderData(const StaticRenderData& other);
    StaticRenderData() = delete;
	virtual ~StaticRenderData();

    void clearData();
    void addData(int vertexCount, GLfloat* data);
    void draw();

    class Texture* texture();
protected:
    GLuint m_vao;
    GLuint m_vbo;

    int m_floatsPerVertex = 5;
    int m_bufferOffset = 0;
    int m_bufferVertexSize;
    static const int defaultBufferVertexSize;

    unsigned int vertexSize();

    class Texture* m_texture;
private:
    void increaseBufferSize(int minimumVertexCount);
};