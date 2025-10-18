# include "BombBoxEffect.hpp"

BombBoxEffect::BombBoxEffect() {
	m_ps = GLSL{
		U"engine/shader/myShader/bomb_box.frag",
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

void BombBoxEffect::draw(const RectF& box, const Params& p) {
	// 타임라인: pulse -> explode -> finish
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
	const Vec2 centerTL = box.center();
	const Vec2 halfSize = box.size * 0.5;


	UBO u;
	u.rt = Float4{ static_cast<float>(sceneSize.x),
				   static_cast<float>(sceneSize.y),
				   static_cast<float>(m_time),
				   static_cast<float>(p.explodeTime <= 0.0 ? 0.0 : (m_explodeT / p.explodeTime)) };

	u.ch = Float4{ static_cast<float>(centerTL.x),
				   static_cast<float>(centerTL.y),
				   static_cast<float>(halfSize.x),
				   static_cast<float>(halfSize.y) };

	// 폭발 전에는 pulseSpeed 사용, 폭발 중에는 유지
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

void BombBoxEffect::drawInst(const RectF& box, const Params& p)
{
	const Size sceneSize = Scene::Size();
	const Vec2 centerTL = box.center();
	const Vec2 halfSize = box.size * 0.5;

	UBO u;
	u.rt = Float4{ (float)sceneSize.x, (float)sceneSize.y, p.timeSeconds, Clamp(p.progress, 0.0f, 1.0f) };
	u.ch = Float4{ (float)centerTL.x, (float)centerTL.y, (float)halfSize.x, (float)halfSize.y };
	u.pp = Float4{ p.pulseAmp, p.pulseSpeed, p.spread, p.gravity };
	u.sd = Float4{ p.seed, 0.0f, 0.0f, 0.0f }; // minThickness를 sd.y로 쓰는 경우 여기 채움
	u.col = Float4{ (float)p.baseColor.r, (float)p.baseColor.g, (float)p.baseColor.b, (float)p.baseColor.a }; // [추가]

	m_cb = u;
	const ScopedCustomShader2D shader{ m_ps };
	Graphics2D::SetPSConstantBuffer(1, m_cb);
	Rect{ Scene::Rect() }.draw(ColorF{ 0.0 }); // FS 전화면 패스
}
