#include "MainMenuScene.hpp"

MainMenuScene::MainMenuScene()
: titleFont_(48, Typeface::Bold)
, buttonFont_(24, Typeface::Bold)
, infoFont_(14)
, backgroundColor_(0.0, 0.0, 0.0) // 배경색을 검정색으로 변경
, bgm_(U"ArtResources/BGM/DeepSea1.mp3")  // 배경음악을 DeepSea1.mp3로 변경
{
	// Start 버튼 애니메이션 프레임 로드 (879x566 비율)
	startButtonFrames_ = {
		Texture(U"ArtResources/Texture2D/Menu/Start/start_0.png"),
		Texture(U"ArtResources/Texture2D/Menu/Start/start_1.png"),
		Texture(U"ArtResources/Texture2D/Menu/Start/start_2.png")
	};
	// Exit 버튼 애니메이션 프레임 로드
	exitButtonFrames_ = {
		Texture(U"ArtResources/Texture2D/Menu/Exit/exit_0.png"),
		Texture(U"ArtResources/Texture2D/Menu/Exit/exit_1.png"),
		Texture(U"ArtResources/Texture2D/Menu/Exit/exit_2.png")
	};

    currentScene_ = SceneType::MainMenu;
    initializeButtons();
}

void MainMenuScene::onEnter()
{
    titleAnimTimer_ = 0.0;
    
    // 배경음악 시작 (무한루프)
    if (!bgm_.isEmpty())
    {
        bgm_.setLoop(true);     // 무한루프 설정
        bgm_.setVolume(0.7);    // 볼륨 설정 (0.0 ~ 1.0)
        bgm_.play();            // 재생 시작
    }
    
    // Print << U"메인메뉴 진입";
}

void MainMenuScene::onExit()
{
    // 배경음악 정지
    if (bgm_.isPlaying())
    {
        bgm_.stop();
    }
    
    // Print << U"메인메뉴 종료";
}

void MainMenuScene::initializeButtons()
{
    const double centerX = Scene::Size().x / 2.0; // 동적 중앙 계산
    const double centerY = Scene::Size().y / 2.0; // 동적 중앙 계산
    
    // Start Game 버튼 (879:566 비율 유지, 0.7배 크기)
    const double startButtonWidth = 300.0 * 0.7;  // 210
    const double startButtonHeight = startButtonWidth * (566.0 / 879.0);  // 약 135.2
    
    // 버튼 간격 (버튼 높이 + 추가 여백)
    const double buttonGap = 30; // 버튼 사이의 추가 공백
    const double totalSpacing = startButtonHeight + buttonGap;
    
    startButton_.rect = Rect{
        static_cast<int32>(centerX - startButtonWidth / 2), 
        static_cast<int32>(centerY - totalSpacing / 2),
        static_cast<int32>(startButtonWidth), static_cast<int32>(startButtonHeight)
    };
    startButton_.text = U"Start Game";
    startButton_.normalColor = ColorF{ 1.0, 1.0, 1.0 };
    startButton_.hoverColor = ColorF{ 1.0, 1.0, 1.0 };
    startButton_.textColor = ColorF{ 0.0, 0.0, 0.0 };
    startButton_.hoverTextColor = ColorF{ 0.0, 0.0, 0.0 };
    
    // Exit 버튼 (879:566 비율 유지, 0.7배 크기)
    const double exitButtonWidth = 300.0 * 0.7;  // 210
    const double exitButtonHeight = exitButtonWidth * (566.0 / 879.0);  // 약 135.2
    
    exitButton_.rect = Rect{
        static_cast<int32>(centerX - exitButtonWidth / 2), 
        static_cast<int32>(centerY + totalSpacing / 2),
        static_cast<int32>(exitButtonWidth), static_cast<int32>(exitButtonHeight)
    };
    exitButton_.text = U"Exit";
    exitButton_.normalColor = ColorF{ 1.0, 1.0, 1.0 };
    exitButton_.hoverColor = ColorF{ 1.0, 1.0, 1.0 };
    exitButton_.textColor = ColorF{ 0.0, 0.0, 0.0 };
    exitButton_.hoverTextColor = ColorF{ 0.0, 0.0, 0.0 };
}

void MainMenuScene::update()
{
    const double deltaTime = Scene::DeltaTime();
    titleAnimTimer_ += deltaTime;

    // 포커스 상태 확인 및 음악 제어
    bool currentFocus = Window::GetState().focused;
    if (!bgm_.isEmpty())
    {
        if (currentFocus && !bgm_.isPlaying())
        {
            // 포커스를 다시 얻었을 때 음악 재생
            bgm_.play();
        }
        else if (!currentFocus && bgm_.isPlaying())
        {
            // 포커스를 잃었을 때 음악 일시정지
            bgm_.pause();
        }
    }
    wasFocused_ = currentFocus;

    // ESC 키로 Settings 씬으로 이동
    if (KeyEscape.down())
    {
        changeScene(SceneType::Settings);
        return;
    }

	animationFrameTimer_ += deltaTime;
	if (animationFrameTimer_ >= animationFrameDuration_) {
		animationFrameTimer_ -= animationFrameDuration_;
		animationFrameIndex_ = (animationFrameIndex_ + 1) % 3; // 3프레임 순환
	}

    // 버튼 업데이트
    updateButton(startButton_);
    updateButton(exitButton_);
    
    // 버튼 클릭 처리
    if (startButton_.rect.leftClicked())
    {
        changeScene(SceneType::StageSelect);  // InGame 대신 StageSelect로 변경
    }
    else if (exitButton_.rect.leftClicked())
    {
        System::Exit();
    }
}

void MainMenuScene::draw()
{
    // 배경 그리기
    Scene::SetBackground(backgroundColor_);
    
    // 동적 중앙 계산
    const double centerX = Scene::Size().x / 2.0;
    const double centerY = Scene::Size().y / 2.0;
    
    // 타이틀 애니메이션 (펄스 효과) - 동적 위치 계산
    const double titlePulse = 0.9 + 0.1 * Math::Sin(titleAnimTimer_ * 2.0);
    const double titleY = centerY - 150;  // 버튼 위로 위치
    
    titleFont_(U"Taco's \n adventure")
        .drawAt(centerX, titleY, ColorF(1.0, 1.0, 1.0));  // 동적 중앙 위치
    
    // 버튼들 그리기
    drawButton(startButton_);
    drawButton(exitButton_);
  
}

void MainMenuScene::updateButton(Button& button)
{
    button.isHovered = button.rect.mouseOver();
}

void MainMenuScene::drawButton(const Button& button)
{
	const Rect& rect = button.rect;
	const Texture* texture = nullptr;

	// 버튼에 따라 애니메이션 프레임 선택
	if (&button == &startButton_) {
		texture = &startButtonFrames_[animationFrameIndex_];
	}
	else if (&button == &exitButton_) {
		texture = &exitButtonFrames_[animationFrameIndex_];
	}

	if (texture && texture->isEmpty() == false) {
		texture->resized(rect.size).draw(rect.pos);
	}
	else {
		ColorF currentColor = button.isHovered ? button.hoverColor : button.normalColor;
		rect.draw(currentColor);
		rect.drawFrame(2, Palette::White);
		ColorF textColor = button.isHovered ? button.hoverTextColor : button.textColor;
		buttonFont_(button.text).drawAt(rect.center(), textColor);
	}
}
