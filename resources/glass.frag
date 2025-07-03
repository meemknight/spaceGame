#version 330
precision highp float;

out vec4 color;
in vec2 v_texture;
in vec4 v_color;
in vec3 v_normal;

uniform sampler2D u_sampler;
uniform sampler2D u_background;
uniform vec2 iResolution;
uniform float iTime;


// Utility: RGB to HSV
vec3 rgb2hsv(vec3 c)
{
	vec4 K = vec4(0., -1./3., 2./3., -1.0);
	vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
	vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

	float d = q.x - min(q.w, q.y);
	float e = 1e-10;
	return vec3(abs(q.z + (q.w - q.y)/(6.*d + e)), d/(q.x + e), q.x);
}


void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	uv = v_texture;
	vec2 centerUV = uv * 2.0 - 1.0;
	centerUV.x *= iResolution.x / iResolution.y;

	vec4 baseColor = texture(u_sampler, uv).rgba * v_color;
	if(baseColor.a < 0.01) discard;

	// Simulated curved normal
	vec3 fakeNormal = normalize(vec3(centerUV, 1.0) + vec3(0.2, 0.1, 0.0));

	// Mix with real normal
	vec3 mixedNormal = normalize(mix(normalize(v_normal), fakeNormal, 0.1));

	// Fresnel effect (milder)
	vec3 viewDir = normalize(vec3(0.0, 0.0, 1.0));
	//float fresnel = pow(dot(viewDir, mixedNormal), 1.2); // looks most transparent when dot 1
	float fresnel = pow(1.0 - dot(viewDir, mixedNormal), 0.5);
	fresnel = mix(0.2, 0.3, fresnel);
	//fresnel = 0.0;

	// Hue-based transparency
	vec3 hsv = rgb2hsv(baseColor.rgb);
	float hueTransparency = smoothstep(0.55, 0.75, hsv.x);

	// Simple specular highlight (Phong)
	vec3 lightDir = normalize(vec3(-0.5, 0.6, 1.9));
	vec3 halfVec = normalize(viewDir + lightDir);
	float spec = pow(max(dot(mixedNormal, halfVec), 0.0), 128.0);

	// Yellow-boosting light
	vec3 yellowLightDir = normalize(vec3(0.3, 0.1, 1.0));
	vec3 yellowTint = vec3(1.0, 0.9, 0.3) * baseColor.rgb;
	float yellowSpec = pow(max(dot(mixedNormal, normalize(viewDir + yellowLightDir)), 0.0), 24.0);

	vec3 specular = 0.9 * spec * vec3(1.0) + 1.1 * yellowSpec * yellowTint;

	// Final color and alpha
	vec3 glassColor = baseColor.rgb + fresnel * 0.4 + specular;
	float alpha = mix(0.2, 0.6, fresnel) * (1.0 - hueTransparency);

	float alphaAdder = 0.2;
	fragColor = vec4(glassColor, (alpha + alphaAdder)/(1.0 + alphaAdder));

	//background
	uv = fragCoord.xy / iResolution.xy;
	vec3 backgroundColor = texture(u_background, uv).rgb;
	fragColor.rgb = mix(backgroundColor, fragColor.rgb, fragColor.a);
	fragColor.a = 1;
}


void main()
{
	vec4 outColor = vec4(0, 0, 0, 0);
	mainImage(outColor, gl_FragCoord.xy);
	color = outColor;
}
