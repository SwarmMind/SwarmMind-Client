#version 400 core

uniform sampler2D textureSampler;

in vec2 v_uv;
out vec4 outputColor;

void main(void)
{
    vec4 color = texture(textureSampler, v_uv);
    if(color.a == 0)
		discard;

    outputColor = color;
}