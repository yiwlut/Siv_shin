#include "TerastalShader.hpp"

TerastalShader::TerastalShader()
{
	m_vs = VertexShader{ GLSL{ Resource(U"engine/shader/myShader/terastal_effect.vert"),
		{{U"VSConstants2D", 0}} } };

	m_ps = PixelShader{ GLSL{ Resource(U"engine/shader/myShader/terastal_effect.frag"),
		{{U"PSConstants2D", 0}, {U"TeraCB1", 1}, {U"TeraCB2", 2}, {U"DrawMode", 3}, {U"ShapeInfo", 4}} } };
}

void TerastalShader::updateEffectCBs() const
{
	m_cb1->color = Float3{ (float)m_color.r,(float)m_color.g,(float)m_color.b };
	m_cb1->intensity = m_intensity;

	m_cb2->size = m_size;
	m_cb2->reflSpeed = m_reflSpeed;
	m_cb2->fresnel = m_fresnel;
	m_cb2->time = static_cast<float>(Scene::Time());

	Graphics2D::SetPSConstantBuffer(1, m_cb1);
	Graphics2D::SetPSConstantBuffer(2, m_cb2);
}
