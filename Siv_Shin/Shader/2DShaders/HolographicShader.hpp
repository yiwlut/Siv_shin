// HolographicShader.hpp
#pragma once
#include <Siv3D.hpp>
#include "../Interface/Shader2DBase.hpp"


class HolographicShader final : public Shader2DBase
{
public:
	HolographicShader();

	void setIntensity(float v) noexcept { m_intensity = Clamp(v, 0.0f, 1.0f); }
	void setSpeed(float v) noexcept { m_speed = Max(0.0f, v); }
	void setScale(float v) noexcept { m_scale = Max(0.0f, v); }
	void setRainbowMode(bool v) noexcept { m_rainbow = v; }
	void setHoloColor(const ColorF& c) noexcept { m_color = c; }

	float getIntensity() const noexcept { return m_intensity; }
	float getSpeed() const noexcept { return m_speed; }
	float getScale() const noexcept { return m_scale; }
	bool  isRainbow() const noexcept { return m_rainbow; }
	ColorF getHoloColor() const noexcept { return m_color; }

protected:
	void updateEffectCBs() const override;

private:
	struct Params1 { float time, intensity, rainbow, speed; };
	struct Params2 { Float3 holoColor; float scale; };

	mutable ConstantBuffer<Params1> m_cb1;
	mutable ConstantBuffer<Params2> m_cb2;

	float  m_intensity = 0.7f;
	float  m_speed = 1.0f;
	float  m_scale = 10.0f;
	bool   m_rainbow = true;
	ColorF m_color = ColorF{ 0.5, 0.8, 1.0 };
};
