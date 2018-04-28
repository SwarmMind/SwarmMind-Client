#version 400 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;

out vec2 v_uv;

uniform float camX;
uniform float camY;
uniform float camWidth;
uniform float camHeight;

void main(void)
{
    v_uv = uv;

    vec3 screenPosition = vec3((position.x - camX)/camWidth, 
                                (position.y - camY)/camHeight, 
                                position.z);
    gl_Position = vec4(screenPosition, 1.0);
}