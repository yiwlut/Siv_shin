#include "Player.h"

// 타일 좌표를 해당 타일의 중심 픽셀 좌표로 변환합니다.
s3d::Vec2 Player::calculateCenterPixelPosition(int tileX, int tileY) const
{
	double centerX = (tileX * m_tileSize) + (m_tileSize / 2.0);
	double centerY = (tileY * m_tileSize) + (m_tileSize / 2.0);
	return s3d::Vec2(centerX, centerY);
}

// 초기화: 시작 타일 중앙에 배치 및 애니메이션 프레임 로드
Player::Player(int startTileX, int startTileY, int tileSize, int frameCount) // ⭐️ 시그니처 변경
	: m_tilePosition(startTileX, startTileY)
	, m_tileSize(tileSize) // ⭐️ m_playerTexture 초기화 제거 (m_playerFrames로 대체)
{
	// ⭐️ 애니메이션 프레임 로딩
	for (int32 i = 0; i < frameCount; i++)
	{
		// s3d::Format()을 사용하여 경로 문자열 생성 (안전한 포맷팅)
		s3d::Texture frame{ U"player_{}.webp"_fmt(i) };

		if (!frame.isEmpty())
		{
			m_playerFrames.push_back(frame);
		}
		else
		{
			s3d::Print << U"Error: Failed to load animation frame: " << U"player_{}.webp"_fmt(i);
		}
	}

	// ⭐️ 프레임 로드 실패 시 (한 장도 로드되지 않았을 경우) 대체 텍스처 준비
	if (m_playerFrames.empty())
	{
		s3d::Print << U"Warning: No player animation frames loaded. Using default yellow square.";

		// ✅ 수정: m_tileSize를 size_t로 명시적으로 형 변환합니다.
		m_playerFrames.push_back(s3d::Texture{ s3d::Image{
			static_cast<size_t>(m_tileSize), // width
			static_cast<size_t>(m_tileSize), // height
			s3d::Palette::Yellow}
		});
	}

	m_pixelPosition = calculateCenterPixelPosition(startTileX, startTileY);
	m_targetPixelPosition = m_pixelPosition;
}


// Player의 현재 픽셀 위치를 업데이트합니다. (부드러운 이동 애니메이션 처리)
void Player::update()
{
	// 1. 이동 처리 (기존 코드와 동일)
	if (m_pixelPosition != m_targetPixelPosition)
	{
		double distance = (m_targetPixelPosition - m_pixelPosition).length();
		if (distance <= m_moveSpeed)
		{
			m_pixelPosition = m_targetPixelPosition;
		}
		else
		{
			s3d::Vec2 direction = (m_targetPixelPosition - m_pixelPosition).normalized();
			m_pixelPosition += direction * m_moveSpeed;
		}
	}

	// 2. ⭐️ 애니메이션 프레임 업데이트
	// 프레임이 2장 이상일 때만 애니메이션을 순환시키고, 한 장일 때는 정지합니다.
	if (m_playerFrames.size() > 1)
	{
		// 경과 시간 누적
		m_frameTimer += s3d::Scene::DeltaTime();

		// 프레임 표시 시간을 초과하면 다음 프레임으로 이동
		if (m_frameTimer >= m_frameDuration)
		{
			m_currentFrameIndex = (m_currentFrameIndex + 1) % m_playerFrames.size();
			m_frameTimer = 0.0; // 타이머 리셋
		}
	}
	// 프레임이 1장이거나 0장이면 (대체 텍스처 사용 시) m_currentFrameIndex는 0으로 유지됨.
}

// 화면에 플레이어를 그립니다. (픽셀 위치 m_pixelPosition이 원의 중심)
void Player::draw() const
{
	// ⭐️ 현재 프레임 텍스처를 가져옵니다. (m_currentFrameIndex는 m_playerFrames.size()보다 작음이 보장됨)
	const s3d::Texture& currentTexture = m_playerFrames[m_currentFrameIndex];

	// 이미지 너비를 타일 크기에 맞게 조절하여 중앙에 그리기
	currentTexture.scaled(m_tileSize / (double)currentTexture.width())
		.drawAt(m_pixelPosition);
}

// 목표 타일로 이동 (기존 코드와 동일)
void Player::move(int targetTileX, int targetTileY)
{
	m_tilePosition.set(targetTileX, targetTileY);
	m_targetPixelPosition = calculateCenterPixelPosition(targetTileX, targetTileY);
}

// 현재 타일 위치 반환 (기존 코드와 동일)
s3d::Point Player::getCurrentTile() const
{
	return m_tilePosition;
}

// 이동 중인지 확인 (기존 코드와 동일)
bool Player::isMoving() const
{
	return m_pixelPosition != m_targetPixelPosition;
}
