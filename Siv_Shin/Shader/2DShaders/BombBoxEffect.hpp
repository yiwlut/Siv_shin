#pragma once
# include <Siv3D.hpp>

class BombBoxEffect {
public:
	struct Params {
		double pulseDuration = 2.0;
		double pulseCount = 3.0;
		double explodeTime = 0.9;
		float  pulseAmp = 0.08f;
		float  pulseSpeed = 5.0f;
		float  spread = 220.0f;
		float  gravity = 800.0f;
		float  seed = 0.37f;

		int lineCount = 8;

		//float  progress = 0.0f;
		//float  timeSeconds = 0.0f;

		//ColorF baseColor = ColorF{ 0.08, 0.08, 0.08 };

		bool wallsDestroyed = false; // true면 벽 파괴 모드
		bool useWallColor = false;           // true면 벽 색상 사용
		ColorF wallColor = ColorF{ 0.3, 0.3, 0.35 };  // 벽 색상

		Params() {}
	};


	BombBoxEffect();

	// 매 프레임 갱신
	void update(double dt);

	// 폭발 트리거 (즉시 폭발 시작)
	void trigger();

	// 현재 상태를 초기화
	void reset();

	// 박스를 그린다 (RectF 기준, Top-left 좌표계)
	void draw(const RectF& box, const Params& p);

	// 진행도 쿼리
	bool exploded() const;   // 폭발 연출이 끝났는가

	void drawInst(const RectF& box, const Params& p);

	bool isExploding() const;   // 폭발(파편) 구간: true
	bool isPulsing()  const;    // 점등 구간: true

	// 배칭용 정적 렌더 함수 추가
	static void drawBatched(
		const PixelShader& ps,
		const Array<RectF>& screenRects,
		const Array<Params>& params,
		const Array<double>& times,
		const Array<double>& explodeTs
	);

	static void drawBatchedExpanded(
	const PixelShader& ps,
	const Array<RectF>& logicRects,   // 논리 박스(분해 기준)
	const Array<RectF>& drawRects,    // 실제 그릴 AABB(확장)
	const Array<BombBoxEffect::Params>& params,
	const Array<double>& times,
	const Array<double>& explodeTs
	);

	// 시간 정보 getter 추가
	double getTime() const { return m_time; }
	double getExplodeT() const { return m_explodeT; }
	bool isInExplode() const { return m_inExplode; }

	const PixelShader& getPixelShader() const { return m_ps; }

private:
	PixelShader m_ps;
	struct UBO {
		Float4 rt;   // resolution.xy, time, explodeT
		Float4 ch;   // centerTL.xy, halfSize.xy
		Float4 pp;   // pulseAmp, pulseSpeed, spread, gravity
		Float4 sd;   // seed, pad, pad, pad
		Float4 col;
	};
	ConstantBuffer<UBO> m_cb;

	// 타임라인
	double m_time = 0.0;
	double m_pulseT = 0.0;
	int    m_pulsesDone = 0;
	double m_explodeT = 0.0;
	bool   m_inExplode = false;
	bool   m_finished = false;
};
