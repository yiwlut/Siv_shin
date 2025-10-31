#pragma once
#include "SceneManager.hpp"

class OpeningScene : public GameScene
{
public:
    OpeningScene();
    ~OpeningScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;

private:

	struct RubyText
	{
		String baseText;   
		String rubyText;   
		ColorF color;       

		RubyText(const String& base, const String& ruby, const ColorF& col = ColorF{ 0.95, 0.95, 0.9, 1.0 })
			: baseText(base), rubyText(ruby), color(col) {
		}
	};

	struct StorySlide
	{
		String imagePath;
		Array<String> textLines;
	};
	double elapsedTime_;
	double slideFadeInDuration_;      
	double slideDisplayDuration_;     
	double slideFadeOutDuration_;     
	double slideInterval_;             

	Font textFont_;     
	Font rubyFont_;      
	Font skipFont_;      

	Audio openingBgm_;

	Array<StorySlide> slides_;
	Array<Texture> slideTextures_;
	size_t currentSlideIndex_;

	bool isFastForward_;
	bool isFullscreen_;
	Array<Texture> ffwdTextures_;
	double ffwdAnimTime_;

	double textTypingTime_;
	size_t visibleCharCount_;
	double getCurrentSlideAlpha() const;
	void drawCurrentSlide();
	void drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color, double alpha) const;
	Array<RubyText> parseRubyText(const String& text) const;
	void drawSkipUI();
};
