#pragma once
#include "SceneManager.hpp"

class OpeningScene : public GameScene
{
public:
    OpeningScene();
    ~OpeningScene() override = default;

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

    // 폰트
    Font titleFont_;
    Font subtitleFont_;

    // 배경음악
    Audio openingBgm_;

    // 페이드 효과
    double getFadeAlpha() const;

    // 스킵 처리
    bool isSkipped_;
};
