#pragma once
#include "SceneManager.hpp"

class SettingsScene : public GameScene
{
private:
Font titleFont_;
Font optionFont_;
Font backButtonFont_;
    
// 배경음악
Audio bgm_;
    
// 설정 값들
    struct Settings
    {
        double masterVolume = 1.0;
        double sfxVolume = 1.0;
        double musicVolume = 1.0;
        bool fullscreen = false;
        int32 resolution = 0; // 0: 512x512, 1: 1024x1024, etc.
    };
    
    Settings settings_;
    Settings originalSettings_; // 취소시 복원용
    
    // UI 요소들
    struct Slider
    {
        Rect rect;
        String label;
        double* value;
        double minValue;
        double maxValue;
        bool isDragging = false;
    };
    
    Array<Slider> sliders_;
    Rect backButton_;
    Rect applyButton_;
    Rect resetButton_;
    
    bool backHovered_ = false;
    bool applyHovered_ = false;
    bool resetHovered_ = false;

public:
    SettingsScene();
    ~SettingsScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;

private:
    void initializeUI();
    void updateSliders();
    void drawSliders();
    void applySettings();
    void resetSettings();
    void loadSettings();
    void saveSettings();
};
