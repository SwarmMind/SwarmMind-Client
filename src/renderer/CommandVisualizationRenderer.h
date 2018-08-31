#pragma once

#include <renderer/CommandVisualizer.h>
#include <glm/fwd.hpp>
#include <renderer/OpenGLHelpers.h>
#include <glbinding/gl41core/gl.h>
using namespace gl41core;

class CommandVisualizationRenderer
{
public:
	CommandVisualizationRenderer(class Camera& _camera);

	~CommandVisualizationRenderer();

	void preDraw();
	void draw();
	void drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer);
private:
	static const unsigned bufferSize = 1024 * 512;
	unsigned bufferOffset = 0;

	static const unsigned floatsPerVertex = 4;
	static const unsigned bytesPerVertex = 4;


	GLuint m_program;
	GLuint m_vertexArrayObject;
	GLuint m_colorBuffer, m_positionBuffer;
	GLubyte* m_mappedColorBuffer = nullptr;
	GLfloat* m_mappedPositionBuffer = nullptr;
	void mapBuffers();
	void unmapBuffers();

    void addPositionData(size_t offset, glm::vec4 position);
    void addColorData(size_t offset, size_t size, void* data);

	std::array<GLint, bufferSize / (CommandVisualizer::numVertices + 1)> offsets;
	std::array<GLint, bufferSize / (CommandVisualizer::numVertices + 1)> counts;

	class Camera& camera;
};
