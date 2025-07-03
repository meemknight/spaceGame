#version 330
precision highp float;

out vec4 color;
in vec2 v_texture;
in vec4 v_color;
in vec3 v_normal;

uniform sampler2D u_sampler;
uniform vec2 iResolution;
uniform float iTime;



//https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
	x += ( x << 10u );
	x ^= ( x >>  6u );
	x += ( x <<  3u );
	x ^= ( x >> 11u );
	x += ( x << 15u );
	return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
	const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
	const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

	m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
	m |= ieeeOne;                          // Add fractional part to 1.0

	float  f = uintBitsToFloat( m );       // Range [1:2]
	return f - 1.0;                        // Range [0:1]
}

// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }
float random( vec2  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec3  v ) { return floatConstruct(hash(floatBitsToUint(v))); }
float random( vec4  v ) { return floatConstruct(hash(floatBitsToUint(v))); }



// 2D hash: returns random vec2 in [-1,1]
vec2 hash2(vec2 p) {
	p = fract(p * vec2(123.34, 456.21));
	p += dot(p, p + 45.32);
	return fract(vec2(p.x * 12.34, p.y * 34.56)) * 2.0 - 1.0;
}

// Dithered noise pattern
float ditherNoise(vec2 uv) {
	vec2 grid = floor(mod(uv * iResolution.xy, 4.0));
	float pattern = mod(dot(grid, vec2(2.0, 3.0)), 4.0);
	return pattern / 3.0 - 0.5; // range [-0.5, 0.5]
}

vec3 reinhard(vec3 v)
{
	return v / (1.0f + v);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	vec2 uv = v_texture;
	vec4 texColor = texture(u_sampler, uv);
	if (texColor.a < 0.01) discard;

	// Base color is vertex color * texture
	vec3 baseColor = texColor.rgb * v_color.rgb;

	// Dithered paper bump using screen-space noise
	float noise = (ditherNoise(v_texture * 0.02));
	float noise2 = (ditherNoise(v_texture * 0.02 - vec2(10,10)));
	vec3 noiseNormal = normalize(v_normal + vec3(noise * 0.2 - 0.1, noise2 * 0.2 - 0.1, 0.0));

	// Lighting setup
	vec3 N = normalize(noiseNormal);
	vec3 V = normalize(vec3(0.0, 0.0, 1.0));
	vec3 L = normalize(vec3(-0.4, 0.6, 2.7));
	vec3 H = normalize(V + L);

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), 64.0);

	vec3 ambient = 0.7 * baseColor;
	vec3 diffuse = 0.3 * diff * baseColor;
	vec3 specular = 0.04 * spec * vec3(1.0); // subtle white specular

	//vec3 finalColor = reinhard((ambient + diffuse + specular) * 1.3);
	//fragColor = vec4(finalColor, 1.0);

	vec3 litColor = reinhard((ambient + diffuse + specular) * 1.3);

	// Convert to HSV
	vec3 hsv;
	{
		vec3 c = litColor;
		vec4 K = vec4(0., -1./3., 2./3., -1.0);
		vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
		vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
		float d = q.x - min(q.w, q.y);
		float e = 1e-10;
		hsv = vec3(abs(q.z + (q.w - q.y)/(6. * d + e)), d / (q.x + e), q.x);
	}
	
	// Post-process: shift hue & brighten light areas
	//hsv.x = fract(hsv.x + 0.01); // slight hue shift
	hsv.z = hsv.z + pow(hsv.z, 1.2) * 0.4; // brighten light areas
	hsv.y = min(hsv.y + 0.05, 1); //saturation
	
	// Convert back to RGB
	vec3 finalColor;
	{
		vec3 c = hsv;
		vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0),
								 6.0) - 3.0) - 1.0, 0.0, 1.0);
		finalColor = c.z * mix(vec3(1.0), rgb, c.y);
	}
	
	fragColor = vec4(finalColor, 1.0);
}

void main()
{
	vec4 outColor = vec4(0, 0, 0, 1);
	mainImage(outColor, gl_FragCoord.xy);
	color = outColor;
}
