#version 410
layout(location=0) in vec4 Color;
layout(location=1) in vec2 UV;
layout(location=0) out vec4 FragColor;

uniform sampler2D Texture0;
layout(std140) uniform PSConstants2D { vec4 g_colorAdd; vec4 g_sdfParam; vec4 g_sdfOutlineColor; vec4 g_sdfShadowColor; vec4 g_internal; };
layout(std140) uniform HoloParams1 { float u_time; float u_intensity; float u_useRainbow; float u_speed; };
layout(std140) uniform HoloParams2 { vec3 u_holoColor; float u_scale; };
layout(std140) uniform DrawMode  { float u_hasTexture; vec3 _pad_dm; };
layout(std140) uniform ShapeInfo { vec4  u_shapeRect;  vec2 u_viewSize; vec2 _pad_si; };

vec2 globalScreenUV()
{
    // gl_FragCoord는 하단 원점 → top-left로 변환
    vec2 scrTopLeft = vec2(gl_FragCoord.x, u_viewSize.y - gl_FragCoord.y);
    return clamp(scrTopLeft / u_viewSize, 0.0, 1.0);
}

// Texture/Shape 경로 통합 - 모두 전역 화면 UV 사용
vec2 useUV(vec2 texUV)
{
    // Paint Spread는 전역 originPoint를 사용하므로
    // Texture든 Shape든 전역 화면 좌표계를 사용해야 함
    return globalScreenUV();
}

vec3 rainbow(float t)
{
    t = fract(t);
    float r = abs(t * 6.0 - 3.0) - 1.0;
    float g = 2.0 - abs(t * 6.0 - 2.0);
    float b = 2.0 - abs(t * 6.0 - 4.0);
    return clamp(vec3(r, g, b), 0.0, 1.0);
}

void main()
{
    vec4 base = (u_hasTexture > 0.5) ? texture(Texture0, UV) : vec4(1.0);
    if (u_hasTexture > 0.5 && base.a < 0.01) { discard; }

    if (u_intensity < 0.01) { FragColor = base; return; }

    vec2 U = useUV(UV);
    float timeOffset = u_time * u_speed * 0.1;
    float t = (U.x + U.y) * 0.2 + timeOffset;

    float wave1 = sin((U.x + timeOffset) * 6.2831);
    float wave2 = cos((U.y - timeOffset) * 6.2831 * 0.8);
    float waveMix = (wave1 + wave2) * 0.05;
    t = fract(t + waveMix);

    vec3 holoColor = (u_useRainbow > 0.5)
        ? rainbow(t)
        : (u_holoColor * (0.4 + (sin(t * 6.283185 + u_time * u_speed * 0.5) * 0.5 + 0.5) * 0.6));

    vec2 centered = U - 0.5;
    float edgeDist = length(centered);
    float fresnel = pow(max(0.0, 1.0 - edgeDist * 2.0), 2.0);
    holoColor += vec3(fresnel * 0.12);

    float pulse = sin(u_time * u_speed * 2.0) * 0.05 + 0.95;
    holoColor *= pulse;

    vec3 rgb = mix(base.rgb, holoColor, u_intensity);
    FragColor = vec4(rgb, base.a);
}
