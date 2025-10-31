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

		int lineCount = 5;

		bool wallsDestroyed = false;     
		bool useWallColor = false;               
		ColorF wallColor = ColorF{ 0.3, 0.3, 0.35 };    

		Params() {}
	};


	BombBoxEffect();

	void update(double dt);

	void trigger();

	void reset();

	void draw(const RectF& box, const Params& p);

	bool exploded() const;      

	void drawInst(const RectF& box, const Params& p);

	bool isExploding() const;      
	bool isPulsing()  const;       

	static void drawBatched(
		const PixelShader& ps,
		const Array<RectF>& screenRects,
		const Array<Params>& params,
		const Array<double>& times,
		const Array<double>& explodeTs
	);

	static void drawBatchedExpanded(
	const PixelShader& ps,
	const Array<RectF>& logicRects,      
	const Array<RectF>& drawRects,       
	const Array<BombBoxEffect::Params>& params,
	const Array<double>& times,
	const Array<double>& explodeTs
	);

	double getTime() const { return m_time; }
	double getExplodeT() const { return m_explodeT; }
	bool isInExplode() const { return m_inExplode; }

	const PixelShader& getPixelShader() const { return m_ps; }

private:
	PixelShader m_ps;
	struct UBO {
		Float4 rt;      
		Float4 ch;     
		Float4 pp;       
		Float4 sd;       
		Float4 col;
	};
	ConstantBuffer<UBO> m_cb;

	double m_time = 0.0;
	double m_pulseT = 0.0;
	int    m_pulsesDone = 0;
	double m_explodeT = 0.0;
	bool   m_inExplode = false;
	bool   m_finished = false;
};
