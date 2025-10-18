#pragma once
#include "SceneManager.hpp"

class StageSelectScene : public GameScene
{
private:
Font titleFont_;
Font stageFont_;
Font infoFont_;
Font stageNumberFont_;  // W100 경고 해결: 스테이지 번호용 폰트 추가
    
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
    
    Array<StageButton> stageButtons_;
    Rect backButton_;
    bool backHovered_ = false;
    
    // 스크롤 시스템
    double scrollOffset_ = 0.0;      // 현재 스크롤 오프셋
    double targetScrollOffset_ = 0.0; // 목표 스크롤 오프셋
    bool isDragging_ = false;         // 드래그 중인지
    Vec2 dragStartPos_;              // 드래그 시작 위치
    double dragStartOffset_;         // 드래그 시작 시 스크롤 오프셋
    
    static constexpr int32 VISIBLE_STAGES = 2;  // 한 화면에 보이는 스테이지 수 (큰 버튼으로 인해 감소)
    static constexpr int32 STAGE_WIDTH = 256;   // 스테이지 버튼 너비 (정사각형)
    static constexpr int32 STAGE_HEIGHT = 256;  // 스테이지 버튼 높이 (정사각형)
    static constexpr int32 STAGE_SPACING = 100; // 스테이지 간격 (더 넓게)
    
    // 애니메이션
    double animTimer_ = 0.0;
    int32 selectedStage_ = -1;  // -1은 선택 없음
    bool wasFocused_ = true;  // 포커스 상태 추적
    
    // 스테이지 버튼 애니메이션
    Array<Array<Texture>> stageTextures_;  // 각 스테이지별 3개 프레임
    int32 animationFrameIndex_ = 0;
    double animationFrameTimer_ = 0.0;
    const double animationFrameDuration_ = 0.3;  // 0.3초마다 프레임 변경
    
    // 게임 데이터 참조 (SceneManager에서 받아옴)
    GameData* gameData_ = nullptr;

public:
    StageSelectScene();
    StageSelectScene(GameData* gameData);  // 게임 데이터를 받는 생성자
    ~StageSelectScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;
    
    // 게임 데이터 설정
    void setGameData(GameData* gameData) { gameData_ = gameData; }

private:
void initializeStages();
void loadStageTextures();  // 스테이지 텍스처 로딩 함수 추가
void updateStageButtons();
void drawStageButtons();
void drawStageInfo();
void updateScrolling();
void handleDragInput();
double getMaxScrollOffset() const;
void clampScrollOffset();
};
