#pragma once
#include <Siv3D.hpp>
#include "../Interface/Shader2DBase.hpp"

class PaintSpreadShader final : public Shader2DBase
{
public:
	PaintSpreadShader();

	// 파라미터 세터
	void setPaintColor(const ColorF& c) noexcept { m_color = c; }
	void setOriginPoint(const Vec2& uv) noexcept { m_origin = uv; }

	// 진행 속도(초당 진행 비율), 파동 강도, 노이즈 스케일
	void setSpreadSpeed(float v) noexcept { m_spreadSpeed = Max(0.0f, v); }
	void setWaveStrength(float v) noexcept { m_wave = Max(0.0f, v); }
	void setNoiseScale(float v) noexcept { m_noiseScale = Max(0.0f, v); }

	// 애니메이션 제어
	void startAnimation() noexcept { m_time = 0.0f; m_done = false; }
	void updateProgress(double dt) noexcept
	{
		if (!m_done)
		{
			m_time += static_cast<float>(dt) * m_spreadSpeed;
			if (m_time >= 1.0f) { m_time = 1.0f; m_done = true; }
		}
	}
	bool  isAnimationComplete() const noexcept { return m_done; }
	float getProgress()         const noexcept { return m_time; }

	// 게터(선택)
	float getSpreadSpeed() const noexcept { return m_spreadSpeed; }
	float getNoiseScale()  const noexcept { return m_noiseScale; }
	float getWaveStrength()const noexcept { return m_wave; }
	ColorF getPaintColor() const noexcept { return m_color; }
	Vec2   getOriginPoint()const noexcept { return m_origin; }

	// 색 지정 도형 경로(추가 오버로드)
	void draw(const RectF& s, const ColorF& color) const;          // RectF
	void draw(const Circle& s, const ColorF& color) const;          // Circle
	void draw(const Triangle& s, const ColorF& color) const;        // Triangle
	void draw(const Ellipse& s, const ColorF& color) const;         // Ellipse
	void draw(const Quad& s, const ColorF& color) const;            // Quad
	void draw(const RoundRect& s, const ColorF& color) const;       // RoundRect
	void draw(const Polygon& s, const ColorF& color) const;         // Polygon
	void draw(const LineString& s, double thickness, const ColorF& color) const; // LineString

private:
	// 효과별 UBO 업데이트(슬롯 1, 2)
	void updateEffectCBs() const override;

	// 파라미터 저장
	ColorF m_color{ 0.2, 0.6, 1.0, 1.0 };
	Vec2   m_origin{ 0.5, 0.0 };
	float  m_spreadSpeed = 1.5f;  // 0.3 → 1.5 (5배 빠르게)
	float  m_wave = 0.4f;
	float  m_noiseScale = 6.0f;

	// 진행 상태
	float m_time = 0.0f;   // 0~1
	bool  m_done = false;
};
