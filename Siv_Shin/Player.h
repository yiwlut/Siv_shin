#ifndef PLAYER_H
#define PLAYER_H

#include <Siv3D.hpp>

class Player {
public:
	/// @brief 플레이어를 생성하고 초기 타일에 배치합니다.
	/// @param startTileX 시작 타일의 X 좌표 (그리드 좌표)
	/// @param startTileY 시작 타일의 Y 좌표 (그리드 좌표)
	/// @param tileSize 타일의 한 변 길이 (픽셀)
	/// @param texturePathPattern ⭐️ 텍스처 파일 경로 패턴 (예: "ArtResources/player_{}.png")
	/// @param frameCount ⭐️ 애니메이션에 사용할 프레임 수
	Player(int startTileX, int startTileY, int tileSize, int frameCount); // ⭐️ 생성자 시그니처 변경

	/// @brief 플레이어의 부드러운 이동(애니메이션) 상태를 업데이트합니다.
	void update();

	/// @brief 플레이어를 화면에 그립니다.
	void draw() const;

	/// @brief 플레이어를 특정 타일 좌표로 이동시킵니다.
	void move(int targetTileX, int targetTileY);

	/// @brief 현재 플레이어가 위치한 타일 좌표를 반환합니다.
	s3d::Point getCurrentTile() const;

	/// @brief 플레이어가 현재 이동 중인지 확인합니다.
	bool isMoving() const;

private:
	s3d::Point m_tilePosition;// 그리드 좌표 (0, 0), (1, 0) 등
	const int m_tileSize;// 타일 크기 (픽셀)

	s3d::Vec2 m_pixelPosition;// 플레이어의 현재 픽셀 위치 (원의 중심)
	s3d::Vec2 m_targetPixelPosition; // 목표 픽셀 위치
	double m_moveSpeed = 8.0;

	// ⭐️ 단일 텍스처 대신 프레임 배열 사용
	s3d::Array<s3d::Texture> m_playerFrames;

	// ⭐️ 애니메이션 관리 변수 추가
	s3d::int32 m_currentFrameIndex = 0; // 현재 프레임 인덱스
	double m_frameTimer = 0.0;         // 프레임 타이머
	const double m_frameDuration = 0.15; // 프레임당 표시 시간 (초)

	/// @brief 타일 좌표를 화면 픽셀 좌표의 중앙으로 변환하는 내부 함수
	s3d::Vec2 calculateCenterPixelPosition(int tileX, int tileY) const;
};

#endif
