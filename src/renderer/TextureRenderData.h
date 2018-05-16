#pragma once

#include <glbinding/gl41core/gl.h>
using namespace gl41core;

class TextureRenderData
{
public:
	TextureRenderData();

	~TextureRenderData();

	bool addData(unsigned int size, GLfloat* data);
	void draw();

private:
	GLuint vertexArrayObject;
	GLuint secondVAO;

	unsigned int vertexBufferSize;
	unsigned int floatsPerVert;
	int bufferOffset;
	GLfloat* mappedBuffer;
	GLuint vertexBuffer;
	GLuint secondBuffer;
	
	void swapBuffers();
	void mapBuffer();
	void unmapBuffer();
	bool bufferIsMapped();
};
