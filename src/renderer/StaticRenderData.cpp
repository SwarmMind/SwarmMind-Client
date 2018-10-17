#include <renderer/StaticRenderData.h>
#include <renderer/Texture.h>

const int StaticRenderData::defaultBufferVertexSize = 8192;

StaticRenderData::StaticRenderData(std::shared_ptr<Texture> texture)
    : m_texture{texture}
    , m_bufferVertexSize{defaultBufferVertexSize}
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    clearData();

    unsigned int stride = m_floatsPerVertex * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

StaticRenderData::StaticRenderData(const StaticRenderData& other)
    : m_floatsPerVertex{other.m_floatsPerVertex}
    , m_texture{other.m_texture}
    , m_bufferVertexSize{other.m_bufferVertexSize}
    , m_bufferOffset{other.m_bufferOffset}
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_bufferVertexSize * vertexSize(), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_COPY_READ_BUFFER, other.m_vbo);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, 0, m_bufferVertexSize * vertexSize());

    unsigned int stride = m_floatsPerVertex * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
}

StaticRenderData::~StaticRenderData()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
}

unsigned int StaticRenderData::vertexSize()
{
    return m_floatsPerVertex * sizeof(GLfloat);
}

void StaticRenderData::clearData()
{
    m_bufferVertexSize = defaultBufferVertexSize;
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_bufferVertexSize * vertexSize(), nullptr, GL_STATIC_DRAW);
}

void StaticRenderData::addData(int vertexCount, GLfloat* data)
{
    if (m_bufferOffset + vertexCount >= m_bufferVertexSize)
    {
        increaseBufferSize(vertexCount);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, m_bufferOffset * vertexSize(), vertexCount * vertexSize(), data);
    m_bufferOffset += vertexCount;
}

void StaticRenderData::draw()
{
    glBindTexture(GL_TEXTURE_2D, m_texture->ID());
    glBindVertexArray(m_vao);
    int vertexCount = m_bufferOffset;
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

std::shared_ptr<Texture> StaticRenderData::texture()
{
    return m_texture;
}

void StaticRenderData::increaseBufferSize(int minimumVertexCount)
{
    int newBufferSize = std::max(m_bufferVertexSize * 2, m_bufferVertexSize + 2 * minimumVertexCount);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	std::vector<GLfloat> oldData (newBufferSize * vertexSize());
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, m_bufferVertexSize * vertexSize(), oldData.data());
    glBufferData(GL_ARRAY_BUFFER, newBufferSize * vertexSize(), oldData.data(), GL_STATIC_DRAW);

    m_bufferVertexSize = newBufferSize;
}
