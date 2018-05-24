#version 400 core

out vec4 color;

in vec2 v_uv;
in vec4 v_staticParticleData;

void main()
{
	float zz = 1.0 - dot(v_uv, v_uv);
	float lifeTime = v_staticParticleData.z;
	if(zz < 0 || lifeTime <= 0)
		discard;

	//smoothly fade out the particles at the end of their lifetime
	float alpha = zz * smoothstep(0, 0.1, lifeTime);

	color = vec4(0, 0, 1, alpha);
}