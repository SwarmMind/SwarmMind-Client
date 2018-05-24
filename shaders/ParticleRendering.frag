#version 400 core

uniform sampler2D particleColorSampler;

out vec4 f_color;

in vec2 v_uv;
flat in ivec2 v_particleCoord;
in vec4 v_dynamicParticleData;

void main()
{
	float zz = 1.0 - dot(v_uv, v_uv);
	float lifeTime = v_dynamicParticleData.z;
	if(zz < 0 || lifeTime <= 0)
		discard;

	//smoothly fade out the particles at the end of their lifetime
	float alpha = zz * smoothstep(0, 0.1, lifeTime);

	vec4 color = texelFetch(particleColorSampler, v_particleCoord, 0);

	f_color = vec4(color.rgb, color.a * alpha);
}