#include <renderer/CommandVisualizationRenderer.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <renderer/CommandVisualizer.h>
#include <game/Camera.h>
#include <iostream>

CommandVisualizationRenderer::CommandVisualizationRenderer(Camera& _camera)
    : camera{ _camera }
{
    for (size_t i = 0; i < offsets.size(); i++)
    {
        offsets.at(i) = i * (CommandVisualizer::numVertices + 1);
    }

    for (auto& count : counts)
    {
        count = CommandVisualizer::numVertices + 1;
    }

    m_program = loadProgram("shaders/commandVisualization.vert", "shaders/commandVisualization.frag");
    glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);

    glGenBuffers(1, &m_positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, CommandVisualizationRenderer::bufferSize * floatsPerVertex * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &m_colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, CommandVisualizationRenderer::bufferSize * bytesPerVertex * sizeof(GLubyte), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, bytesPerVertex * sizeof(GLubyte), 0);
    glEnableVertexAttribArray(1);
}

CommandVisualizationRenderer::~CommandVisualizationRenderer()
{
    if (m_mappedPositionBuffer != nullptr)
        unmapBuffers();

    glDeleteProgram(m_program);
    glDeleteVertexArrays(1, &m_vertexArrayObject);
    glDeleteBuffers(1, &m_colorBuffer);
    glDeleteBuffers(1, &m_positionBuffer);

}

void CommandVisualizationRenderer::preDraw()
{
    mapBuffers();

    glUseProgram(m_program);
    GLuint positionLocation = glGetUniformLocation(m_program, "camPosition");
    GLuint sizeLocation = glGetUniformLocation(m_program, "camSize");

    glUniform2f(positionLocation, camera.getX(), camera.getY());
    glUniform2f(sizeLocation, camera.getWidth(), camera.getHeight());
}

void CommandVisualizationRenderer::draw()
{
    unsigned int previousBufferOffset = bufferOffset;
    unmapBuffers();
    glBindVertexArray(m_vertexArrayObject);
    glUseProgram(m_program);
    glDisable(GL_DEPTH_TEST);

    glMultiDrawArrays(GL_TRIANGLE_FAN, offsets.data(), counts.data(), previousBufferOffset / (CommandVisualizer::numVertices + 1));
    //glDrawArrays(GL_TRIANGLE_FAN, 0, previousBufferOffset);
}

void CommandVisualizationRenderer::addPositionData(size_t offset, glm::vec4 position)
{
    if (m_mappedPositionBuffer)
    {
        memcpy(m_mappedPositionBuffer + offset * floatsPerVertex, glm::value_ptr(position), floatsPerVertex * sizeof(GLfloat));
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset * floatsPerVertex * sizeof(GLfloat), floatsPerVertex * sizeof(GLfloat), glm::value_ptr(position));
    }
}


void CommandVisualizationRenderer::addColorData(size_t offset, size_t size, void* data)
{
    if (m_mappedColorBuffer)
    {
        memcpy(m_mappedColorBuffer + offset * bytesPerVertex, data, size * bytesPerVertex * sizeof(GLubyte));
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
        glBufferSubData(GL_ARRAY_BUFFER, offset * bytesPerVertex * sizeof(GLubyte), size * bytesPerVertex * sizeof(GLubyte), data);
    }
}

void CommandVisualizationRenderer::drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer)
{

    if (bufferOffset >= CommandVisualizationRenderer::bufferSize - CommandVisualizer::numVertices - 1)
    {
        draw();
        mapBuffers();
    }

    addPositionData(bufferOffset, glm::vec4(pos, 0.f));


    GLubyte color[bytesPerVertex] = { visualizer.baseRed, visualizer.baseGreen, visualizer.baseBlue, 0 };
    addColorData(bufferOffset, 1, color);


    bufferOffset++;
    addColorData(bufferOffset, CommandVisualizer::numVertices, visualizer.vertexColors.data());

    glm::vec2 offset(-visualizer.radius, 0);
    for (size_t i = 0; i < CommandVisualizer::numVertices; i++)
    {
        glm::vec4 vertexPosition = glm::vec4(pos + glm::vec3(offset, 0.f), 1);
        addPositionData(bufferOffset, vertexPosition);
        bufferOffset++;

        offset = glm::rotate(offset, glm::radians(360.f / (CommandVisualizer::numVertices - 1)));
    }
}

const unsigned CommandVisualizationRenderer::bufferSize;

const unsigned CommandVisualizationRenderer::floatsPerVertex;

const unsigned CommandVisualizationRenderer::bytesPerVertex;

void CommandVisualizationRenderer::mapBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
    m_mappedColorBuffer = (GLubyte*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
    m_mappedPositionBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

    bufferOffset = 0;
}

void CommandVisualizationRenderer::unmapBuffers()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glBindBuffer(GL_ARRAY_BUFFER, m_positionBuffer);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    m_mappedPositionBuffer = nullptr;
    m_mappedColorBuffer = nullptr;
    bufferOffset = 0;
}
