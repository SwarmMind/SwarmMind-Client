#pragma once

#include <glbinding/gl/gl.h>
using namespace gl;

class TextureRenderData
{
public:
	TextureRenderData();

	~TextureRenderData();

	bool addData(unsigned int size, GLfloat* data);

	unsigned int vertexBufferSize;
	int bufferOffset;
	GLfloat* mappedBuffer;
	GLuint vertexBuffer;
	GLuint secondBuffer;
	
	void swapBuffers();
	void mapBuffer();
	void unmapBuffer();
private:
};
