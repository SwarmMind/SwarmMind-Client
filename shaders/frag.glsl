#version 400 core

uniform sampler2D textureSampler;

in vec2 v_uv;
out vec4 outputColor;

void main(void)
{
    vec4 color = texture(textureSampler, v_uv);
    gl_FragDepth = color.a * gl_FragCoord.z;
    outputColor = color;
}