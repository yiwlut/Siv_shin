#ifndef PLAYER_H
#define PLAYER_H

#include <Siv3D.hpp>

class Player {
public:
	Player(int startTileX, int startTileY, int tileSize, int frameCount);     

	void update();

	void draw() const;

	void move(int targetTileX, int targetTileY);

	s3d::Point getCurrentTile() const;

	bool isMoving() const;

private:
	s3d::Point m_tilePosition;       
	const int m_tileSize;   

	s3d::Vec2 m_pixelPosition;      
	s3d::Vec2 m_targetPixelPosition;    
	double m_moveSpeed = 8.0;

	s3d::Array<s3d::Texture> m_playerFrames;

	s3d::int32 m_currentFrameIndex = 0;    
	double m_frameTimer = 0.0;           
	const double m_frameDuration = 0.15;     

	s3d::Vec2 calculateCenterPixelPosition(int tileX, int tileY) const;
};

#endif
