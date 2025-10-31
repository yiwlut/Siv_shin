// noise.hlsl
Texture2D g_texture0 : register(t0);
SamplerState g_sampler0 : register(s0);

cbuffer PSConstants2D : register(b0)
{
    float4 g_BaseColor;
    float4 g_ScreenSize;
    float4 g_Time;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
};

float hue2rgb(float t)
{
    t = frac(t);
    return saturate(abs(frac(t) * 6.0 - 3.0) - 1.0);
}

float3 hsv2rgb(float h, float s, float v)
{
    float3 rgb;
    rgb.r = hue2rgb(h + 0.0);
    rgb.g = hue2rgb(h + 0.3333);
    rgb.b = hue2rgb(h + 0.6667);
    rgb = rgb * s + (1.0 - s);
    return rgb * v;
}

float4 PS(PSInput input) : SV_TARGET
{
    float4 col = g_texture0.Sample(g_sampler0, input.uv);

    float t = g_Time.x * 0.5f;
    float wave = sin(input.position.x * 0.04f + t) * 0.5f + 0.5f;
    float hue = frac(input.position.y * 0.004f + wave + t);
    float3 rainbow = hsv2rgb(hue, 1, 1);

    float3 final = lerp(col.rgb, rainbow, 0.8f);
    return float4(final, col.a) * input.color;
}
