#version 400 core

layout (location = 0) in vec2 position;

void main()
{
	/*vec2 positions[6] = vec2[](
		vec2(-1, -1),
		vec2(1, -1),
		vec2(1, 1),
		vec2(-1, -1),
		vec2(1, 1),
		vec2(-1, 1)
	);*/
	gl_Position = vec4(position, 1, 1.0);
}