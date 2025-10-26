#include "MainMenuScene.hpp"

MainMenuScene::MainMenuScene()
: titleFont_(FontMethod::MSDF, 64, Resource(U"ArtResources/Fonts/TetsubinGothic.otf")), buttonFont_(24)
, infoFont_(14)
, backgroundColor_(0.0, 0.0, 0.0) 
, bgm_(Resource(U"ArtResources/BGM/HappyOcean.mp3"))
, fadeTimer_(0.0)
, fadeDuration_(1.0)
, isFadingOut_(false)
, fadeOutTimer_(0.0)
, fadeOutDuration_(1.0)
, nextScene_(SceneType::MainMenu)
{
	startButtonFrames_ = {
		Texture(Resource(U"ArtResources/Texture2D/Menu/Start/start_0.png")),
		Texture(Resource(U"ArtResources/Texture2D/Menu/Start/start_1.png")),
		Texture(Resource(U"ArtResources/Texture2D/Menu/Start/start_2.png"))
	};
	exitButtonFrames_ = {
		Texture(Resource(U"ArtResources/Texture2D/Menu/Exit/exit_0.png")),
		Texture(Resource(U"ArtResources/Texture2D/Menu/Exit/exit_1.png")),
		Texture(Resource(U"ArtResources/Texture2D/Menu/Exit/exit_2.png"))
	};

    currentScene_ = SceneType::MainMenu;
    initializeButtons();
}

void MainMenuScene::onEnter()
{
    titleAnimTimer_ = 0.0;
    fadeTimer_ = 0.0;  // 페이드 타이머 초기화
    isFadingOut_ = false;  // 페이드아웃 상태 초기화
    fadeOutTimer_ = 0.0;
    
    if (!bgm_.isEmpty())
    {
        bgm_.setLoop(true);     
        bgm_.setVolume(0.28);    
        bgm_.play();            
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
    
    // 페이드아웃 중이면 페이드아웃 처리
    if (isFadingOut_)
    {
        fadeOutTimer_ += deltaTime;
        
        // 페이드아웃 완료 시 씬 전환
        if (fadeOutTimer_ >= fadeOutDuration_)
        {
            changeScene(nextScene_);
            return;
        }
        
        // 페이드아웃 중에는 버튼 입력 무시
        return;
    }
    
    // 페이드 타이머 업데이트
    fadeTimer_ += deltaTime;

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
        startFadeOut(SceneType::Settings);
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
        startFadeOut(SceneType::Opening);  // 페이드아웃 시작
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
    
    // 페이드 알파값 (페이드아웃 중이면 페이드아웃 알파 사용)
    double fadeAlpha = getFadeAlpha();
    
    if (isFadingOut_)
    {
        // 페이드아웃 알파 계산
        fadeAlpha = 1.0 - (fadeOutTimer_ / fadeOutDuration_);
    }
    
    // 동적 중앙 계산
    const double centerX = Scene::Size().x / 2.0;
    const double centerY = Scene::Size().y / 2.0;
    
    // 타이틀 애니메이션 (펄스 효과) - 동적 위치 계산
    const double titlePulse = 0.9 + 0.1 * Math::Sin(titleAnimTimer_ * 2.0);
    const double titleY = centerY - 150;  // 버튼 위로 위치
    
    titleFont_(U"タコの伝説")  
        .drawAt(centerX, titleY, ColorF(1.0, 1.0, 1.0, fadeAlpha));  // 페이드 효과 적용
    
    // 버튼들 그리기 (페이드 효과 적용)
    drawButton(startButton_);
    drawButton(exitButton_);
    
    // 검은색 오버레이로 페이드 효과
    if (isFadingOut_)
    {
        // 페이드아웃 중 - 검은색이 점점 진해짐
        Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, fadeOutTimer_ / fadeOutDuration_ });
    }
    else if (fadeAlpha < 1.0)
    {
        // 페이드인 중 - 검은색이 점점 옅어짐
        Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, 1.0 - fadeAlpha });
    }
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

double MainMenuScene::getFadeAlpha() const
{
    if (fadeTimer_ < fadeDuration_)
    {
        // 페이드인 중
        return fadeTimer_ / fadeDuration_;
    }
    // 페이드인 완료
    return 1.0;
}

void MainMenuScene::startFadeOut(SceneType targetScene)
{
    isFadingOut_ = true;
    fadeOutTimer_ = 0.0;
    nextScene_ = targetScene;
}
