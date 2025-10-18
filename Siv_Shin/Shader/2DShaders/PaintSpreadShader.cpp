// PaintSpreadShader.cpp
#include "PaintSpreadShader.hpp"

// 효과 파라미터 UBO
struct PaintParams1CB
{
	float time;
	float progress;
	float spreadSpeed;
	float noiseScale;
};

struct PaintParams2CB
{
	Float2 originPoint;
	Float2 _pad;
	Float3 paintColor;
	float  waveStrength;
};

PaintSpreadShader::PaintSpreadShader()
{
	// VS/PS 로드
	m_vs = VertexShader{ GLSL{
		U"engine/shader/myShader/paint_spread.vert",
		{ { U"VSConstants2D", 0 } }
	} };

	// PS 바인딩: 0=PSConstants2D, 1=PaintParams1, 2=PaintParams2, 3=DrawMode, 4=ShapeInfo
	m_ps = PixelShader{ GLSL{
		U"engine/shader/myShader/paint_spread.frag",
		{ { U"PSConstants2D", 0 }, { U"PaintParams1", 1 }, { U"PaintParams2", 2 }, { U"DrawMode", 3 }, { U"ShapeInfo", 4 } }
	} };
}

void PaintSpreadShader::updateEffectCBs() const
{
	// 스레드 로컬 상수 버퍼 (필요 시 재사용)
	static thread_local ConstantBuffer<PaintParams1CB> cb1;
	static thread_local ConstantBuffer<PaintParams2CB> cb2;

	PaintParams1CB p1{};
	p1.time = m_time;
	p1.progress = m_time;       // 동일 의미로 사용 (0~1)
	p1.spreadSpeed = m_spreadSpeed;
	p1.noiseScale = m_noiseScale;

	PaintParams2CB p2{};
	p2.originPoint = Float2{ (float)m_origin.x, (float)m_origin.y };
	p2._pad = Float2{ 0.0f, 0.0f };
	p2.paintColor = Float3{ (float)m_color.r, (float)m_color.g, (float)m_color.b };
	p2.waveStrength = m_wave;

	cb1->time = p1.time;
	cb1->progress = p1.progress;
	cb1->spreadSpeed = p1.spreadSpeed;
	cb1->noiseScale = p1.noiseScale;

	cb2->originPoint = p2.originPoint;
	cb2->_pad = p2._pad;
	cb2->paintColor = p2.paintColor;
	cb2->waveStrength = p2.waveStrength;

	Graphics2D::SetPSConstantBuffer(1, cb1);
	Graphics2D::SetPSConstantBuffer(2, cb2);
}

// ============== 색 지정 도형 경로(추가 오버로드) ==============

void PaintSpreadShader::draw(const RectF& s, const ColorF& color) const
{
	setShapeInfo(s);
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const Circle& s, const ColorF& color) const
{
	setShapeInfo(s.boundingRect());
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const Triangle& s, const ColorF& color) const
{
	setShapeInfo(s.boundingRect());
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const Ellipse& s, const ColorF& color) const
{
	setShapeInfo(s.boundingRect());
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const Quad& s, const ColorF& color) const
{
	setShapeInfo(s.boundingRect());
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const RoundRect& s, const ColorF& color) const
{
	setShapeInfo(s.rect);
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const Polygon& s, const ColorF& color) const
{
	setShapeInfo(s.boundingRect());
	beginShape();
	s.draw(color);
	end();
}

void PaintSpreadShader::draw(const LineString& s, double thickness, const ColorF& color) const
{
	setShapeInfo(computeAABB(s, thickness));
	beginShape();
	s.draw(LineStyle::Default, thickness, color);
	end();
}
