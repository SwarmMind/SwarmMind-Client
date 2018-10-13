#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 v_uv;

uniform vec2 camPosition;
uniform float camWidth;
uniform float camHeight;
uniform mat3 model, view;

void main(void)
{
    v_uv = uv;
	
	vec3 pos = view * (position - vec3(camPosition, 0.0));
	
    gl_Position = vec4(pos, 1.0);
}