#include "OpeningScene.hpp"

OpeningScene::OpeningScene()
	: elapsedTime_(0.0)
	, slideFadeInDuration_(0.7)
	, slideDisplayDuration_(6.5)
	, slideFadeOutDuration_(0.8)
	, slideInterval_(8.0)
	, textFont_(FontMethod::MSDF, 28, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, rubyFont_(FontMethod::MSDF, 14, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, skipFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, openingBgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"))
	, currentSlideIndex_(0)
	, isFastForward_(false)
	, isFullscreen_(false)
{
	currentScene_ = SceneType::Opening;
	slides_ = {
	{ U"ArtResources/Texture2D/Story/op_0.png", { U"深い海の底に、タコの家族が暮らしていた。", U"彼らはある神のペットとして、幸せな日々を送っていた。" } },
	{ U"ArtResources/Texture2D/Story/op_1.png", { U"しかし、主人公のタコは神の一番のお気に入りであり、", U"唯一、カゴの外を自由に泳ぎ回ることを許されていた。" } },
	{ U"ArtResources/Texture2D/Story/op_2.png", { U"好奇心旺盛なタコは、ある日、", U"キッチンのほうまで泳いでいってしまう。", U"そこには、一冊の本が開かれたまま置かれていた。" } },	{ U"ArtResources/Texture2D/Story/op_3.png", { U"タコが泳ぎ寄って覗き込むと、そこには…", U"「たこ焼きの作り方」と書かれたページが開かれていた。" } },
	{ U"ArtResources/Texture2D/Story/op_4.png", { U"タコは気づいてしまった。", U"自分たちはペットではなく…", U"「食材」だったのだという真実に！" } },	{ U"ArtResources/Texture2D/Story/op_5.png", { U"兄弟たちを残し、タコは必死で逃げ出した。", U"心を引き裂かれるような思いで。" } },
	{ U"ArtResources/Texture2D/Story/op_6.png", { U"こうしてタコの新たな旅が始まった。", U"深海に散らばった「色」を集め、", U"あの神に立ち向かい、兄弟たちを救い出すために！" } }
	};
	for (const auto& slide : slides_)
	{
		slideTextures_.push_back(Texture(Resource(slide.imagePath)));
	}
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_0.png")));
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_1.png")));
	ffwdAnimTime_ = 0.0;
	textTypingTime_ = 0.0;
	visibleCharCount_ = 0;
}
void OpeningScene::onEnter()
{
	elapsedTime_ = 0.0;
	isFastForward_ = false;
	ffwdAnimTime_ = 0.0;
	textTypingTime_ = 0.0;
	visibleCharCount_ = 0;
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
	const size_t prevIndex = currentSlideIndex_;
	currentSlideIndex_ = static_cast<size_t>(elapsedTime_ / slideInterval_);

	if (prevIndex != currentSlideIndex_) {
		textTypingTime_ = 0.0;
		visibleCharCount_ = 0;
	}

	if (currentSlideIndex_ < slides_.size()) {
		textTypingTime_ += dt;
		const double charsPerSecond = 12.5;
		visibleCharCount_ = static_cast<size_t>(textTypingTime_ * charsPerSecond);
	}

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
		return slideTime / slideFadeInDuration_;
	}
	else if (slideTime < slideFadeInDuration_ + slideDisplayDuration_)
	{
		return 1.0;
	}
	else
	{
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
	const double scale = isFullscreen_ ? 0.88 : 0.66;
	const double yOffset = isFullscreen_ ? -70 : -140;
	const double scaledHeight = texture.height() * scale;
	const Vec2 imagePos = Vec2{ center.x - texture.width() * scale / 2.0, center.y - scaledHeight / 2.0 + yOffset };
	texture.scaled(scale).draw(imagePos, ColorF{ 1.0, alpha });
	const Array<String>& textLines = slides_[currentSlideIndex_].textLines;
	const double lineHeight = 40.0;
	const double startY = Scene::Height() - 260.0;
	const double leftMargin = isFullscreen_ ? (Scene::Width() - 1280.0) / 2.0 + 80.0 : 80.0;
	size_t charCount = 0;
	for (size_t i = 0; i < textLines.size(); ++i) {
		const String& line = textLines[i];
		const size_t lineLength = line.length();

		if (charCount < visibleCharCount_) {
			const size_t visibleInLine = std::min(visibleCharCount_ - charCount, lineLength);
			const String visibleText = line.substr(0, visibleInLine);
			const Vec2 textPos = Vec2{ leftMargin, startY + i * lineHeight };
			drawTextWithRuby(visibleText, textPos, ColorF{ 0.95, 0.95, 0.9 }, alpha);
		}
		charCount += lineLength;
	}
}
void OpeningScene::drawSkipUI()
{
	const double gameWidth = Scene::Size().x;
	const double gameHeight = Scene::Size().y;
	const Vec2 imagePos = Vec2{ gameWidth - 60, gameHeight - 80 };

	if (!ffwdTextures_.isEmpty()) {
		const size_t frameIndex = isFastForward_ ? (static_cast<size_t>(ffwdAnimTime_ * 16.0) % 2) : 0;
		const Texture& ffwdTex = ffwdTextures_[frameIndex];
		const Vec2 texPos = Vec2{ imagePos.x - ffwdTex.width() * 0.072 / 2.0, imagePos.y - ffwdTex.height() * 0.072 / 2.0 };
		ffwdTex.scaled(0.072).draw(texPos, ColorF{ 1.0, 0.8 });
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
	double currentX = basePos.x;
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
