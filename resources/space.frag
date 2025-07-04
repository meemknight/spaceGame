#version 330
precision highp float;

out vec4 color;
in vec2 v_texture;

uniform sampler2D u_sampler;
uniform vec2 iResolution;
uniform float iTime = 10;
const float timeMultiplier = 0.1;

//https://www.shadertoy.com/view/MslGWN
//CBS
//Parallax scrolling fractal galaxy.
//Inspired by JoshP's Simplicity shader: https://www.shadertoy.com/view/lslGWr


// http://www.fractalforums.com/new-theories-and-research/very-simple-formula-for-fractal-patterns/
float field(in vec3 p,float s) {
	float strength = 7. + .03 * log(1.e-6 + fract(sin(iTime * timeMultiplier) * 4373.11));
	float accum = s/4.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 26; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + vec3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.2));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

// Less iterations for second layer
float field2(in vec3 p, float s) {
	float strength = 7. + .03 * log(1.e-6 + fract(sin(iTime* timeMultiplier) * 4373.11));
	float accum = s/4.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 18; ++i) {
		float mag = dot(p, p)  + (sin(iTime * 0.01 * timeMultiplier)) * 0.01;
		p = abs(p) / mag + vec3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 6.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.2));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

vec3 nrand3( vec2 co )
{
	vec3 a = fract( cos( co.x*8.3e-3 + co.y )*vec3(1.3e5, 4.7e5, 2.9e5) );
	vec3 b = fract( sin( co.x*0.3e-3 + co.y )*vec3(8.1e5, 1.0e5, 0.1e5) );
	vec3 c = mix(a, b, 0.5);
	return c;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	fragColor = vec4(0,0,0,0);

	float timeSpeed = iTime * timeMultiplier * 0.5;

	vec2 uv = 2. * fragCoord.xy / iResolution.xy - 1.;
	uv *= 0.95;
	vec2 uvs = uv * iResolution.xy / max(iResolution.x, iResolution.y);
	uvs *= 0.5;
	vec3 p = vec3(uvs / 4., 0) + vec3(1., -1.3, 0.);
	p += .2 * vec3(sin(timeSpeed / 16.), sin(timeSpeed / 12.),  sin(timeSpeed / 128.));


	float freqs[4];
	freqs[0] = 0.2;
	freqs[1] = 0.3;
	freqs[2] = 0.4;
	freqs[3] = 0.5;

	//Sound
	//freqs[0] = texture( iChannel0, vec2( 0.01, 0.25 ) ).x;
	//freqs[1] = texture( iChannel0, vec2( 0.07, 0.25 ) ).x;
	//freqs[2] = texture( iChannel0, vec2( 0.15, 0.25 ) ).x;
	//freqs[3] = texture( iChannel0, vec2( 0.30, 0.25 ) ).x;

	float t = field(p,freqs[2]);
	float v = (1. - exp((abs(uv.x) - 1.) * 6.)) * (1. - exp((abs(uv.y) - 1.) * 6.));
	
	//Second Layer
	vec3 p2 = vec3(uvs / (4.+sin(timeSpeed*0.11)*0.2+0.2+sin(timeSpeed*0.15)*0.3+0.4), 1.5) + vec3(2., -1.3, -1.);
	p2 += 0.25 * vec3(sin(timeSpeed / 16.), sin(timeSpeed / 12.),  sin(timeSpeed / 128.));
	float t2 = field2(p2,freqs[3]);
	vec4 c2 = mix(.4, 1., v) * vec4(1.3 * t2 * t2 * t2 ,1.8  * t2 * t2 , t2* freqs[0], t2);
	c2.rgb = pow(c2.rgb, vec3(1.2));

	//Let's add some stars
	//Thanks to http://glsl.heroku.com/e#6904.0
	vec4 starcolor = vec4(0.0);
	//vec2 seed = p.xy * 2.0;	
	//seed = floor(seed * iResolution.x);
	//vec3 rnd = nrand3( seed );
	//vec4 starcolor = vec4(pow(rnd.y,32.0));
	//starcolor = vec4(0);

	//Second Layer
	vec2 seed2 = p2.xy * 2.0;
	seed2 = floor(seed2 * iResolution.x);
	vec3 rnd2 = nrand3( seed2 );
	starcolor += vec4(pow(rnd2.y,40.0));
	
	fragColor = mix(freqs[3]-.3, 1., v) * vec4(1.5*freqs[2] * t * t* t 
	, 1.2*freqs[1] * t * t, freqs[3]*t, 1.0)+c2;

	fragColor.rgb = mix(fragColor.rgb, starcolor.rgb, 
		pow(vec3(1.0) - min(vec3(fragColor.rgb), vec3(1.0)), vec3(4.0)) );


}


void main()
{
	//float opacity = texture(u_sampler, v_texture).a;
	//if(opacity < 0.1){discard;}

	//gl_FragCoord.xy
	vec4 outColor = vec4(0, 0, 0, 0);

	mainImage(outColor, gl_FragCoord.xy);
	outColor.a = 0.1;

	color = outColor;

}

