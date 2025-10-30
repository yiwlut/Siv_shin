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
	for (int i = 0; i <= 1; ++i) {
		cornerTL_ << Texture(Resource(U"ArtResources/Texture2D/MainMenu/tl_{}.png"_fmt(i)));
		cornerTR_ << Texture(Resource(U"ArtResources/Texture2D/MainMenu/tr_{}.png"_fmt(i)));
		cornerBL_ << Texture(Resource(U"ArtResources/Texture2D/MainMenu/bl_{}.png"_fmt(i)));
		cornerBR_ << Texture(Resource(U"ArtResources/Texture2D/MainMenu/br_{}.png"_fmt(i)));
	}
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
    
}

void MainMenuScene::onExit()
{
    if (bgm_.isPlaying())
    {
        bgm_.stop();
    }
    
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

	if (isFadingOut_)
	{
		fadeOutTimer_ += deltaTime;
		if (fadeOutTimer_ >= fadeOutDuration_)
		{
			changeScene(nextScene_);
			return;
		}
		return;
	}

	fadeTimer_ += deltaTime;

	bool currentFocus = Window::GetState().focused;
	if (!bgm_.isEmpty())
	{
		if (currentFocus && !bgm_.isPlaying())
			bgm_.play();
		else if (!currentFocus && bgm_.isPlaying())
			bgm_.pause();
	}
	wasFocused_ = currentFocus;

	animationFrameTimer_ += deltaTime;
	if (animationFrameTimer_ >= animationFrameDuration_)
	{
		animationFrameTimer_ -= animationFrameDuration_;
		animationFrameIndex_ = (animationFrameIndex_ + 1) % 3;
	}

	updateButton(startButton_);
	updateButton(exitButton_);

	if (startButton_.rect.leftClicked())
	{
		SceneType target = SceneType::StageSelect;

		// 오프닝을 본 적이 없으면 오프닝으로
		if (gameData_ && !gameData_->saveManager.hasSeenOpening())
		{
			target = SceneType::Opening;
		}
		// 파이널 스테이지를 클리어했으면 스테이지 선택으로
		else if (gameData_ && gameData_->finalStageCleared())
		{
			target = SceneType::StageSelect;
		}
		// 진행중인 스테이지로 바로 이동
		else if (gameData_)
		{
			gameData_->currentStage = gameData_->saveManager.getCurrentStage();
			target = SceneType::InGame;
		}

		startFadeOut(target);
	}
	else if (exitButton_.rect.leftClicked())
	{
		System::Exit();
	}

	cornerTimer_ += Scene::DeltaTime();
	if (cornerTimer_ >= CORNER_ANIM_SPEED)
	{
		cornerTimer_ = 0.0;
		cornerFrame_ = (cornerFrame_ + 1) % 2;
	}
}

void MainMenuScene::draw()
{
    Scene::SetBackground(backgroundColor_);
    double fadeAlpha = getFadeAlpha();
    
    if (isFadingOut_)
    {
        fadeAlpha = 1.0 - (fadeOutTimer_ / fadeOutDuration_);
    }
    
    const double centerX = Scene::Size().x / 2.0;
    const double centerY = Scene::Size().y / 2.0;
    
    const double titlePulse = 0.9 + 0.1 * Math::Sin(titleAnimTimer_ * 2.0);
    const double titleY = centerY - 150;  // 버튼 위로 위치
    
    titleFont_(U"タコの伝説")  
        .drawAt(centerX, titleY, ColorF(1.0, 1.0, 1.0, fadeAlpha));  // 페이드 효과 적용

	initializeButtons();
    // 버튼들 그리기 (페이드 효과 적용)
	drawButton(startButton_);
	drawButton(exitButton_);

	const Size window = Scene::Size();
	const int32 f = cornerFrame_;
	const bool isFullscreen = Window::GetState().fullscreen;
	const double scale = isFullscreen ? 0.75 : 0.5;

	if (gameData_ && gameData_->finalStageCleared())
	{
		cornerTL_[f].scaled(scale).draw(0, 0);
		cornerTR_[f].scaled(scale).draw(window.x - cornerTR_[f].width() * scale, 0);
		cornerBL_[f].scaled(scale).draw(0, window.y - cornerBL_[f].height() * scale);
		cornerBR_[f].scaled(scale).draw(window.x - cornerBR_[f].width() * scale, window.y - cornerBR_[f].height() * scale);
	}

	if (isFadingOut_)
	{
		Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, fadeOutTimer_ / fadeOutDuration_ });
	}
	else if (fadeAlpha < 1.0)
	{
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
	double fadeAlpha = getFadeAlpha();
	if (isFadingOut_) {
		fadeAlpha = 1.0 - (fadeOutTimer_ / fadeOutDuration_);
	}

	if (&button == &startButton_) {
		texture = &startButtonFrames_[animationFrameIndex_];
	}
	else if (&button == &exitButton_) {
		texture = &exitButtonFrames_[animationFrameIndex_];
	}

	if (texture && texture->isEmpty() == false) {
		texture->resized(rect.size).draw(rect.pos, ColorF(1.0, fadeAlpha));
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
        return fadeTimer_ / fadeDuration_;
    }
    return 1.0;
}

void MainMenuScene::startFadeOut(SceneType targetScene)
{
    isFadingOut_ = true;
    fadeOutTimer_ = 0.0;
    nextScene_ = targetScene;
}
