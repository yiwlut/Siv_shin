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

	// 루비(후리가나) 텍스트 구조체
	struct RubyText
	{
		String baseText;  // 한자
		String rubyText;  // 후리가나
		ColorF color;     // 텍스트 색상

		RubyText(const String& base, const String& ruby, const ColorF& col = ColorF{ 0.95, 0.95, 0.9, 1.0 })
			: baseText(base), rubyText(ruby), color(col) {
		}
	};

	// 스토리 슬라이드 구조체
	struct StorySlide
	{
		String imagePath;  // 이미지 경로
		String text;       // 하단 텍스트
	};

	// 타이머
	double elapsedTime_;
	double slideFadeInDuration_;   // 슬라이드 페이드인 시간
	double slideDisplayDuration_;  // 슬라이드 표시 시간
	double slideFadeOutDuration_;  // 슬라이드 페이드아웃 시간
	double slideInterval_;         // 한 슬라이드의 총 시간

	// 폰트
	Font textFont_;   // 텍스트용 폰트
	Font rubyFont_;   // 루비(후리가나)용 작은 폰트
	Font skipFont_;   // 스킵 안내용 폰트

	// 배경음악
	Audio openingBgm_;

	// 스토리 데이터
	Array<StorySlide> slides_;
	Array<Texture> slideTextures_;
	size_t currentSlideIndex_;

	// 패스트포워드 기능
	bool isFastForward_;
	bool isFullscreen_;
	Array<Texture> ffwdTextures_;
	double ffwdAnimTime_;

	// 헬퍼 함수
	double getCurrentSlideAlpha() const;
	void drawCurrentSlide();
	void drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color, double alpha) const;
	Array<RubyText> parseRubyText(const String& text) const;
	void drawSkipUI();
};
