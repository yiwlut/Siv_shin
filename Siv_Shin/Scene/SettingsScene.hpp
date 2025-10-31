#pragma once
#include "SceneManager.hpp"

class SettingsScene : public GameScene
{
private:
Font titleFont_;
Font optionFont_;
Font backButtonFont_;
    
Audio bgm_;
    
    struct Settings
    {
        double masterVolume = 1.0;
        double sfxVolume = 1.0;
        double musicVolume = 1.0;
        bool fullscreen = false;
        int32 resolution = 0;      
    };
    
    Settings settings_;
    Settings originalSettings_;   
    
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
