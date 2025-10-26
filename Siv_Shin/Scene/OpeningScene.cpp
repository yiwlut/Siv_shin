#include "OpeningScene.hpp"

OpeningScene::OpeningScene()
    : elapsedTime_(0.0)
    , fadeInDuration_(1.5)      // 페이드인 1.5초
    , displayDuration_(3.0)     // 표시 3초
    , fadeOutDuration_(1.5)     // 페이드아웃 1.5초
    , totalDuration_(30.0)      // 총 30초
    , titleFont_(FontMethod::MSDF, 72, Resource(U"ArtResources/Fonts/DarumaDropOne-Regular.ttf"))
    , subtitleFont_(FontMethod::MSDF, 32, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
    , openingBgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"))
    , isSkipped_(false)
{
    currentScene_ = SceneType::Opening;
}

void OpeningScene::onEnter()
{
    elapsedTime_ = 0.0;
    isSkipped_ = false;

    // 배경음악 재생
    if (!openingBgm_.isEmpty())
    {
        openingBgm_.setLoop(false);
        openingBgm_.setVolume(0.3);
        openingBgm_.play();
    }
}

void OpeningScene::onExit()
{
    // 배경음악 정지
    if (!openingBgm_.isEmpty() && openingBgm_.isPlaying())
    {
        openingBgm_.stop();
    }
}

void OpeningScene::update()
{
    const double dt = Scene::DeltaTime();
    elapsedTime_ += dt;

    // F2 디버그 키로 즉시 스킵
    if (KeyF2.down())
    {
        isSkipped_ = true;
        changeScene(SceneType::InGame);
        return;
    }

    // 스킵 처리 (스페이스, 엔터, 마우스 클릭)
    if (KeySpace.down() || KeyEnter.down() || MouseL.down())
    {
        isSkipped_ = true;
        changeScene(SceneType::InGame);
        return;
    }

    // 자동 진행 (총 시간 경과 시)
    if (elapsedTime_ >= totalDuration_)
    {
        changeScene(SceneType::InGame);
        return;
    }
}

void OpeningScene::draw()
{
    // 검은색 배경
    Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });

    // 페이드 알파값 계산
    const double alpha = getFadeAlpha();

    // 화면 중앙
    const Vec2 center = Scene::Center();

    // 타이틀 텍스트
    titleFont_(U"タコの伝説")
        .drawAt(center.x, center.y - 80, ColorF{ 1.0, 1.0, 1.0, alpha });

    // 서브타이틀 텍스트
    subtitleFont_(U"The Legend of Taco")
        .drawAt(center.x, center.y + 40, ColorF{ 0.8, 0.8, 0.8, alpha });

    // 스킵 안내 메시지 (항상 표시)
    const Font& skipFont = subtitleFont_;
    const double skipAlpha = 0.3 + 0.3 * Math::Sin(elapsedTime_ * 3.0);  // 깜빡임 효과
    skipFont(U"Press SPACE or ENTER to skip")
        .drawAt(center.x, Scene::Height() - 60, ColorF{ 0.6, 0.6, 0.6, skipAlpha });
}

double OpeningScene::getFadeAlpha() const
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
