#version 410
layout(location = 0) out vec4 FragColor;

layout(std140) uniform BombParams {
    vec4 rt;  // rt.xy=resolution, rt.z=timeSeconds, rt.w=progress(0~1)
    vec4 ch;  // ch.xy=center(TL), ch.zw=halfSize
    vec4 pp;  // pp.x=pulseAmp, pp.y=pulseSpeed, pp.z=spread, pp.w=gravity(또는 unused)
    vec4 sd;  // sd.x=seed, sd.y=minThickness(0=auto)
    vec4 col; // baseColor rgba
};

// ---------- utils ----------
float hash11(float n){ return fract(sin(n) * 43758.5453123); }
float hash21(vec2 p){ float n = dot(p, vec2(127.1, 311.7)); return fract(sin(n) * 43758.5453123); }
vec2  hash22(vec2 p){ float n = sin(dot(p, vec2(127.1, 311.7))); return fract(vec2(262144.0, 32768.0) * n); }
mat2  rot(float a){ float c=cos(a), s=sin(a); return mat2(c,-s,s,c); }
float easeOutCubic(float t){ t=clamp(t,0.0,1.0); float k=1.0-t; return 1.0-k*k*k; }
float rectSDF(vec2 p, vec2 halfSize){
    vec2 d = abs(p) - halfSize;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}
vec3 lerp3(vec3 a, vec3 b, float t){ return a + (b - a) * t; }

// NaN 안전 normalize
vec2 safeNormalize(vec2 v){
    float len = length(v);
    if (len < 1e-5) return vec2(1.0, 0.0);
    return v / len;
}

bool lineIntersect(in vec2 n0, float d0, in vec2 n1, float d1, out vec2 p){
    float det = n0.x * n1.y - n0.y * n1.x;
    if (abs(det) < 1e-5) { p = vec2(0.0); return false; }
    p = vec2((d0 * n1.y - d1 * n0.y) / det, (-d0 * n1.x + d1 * n0.x) / det);
    return true;
}

float halfspaceAlpha(float signedVal, float aa){
    return smoothstep(0.0, aa, signedVal);
}

// *** 수정: 배열 크기를 K로 통일 ***
vec2 inwardDirection(int mask, vec2 N[6], int K){
    vec2 v = vec2(0.0);
    for (int k = 0; k < K; ++k){
        bool needPos = ((mask >> k) & 1) != 0;
        v += needPos ? N[k] : -N[k];
    }
    return safeNormalize(v);
}

// *** 수정: 배열 크기를 K로 통일 ***
float pieceMarginInterior(vec2 pc, int mask, vec2 N[6], float D[6], int K, float sampleR){
    vec2 vIn = inwardDirection(mask, N, K);
    vec2 q0 = pc + vIn * sampleR;
    float m = 1e9;
    for (int k = 0; k < K; ++k){
        float sideVal = dot(N[k], q0) - D[k];
        bool needPos = ((mask >> k) & 1) != 0;
        float signedVal = needPos ? sideVal : -sideVal;
        m = min(m, signedVal);
    }
    return m;
}

void main() {
    vec2  resolution = rt.xy;
    float time       = rt.z;
    float progress   = rt.w;

    vec2 centerTL = ch.xy;
    vec2 p   = gl_FragCoord.xy;
    vec2 pos = p - centerTL;

    float pulseEnd = 0.4;
    float pulsePhase = clamp(progress / pulseEnd, 0.0, 1.0);

    float pulseCount = max(pp.z, 1.0);
    float pulseAmp   = pp.x;
    float pulseSpeed = pp.y;
    float pulse = 1.0 + pulseAmp * sin(time * pulseSpeed * 2.0 + pulsePhase * 6.2831 * pulseCount);

    vec2 halfPre = ch.zw * pulse;
    float aaPre = max(fwidth(halfPre.x + halfPre.y) * 0.25 + 1.0, 1.0);
    float sdPre = rectSDF(pos, halfPre);
    float aPre  = smoothstep(0.0, aaPre, -sdPre);

    if (progress < pulseEnd) {
        if (aPre < 1e-3) discard;
        FragColor = vec4(col.rgb, aPre);
        return;
    }

    float explodePhase = clamp((progress - pulseEnd) / (1.0 - pulseEnd), 0.0, 1.0);

    const int K = 6;
    vec2  N[6];
    float D[6];
    
    for (int i = 0; i < K; ++i){
        float ang = hash11(sd.x * 113.0 + float(i) * 17.0);
        ang *= 6.2831853;
        vec2 n = safeNormalize(vec2(cos(ang), sin(ang)));
        float maxd = dot(abs(n), ch.zw);
        float t = hash11(sd.x * 719.0 + float(i) * 53.0) * 2.0 - 1.0;
        float d = t * maxd * 0.80;
        N[i] = n; D[i] = d;
    }

    float kDisp = 1.0 - pow(max(1.0 - explodePhase, 0.0), 3.0);
    float kRot  = easeOutCubic(explodePhase);

    // *** 핵심 수정: 박스 크기에 비례하도록 ***
    float boxScale = min(ch.z, ch.w);
    float minThickness = max(boxScale * 0.03, 0.5);  // 최소 0.5px
    float sampleR = boxScale * 0.15;  // 5% → 15%
    float effectiveSpread = pp.z;  // 일단 그대로 사용, 또는 boxScale * 2.5

    float bestA = 0.0;
    vec3  bestCol = vec3(0.0);

    for (int pass = 0; pass < 2; ++pass){
        bestA = 0.0;

        for (int i = 0; i < K; ++i){
            for (int j = i+1; j < K; ++j){
                vec2 pc;
                if (!lineIntersect(N[i], D[i], N[j], D[j], pc)) continue;
                if (any(greaterThan(abs(pc), ch.zw * 1.1))) continue;

                int mask = 0;
                for (int k = 0; k < K; ++k){
                    float side = dot(N[k], pc) - D[k];
                    if (side >= 0.0) mask |= (1 << k);
                }
                if (mask == 0) continue;

                float need = (pass == 0) ? minThickness : (minThickness * 0.2);
                float margin = pieceMarginInterior(pc, mask, N, D, K, sampleR);
                if (margin < need) continue;

                vec2 dir = safeNormalize(pc);
                float angleJitter = (hash11(float(mask) * 0.731 + sd.x) - 0.5) * 0.25;
                dir = safeNormalize(rot(angleJitter) * dir);

                float h = hash11(float(mask) * 37.0 + sd.x * 911.0);
                float speedJit = 0.75 + 0.50 * h;
                float angMax = 3.0;
                float angleRnd = (h * 2.0 - 1.0) * angMax * kRot;

                // *** spread 사용 ***
                vec2 trans = dir * effectiveSpread * kDisp * speedJit;
                vec2 halfPiece = ch.zw * 1.1;

                vec2 q = pos - trans;
                q = rot(-angleRnd) * (q - pc) + pc;

                float sdRect = rectSDF(q, halfPiece);
                float aRect = smoothstep(0.0, aaPre, -sdRect);
                if (aRect <= 0.0) continue;

                float aCuts = 1.0;
                for (int k = 0; k < K; ++k){
                    float sideVal = dot(N[k], q) - D[k];
                    bool needPos = ((mask >> k) & 1) != 0;
                    float signedVal = needPos ? sideVal : -sideVal;
                    float aH = halfspaceAlpha(signedVal, aaPre);
                    aCuts = min(aCuts, aH);
                    if (aCuts <= 0.0) break;
                }

                float aE = aRect * aCuts;
                if (aE > bestA){
                    bestA = aE;

                    vec3 fireCol = lerp3(vec3(0.75, 0.22, 0.05), vec3(0.1, 0.02, 0.02), kRot);
                    fireCol = mix(fireCol, col.rgb, 0.25);
                    float edge = smoothstep(aaPre * 3.0, 0.0, abs(sdRect));
                    fireCol *= (0.9 + 0.1 * edge);
                    float spark = 0.28 * (hash21(pos * 0.7 + sd.xx + time * 2.3) - 0.5);
                    fireCol += vec3(max(0.0, spark));

                    bestCol = fireCol;
                }
            }
        }
        if (bestA > 0.0) break;
    }

    float fadeStart = 0.70;
    float fade = 1.0 - smoothstep(fadeStart, 1.0, explodePhase);

    float a = bestA * fade;
    if (a < 1e-3) discard;
    FragColor = vec4(bestCol, a);
}

