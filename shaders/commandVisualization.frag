#version 400 core

uniform sampler2D textureSampler;

in vec4 v_color;
in float v_distanceToCenter;

out vec4 outputColor;

float calculateAlpha()
{
	float commandsAlpha =  (-40 * pow(v_distanceToCenter - 0.9, 2) + 1.5) * v_color.a;
	//float circleAlpha = max(-128 * pow(v_distanceToCenter - 1.0, 2) + 0.5, 0.0);
	//return min(commandsAlpha + circleAlpha, 1.0);
	return commandsAlpha;
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

/*
float restoredAlpha()
{
	return v_color.a / v_distanceToCenter;
}

float calculateAlpha()
{
	float alpha = restoredAlpha();
	float circleRadius = 0.7;
	float circleAlpha;
	if(v_distanceToCenter <= circleRadius) 
		circleAlpha = max(-64 * pow(v_distanceToCenter - circleRadius, 2) + 0.5, 0.0);
	else
		circleAlpha = 1 - step(alpha, (v_distanceToCenter - circleRadius)/(1-circleRadius));
	return circleAlpha;
}

void main(void)
{
	if(v_distanceToCenter == 0)
		discard;
	float alpha = calculateAlpha();
    if(alpha <= 0)
		discard;

    outputColor = vec4(v_color.rgb, alpha);
}*/