#pragma once
#include <Siv3D.hpp>

class CustomCamera2D : public BasicCamera2D
{
public:
	CustomCamera2D() = default;

	explicit CustomCamera2D(const Vec2& center, double scale = 1.0)
		: BasicCamera2D(center, scale)
		, m_targetPos(center)
		, m_targetScale(scale)
		, m_perlinNoise(RandomUint32()) {
	} 
	void setTargetPos(const Vec2& pos)
	{
		m_targetPos = pos;
	}

	void jumpToPos(const Vec2& pos)
	{
		m_targetPos = pos;
		setCenter(pos);
	}
	void setTargetScale(double scale)
	{
		m_targetScale = Math::Clamp(scale, 0.1, 10.0);
	}

	void jumpToScale(double scale)
	{
		scale = Math::Clamp(scale, 0.1, 10.0);
		m_targetScale = scale;
		setScale(scale);
	}
	void shake(double duration, double intensity)
	{
		m_shakeDuration = duration;
		m_shakeIntensity = intensity;
		m_shakeTime = 0.0;
	}
	void update()
	{
		const Vec2 currentCenter = getCenter();
		const Vec2 newCenter = Math::Lerp(currentCenter, m_targetPos, lerpSpeed);
		setCenter(newCenter);
		const double currentScale = getScale();
		const double newScale = Math::Lerp(currentScale, m_targetScale, lerpSpeed);
		setScale(newScale);
		updateShake();
	}
	Transformer2D createTransformer() const
	{
		const Vec2 center = getCenter() + m_shakeOffset;
		const double scale = getScale();

		const Mat3x2 mat = Mat3x2::Identity()
			.scaled(scale)
			.translated(Scene::Center() - center * scale);

		return Transformer2D{ mat, TransformCursor::Yes };
	}
	double lerpSpeed = 0.1;

private:
	Vec2 m_targetPos = Scene::Center();
	double m_targetScale = 1.0;
	double m_shakeDuration = 0.0;
	double m_shakeIntensity = 0.0;
	double m_shakeTime = 0.0;
	Vec2 m_shakeOffset{ 0, 0 };
	PerlinNoise m_perlinNoise;

	void updateShake()
	{
		if (m_shakeDuration > 0.0)
		{
			m_shakeDuration -= Scene::DeltaTime();
			m_shakeTime += Scene::DeltaTime();

			const double frequency = 25.0;
			m_shakeOffset.x = (m_perlinNoise.noise1D(m_shakeTime * frequency) * 2.0 - 1.0)
				* m_shakeIntensity;
			m_shakeOffset.y = (m_perlinNoise.noise1D(100.0 + m_shakeTime * frequency) * 2.0 - 1.0)
				* m_shakeIntensity;
			const double dampening = Math::Max(0.0, m_shakeDuration / 0.5);
			m_shakeOffset *= dampening;
		}
		else
		{
			m_shakeOffset = Vec2::Zero();
		}
	}
};
