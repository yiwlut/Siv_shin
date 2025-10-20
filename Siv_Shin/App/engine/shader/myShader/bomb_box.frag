#version 410
layout(location = 0) out vec4 FragColor;

layout(std140) uniform BombParams {
    vec4 rt;  // rt.xy=resolution, rt.z=time, rt.w=explodeT
    vec4 ch;  // ch.xy=center(TL), ch.zw=halfSize
    vec4 pp;  // pp.x=pulseAmp, pp.y=pulseSpeed, pp.z=spread, pp.w=unused
    vec4 sd;  // sd.x=seed, sd.y=minThickness, sd.z=tintMode(0=bomb,1=wall), sd.w=unused
    vec4 col; // col.rgb = tintColor (벽 색), col.a=unused
};

float hash11(float n){ return fract(sin(n) * 43758.5453123); }        
float hash21(vec2 p){ float n = dot(p, vec2(127.1, 311.7)); return fract(sin(n) * 43758.5453123); } 
vec2  hash22(vec2 p){ float n = sin(dot(p, vec2(127.1, 311.7))); return fract(vec2(262144.0, 32768.0) * n); } 
mat2  rot(float a){ float c=cos(a), s=sin(a); return mat2(c,-s,s,c); } 
float easeOutCubic(float t){ t=clamp(t,0.0,1.0); float k=1.0-t; return 1.0-k*k*k; } 
float saturate(float x){ return clamp(x, 0.0, 1.0); }                  

float rectSDF(vec2 p, vec2 halfSize){
    halfSize = max(halfSize, vec2(1e-3));
    vec2 d = abs(p) - halfSize;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
} 

bool lineIntersect(in vec2 n0, float d0, in vec2 n1, float d1, out vec2 p){
    float det = n0.x * n1.y - n0.y * n1.x;
    if (abs(det) < 1e-5) { p = vec2(0.0); return false; }
    p = vec2((d0 * n1.y - d1 * n0.y) / det, (-d0 * n1.x + d1 * n0.x) / det);
    return true;
} 

float halfspaceAlpha(float signedVal, float aa){
    return smoothstep(0.0, max(aa, 1e-4), signedVal);
} 

float innerEdgeMask(float dInner, float rangePx){
    float r = max(rangePx, 1e-3);
    float x = saturate((-dInner) / r);
    return 1.0 - x;
} 

void main(){
    vec2 resolution = rt.xy;
    float time      = rt.z;
    float explodeT  = rt.w;

    vec2 centerTL = ch.xy;
    //vec2 p = gl_FragCoord.xy; p.y = resolution.y - p.y;
    vec2 p = gl_FragCoord.xy;
    vec2 pos = p - centerTL;                                              

    // 프리(점등) 단계
    float pulse    = 1.0 + pp.x * sin(time * pp.y);
    vec2  halfPre  = ch.zw * pulse;
    vec2  halfFix  = ch.zw;
    float aaPre = fwidth(halfPre.x + halfPre.y) * 0.25 + 1.0;
    float aaExp = fwidth(halfFix.x + halfFix.y) * 0.25 + 1.0;             

    // 색상 램프: 3초 동안 거의 검정→붉은색
    float env = smoothstep(0.0, 3.0, time);
    vec3 nearBlack = vec3(0.18, 0.06, 0.06);
    vec3 redDeep   = vec3(0.22, 0.05, 0.05);
    vec3 redBright = vec3(0.58, 0.12, 0.12);
    float warm = 0.5 + 0.5 * sin(time * 3.73 + sd.x * 6.2831);
    warm *= (0.85 + 0.15 * sin(time * 11.1));
    vec3 redWarm = mix(redDeep, redBright, saturate(0.35 + 0.65 * warm));
    vec3 preFillCol = mix(nearBlack, redWarm, env);                       

    const float framePx = 3.0;
    vec2 halfInnerSafe = max(halfPre - vec2(framePx), vec2(1.0));
    float idleJit = (explodeT <= 0.0) ? 0.75 : 0.0;
    vec2 jitter = (hash22(floor(pos * 0.5) + vec2(sd.x)) - 0.5) * idleJit;
    vec2 posPre = pos + jitter;                                           

    float dOuter = rectSDF(posPre, halfPre);
    float dInner = rectSDF(posPre, halfInnerSafe);
    float aOuter = smoothstep(0.0, aaPre, -dOuter);
    float aInner = smoothstep(0.0, aaPre, -dInner);
    float aFrame = clamp(aOuter - aInner, 0.0, 1.0);
    float aFill  = aInner;                                                

    float edgeMask = innerEdgeMask(dInner, 6.0);
    vec2  uv = posPre / max(halfInnerSafe, vec2(1.0));
    float dirHL = saturate(0.5 + 0.5 * (-uv.x + uv.y));
    vec3  frameCol = vec3(0.24, 0.24, 0.27);
    vec3  fillCol  = preFillCol * (1.0 - 0.10 * (1.0 - edgeMask)) + 0.08 * dirHL * edgeMask; 

    if (explodeT <= 1e-6){
        float a = clamp(aFrame + aFill, 0.0, 1.0);
        if (a < 1e-3) discard;
        vec3 rgb = frameCol * aFrame + fillCol * aFill;
        FragColor = vec4(rgb, a);
        return;
    } 

    // ---------- 폭발 조각 ----------
    vec2  N[10]; float D[10];
    for (int i = 0; i < 10; ++i){
        float ang = hash11(sd.x * 113.0 + float(i) * 17.0) * 6.2831853;
        vec2 n = normalize(vec2(cos(ang), sin(ang)));
        float maxd = dot(abs(n), halfFix);
        float t = hash11(sd.x * 719.0 + float(i) * 53.0) * 2.0 - 1.0;
        float d = t * maxd * 0.80;
        N[i] = n; D[i] = d;
    } 

    float kDisp = 1.0 - pow(1.0 - saturate(explodeT), 3.0);
    float kRot  = easeOutCubic(saturate(explodeT));                       
    float minThickness = (sd.y > 0.0) ? sd.y : 5.0;
    float sampleR = min(halfFix.x, halfFix.y) * 0.10;                     

    float bestA = 0.0; vec3 bestCol = vec3(0.0);
    for (int pass = 0; pass < 2; ++pass){
        bestA = 0.0;
        for (int i = 0; i < 10; ++i){
            for (int j = i+1; j < 10; ++j){
                vec2 pc; if (!lineIntersect(N[i], D[i], N[j], D[j], pc)) continue;
                if (any(greaterThan(abs(pc), halfFix * 1.3))) continue;

                int mask = 0;
                for (int k = 0; k < 10; ++k){ float side = dot(N[k], pc) - D[k]; if (side >= 0.0) mask |= (1 << k); }
                if (mask == 0) continue;

                float need = (pass == 0) ? minThickness : (minThickness * 0.2);
                float margin = 1e9; {
                    vec2 v = vec2(0.0);
                    for (int k = 0; k < 10; ++k){ bool needPos = ((mask >> k) & 1) != 0; v += needPos ? N[k] : -N[k]; }
                    if (dot(v, v) < 1e-6) v = vec2(1.0, 0.0);
                    v = normalize(v);
                    vec2 q0 = pc + v * max(sampleR, 1.0);
                    for (int k = 0; k < 10; ++k){
                        float sideVal = dot(N[k], q0) - D[k];
                        bool needPos = ((mask >> k) & 1) != 0;
                        float signedVal = needPos ? sideVal : -sideVal;
                        margin = min(margin, signedVal);
                    }
                }
                if (margin < need) continue;

                vec2 dir = normalize(pc);
                float angleJitter = (hash11(float(mask) * 0.731 + sd.x) - 0.5) * 0.25;
                mat2 rotJit = rot(angleJitter);
                dir = normalize(rotJit * dir);

                float h = hash11(float(mask) * 37.0 + sd.x * 911.0);
                float speedJit = 0.75 + 0.50 * h;

                float angMax = 3.0;
                float angleRnd = (h * 2.0 - 1.0) * angMax * kRot;

                vec2 trans = dir * pp.z * kDisp * speedJit;

                vec2 halfPiece = halfFix * 1.8;
                vec2 q = pos - trans;
                q = rot(-angleRnd) * (q - pc) + pc;

                float sdRect = rectSDF(q, halfPiece);
                float aRect  = smoothstep(0.0, aaExp, -sdRect);
                if (aRect <= 0.0) continue;

                float aCuts = 1.0;
                for (int k = 0; k < 10; ++k){
                    float sideVal = dot(N[k], q) - D[k];
                    bool needPos = ((mask >> k) & 1) != 0;
                    float signedVal = needPos ? sideVal : -sideVal;
                    float aH = halfspaceAlpha(signedVal, aaExp);
                    aCuts = min(aCuts, aH);
                    if (aCuts <= 0.0) break;
                }

                float aE = aRect * aCuts;
                if (aE > bestA){
                    bestA = aE;

                    // 0=bomb(붉은 불빛), 1=wall(회색 파편)
                    float tintMode = step(0.5, sd.z);
                    vec3 tintCol = col.rgb;

                    // 폭탄용 붉은 조각
                    vec3 fireCol = mix(vec3(0.78, 0.22, 0.08), vec3(0.10, 0.02, 0.02), kRot);
                    float edge = smoothstep(aaExp * 3.0, 0.0, abs(sdRect));
                    fireCol *= (0.90 + 0.10 * edge);
                    float sparkR = 0.28 * (hash21(pos * 0.7 + vec2(sd.x) + time * 2.3) - 0.5);
                    fireCol += vec3(max(0.0, sparkR));

                    // 벽용 회색 조각(벽 색 유지 + 에지 하이라이트)
                    vec3 wallCol = tintCol * (0.90 + 0.10 * edge);
                    float sparkG = 0.18 * (hash21(q * 0.9 + vec2(sd.x * 3.1)) - 0.5);
                    wallCol += vec3(max(0.0, sparkG));

                    bestCol = mix(fireCol, wallCol, tintMode);
                }
            }
        }
        if (bestA > 0.0) break;
    }

    float fadeStart = 0.55;
    float fade = 1.0 - smoothstep(fadeStart, 1.0, saturate(explodeT));
    float a = bestA * fade;
    if (a < 1e-3) discard;
    FragColor = vec4(bestCol, a);
}
