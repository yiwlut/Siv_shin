#pragma once
#include "SceneManager.hpp"

class EndingScene : public GameScene {
	friend class GameSceneManager;  // ★ 추가

public:
	EndingScene();
	~EndingScene() override = default;
	void update() override;
	void draw() override;
	void onEnter() override;
	void onExit() override;

private:
	GameData* gameData_ = nullptr;  // ★ 추가 (가장 아래에)

	struct RubyText {
		String baseText, rubyText;
		ColorF color;
		RubyText(const String& base, const String& ruby, const ColorF& col = ColorF{ 0.95, 0.95, 0.9, 1.0 })
			: baseText(base), rubyText(ruby), color(col) {
		}
	};
	struct StorySlide { String imagePath; Array<String> textLines; };
	double elapsedTime_, slideFadeInDuration_, slideDisplayDuration_, slideFadeOutDuration_, slideInterval_;
	Font textFont_, rubyFont_, skipFont_;
	Audio endingBgm_;
	Array<StorySlide> slides_;
	Array<Texture> slideTextures_, ffwdTextures_;
	size_t currentSlideIndex_;
	bool isFastForward_, isFullscreen_;
	double ffwdAnimTime_, textTypingTime_;
	size_t visibleCharCount_;
	double getCurrentSlideAlpha() const;
	void drawCurrentSlide();
	void drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color, double alpha) const;
	Array<RubyText> parseRubyText(const String& text) const;
	void drawSkipUI();
};
