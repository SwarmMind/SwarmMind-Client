#version 400 core

uniform sampler2D dynamicDataSampler;
uniform uint particleTextureSize;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out vec2 v_uv;
flat out ivec2 v_particleCoord;
out vec4 v_dynamicParticleData;

void main()
{
	v_uv = uv;
	
	ivec2 particleCoord = ivec2(gl_InstanceID / particleTextureSize, mod(gl_InstanceID, particleTextureSize));
	v_particleCoord = particleCoord;
	vec4 dynamicParticleData = texelFetch(dynamicDataSampler, particleCoord, 0);
	v_dynamicParticleData = dynamicParticleData;
	
	float lifeTime = dynamicParticleData.z;
	if(lifeTime <= 0.0)
	{
		gl_Position = vec4(0, 0, 0, 1);
		return;
	}

	vec2 offset = dynamicParticleData.xy;
	gl_Position = vec4(position + offset, 1, 1);
}