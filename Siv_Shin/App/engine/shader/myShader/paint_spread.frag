//-----------------------------------------------
// engine/shader/myShader/paint_spread.frag
// (DrawMode/ShapeInfo 통합 + RectF 방향 교정)
//-----------------------------------------------
#version 410

layout(location = 0) in vec4 Color;
layout(location = 1) in vec2 UV;
layout(location = 0) out vec4 FragColor;

uniform sampler2D Texture0;

// Siv3D PS 공통
layout(std140) uniform PSConstants2D
{
    vec4 g_colorAdd;
    vec4 g_sdfParam;
    vec4 g_sdfOutlineColor;
    vec4 g_sdfShadowColor;
    vec4 g_internal;
};

// 원래 쓰던 파라미터 블록 1
layout(std140) uniform PaintParams1
{
    float u_time;
    float u_progress;     // 0 -> 1
    float u_spreadSpeed;  // (여기서는 시각효과엔 직접 미사용, u_progress 갱신용)
    float u_noiseScale;   // 노이즈 스케일
};

// 원래 쓰던 파라미터 블록 2
layout(std140) uniform PaintParams2
{
    vec2  u_originPoint;    // 시작점 (UV)
    vec2  _padding0;
    vec3  u_paintColor;     // 페인트 색
    float u_waveStrength;   // 물결 강도
};

// DrawMode: 1=Texture 경로, 0=Shape 경로
layout(std140) uniform DrawMode
{
    float u_hasTexture;
    vec3  _pad_dm;
};

// ShapeInfo: rect(x,y,w,h) in screen(top-left), view size
layout(std140) uniform ShapeInfo
{
    vec4  u_shapeRect;
    vec2  u_viewSize;
    vec2  _pad_si;
};

//---------------------- 노이즈 유틸 ----------------------
float hash(vec2 p) { return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); }

float noise2D(vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);

    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float fbm(vec2 p, int octaves)
{
    float value = 0.0;
    float amp = 0.5;
    float freq = 1.0;
    for (int i = 0; i < octaves; i++)
    {
        value += amp * noise2D(p * freq);
        amp  *= 0.5;
        freq *= 2.0;
    }
    return value;
}

vec2 domainWarp(vec2 p, float time)
{
    float warp1 = fbm(p + vec2(time * 0.10,  time * 0.15), 3);
    float warp2 = fbm(p + vec2(-time * 0.12, time * 0.08), 3);
    return p + vec2(warp1, warp2) * 0.5;
}

//---------------------- UV 통합 ----------------------
// Shape의 “raw” UV: top-left 0,0 → bottom-right 1,1
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

//---------------------- 메인 ----------------------
void main()
{
    // 텍스처 경로일 때만 알파 discard
    vec4 base = (u_hasTexture > 0.5) ? texture(Texture0, UV) : vec4(1.0);
    if (u_hasTexture > 0.5 && base.a < 0.01) { discard; }

    // 통합 UV
    vec2 U = useUV(UV);

    // 시작점/거리/각도
    vec2 diff = U - u_originPoint;
    float dist  = length(diff);
    float angle = atan(diff.y, diff.x);

    // 도메인 워핑 + FBM
    vec2 warpedUV = domainWarp(U * u_noiseScale, u_time);
    float noise1 = fbm(warpedUV + u_time * 0.20, 5);
    float noise2 = fbm(warpedUV * 2.0 - u_time * 0.15, 4);
    float noise3 = fbm(warpedUV * 0.5 + u_time * 0.10, 3);
    float angleNoise = sin(angle * 3.0 + u_time) * 0.5 + 0.5;

    float combinedNoise = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;
    combinedNoise = combinedNoise * 0.7 + angleNoise * 0.3;

    // 물결 라인(경계) 패턴
    float wavePattern = sin((dist - u_progress * 2.0) * 15.0 + combinedNoise * 5.0) * 0.5 + 0.5;
    wavePattern = pow(wavePattern, 2.0); // 대비 강화

    // 확산 반경 + 노이즈로 변형
    float spreadRadius = u_progress * 1.5;
    float noiseOffset  = (combinedNoise - 0.5) * u_waveStrength;
    float adjustedRadius = spreadRadius + noiseOffset;

    // 부드러운 경계 (feather)
    float edgeSoftness = 0.08;

    // 채워진 영역(=1)이 되도록 마스크 구성
    float paintMask = 1.0 - smoothstep(adjustedRadius - edgeSoftness,
                                       adjustedRadius + edgeSoftness,
                                       dist);

    // 경계 파문 강조
    float edgeDistance = abs(dist - adjustedRadius);
    float edgeWave = exp(-edgeDistance * 15.0) * wavePattern * 0.4;

    // 추가 물결 (여러 링)
    for (int i = 0; i < 3; i++)
    {
        float waveOffset = float(i) * 0.15;
        float waveDist = abs(dist - (adjustedRadius - waveOffset));
        edgeWave += exp(-waveDist * 20.0) * wavePattern * (0.3 - float(i) * 0.1);
    }

    // 최종 마스크
    // 최종 마스크
    paintMask = clamp(paintMask + edgeWave, 0.0, 1.0);

    // 액체 깊이감 (살짝 어둡게)
    float depthEffect = paintMask * 0.95;
    vec3 darkPaint = u_paintColor * depthEffect;

    // [변경] 도형 경로에서만 베이스에 색 틴트 적용
    vec3 baseRGB = base.rgb;
    float outAlpha = base.a;
    if (u_hasTexture < 0.5) {
        baseRGB *= Color.rgb;      // 도형의 지정 색은 베이스에만 반영
        outAlpha *= Color.a;       // 도형 알파도 필요 시 반영
    }

    // base(베이스) ↔ paint(페인트) 보간
    vec3 finalColor = mix(baseRGB, darkPaint, paintMask);

    // 표면 하이라이트
    finalColor += vec3(edgeWave * 0.3);

    // [수정] 최종 출력: 아웃라인(프레임)은 항상 Color.rgb를 유지
    // paintMask가 적용되지 않은 영역(프레임)은 원본 색상 유지
    if (u_hasTexture < 0.5 && paintMask < 0.99) {
        // 프레임 영역: 원본 색상 유지 (밝게)
        finalColor = mix(Color.rgb * baseRGB, finalColor, paintMask);
    }
    
    FragColor = vec4(finalColor, outAlpha);

}
