#pragma once
#include <Siv3D.hpp>
#include "../Interface/Shader2DBase.hpp"

class PaintSpreadShader final : public Shader2DBase
{
public:
	PaintSpreadShader();

	void setPaintColor(const ColorF& c) noexcept { m_color = c; }
	void setOriginPoint(const Vec2& uv) noexcept { m_origin = uv; }

	void setSpreadSpeed(float v) noexcept { m_spreadSpeed = Max(0.0f, v); }
	void setWaveStrength(float v) noexcept { m_wave = Max(0.0f, v); }
	void setNoiseScale(float v) noexcept { m_noiseScale = Max(0.0f, v); }

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

	float getSpreadSpeed() const noexcept { return m_spreadSpeed; }
	float getNoiseScale()  const noexcept { return m_noiseScale; }
	float getWaveStrength()const noexcept { return m_wave; }
	ColorF getPaintColor() const noexcept { return m_color; }
	Vec2   getOriginPoint()const noexcept { return m_origin; }

	void draw(const RectF& s, const ColorF& color) const;           
	void draw(const Circle& s, const ColorF& color) const;           
	void draw(const Triangle& s, const ColorF& color) const;         
	void draw(const Ellipse& s, const ColorF& color) const;          
	void draw(const Quad& s, const ColorF& color) const;             
	void draw(const RoundRect& s, const ColorF& color) const;        
	void draw(const Polygon& s, const ColorF& color) const;          
	void draw(const LineString& s, double thickness, const ColorF& color) const;  

private:
	void updateEffectCBs() const override;

	ColorF m_color{ 0.2, 0.6, 1.0, 1.0 };
	Vec2   m_origin{ 0.5, 0.0 };
	float  m_spreadSpeed = 1.5f;       
	float  m_wave = 0.4f;
	float  m_noiseScale = 6.0f;

	float m_time = 0.0f;    
	bool  m_done = false;
};
