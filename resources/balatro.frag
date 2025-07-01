#version 330
precision highp float;

out vec4 color;
in vec2 v_texture;
in vec3 v_normal;

uniform sampler2D u_sampler;
uniform vec2 iResolution;
uniform float iTime = 0;

const vec3 ro = vec3(0.0, 0.0, 0.8); // camera position


const int kEffectMode = 2; // 0: Gold, 1: Silver, 2: Holo, 3: Glitter, 4: Disk, 5: Stripes

float luminosity(vec3 color) {
	return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

vec3 screenBlend(vec3 base, vec3 effect) {
	return 1.0 - (1.0 - base) * (1.0 - effect);
}

vec3 stickerBlend(vec3 base, vec3 effect) 
{
	return screenBlend(base, effect);
}


vec3 overlayBlend(vec3 base, vec3 blend) {
	return mix(
		2.0 * base * blend,
		1.0 - 2.0 * (1.0 - base) * (1.0 - blend),
		step(0.5, base)
	);
}


vec3 multiplyBlend(vec3 base, vec3 effect) 
{
	return base * effect;
}

//vec3 fresnelBlend(vec3 base, vec3 effect)
//{
//    float fresnel = pow(1.0 - dot(v, planeNormal), 3.0);
//    vec3 finalColor = base + effect * fresnel;
//}

vec3 glitterBlend(vec3 base, vec3 effect) {
	
   
	return max(overlayBlend(base, effect), base);
   
	//float luminosity = luminosity(base);
	//if(luminosity > 0.9)return overlayBlend(base, effect);
	//return base;

}



vec3 holoBlend(vec3 base, vec3 effect) {
	
	//return effect;
	
	float luminosity = luminosity(base);
	luminosity = pow(luminosity, 16.0);
	luminosity *= 0.8;
	luminosity += 0.1;


	//base *= (0.7,0.75,0.85);
	base = mix(base  *= (0.7,0.75,0.85), (vec3(102, 0, 255) / 255.f) * 1.4, 0.1);
	   
	   
	return mix(base, max(overlayBlend(base, effect), base), luminosity);
	
	
	
	return max(overlayBlend(base, effect), base);
   
	//float luminosity = luminosity(base);
	//if(luminosity > 0.9)return overlayBlend(base, effect);
	//return base;

}

// Gold foil effect
vec3 effectGoldFoil(vec3 baseColor, vec2 uv, vec3 n, vec3 v, vec3 l) {
	float ndotl = clamp(dot(n, l), 0.0, 1.0);
	float spec = pow(clamp(dot(reflect(-l, n), v), 0.0, 1.0), 64.0);
	vec3 foil = vec3(0.9, 0.75, 0.2) * ndotl + vec3(1.0, 0.8, 0.4) * spec;
	return stickerBlend(baseColor, foil);
}

// Silver foil
vec3 effectSilverFoil(vec3 baseColor, vec2 uv, vec3 n, vec3 v, vec3 l) {
	float ndotl = clamp(dot(n, l), 0.0, 1.0);
	float spec = pow(clamp(dot(reflect(-l, n), v), 0.0, 1.0), 128.0);
	vec3 foil = vec3(0.6) * ndotl + vec3(1.0) * spec * 1.2;
	return stickerBlend(baseColor, foil);
}

vec3 effectHolographic(vec3 baseColor, vec2 uv, vec3 n, vec3 v, vec3 l) {

	float time = iTime * 1.0;
	float timeShift = 0.0;
	float timeHue = iTime * 0.0;
	float timeDistort = sin(iTime * 0.1);
	
	float holoMultiplier = 2.3;
	float lightingMultiplier = 1.3;

	// View-dependent shift using Fresnel term
	float fresnel = pow(1.0 - max(dot(n, v), 0.0), 0.9);
	//fresnel = 0;

	// Use fresnel to shift UV slightly for diffraction shimmer
	vec2 shift = fresnel * vec2(0.1, 0.05);
	vec2 hologrid = (uv + shift) * (2.0 + v.y * 3.2) + sin(timeShift + (uv + shift).yx * (10.0
		+ v.x * 3.0)
	);
	
   
	// Holographic wave pattern
	float holo = sin(hologrid.x) * cos(hologrid.y);

	// Color shifting
	float hue = fract(holo + timeHue * 0.1);
	vec3 holoColor = 0.5 + 0.5 * cos(6.2831 * (hue + vec3(0.0, 0.3, 0.6)));
	
	float dotYellow = clamp(dot(holoColor, vec3(1,1,0)), 0.0, 1.0);
	dotYellow = pow(dotYellow, 16.0);
	//holoColor *= (1.0 - dotYellow); 
	
	holoColor *= holoMultiplier;

	float dotNC = clamp(dot(n, normalize(ro)), 0.0, 1.0);

	// Light reflection intensity
	float lighting = 0.3 + 0.7 * clamp(dot(n, l), 0.0, 1.0);
	float ndotl = clamp(dot(n, l), 0.0, 1.0);
	float spec = 0.5 + 1.2 * pow(clamp(dot(reflect(-l, n), v), 0.0, 1.0), 32.0);
	
	lighting *= spec * lightingMultiplier;

	// Distortion shimmer
	float distort = 0.08 * sin(uv.y * 100.0 + timeDistort * 5.0 + dotNC * 210.0
		 + (v.y * 20.0 + v.x * 30.0) * 0.55
	);
	//float distort = 0.08 * sin(uv.y * 100.0 + timeDistort * 5.0 + v.y * 210.0 + v.x * 30.0 );
	
	
	return holoBlend(baseColor, holoColor * lighting) + distort * 0.4;
	
	//old
	// Final foil color
	vec3 foil = holoColor * lighting + distort;
	return holoBlend(baseColor, foil);
}
// Hash a float
float hash(float x) {
	return fract(sin(x * 17.323912) * 43758.5453123);
}

// Hash a vec2 (for glitter grid and similar)
float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

// Glitter shine with triangle grid
vec3 effectGlitter(vec3 baseColor, vec2 uv, vec3 n, vec3 v, vec3 l) {
	vec2 st = uv * 30.0; // fewer, larger glitter triangles
	vec2 baseCell = floor(st);
	vec3 foil = vec3(0.0);

	// Loop over neighbors to allow overlap
	for (int j = -1; j <= 1; ++j) {
		for (int i = -1; i <= 1; ++i) {
			vec2 cell = baseCell + vec2(float(i), float(j)) * 0.0001;

			// Jitter position inside cell
			float rnd = hash(cell);
			vec2 jitter = vec2(hash(cell + 1.3), hash(cell + 2.1)) - 0.5;
			vec2 center = cell + 0.5 + 0.4 * jitter;

			// Rotation
			float angle = hash(cell + 3.7) * 6.2831;
			mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));

			// Local UV
			vec2 local = (st - center);
			vec2 triUV = rot * local;

			// Simple triangle shape (pointing up)
			float inside = step(triUV.y, 0.5) * step(-triUV.x * 0.866 - triUV.y * 0.5, 0.0) * step(triUV.x * 0.866 - triUV.y * 0.5, 0.0);

			if (inside > 0.5) {
				// Fake glitter normal
				vec3 fakeNormal = normalize(vec3(hash(cell + 4.0) - 0.5, hash(cell + 5.0) - 0.5, 1.0));

				float shine = pow(clamp(dot(reflect(-l, fakeNormal), v), 0.0, 1.0), 64.0);
				vec3 glitterColor = vec3(0.5 + 0.5 * sin(rnd * 123.0 + vec3(0, 2, 4)));
				foil += glitterColor * shine * 3.0;
			}
		}
	}

	//if(foil == vec3(0))foil = vec3(1);

	return glitterBlend(baseColor, foil);
}

// Disk effect
vec3 effectDisk(vec3 baseColor, vec2 uv, vec3 n, vec3 v, vec3 l) {
	vec2 toCenter = uv - 0.5;
	float r = length(toCenter);
	float ring = sin(400.0 * r + iTime * 5.0);
	float band = smoothstep(0.0, 0.02, abs(ring));
	vec3 color = vec3(0.5 + 0.5 * cos(10.0 * r + vec3(0.0, 0.5, 1.0)));
	vec3 foil = color * band * (0.3 + 0.7 * dot(n, l));
	return stickerBlend(baseColor, foil);
}

// Stripes pattern
vec3 effectStripes(vec3 baseColor, vec2 uv, vec3 n, vec3 v, vec3 l) {
	float stripe = sin((uv.x + uv.y) * 100.0 + iTime * 2.0);
	float mask = smoothstep(0.0, 0.01, abs(stripe));
	vec3 color = vec3(0.6, 0.3, 1.0);
	vec3 foil = color * mask * (0.3 + 0.7 * dot(n, l));
	return stickerBlend(baseColor, foil);
}





vec3 sticker(vec2 planeUV, vec3 planeNormal, vec3 hitPos)
{
	vec3 camPos = ro;                        // camera in front of plane
	vec3 fragPos = hitPos;                   // from your ray-plane intersection
	vec3 v = normalize(camPos - fragPos);    // view direction
	vec3 l = normalize(vec3(sin(iTime*0.1)*0.3 + 0.85, cos(iTime*0.07)*0.2 + 0.88, 4.0)); // some constant directional light


	vec3 baseColor = texture(u_sampler, planeUV).rgb;
	
	vec3 finalColor;

	if (kEffectMode == 0) finalColor = effectGoldFoil(baseColor, planeUV, planeNormal, v, l);
	else if (kEffectMode == 1) finalColor = effectSilverFoil(baseColor, planeUV, planeNormal, v, l);
	else if (kEffectMode == 2) finalColor = effectHolographic(baseColor, planeUV, planeNormal, v, l);
	else if (kEffectMode == 3) finalColor = effectGlitter(baseColor, planeUV, planeNormal, v, l);
	else if (kEffectMode == 4) finalColor = effectDisk(baseColor, planeUV, planeNormal, v, l);
	else if (kEffectMode == 5) finalColor = effectStripes(baseColor, planeUV, planeNormal, v, l);
	
	return finalColor;
}



void main()
{
	float opacity = texture(u_sampler, v_texture).a;
	
	if(opacity < 0.1){discard;}

	//gl_FragCoord.xy
	color = vec4(sticker(v_texture, v_normal, vec3(v_texture.xy, 0)), 1);

}

