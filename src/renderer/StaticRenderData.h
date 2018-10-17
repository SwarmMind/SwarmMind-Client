#pragma once

#include <memory>

#include <glbinding/gl41core/gl.h>
using namespace gl41core;

#include <renderer/Texture.h>

class StaticRenderData
{
public:
	StaticRenderData(std::shared_ptr<Texture> texture);
    StaticRenderData(const StaticRenderData& other);
    StaticRenderData() = delete;
	virtual ~StaticRenderData();

    void clearData();
    void addData(int vertexCount, GLfloat* data);
    void draw();

    std::shared_ptr<Texture> texture();
protected:
    GLuint m_vao;
    GLuint m_vbo;

    int m_floatsPerVertex = 5;
    int m_bufferOffset = 0;
    int m_bufferVertexSize;
    static const int defaultBufferVertexSize;

    unsigned int vertexSize();

    std::shared_ptr<Texture> m_texture;
private:
    void increaseBufferSize(int minimumVertexCount);
};