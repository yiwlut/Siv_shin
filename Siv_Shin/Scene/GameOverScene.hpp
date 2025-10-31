#pragma once
#include "SceneManager.hpp"

class GameOverScene : public GameScene
{
private:
Font titleFont_;
Font scoreFont_;
Font buttonFont_;
Font statFont_;         
Font helpFont_;         
    
Audio bgm_;
    
    struct GameResult
    {
        int32 finalScore = 0;
        double playTime = 0.0;
        int32 totalMoves = 0;
        bool isNewRecord = false;
    };
    
    GameResult gameResult_;
    
    double animTimer_ = 0.0;
    double scoreCountUp_ = 0.0;
    bool scoreAnimFinished_ = false;
    
    Rect retryButton_;
    Rect menuButton_;
    bool retryHovered_ = false;
    bool menuHovered_ = false;
    
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
