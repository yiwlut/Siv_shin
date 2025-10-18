// HolographicShader.cpp
#include "HolographicShader.hpp"

HolographicShader::HolographicShader()
{
	m_vs = VertexShader{ GLSL{ U"engine/shader/myShader/holographic_time.vert",
		{{U"VSConstants2D", 0}} } };

	m_ps = PixelShader{ GLSL{ U"engine/shader/myShader/holographic_time.frag",
		{{U"PSConstants2D", 0}, {U"HoloParams1", 1}, {U"HoloParams2", 2}, {U"DrawMode", 3}, {U"ShapeInfo", 4}} } };
}

void HolographicShader::updateEffectCBs() const
{
	const float t = static_cast<float>(Scene::Time());

	m_cb1->time = t;
	m_cb1->intensity = m_intensity;
	m_cb1->rainbow = m_rainbow ? 1.0f : 0.0f;
	m_cb1->speed = m_speed;

	m_cb2->holoColor = Float3{
		static_cast<float>(m_color.r),
		static_cast<float>(m_color.g),
		static_cast<float>(m_color.b) };
	m_cb2->scale = m_scale;

	Graphics2D::SetPSConstantBuffer(1, m_cb1);
	Graphics2D::SetPSConstantBuffer(2, m_cb2);
}
