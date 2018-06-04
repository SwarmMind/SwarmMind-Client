#version 400 core

uniform sampler2D textureSampler;

in vec4 v_color;
in float v_distanceToCenter;

out vec4 outputColor;

float calculateAlpha()
{
	return min((-20 * pow(v_distanceToCenter - 0.7, 2) + 1.5) * v_color.a, 1.0);
	//float curveValue = sin(2.5 * pow(v_distanceToCenter, 2));
	//return curveValue * pow(mix(v_color.a, 1.0, curveValue), 10);
}

void main(void)
{
	float alpha = calculateAlpha();
    if(alpha <= 0)
		discard;

    outputColor = vec4(v_color.rgb, alpha);
}