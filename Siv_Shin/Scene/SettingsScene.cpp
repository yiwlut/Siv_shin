#include "SettingsScene.hpp"

SettingsScene::SettingsScene()
    : titleFont_(48, Typeface::Bold)
    , optionFont_(20)
    , backButtonFont_(24)
    , bgm_(U"ArtResources/BGM/DeepSea1.mp3", Loop::Yes)  // 배경음악을 DeepSea1.mp3로 변경
{
    currentScene_ = SceneType::Settings;
    loadSettings();
    initializeUI();
}

void SettingsScene::onEnter()
{
    originalSettings_ = settings_; // 백업
    
    // 배경음악 재생
    if (!bgm_.isEmpty())
    {
        bgm_.setVolume(0.2);  // 볼륨 설정 (0.05 * 4 = 0.2)
        bgm_.play();
    }
    
    Print << U"설정 메뉴 진입";
}

void SettingsScene::onExit()
{
    // 배경음악 정지
    if (!bgm_.isEmpty() && bgm_.isPlaying())
    {
        bgm_.stop();
    }
    
    Print << U"설정 메뉴 종료";
}

void SettingsScene::initializeUI()
{
    // 슬라이더 초기화
    sliders_.clear();
    
    // 마스터 볼륨 슬라이더
    sliders_.push_back({
        Rect{150, 200, 200, 20},
        U"Master Volume",
        &settings_.masterVolume,
        0.0, 1.0
    });
    
    // 효과음 볼륨 슬라이더
    sliders_.push_back({
        Rect{150, 250, 200, 20},
        U"SFX Volume",
        &settings_.sfxVolume,
        0.0, 1.0
    });
    
    // 음악 볼륨 슬라이더
    sliders_.push_back({
        Rect{150, 300, 200, 20},
        U"Music Volume",
        &settings_.musicVolume,
        0.0, 1.0
    });
    
    // 버튼들 초기화
    backButton_ = Rect{50, 450, 120, 40};
    applyButton_ = Rect{200, 450, 120, 40};
    resetButton_ = Rect{350, 450, 120, 40};
}

void SettingsScene::update()
{
    updateSliders();
    
    // 버튼 호버 상태 업데이트
    backHovered_ = backButton_.mouseOver();
    applyHovered_ = applyButton_.mouseOver();
    resetHovered_ = resetButton_.mouseOver();
    
    // 버튼 클릭 처리
    if (backButton_.leftClicked())
    {
        settings_ = originalSettings_; // 설정 복원
        changeScene(SceneType::MainMenu);
    }
    else if (applyButton_.leftClicked())
    {
        applySettings();
        saveSettings();
        changeScene(SceneType::MainMenu);
    }
    else if (resetButton_.leftClicked())
    {
        resetSettings();
    }
    
    // ESC키로 뒤로가기
    if (KeyEscape.down())
    {
        settings_ = originalSettings_; // 설정 복원
        changeScene(SceneType::MainMenu);
    }
}

void SettingsScene::updateSliders()
{
    for (auto& slider : sliders_)
    {
        if (slider.rect.mouseOver() && MouseL.pressed())
        {
            slider.isDragging = true;
        }
        
        if (MouseL.up())
        {
            slider.isDragging = false;
        }
        
        if (slider.isDragging)
        {
            double mouseX = Cursor::Pos().x;
            double ratio = Math::Clamp((mouseX - slider.rect.x) / slider.rect.w, 0.0, 1.0);
            *slider.value = Math::Lerp(slider.minValue, slider.maxValue, ratio);
        }
    }
}

void SettingsScene::draw()
{
    Scene::SetBackground(ColorF{ 0.15, 0.1, 0.2 });
    
    // 타이틀
    titleFont_(U"Settings").drawAt(256, 100, Palette::White);
    
    // 슬라이더들 그리기
    drawSliders();
    
    // 해상도 옵션 (예시)
    optionFont_(U"Resolution: 512x512").draw(Vec2{100, 350}, Palette::White);
    optionFont_(U"Fullscreen: {}"_fmt(settings_.fullscreen ? U"ON" : U"OFF"))
        .draw(Vec2{100, 380}, Palette::White);
    
    // 버튼들 그리기
    // Back 버튼
    ColorF backColor = backHovered_ ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.4, 0.6};
    backButton_.draw(backColor);
    backButton_.drawFrame(2, Palette::White);
    backButtonFont_(U"Back").drawAt(backButton_.center(), 
        backHovered_ ? Palette::Yellow : Palette::White);
    
    // Apply 버튼
    ColorF applyColor = applyHovered_ ? ColorF{0.3, 0.7, 0.3} : ColorF{0.2, 0.6, 0.2};
    applyButton_.draw(applyColor);
    applyButton_.drawFrame(2, Palette::White);
    backButtonFont_(U"Apply").drawAt(applyButton_.center(), 
        applyHovered_ ? Palette::Yellow : Palette::White);
    
    // Reset 버튼
    ColorF resetColor = resetHovered_ ? ColorF{0.7, 0.3, 0.3} : ColorF{0.6, 0.2, 0.2};
    resetButton_.draw(resetColor);
    resetButton_.drawFrame(2, Palette::White);
    backButtonFont_(U"Reset").drawAt(resetButton_.center(), 
        resetHovered_ ? Palette::Yellow : Palette::White);
}

void SettingsScene::drawSliders()
{
    for (const auto& slider : sliders_)
    {
        // 슬라이더 라벨
        optionFont_(slider.label).draw(Vec2{50, slider.rect.y - 5}, Palette::White);
        
        // 슬라이더 배경
        slider.rect.draw(ColorF{0.3, 0.3, 0.3});
        slider.rect.drawFrame(1, Palette::White);
        
        // 슬라이더 값 표시
        double ratio = (*slider.value - slider.minValue) / (slider.maxValue - slider.minValue);
        Rect valueRect{slider.rect.x, slider.rect.y, 
                      static_cast<int32>(slider.rect.w * ratio), slider.rect.h};
        valueRect.draw(ColorF{0.4, 0.6, 0.8});
        
        // 값 텍스트
        optionFont_(U"{:.0f}%"_fmt(*slider.value * 100))
            .draw(Vec2{slider.rect.x + slider.rect.w + 10, slider.rect.y - 5}, Palette::White);
    }
}

void SettingsScene::applySettings()
{
    // 실제 설정 적용 로직
    // 오디오 볼륨 설정 등
    Print << U"설정 적용됨";
}

void SettingsScene::resetSettings()
{
    settings_.masterVolume = 1.0;
    settings_.sfxVolume = 1.0;
    settings_.musicVolume = 1.0;
    settings_.fullscreen = false;
    settings_.resolution = 0;
}

void SettingsScene::loadSettings()
{
    // 설정 파일에서 로드 (향후 구현)
    // 현재는 기본값 사용
    resetSettings();
}

void SettingsScene::saveSettings()
{
    // 설정 파일에 저장 (향후 구현)
    Print << U"설정 저장됨";
}
