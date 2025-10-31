#pragma once
#include <Siv3D.hpp>
#include "../Interface/Shader2DBase.hpp"


class TerastalShader final : public Shader2DBase
{
public:
	TerastalShader();

	void setCrystalColor(const ColorF& c) noexcept { m_color = c; }
	void setIntensity(float v) noexcept { m_intensity = Clamp(v, 0.0f, 1.0f); }
	void setCrystalSize(float v) noexcept { m_size = Max(0.0f, v); }
	void setReflectionSpeed(float v) noexcept { m_reflSpeed = Max(0.0f, v); }
	void setFresnelPower(float v) noexcept { m_fresnel = Max(0.0f, v); }

protected:
	void updateEffectCBs() const override;

private:
	struct TeraCB1 { Float3 color; float intensity; };
	struct TeraCB2 { float size; float reflSpeed; float fresnel; float time; };

	mutable ConstantBuffer<TeraCB1> m_cb1;
	mutable ConstantBuffer<TeraCB2> m_cb2;

	ColorF m_color{ 1.0, 0.4, 0.2 };
	float  m_intensity = 0.9f;
	float  m_size = 12.0f;
	float  m_reflSpeed = 1.0f;
	float  m_fresnel = 2.5f;
};
