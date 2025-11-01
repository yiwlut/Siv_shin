#include "SettingsScene.hpp"

SettingsScene::SettingsScene()
    : titleFont_(48)
    , optionFont_(20)
    , backButtonFont_(24)
    , bgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"), Loop::Yes)     
{
    currentScene_ = SceneType::Settings;
    loadSettings();
    initializeUI();
}

void SettingsScene::onEnter()
{
    originalSettings_ = settings_;  
    
    if (!bgm_.isEmpty())
    {
        bgm_.setVolume(0.2);         
        bgm_.play();
    }
}

void SettingsScene::onExit()
{
    if (!bgm_.isEmpty() && bgm_.isPlaying())
    {
        bgm_.stop();
    }
}

void SettingsScene::initializeUI()
{
    sliders_.clear();
    
    sliders_.push_back({
        Rect{150, 200, 200, 20},
        U"Master Volume",
        &settings_.masterVolume,
        0.0, 1.0
    });
    
    sliders_.push_back({
        Rect{150, 250, 200, 20},
        U"SFX Volume",
        &settings_.sfxVolume,
        0.0, 1.0
    });
    
    sliders_.push_back({
        Rect{150, 300, 200, 20},
        U"Music Volume",
        &settings_.musicVolume,
        0.0, 1.0
    });
    
    backButton_ = Rect{50, 450, 120, 40};
    applyButton_ = Rect{200, 450, 120, 40};
    resetButton_ = Rect{350, 450, 120, 40};
}

void SettingsScene::update()
{
    updateSliders();
    
    backHovered_ = backButton_.mouseOver();
    applyHovered_ = applyButton_.mouseOver();
    resetHovered_ = resetButton_.mouseOver();
    
    if (backButton_.leftClicked())
    {
        settings_ = originalSettings_;   
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
    
    if (KeyEscape.down())
    {
        settings_ = originalSettings_;   
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
    
    titleFont_(U"Settings").drawAt(256, 100, Palette::White);
    
    drawSliders();
    
    optionFont_(U"Resolution: 512x512").draw(Vec2{100, 350}, Palette::White);
    optionFont_(U"Fullscreen: {}"_fmt(settings_.fullscreen ? U"ON" : U"OFF"))
        .draw(Vec2{100, 380}, Palette::White);
    
    ColorF backColor = backHovered_ ? ColorF{0.3, 0.5, 0.7} : ColorF{0.2, 0.4, 0.6};
    backButton_.draw(backColor);
    backButton_.drawFrame(2, Palette::White);
    backButtonFont_(U"Back").drawAt(backButton_.center(), 
        backHovered_ ? Palette::Yellow : Palette::White);
    
    ColorF applyColor = applyHovered_ ? ColorF{0.3, 0.7, 0.3} : ColorF{0.2, 0.6, 0.2};
    applyButton_.draw(applyColor);
    applyButton_.drawFrame(2, Palette::White);
    backButtonFont_(U"Apply").drawAt(applyButton_.center(), 
        applyHovered_ ? Palette::Yellow : Palette::White);
    
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
        optionFont_(slider.label).draw(Vec2{50, slider.rect.y - 5}, Palette::White);
        
        slider.rect.draw(ColorF{0.3, 0.3, 0.3});
        slider.rect.drawFrame(1, Palette::White);
        
        double ratio = (*slider.value - slider.minValue) / (slider.maxValue - slider.minValue);
        Rect valueRect{slider.rect.x, slider.rect.y, 
                      static_cast<int32>(slider.rect.w * ratio), slider.rect.h};
        valueRect.draw(ColorF{0.4, 0.6, 0.8});
        
        optionFont_(U"{:.0f}%"_fmt(*slider.value * 100))
            .draw(Vec2{slider.rect.x + slider.rect.w + 10, slider.rect.y - 5}, Palette::White);
    }
}

void SettingsScene::applySettings()
{
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
    resetSettings();
}

void SettingsScene::saveSettings()
{
}
