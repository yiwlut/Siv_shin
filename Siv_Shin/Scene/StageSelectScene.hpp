#pragma once
#include "SceneManager.hpp"

class StageSelectScene : public GameScene
{
private:
Font titleFont_;
Font stageFont_;
Font infoFont_;
Font stageNumberFont_;
Font stageTitleFont_;        
    
Audio bgm_;
    
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
	Array<Array<Texture>> stageTextures_;   
	int32 animationFrameIndex_ = 0;
	double animationFrameTimer_ = 0.0;
	double animationFrameDuration_ = 0.33;    

	int32 bossAnimationFrameIndex_ = 0;
	double bossAnimationFrameTimer_ = 0.0;
	double bossAnimationFrameDuration_ = 0.1;     
	bool bossAnimationReverse_ = false;    

	void loadStageTextures();
    Array<StageButton> stageButtons_;
    Rect backButton_;
    bool backHovered_ = false;
    
    double scrollOffset_ = 0.0;
    double targetScrollOffset_ = 0.0;
    bool isDragging_ = false;
    Vec2 dragStartPos_;
    double dragStartOffset_;
    
    static constexpr int32 VISIBLE_STAGES = 2;
    static constexpr int32 STAGE_WIDTH = 256;
    static constexpr int32 STAGE_HEIGHT = 256;
    static constexpr int32 STAGE_SPACING = 100;
    
    double animTimer_ = 0.0;
    int32 selectedStage_ = -1;
    bool wasFocused_ = true;
    
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
