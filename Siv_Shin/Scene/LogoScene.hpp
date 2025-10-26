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
    // 타이머
    double elapsedTime_;
    double fadeInDuration_;
    double displayDuration_;
    double fadeOutDuration_;
    double totalDuration_;

    // 로고 텍스처
    Texture logoTexture_;
    
    // 디버그용 폰트
    Font debugFont_;

    // 페이드 효과
    double getFadeAlpha() const;
};
