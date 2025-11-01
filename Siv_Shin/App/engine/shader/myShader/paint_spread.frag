//-----------------------------------------------
// engine/shader/myShader/paint_spread.frag
//-----------------------------------------------
#version 410

layout(location = 0) in vec4 Color;
layout(location = 1) in vec2 UV;
layout(location = 0) out vec4 FragColor;

uniform sampler2D Texture0;

layout(std140) uniform PSConstants2D
{
    vec4 g_colorAdd;
    vec4 g_sdfParam;
    vec4 g_sdfOutlineColor;
    vec4 g_sdfShadowColor;
    vec4 g_internal;
};

layout(std140) uniform PaintParams1
{
    float u_time;
    float u_progress;    
    float u_spreadSpeed;  
    float u_noiseScale;   
};

layout(std140) uniform PaintParams2
{
    vec2  u_originPoint;   
    vec2  _padding0;
    vec3  u_paintColor;    
    float u_waveStrength;  
};

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

vec2 globalScreenUV()
{
    vec2 scrTopLeft = vec2(gl_FragCoord.x, u_viewSize.y - gl_FragCoord.y);
    return clamp(scrTopLeft / u_viewSize, 0.0, 1.0);
}

vec2 useUV(vec2 texUV)
{
    return globalScreenUV();
}

void main()
{
    vec4 base = (u_hasTexture > 0.5) ? texture(Texture0, UV) : vec4(1.0);
    if (u_hasTexture > 0.5 && base.a < 0.01) { discard; }

    vec2 U = useUV(UV);

    vec2 diff = U - u_originPoint;
    float dist  = length(diff);
    float angle = atan(diff.y, diff.x);

    vec2 warpedUV = domainWarp(U * u_noiseScale, u_time);
    float noise1 = fbm(warpedUV + u_time * 0.20, 5);
    float noise2 = fbm(warpedUV * 2.0 - u_time * 0.15, 4);
    float noise3 = fbm(warpedUV * 0.5 + u_time * 0.10, 3);
    float angleNoise = sin(angle * 3.0 + u_time) * 0.5 + 0.5;

    float combinedNoise = noise1 * 0.5 + noise2 * 0.3 + noise3 * 0.2;
    combinedNoise = combinedNoise * 0.7 + angleNoise * 0.3;

    float wavePattern = sin((dist - u_progress * 2.0) * 15.0 + combinedNoise * 5.0) * 0.5 + 0.5;
    wavePattern = pow(wavePattern, 2.0); 

    float spreadRadius = u_progress * 1.5;
    float noiseOffset  = (combinedNoise - 0.5) * u_waveStrength;
    float adjustedRadius = spreadRadius + noiseOffset;

    float edgeSoftness = 0.08;

    float paintMask = 1.0 - smoothstep(adjustedRadius - edgeSoftness,
                                       adjustedRadius + edgeSoftness,
                                       dist);

    float edgeDistance = abs(dist - adjustedRadius);
    float edgeWave = exp(-edgeDistance * 15.0) * wavePattern * 0.4;

    for (int i = 0; i < 3; i++)
    {
        float waveOffset = float(i) * 0.15;
        float waveDist = abs(dist - (adjustedRadius - waveOffset));
        edgeWave += exp(-waveDist * 20.0) * wavePattern * (0.3 - float(i) * 0.1);
    }

    paintMask = clamp(paintMask + edgeWave, 0.0, 1.0);

    float depthEffect = paintMask * 0.95;
    vec3 darkPaint = u_paintColor * depthEffect;

    vec3 baseRGB = base.rgb;
    float outAlpha = base.a;
    if (u_hasTexture < 0.5) {
        baseRGB *= Color.rgb;
        outAlpha *= Color.a;
    }

    vec3 finalColor = mix(baseRGB, darkPaint, paintMask);

    finalColor += vec3(edgeWave * 0.3);

    if (u_hasTexture < 0.5 && paintMask < 0.99) {
        finalColor = mix(Color.rgb * baseRGB, finalColor, paintMask);
    }
    
    FragColor = vec4(finalColor, outAlpha);

}
