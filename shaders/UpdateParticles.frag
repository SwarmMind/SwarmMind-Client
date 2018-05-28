#version 400 core

uniform sampler2D particleSampler;
uniform sampler2D staticDataSampler;
uniform float deltaTime;

layout(pixel_center_integer) in vec4 gl_FragCoord;

layout (location = 0) out vec4 particleData;

void main()
{
	ivec2 textureCoords = ivec2(gl_FragCoord.xy);
	vec4 currentData = texelFetch(particleSampler, textureCoords, 0);
	float lifeTime = currentData.z;
	if(lifeTime <= 0.0)
	{
		particleData = vec4(0, 0, 0, 0);
		return;
	}

	vec2 position = currentData.xy;

	vec4 staticData = texelFetch(staticDataSampler, textureCoords, 0);
	vec2 velocity = staticData.xy;

	particleData = vec4(position + velocity * deltaTime, lifeTime - deltaTime, 0);
}