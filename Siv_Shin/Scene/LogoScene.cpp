#include "LogoScene.hpp"

LogoScene::LogoScene()
    : elapsedTime_(0.0)
    , fadeInDuration_(1.0)      // 페이드인 1초
    , displayDuration_(2.0)     // 표시 2초
    , fadeOutDuration_(1.0)     // 페이드아웃 1초
    , totalDuration_(4.0)       // 총 4초
    , logoTexture_(Resource(U"ArtResources/Logo/L_siv3D.png"))
    , debugFont_(20)
{
    currentScene_ = SceneType::Logo;
}

void LogoScene::onEnter()
{
    elapsedTime_ = 0.0;
}

void LogoScene::onExit()
{
    // 정리 작업
}

void LogoScene::update()
{
    const double dt = Scene::DeltaTime();
    elapsedTime_ += dt;

    // F2 디버그 키로 즉시 스킵 (디버그용)
    if (KeyF2.down())
    {
        changeScene(SceneType::MainMenu);
        return;
    }

    // 자동 진행 (총 시간 경과 시)
    if (elapsedTime_ >= totalDuration_)
    {
        changeScene(SceneType::MainMenu);
        return;
    }
}

void LogoScene::draw()
{
    // 검은색 배경
    Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });

    // 페이드 알파값 계산
    const double alpha = getFadeAlpha();

    const Vec2 center = Scene::Center();

    // 화면 중앙에 로고 표시
    if (!logoTexture_.isEmpty())
    {
        // 로고 크기 조정 (화면에 맞게)
        const double maxWidth = Scene::Width() * 0.6;
        const double maxHeight = Scene::Height() * 0.6;
        
        double scale = 1.0;
        if (logoTexture_.width() > maxWidth || logoTexture_.height() > maxHeight)
        {
            const double scaleX = maxWidth / logoTexture_.width();
            const double scaleY = maxHeight / logoTexture_.height();
            scale = Min(scaleX, scaleY);
        }
        
        logoTexture_.scaled(scale).drawAt(center, ColorF{ 1.0, 1.0, 1.0, alpha });
    }
}

double LogoScene::getFadeAlpha() const
{
    if (elapsedTime_ < fadeInDuration_)
    {
        // 페이드인 구간
        return elapsedTime_ / fadeInDuration_;
    }
    else if (elapsedTime_ < fadeInDuration_ + displayDuration_)
    {
        // 완전히 표시되는 구간
        return 1.0;
    }
    else
    {
        // 페이드아웃 구간
        const double fadeOutElapsed = elapsedTime_ - (fadeInDuration_ + displayDuration_);
        return 1.0 - (fadeOutElapsed / fadeOutDuration_);
    }
}
