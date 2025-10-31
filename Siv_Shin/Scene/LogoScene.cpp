#include "LogoScene.hpp"

LogoScene::LogoScene()
    : elapsedTime_(0.0)
    , fadeInDuration_(1.0)        
    , displayDuration_(2.0)       
    , fadeOutDuration_(1.0)       
    , totalDuration_(4.0)         
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
}

void LogoScene::update()
{
    const double dt = Scene::DeltaTime();
    elapsedTime_ += dt;

    if (KeyF2.down())
    {
        changeScene(SceneType::MainMenu);
        return;
    }

    if (elapsedTime_ >= totalDuration_)
    {
        changeScene(SceneType::MainMenu);
        return;
    }
}

void LogoScene::draw()
{
    Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });

    const double alpha = getFadeAlpha();

    const Vec2 center = Scene::Center();

    if (!logoTexture_.isEmpty())
    {
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
        return elapsedTime_ / fadeInDuration_;
    }
    else if (elapsedTime_ < fadeInDuration_ + displayDuration_)
    {
        return 1.0;
    }
    else
    {
        const double fadeOutElapsed = elapsedTime_ - (fadeInDuration_ + displayDuration_);
        return 1.0 - (fadeOutElapsed / fadeOutDuration_);
    }
}
