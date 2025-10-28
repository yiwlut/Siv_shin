#include "OpeningScene.hpp"

OpeningScene::OpeningScene()
	: elapsedTime_(0.0)
	, slideFadeInDuration_(1.5)
	, slideDisplayDuration_(3.0)
	, slideFadeOutDuration_(1.5)
	, slideInterval_(6.0)  // 1.5 + 3.0 + 1.5
	, textFont_(FontMethod::MSDF, 28, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, rubyFont_(FontMethod::MSDF, 14, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, skipFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, openingBgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"))
	, currentSlideIndex_(0)
	, isFastForward_(false)
	, isFullscreen_(false)
{
	currentScene_ = SceneType::Opening;

	// 스토리 슬라이드 데이터 초기화
	slides_ = {
		{ U"ArtResources/Texture2D/Story/op_0.png", U"かつて、あの【愚（おろ）】かなる【蛸（たこ）】は、深海（しんかい）のすべての【色】を手にしたと自惚（うぬぼ）れていた。" },
		{ U"ArtResources/Texture2D/Story/op_1.png", U"身の程も知らず、【神】の領域（りょういき）たる「【パレット】」を欲（ほっ）したその【罪（つみ）】。" },
		{ U"ArtResources/Texture2D/Story/op_2.png", U"我（われ）はやつから【光】を奪（うば）い、永遠（えいえん）の【闇（やみ）】をくれてやった。" },
		{ U"ArtResources/Texture2D/Story/op_3.png", U"今や、やつがその狭（せま）い深海（しんかい）でできる唯一（ゆいいつ）のこととは…" },
		{ U"ArtResources/Texture2D/Story/op_4.png", U"【蛸壺（たこつぼ）】やはかなき【夢（ゆめ）】を【夏（なつ）】の【月（つき）】" },
		{ U"ArtResources/Texture2D/Story/op_5.png", U"その【夢】の中で、もがき続けることだけだ。" },
		{ U"ArtResources/Texture2D/Story/op_6.png", U"さあ、【愚】かなる【蛸】よ。お前の【罪】を思い出せ。" }
	};

	// 텍스처 로드
	for (const auto& slide : slides_)
	{
		slideTextures_.push_back(Texture(Resource(slide.imagePath)));
	}
	// FFWD 텍스처 로드
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_0.png")));
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_1.png")));
	ffwdAnimTime_ = 0.0;
}
void OpeningScene::onEnter()
{
	elapsedTime_ = 0.0;
	isFastForward_ = false;
	ffwdAnimTime_ = 0.0;
	if (!openingBgm_.isEmpty())
	{
		openingBgm_.setLoop(false);
		openingBgm_.setVolume(0.3);
		openingBgm_.play();
	}
}
void OpeningScene::onExit()
{
	if (!openingBgm_.isEmpty() && openingBgm_.isPlaying())
	{
		openingBgm_.stop();
	}
}
void OpeningScene::update()
{
	double dt = Scene::DeltaTime();
	isFullscreen_ = Window::GetState().fullscreen;

	if (KeySpace.pressed()) {
		isFastForward_ = true;
		ffwdAnimTime_ += dt;
		dt *= 3.0;
	}
	else {
		isFastForward_ = false;
		ffwdAnimTime_ = 0.0;
	}

	elapsedTime_ += dt;
	currentSlideIndex_ = static_cast<size_t>(elapsedTime_ / slideInterval_);

	if (KeyF2.down()) {
		changeScene(SceneType::InGame);
		return;
	}

	if (currentSlideIndex_ >= slides_.size()) {
		changeScene(SceneType::InGame);
		return;
	}
}
void OpeningScene::draw()
{
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });
	drawCurrentSlide();
	drawSkipUI();
}

double OpeningScene::getCurrentSlideAlpha() const
{
	const double slideTime = elapsedTime_ - (currentSlideIndex_ * slideInterval_);

	if (slideTime < slideFadeInDuration_)
	{
		// 페이드인
		return slideTime / slideFadeInDuration_;
	}
	else if (slideTime < slideFadeInDuration_ + slideDisplayDuration_)
	{
		// 표시
		return 1.0;
	}
	else
	{
		// 페이드아웃
		const double fadeOutTime = slideTime - (slideFadeInDuration_ + slideDisplayDuration_);
		return Math::Max(0.0, 1.0 - (fadeOutTime / slideFadeOutDuration_));
	}
}

void OpeningScene::drawCurrentSlide()
{
	if (currentSlideIndex_ >= slides_.size())
	{
		return;
	}

	const double alpha = getCurrentSlideAlpha();
	if (alpha <= 0.0)
	{
		return;
	}
	const Texture& texture = slideTextures_[currentSlideIndex_];
	const Vec2 center = Scene::Center();
	const double scale = isFullscreen_ ? 1.0 : 0.66;
	const double yOffset = isFullscreen_ ? -50 : -120;
	const double scaledHeight = texture.height() * scale;
	const Vec2 imagePos = Vec2{ center.x - texture.width() * scale / 2.0, center.y - scaledHeight / 2.0 + yOffset };
	texture.scaled(scale).draw(imagePos, ColorF{ 1.0, alpha });
	// 하단 텍스트 그리기
	const String& text = slides_[currentSlideIndex_].text;
	const Vec2 textPos = Vec2{ center.x, Scene::Height() - 150 };
	drawTextWithRuby(text, textPos, ColorF{ 0.95, 0.95, 0.9 }, alpha);
}
void OpeningScene::drawSkipUI()
{
	const double gameWidth = Scene::Size().x;
	const double gameHeight = Scene::Size().y;
	const Vec2 imagePos = Vec2{ gameWidth - 60, gameHeight - 80 };

	if (!ffwdTextures_.isEmpty()) {
		const size_t frameIndex = isFastForward_ ? (static_cast<size_t>(ffwdAnimTime_ * 16.0) % 2) : 0;
		const Texture& ffwdTex = ffwdTextures_[frameIndex];
		const Vec2 texPos = Vec2{ imagePos.x - ffwdTex.width() * 0.12 / 2.0, imagePos.y - ffwdTex.height() * 0.12 / 2.0 };
		ffwdTex.scaled(0.12).draw(texPos);
	}

	const Vec2 spaceTextPos = Vec2{ gameWidth - 60, gameHeight - 40 };
	skipFont_(U"SPACE").drawAt(spaceTextPos, ColorF{ 1.0, 1.0, 1.0, 0.85 });
}
Array<OpeningScene::RubyText> OpeningScene::parseRubyText(const String& text) const
{
	Array<RubyText> result;
	String currentBase;
	String currentRuby;
	bool inParenthesis = false;
	bool hasRubyForCurrent = false;
	bool isHighlighted = false;

	for (size_t i = 0; i < text.length(); ++i)
	{
		const char32 ch = text[i];

		if (ch == U'【')
		{
			isHighlighted = true;
		}
		else if (ch == U'】')
		{
			if (!currentBase.isEmpty())
			{
				ColorF highlightColor = ColorF{ 1.0, 1.0, 0.7 };
				result.push_back(RubyText{ currentBase, currentRuby, highlightColor });
				currentBase.clear();
				currentRuby.clear();
			}
			isHighlighted = false;
			hasRubyForCurrent = false;
		}
		else if (ch == U'（')
		{
			inParenthesis = true;
			hasRubyForCurrent = true;
			currentRuby.clear();
		}
		else if (ch == U'）')
		{
			if (!currentBase.isEmpty())
			{
				ColorF textColor = isHighlighted ? ColorF{ 1.0, 1.0, 0.7 } : ColorF{ 0.95, 0.95, 0.9 };
				result.push_back(RubyText{ currentBase, currentRuby, textColor });
				currentBase.clear();
				currentRuby.clear();
			}
			inParenthesis = false;
			hasRubyForCurrent = false;
		}
		else if (inParenthesis)
		{
			currentRuby.push_back(ch);
		}
		else
		{
			bool nextIsRuby = (i + 1 < text.length() && text[i + 1] == U'（');
			bool nextIsHighlightEnd = (i + 1 < text.length() && text[i + 1] == U'】');

			if (!nextIsRuby && !currentBase.isEmpty() && !hasRubyForCurrent && !isHighlighted)
			{
				result.push_back(RubyText{ currentBase, U"", ColorF{ 0.95, 0.95, 0.9 } });
				currentBase.clear();
			}

			currentBase.push_back(ch);

			if (!nextIsRuby && !hasRubyForCurrent && !isHighlighted && !nextIsHighlightEnd)
			{
				result.push_back(RubyText{ currentBase, U"", ColorF{ 0.95, 0.95, 0.9 } });
				currentBase.clear();
			}
		}
	}

	if (!currentBase.isEmpty())
	{
		ColorF textColor = isHighlighted ? ColorF{ 1.0, 1.0, 0.7 } : ColorF{ 0.95, 0.95, 0.9 };
		result.push_back(RubyText{ currentBase, U"", textColor });
	}

	return result;
}

void OpeningScene::drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color, double alpha) const
{
	Array<RubyText> parts = parseRubyText(text);

	double totalWidth = 0.0;
	for (const auto& part : parts)
	{
		totalWidth += textFont_(part.baseText).region().w;
	}

	double currentX = basePos.x - (totalWidth / 2.0);

	for (const auto& part : parts)
	{
		const double baseWidth = textFont_(part.baseText).region().w;

		if (!part.rubyText.isEmpty())
		{
			const double rubyWidth = rubyFont_(part.rubyText).region().w;
			const double baseCenterX = currentX + (baseWidth / 2.0);
			const double rubyX = baseCenterX - (rubyWidth / 2.0);

			ColorF rubyColor = part.color.withAlpha(alpha);
			rubyFont_(part.rubyText).draw(rubyX, basePos.y - 20.0, rubyColor);
		}

		ColorF textColor = part.color.withAlpha(alpha);
		textFont_(part.baseText).draw(currentX, basePos.y, textColor);

		currentX += baseWidth;
	}
}
