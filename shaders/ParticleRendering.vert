#version 400 core

uniform sampler2D particleSampler;
uniform uint particleTextureSize;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 uv;

out vec2 v_uv;
out vec4 v_staticParticleData;

void main()
{
	v_uv = uv;
	
	ivec2 particleCoord = ivec2(gl_InstanceID / particleTextureSize, mod(gl_InstanceID, particleTextureSize));
	vec4 staticParticleData = texelFetch(particleSampler, particleCoord, 0);
	v_staticParticleData = staticParticleData;
	
	float lifeTime = staticParticleData.z;
	if(lifeTime <= 0.0)
	{
		gl_Position = vec4(0, 0, 0, 1);
		return;
	}

	vec2 offset = staticParticleData.xy;
	gl_Position = vec4(position + offset, 1, 1);
}