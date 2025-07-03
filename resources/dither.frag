#version 330
precision highp float;

out vec4 color;
in vec2 v_texture;
in vec4 v_color;

uniform sampler2D u_sampler;
uniform vec2 iResolution;
uniform float iTime;

uniform vec2 u_CursorPosition; // In screen space (pixels)

// Hash-based noise function
float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// Slow varying noise with uniform scale
float animatedNoise(vec2 uv, float time) {
    float staticNoise = rand(floor(uv * 100.0));
    float t = sin(time * 0.2 + staticNoise * 6.2831) * 0.5 + 0.5;
    return t;
}

void main()
{
    float scale = max(iResolution.x, iResolution.y);
    vec2 uv = (gl_FragCoord.xy / scale) * 10.0;

    float alpha = animatedNoise(uv, iTime * 10.0);

    // --- Cursor effect ---
    float dist = distance(vec2(gl_FragCoord.x, iResolution.y - gl_FragCoord.y), u_CursorPosition);
    float radius = scale * 0.2; // 10% of the larger screen dimension
    float intensity = 1.0 - smoothstep(0.0, radius, dist);

    vec3 baseColor = v_color.rgb;
    vec3 cursorHighlight = mix(baseColor, vec3(1.0), intensity);

    color = vec4(cursorHighlight, v_color.a * alpha);
}
