#include "Player.h"

s3d::Vec2 Player::calculateCenterPixelPosition(int tileX, int tileY) const
{
	double centerX = (tileX * m_tileSize) + (m_tileSize / 2.0);
	double centerY = (tileY * m_tileSize) + (m_tileSize / 2.0);
	return s3d::Vec2(centerX, centerY);
}

Player::Player(int startTileX, int startTileY, int tileSize, int frameCount)    
	: m_tilePosition(startTileX, startTileY)
	, m_tileSize(tileSize)       
{
	for (int32 i = 0; i < frameCount; i++)
	{
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

	if (m_playerFrames.empty())
	{
		s3d::Print << U"Warning: No player animation frames loaded. Using default yellow square.";

		m_playerFrames.push_back(s3d::Texture{ s3d::Image{
			static_cast<size_t>(m_tileSize),  
			static_cast<size_t>(m_tileSize),  
			s3d::Palette::Yellow}
		});
	}

	m_pixelPosition = calculateCenterPixelPosition(startTileX, startTileY);
	m_targetPixelPosition = m_pixelPosition;
}


void Player::update()
{
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

	if (m_playerFrames.size() > 1)
	{
		m_frameTimer += s3d::Scene::DeltaTime();

		if (m_frameTimer >= m_frameDuration)
		{
			m_currentFrameIndex = (m_currentFrameIndex + 1) % m_playerFrames.size();
			m_frameTimer = 0.0;   
		}
	}
}

void Player::draw() const
{
	const s3d::Texture& currentTexture = m_playerFrames[m_currentFrameIndex];

	currentTexture.scaled(m_tileSize / (double)currentTexture.width())
		.drawAt(m_pixelPosition);
}

void Player::move(int targetTileX, int targetTileY)
{
	m_tilePosition.set(targetTileX, targetTileY);
	m_targetPixelPosition = calculateCenterPixelPosition(targetTileX, targetTileY);
}

s3d::Point Player::getCurrentTile() const
{
	return m_tilePosition;
}

bool Player::isMoving() const
{
	return m_pixelPosition != m_targetPixelPosition;
}
