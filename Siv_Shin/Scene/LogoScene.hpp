#pragma once
#include "SceneManager.hpp"

class LogoScene : public GameScene
{
public:
    LogoScene();
    ~LogoScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;

private:
    double elapsedTime_;
    double fadeInDuration_;
    double displayDuration_;
    double fadeOutDuration_;
    double totalDuration_;

    Texture logoTexture_;
    
    Font debugFont_;

    double getFadeAlpha() const;
};
