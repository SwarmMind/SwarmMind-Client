#pragma once

#include <glbinding/gl41core/gl.h>
using namespace gl41core;

class TextureRenderData
{
public:
	TextureRenderData();

	~TextureRenderData();

	bool addData(unsigned int size, const GLfloat* data);
	void draw();

private:
	GLuint m_vertexArrayObject;
	GLuint m_secondVAO;

	unsigned int m_vertexBufferSize;
	unsigned int m_floatsPerVert;
	int m_bufferOffset;
	GLfloat* m_mappedBuffer;
	GLuint m_vertexBuffer;
	GLuint m_secondBuffer;
	
	void swapBuffers();
	void mapBuffer();
	void unmapBuffer();
	bool bufferIsMapped();
};
