#version 330
precision highp float;

out vec4 color;
in vec2 v_texture;

uniform sampler2D u_sampler;
uniform vec2 iResolution;
uniform float iTime = 0;

void main()
{
	color = texture2D(u_sampler, v_texture).rgba;

	color.rgb *= u_strength;		
	color.rgb = floor(color.rgb);
	color.rgb /= u_strength;

}