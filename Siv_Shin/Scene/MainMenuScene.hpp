#pragma once
#include "SceneManager.hpp"

class MainMenuScene : public GameScene
{
private:
    Font titleFont_;
    Font buttonFont_;
    Font infoFont_;  // W100 경고 해결: 멤버변수로 이동
    
    // 배경음악
    Audio bgm_;
    
    // 버튼 상태 관리
    struct Button
    {
        Rect rect;
        String text;
        bool isHovered = false;
        ColorF normalColor = ColorF{ 0.2, 0.4, 0.6 };
        ColorF hoverColor = ColorF{ 0.3, 0.5, 0.7 };
        ColorF textColor = Palette::White;
        ColorF hoverTextColor = Palette::Yellow;
    };
    
    Button startButton_;
    Button settingsButton_;
    Button exitButton_;

	Array<Texture> startButtonFrames_;
	Array<Texture> settingsButtonFrames_;
	Array<Texture> exitButtonFrames_;

	int animationFrameIndex_ = 0;
	double animationFrameTimer_ = 0.0;
	const double animationFrameDuration_ = 0.15;

    // 애니메이션 효과
    double titleAnimTimer_ = 0.0;
    ColorF backgroundColor_;
    bool wasFocused_ = true;  // 포커스 상태 추적

public:
    MainMenuScene();
    ~MainMenuScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;

private:
    void initializeButtons();
    void updateButton(Button& button);
    void drawButton(const Button& button);
};
