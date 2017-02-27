#version 330 core

in vec2 UV;

out vec3 color;

uniform sampler2D renderedTexture;
uniform float time;

void main()
{
	color = texture(
	    renderedTexture,
	    UV + 0.005 * vec2(sin(time + 800.0 * UV.x), cos(time + 600.0 * UV.y))
	).xyz;

    // EXERCISE 2: Experiment with other UV coordinates
    //    color = texture(
    //	    renderedTexture,
    //	    UV + 0.005 * vec2(cos(time + 800.0 * UV.x) * tan(time / 32),
    //	                      sin(time + 600.0 * UV.y) * tan(time / 32))
    //	).xyz;
}


