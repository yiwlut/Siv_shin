# include "BombBoxEffect.hpp"

BombBoxEffect::BombBoxEffect() {
	m_ps = GLSL{
		Resource(U"engine/shader/myShader/bomb_box.frag"),
		{ { U"PSConstants2D", 0 }, { U"BombParams", 1 } }
	};
	if (!m_ps) {
		throw Error{ U"engine/shader/myShader/bomb_box.frag" };
	}
}

void BombBoxEffect::update(double dt) {
	if (m_finished) return;

	m_time += dt;

	if (!m_inExplode) {
		m_pulseT += dt;
	}
	else {
		m_explodeT += dt;
	}
}

void BombBoxEffect::trigger() {
	if (!m_inExplode) {
		m_inExplode = true;
		m_explodeT = 0.0;
	}
}

void BombBoxEffect::reset() {
	m_time = 0.0;
	m_pulseT = 0.0;
	m_pulsesDone = 0;
	m_explodeT = 0.0;
	m_inExplode = false;
	m_finished = false;
}

bool BombBoxEffect::exploded() const {
	return m_finished;
}

bool BombBoxEffect::isExploding() const
{
	return m_inExplode && !m_finished;
}

bool BombBoxEffect::isPulsing() const
{
	return (!m_inExplode) && (!m_finished);
}



void BombBoxEffect::draw(const RectF& boxScreenTL, const Params& p)
{
	if (!m_inExplode) {
		if (m_pulseT >= p.pulseDuration) {
			m_pulseT = 0.0;
			++m_pulsesDone;
			if (m_pulsesDone >= static_cast<int>(Math::Round(p.pulseCount))) {
				m_inExplode = true;
				m_explodeT = 0.0;
			}
		}
	}
	else if (!m_finished) {
		if (m_explodeT >= p.explodeTime) {
			m_explodeT = p.explodeTime;
			m_finished = true;
		}
	}

	const Size sceneSize = Scene::Size();

	const Vec2 centerTL = boxScreenTL.center();
	const Vec2 halfSize = boxScreenTL.size * 0.5;

	UBO u;
	u.rt = Float4{
		static_cast<float>(sceneSize.x),
		static_cast<float>(sceneSize.y),
		static_cast<float>(m_time),
		m_inExplode ? static_cast<float>(m_explodeT / p.explodeTime) : 0.0f
	};
	u.ch = Float4{
		static_cast<float>(centerTL.x),
		static_cast<float>(centerTL.y),
		static_cast<float>(halfSize.x),
		static_cast<float>(halfSize.y)
	};
	u.pp = Float4{ p.pulseAmp, p.pulseSpeed, p.spread, p.gravity };

	float tintMode = p.useWallColor ? 1.0f : 0.0f;
	u.sd = Float4{ p.seed, 0.0f, tintMode, 0.0f };
	u.col = Float4{
		static_cast<float>(p.wallColor.r),
		static_cast<float>(p.wallColor.g),
		static_cast<float>(p.wallColor.b),
		0.0f
	};

	m_cb = u;

	const ScopedCustomShader2D shader{ m_ps };
	Graphics2D::SetPSConstantBuffer(1, m_cb);

	const Transformer2D identity{ Mat3x2::Identity(), TransformCursor::Yes };
	Rect{ Scene::Rect() }.draw(ColorF{ 0.0, 0.0 });
}




void BombBoxEffect::drawInst(const RectF& box, const Params& p) {
	const Size sceneSize = Scene::Size();
	const Vec2 centerTL = box.center();
	const Vec2 halfSize = box.size * 0.5;

	UBO u;
	u.rt = Float4{
		static_cast<float>(sceneSize.x),
		static_cast<float>(sceneSize.y),
		static_cast<float>(m_time),
		p.explodeTime
	};

	u.ch = Float4{
		static_cast<float>(centerTL.x),
		static_cast<float>(centerTL.y),
		static_cast<float>(halfSize.x),
		static_cast<float>(halfSize.y)
	};

	u.pp = Float4{
		p.pulseAmp,
		p.pulseSpeed,
		p.spread,
		p.gravity
	};

	u.sd = Float4{ p.seed, 0.0f, 0.0f, 0.0f };

	m_cb = u;

	const ScopedCustomShader2D shader{ m_ps };
	Graphics2D::SetPSConstantBuffer(1, m_cb);
	Rect{ Scene::Rect() }.draw(ColorF{ 0.0, 0.0 });
}


void BombBoxEffect::drawBatched(
	const PixelShader& ps,
	const Array<RectF>& screenRects,
	const Array<Params>& params,
	const Array<double>& times,
	const Array<double>& explodeTs
)
{
	if (screenRects.isEmpty() || !ps) return;

	const Size sceneSize = Scene::Size();
	const ScopedCustomShader2D shader{ ps };
	const Transformer2D identity{ Mat3x2::Identity(), TransformCursor::Yes };

	for (size_t i = 0; i < screenRects.size(); ++i)
	{
		const RectF& rect = screenRects[i];
		const Params& p = params[i];
		const double time = times[i];
		const double explodeT = explodeTs[i];

		const Vec2 centerTL = rect.center();
		const Vec2 halfSize = rect.size * 0.5;

		UBO u;
		u.rt = Float4{
			static_cast<float>(sceneSize.x),
			static_cast<float>(sceneSize.y),
			static_cast<float>(time),
			static_cast<float>(explodeT)
		};
		u.ch = Float4{
			static_cast<float>(centerTL.x),
			static_cast<float>(centerTL.y),
			static_cast<float>(halfSize.x),
			static_cast<float>(halfSize.y)
		};
		u.pp = Float4{
			p.pulseAmp,
			p.pulseSpeed,
			p.spread,
			p.gravity
		};

		float tintMode = p.useWallColor ? 1.0f : 0.0f;
		u.sd = Float4{ p.seed, 0.0f, tintMode, 0.0f };
		u.col = Float4{
			static_cast<float>(p.wallColor.r),
			static_cast<float>(p.wallColor.g),
			static_cast<float>(p.wallColor.b),
			0.0f
		};

		ConstantBuffer<UBO> cb = u;
		Graphics2D::SetPSConstantBuffer(1, cb);

		rect.draw(ColorF{ 0.0, 0.0 });
	}
}


void BombBoxEffect::drawBatchedExpanded(
	const PixelShader& ps,
	const Array<RectF>& logicRects,
	const Array<RectF>& drawRects,
	const Array<Params>& params,
	const Array<double>& times,
	const Array<double>& explodeTs
) {
	if (logicRects.isEmpty() || drawRects.isEmpty() || !ps) return;
	const Size sceneSize = Scene::Size();
	const ScopedCustomShader2D shader{ ps };
	const Transformer2D identity{ Mat3x2::Identity(), TransformCursor::Yes };

	for (size_t i = 0; i < logicRects.size(); ++i) {
		const RectF& logic = logicRects[i];
		const RectF& drawR = drawRects[i];
		const Params& p = params[i];

		UBO u;
		u.rt = Float4{ (float)sceneSize.x, (float)sceneSize.y,
					   (float)times[i], (float)explodeTs[i] };



		const Vec2 logicCenter = logic.center();
		const Vec2 logicHalf = logic.size * 0.5;
		u.ch = Float4{ (float)logicCenter.x, (float)logicCenter.y,
					   (float)logicHalf.x,   (float)logicHalf.y };

		u.pp = Float4{ p.pulseAmp, p.pulseSpeed, p.spread, p.gravity };
		const float tintMode = p.useWallColor ? 1.0f : 0.0f;
		u.sd = Float4{ p.seed, 0.0f, tintMode, 0.0f };
		u.col = Float4{ (float)p.wallColor.r, (float)p.wallColor.g,
						(float)p.wallColor.b, 0.0f };

		u.sd = Float4{ p.seed, 0.0f, tintMode, (float)Clamp(p.lineCount, 2, 8) };

		ConstantBuffer cb = u;
		Graphics2D::SetPSConstantBuffer(1, cb);

		drawR.draw(ColorF{ 0.0, 0.0 });
	}
}
