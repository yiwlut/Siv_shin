#pragma once
#include "SceneManager.hpp"

class StageSelectScene : public GameScene
{
private:
Font titleFont_;
Font stageFont_;
Font infoFont_;
Font stageNumberFont_;
Font stageTitleFont_;  // ★ 스테이지 이름용 큰 폰트 추가
    
// 배경음악
Audio bgm_;
    
// 스테이지 버튼 구조체
    struct StageButton
    {
        Rect rect;
        int32 stageNumber;
        bool isLocked;
        bool isHovered = false;
        ColorF normalColor = ColorF{ 0.2, 0.5, 0.7 };
        ColorF hoverColor = ColorF{ 0.3, 0.6, 0.8 };
        ColorF lockedColor = ColorF{ 0.3, 0.3, 0.3 };
        String stageName;
        String description;
    };
	// ★ 텍스처 관련 멤버 추가
	Array<Array<Texture>> stageTextures_;  // [스테이지][프레임]
	int32 animationFrameIndex_ = 0;
	double animationFrameTimer_ = 0.0;
	double animationFrameDuration_ = 0.33;  // 약 3fps

	// ★ 보스 스테이지 전용 애니메이션 추가
	int32 bossAnimationFrameIndex_ = 0;
	double bossAnimationFrameTimer_ = 0.0;
	double bossAnimationFrameDuration_ = 0.1;  // 페이드 효과 속도
	bool bossAnimationReverse_ = false;  // 역재생 플래그

	// ★ 함수 선언 추가
	void loadStageTextures();
    Array<StageButton> stageButtons_;
    Rect backButton_;
    bool backHovered_ = false;
    
    // 스크롤 시스템
    double scrollOffset_ = 0.0;
    double targetScrollOffset_ = 0.0;
    bool isDragging_ = false;
    Vec2 dragStartPos_;
    double dragStartOffset_;
    
    static constexpr int32 VISIBLE_STAGES = 2;
    static constexpr int32 STAGE_WIDTH = 256;
    static constexpr int32 STAGE_HEIGHT = 256;
    static constexpr int32 STAGE_SPACING = 100;
    
    // 애니메이션
    double animTimer_ = 0.0;
    int32 selectedStage_ = -1;
    bool wasFocused_ = true;
    
    // 게임 데이터 참조
    GameData* gameData_ = nullptr;

public:
    StageSelectScene();
    StageSelectScene(GameData* gameData);
    ~StageSelectScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;
    
    void setGameData(GameData* gameData) { gameData_ = gameData; }

private:
void initializeStages();
void updateStageButtons();
void drawStageButtons();
void drawStageInfo();
void updateScrolling();
void handleDragInput();
double getMaxScrollOffset() const;
void clampScrollOffset();
};
