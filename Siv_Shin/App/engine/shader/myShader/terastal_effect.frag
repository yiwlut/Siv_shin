#version 410
layout(location=0) in vec4 Color;
layout(location=1) in vec2 UV;
layout(location=0) out vec4 FragColor;

uniform sampler2D Texture0;
layout(std140) uniform PSConstants2D { vec4 g_colorAdd; vec4 g_sdfParam; vec4 g_sdfOutlineColor; vec4 g_sdfShadowColor; vec4 g_internal; };
layout(std140) uniform TeraCB1 { vec3 u_color; float u_intensity; };
layout(std140) uniform TeraCB2 { float u_size; float u_reflSpeed; float u_fresnel; float u_time; };
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

float hash(vec2 p) { return fract(sin(dot(p, vec2(12.9898,78.233))) * 43758.5453); }

void main()
{
    vec4 base = (u_hasTexture > 0.5) ? texture(Texture0, UV) : vec4(1.0);
    if (u_hasTexture > 0.5 && base.a < 0.01) { discard; }
    if (u_intensity < 0.01) { FragColor = base; return; }

    vec2 U = useUV(UV) * u_size;
    vec2 cell = floor(U);
    float h = hash(cell);
    float reflect = 0.5 + 0.5 * sin(u_time * u_reflSpeed + h * 6.283185);
    float fx = abs(fract(U.x) - 0.5);
    float fy = abs(fract(U.y) - 0.5);
    float facet = pow(1.0 - min(fx, fy) * 2.0, u_fresnel);

    vec3 crystal = u_color * (0.6 + 0.4 * reflect) + vec3(facet * 0.15);
    vec3 rgb = mix(base.rgb, crystal, u_intensity);
    FragColor = vec4(rgb, base.a);
}
