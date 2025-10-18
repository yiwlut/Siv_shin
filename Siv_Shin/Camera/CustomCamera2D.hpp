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
	} // Perlin 인스턴스 초기화

// 특정 위치로 이동
	void setTargetPos(const Vec2& pos)
	{
		m_targetPos = pos;
	}

	void jumpToPos(const Vec2& pos)
	{
		m_targetPos = pos;
		setCenter(pos);
	}

	// 줌 설정
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

	// 카메라 쉐이크
	void shake(double duration, double intensity)
	{
		m_shakeDuration = duration;
		m_shakeIntensity = intensity;
		m_shakeTime = 0.0;
	}

	// 업데이트 (부드러운 이동 + 쉐이크)
	void update()
	{
		// 부드러운 이동
		const Vec2 currentCenter = getCenter();
		const Vec2 newCenter = Math::Lerp(currentCenter, m_targetPos, lerpSpeed);
		setCenter(newCenter);

		// 부드러운 줌
		const double currentScale = getScale();
		const double newScale = Math::Lerp(currentScale, m_targetScale, lerpSpeed);
		setScale(newScale);

		// 쉐이크 업데이트
		updateShake();
	}

	// Transformer 생성 (쉐이크 오프셋 적용)
	Transformer2D createTransformer() const
	{
		const Vec2 center = getCenter() + m_shakeOffset;
		const double scale = getScale();

		const Mat3x2 mat = Mat3x2::Identity()
			.scaled(scale)
			.translated(Scene::Center() - center * scale);

		return Transformer2D{ mat, TransformCursor::Yes };
	}

	// 설정
	double lerpSpeed = 0.1;

private:
	Vec2 m_targetPos = Scene::Center();
	double m_targetScale = 1.0;

	// 쉐이크 관련
	double m_shakeDuration = 0.0;
	double m_shakeIntensity = 0.0;
	double m_shakeTime = 0.0;
	Vec2 m_shakeOffset{ 0, 0 };

	// PerlinNoise 인스턴스
	PerlinNoise m_perlinNoise;

	void updateShake()
	{
		if (m_shakeDuration > 0.0)
		{
			m_shakeDuration -= Scene::DeltaTime();
			m_shakeTime += Scene::DeltaTime();

			const double frequency = 25.0;

			// noise1D() 인스턴스 메서드 사용 (소문자)
			m_shakeOffset.x = (m_perlinNoise.noise1D(m_shakeTime * frequency) * 2.0 - 1.0)
				* m_shakeIntensity;
			m_shakeOffset.y = (m_perlinNoise.noise1D(100.0 + m_shakeTime * frequency) * 2.0 - 1.0)
				* m_shakeIntensity;

			// 감쇠
			const double dampening = Math::Max(0.0, m_shakeDuration / 0.5);
			m_shakeOffset *= dampening;
		}
		else
		{
			m_shakeOffset = Vec2::Zero();
		}
	}
};
