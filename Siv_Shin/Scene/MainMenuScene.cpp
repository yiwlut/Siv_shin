#include "MainMenuScene.hpp"
#include <Windows.h>
#include <thread>
#include <GL/gl.h>
#pragma comment(lib,"opengl32")
#pragma comment(lib,"Advapi32")

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
    fadeTimer_ = 0.0;     
    isFadingOut_ = false;     
    fadeOutTimer_ = 0.0;

	logSystemInfo_();
    
    if (!bgm_.isEmpty())
    {
        bgm_.setLoop(true);     
        bgm_.setVolume(0.28);    
        bgm_.play();            
    }
    
}
void MainMenuScene::logSystemInfo_()
{
	auto rd = [&](HKEY r, const wchar_t* sub, const wchar_t* name)->std::wstring {
		HKEY h{}; if (RegOpenKeyExW(r, sub, 0, KEY_READ | KEY_WOW64_64KEY, &h) != ERROR_SUCCESS) return L"";
		std::wstring o; DWORD t = 0, s = 0;
		if (RegQueryValueExW(h, name, nullptr, &t, nullptr, &s) == ERROR_SUCCESS && t == REG_SZ) { o.resize(s / sizeof(wchar_t)); RegQueryValueExW(h, name, nullptr, &t, reinterpret_cast<LPBYTE>(o.data()), &s); if (!o.empty() && o.back() == L'\0') o.pop_back(); }
		RegCloseKey(h); return o;
		};
	auto cpu = [&]() -> String {
		auto s = rd(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString");
		return s.empty() ? String(U"Unknown") : Unicode::FromWstring(s);
		};
	auto os = [&]() -> String {
		auto pn = rd(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ProductName");
		auto dv = rd(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"DisplayVersion");
		if (dv.empty()) dv = rd(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"ReleaseId");
		auto bn = rd(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"CurrentBuildNumber");
		String s = Unicode::FromWstring(pn);
		if (!dv.empty()) s += U" " + Unicode::FromWstring(dv);
		if (!bn.empty()) s += U" (Build " + Unicode::FromWstring(bn) + U")";
		return s;
		};

	auto gpu = [&]() -> String {
		const GLubyte* v = glGetString(GL_VENDOR);
		const GLubyte* r = glGetString(GL_RENDERER);
		if (v && r) {
			String vendor = Unicode::FromUTF8((const char*)v);
			String renderer = Unicode::FromUTF8((const char*)r);
			return vendor + U" / " + renderer;
		}
		return U"Unknown";
	};

	const FilePath dir = FileSystem::GetFolderPath(SpecialFolder::UserProfile) + U"/AppData/LocalLow/TaconoDensetsuShinKai/";
	FileSystem::CreateDirectories(dir);
	TextWriter w(dir + U"Player.log");
	DateTime now = DateTime::Now();
	w.writeln(U"[System Information Log]");
	w.writeln(U"Timestamp: {0}/{1}/{2} {3}:{4}:{5}"_fmt(now.year, now.month, now.day, now.hour, now.minute, now.second));
	w.writeln(U"");
	w.writeln(U"[OS] {0}"_fmt(os()));
	w.writeln(U"[GPU] {0}"_fmt(gpu()));
	w.writeln(U"[CPU] {0}"_fmt(cpu()));
	w.writeln(U"");
	w.writeln(U"[Display]");
	w.writeln(U"Window Size: {0}x{1}"_fmt(Scene::Size().x, Scene::Size().y));
	w.writeln(U"");
	w.writeln(U"[Platform]");
	w.writeln(U"CPU Count: {0}"_fmt((uint32)std::thread::hardware_concurrency()));
	w.writeln(U"");
	w.writeln(U"[Game Info]");
	w.writeln(U"Fullscreen: {0}"_fmt(Window::GetState().fullscreen ? U"Yes" : U"No"));
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
    const double centerX = Scene::Size().x / 2.0;    
    const double centerY = Scene::Size().y / 2.0;    
    
    const double startButtonWidth = 300.0 * 0.7;   
    const double startButtonHeight = startButtonWidth * (566.0 / 879.0);    
    
    const double buttonGap = 30;     
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
    
    const double exitButtonWidth = 300.0 * 0.7;   
    const double exitButtonHeight = exitButtonWidth * (566.0 / 879.0);    
    
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
        {
            bgm_.play();
        }
        else if (!currentFocus && bgm_.isPlaying())
        {
            bgm_.pause();
        }
    }
    wasFocused_ = currentFocus;

	animationFrameTimer_ += deltaTime;
	if (animationFrameTimer_ >= animationFrameDuration_) {
		animationFrameTimer_ -= animationFrameDuration_;
		animationFrameIndex_ = (animationFrameIndex_ + 1) % 3;   
	}

    updateButton(startButton_);
    updateButton(exitButton_);
    
	if (startButton_.rect.leftClicked()) {
		SceneType target = SceneType::Opening;
		if (gameData_ && gameData_->finalStageCleared) target = SceneType::StageSelect;
		startFadeOut(target);
	}
    else if (exitButton_.rect.leftClicked())
    {
        System::Exit();
    }
	cornerTimer_ += Scene::DeltaTime();
	if (cornerTimer_ >= CORNER_ANIM_SPEED) {
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
    const double titleY = centerY - 150;     
    
    titleFont_(U"タコの伝説")  
        .drawAt(centerX, titleY, ColorF(1.0, 1.0, 1.0, fadeAlpha));     

	initializeButtons();
	drawButton(startButton_);
	drawButton(exitButton_);

	const Size window = Scene::Size();
	const int32 f = cornerFrame_;
	const bool isFullscreen = Window::GetState().fullscreen;
	const double scale = isFullscreen ? 0.75 : 0.5;

	if (gameData_ && gameData_->finalStageCleared)
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
