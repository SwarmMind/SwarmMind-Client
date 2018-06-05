#include <renderer/CommandVisualizationRenderer.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <renderer/CommandVisualizer.h>
#include <game/Camera.h>
#include <iostream>

CommandVisualizationRenderer::CommandVisualizationRenderer(Camera& _camera)
	: camera{_camera}
{
	for (size_t i = 0; i < offsets.size(); i++)
	{
		offsets.at(i) = i * (CommandVisualizer::numVertices + 1);
	}

	for (auto& count : counts)
	{
		count = CommandVisualizer::numVertices + 1;
	}
	
	program = loadProgram("shaders/commandVisualization.vert", "shaders/commandVisualization.frag");
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, CommandVisualizationRenderer::bufferSize * floatsPerVertex * sizeof(GLfloat), nullptr, GL_MAP_WRITE_BIT);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, floatsPerVertex * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferStorage(GL_ARRAY_BUFFER, CommandVisualizationRenderer::bufferSize * bytesPerVertex * sizeof(GLubyte), nullptr, GL_MAP_WRITE_BIT);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, bytesPerVertex * sizeof(GLubyte), 0);
	glEnableVertexAttribArray(1);


}

CommandVisualizationRenderer::~CommandVisualizationRenderer()
{
	if(mappedPositionBuffer != nullptr)
		unmapBuffers();

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &positionBuffer);

}

void CommandVisualizationRenderer::preDraw()
{
	mapBuffers();

	glUseProgram(program);
	GLuint positionLocation = glGetUniformLocation(program, "camPosition");
	GLuint sizeLocation = glGetUniformLocation(program, "camSize");

	glUniform2f(positionLocation, camera.getX(), camera.getY());
	glUniform2f(sizeLocation, camera.getWidth(), camera.getHeight());
}

void CommandVisualizationRenderer::draw()
{
	unsigned int previousBufferOffset = bufferOffset;
	unmapBuffers();
	glBindVertexArray(vertexArrayObject);
	glUseProgram(program);

	glMultiDrawArrays(GL_TRIANGLE_FAN, offsets.data(), counts.data(), previousBufferOffset / (CommandVisualizer::numVertices + 1));
	//glDrawArrays(GL_TRIANGLE_FAN, 0, previousBufferOffset);
}

void CommandVisualizationRenderer::drawCommandVisualizer(glm::vec3 pos, CommandVisualizer& visualizer)
{
	if (bufferOffset >= CommandVisualizationRenderer::bufferSize - CommandVisualizer::numVertices - 1)
	{
		draw();
		mapBuffers();
	}

	GLfloat position[floatsPerVertex] = { pos.x, pos.y, pos.z, 0 };
	memcpy(mappedPositionBuffer + bufferOffset * floatsPerVertex, position, floatsPerVertex * sizeof(GLfloat));
	
	GLubyte color[bytesPerVertex] = { visualizer.baseRed, visualizer.baseGreen, visualizer.baseBlue, 0 };
	memcpy(mappedColorBuffer + bufferOffset * bytesPerVertex, color, bytesPerVertex * sizeof(GLubyte));
	
	bufferOffset++;

	memcpy(mappedColorBuffer + bufferOffset * bytesPerVertex, visualizer.vertexColors.data(), CommandVisualizer::numVertices * bytesPerVertex * sizeof(GLubyte));

	glm::vec2 offset(-0.5, 0);
	for (size_t i = 0; i < CommandVisualizer::numVertices; i++)
	{
		glm::vec4 vertexPosition = glm::vec4(pos + glm::vec3(offset, 0), 1);
		memcpy(mappedPositionBuffer + bufferOffset * floatsPerVertex, glm::value_ptr(vertexPosition), floatsPerVertex * sizeof(GLfloat));
		bufferOffset++;

		offset = glm::rotate(offset, glm::radians(360.f / (CommandVisualizer::numVertices - 1)));
	}
}

const unsigned CommandVisualizationRenderer::bufferSize;

const unsigned CommandVisualizationRenderer::floatsPerVertex;

const unsigned CommandVisualizationRenderer::bytesPerVertex;

void CommandVisualizationRenderer::mapBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	mappedColorBuffer = (GLubyte*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	mappedPositionBuffer = (GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);

	bufferOffset = 0;
}

void CommandVisualizationRenderer::unmapBuffers()
{
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	mappedPositionBuffer = nullptr;
	mappedColorBuffer = nullptr;
	bufferOffset = 0;
}

