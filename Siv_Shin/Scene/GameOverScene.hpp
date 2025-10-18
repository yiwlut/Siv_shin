#pragma once
#include "SceneManager.hpp"

class GameOverScene : public GameScene
{
private:
Font titleFont_;
Font scoreFont_;
Font buttonFont_;
Font statFont_;    // W100 경고 해결: 멤버변수로 이동
Font helpFont_;    // W100 경고 해결: 멤버변수로 이동
    
// 배경음악
Audio bgm_;
    
// 게임 결과 데이터
    struct GameResult
    {
        int32 finalScore = 0;
        double playTime = 0.0;
        int32 totalMoves = 0;
        bool isNewRecord = false;
    };
    
    GameResult gameResult_;
    
    // 애니메이션
    double animTimer_ = 0.0;
    double scoreCountUp_ = 0.0;
    bool scoreAnimFinished_ = false;
    
    // 버튼들
    Rect retryButton_;
    Rect menuButton_;
    bool retryHovered_ = false;
    bool menuHovered_ = false;
    
    // 파티클 효과 (신기록용)
    struct Particle
    {
        Vec2 pos;
        Vec2 velocity;
        ColorF color;
        double life;
        double maxLife;
    };
    Array<Particle> particles_;

public:
    GameOverScene();
    ~GameOverScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;
    
    // 게임 결과 설정
    void setGameResult(int32 score, double time, int32 moves);

private:
    void initializeUI();
    void updateAnimations();
    void updateParticles();
    void createParticles();
    void drawResults();
    void drawButtons();
    void drawParticles();
};
