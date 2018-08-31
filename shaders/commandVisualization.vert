#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec4 color;
layout (location = 2) in float distanceToCenter;

out vec4 v_color;
out float v_distanceToCenter;

uniform vec2 camPosition;
uniform vec2 camSize;

void main(void)
{
	v_color = color;
	v_distanceToCenter = distanceToCenter;
    vec3 screenPosition = vec3((position.xy - camPosition) / camSize, position.z);
    gl_Position = vec4(screenPosition, 1.0);
}