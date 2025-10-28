#include "OpeningScene.hpp"

OpeningScene::OpeningScene()
	: elapsedTime_(0.0)
	, fadeInDuration_(1.5)      // フェードイン 1.5秒
	, displayDuration_(3.0)     // 表示 3秒
	, fadeOutDuration_(1.5)     // フェードアウト 1.5秒
	, totalDuration_(30.0)
	, haikuFont_(FontMethod::MSDF, 32, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // 横書き用に小さく
	, rubyFont_(FontMethod::MSDF, 16, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))   // 루비용 작은 폰트
	, authorFont_(FontMethod::MSDF, 40, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, skipFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, openingBgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"))
	, haikuLineDelay_(2.4)       // 各行ごとに2.4秒間隔
	, haikuLineFadeDuration_(2.0) // 各行が2秒でフェードイン
	, typewriterSpeed_(0.1)     // 문자당 0.1초
	, skipPressTime_(0.0)
	, skipRequiredTime_(3.0)     // 3秒押せばスキップ
	, isSkipping_(false)
{
	currentScene_ = SceneType::Opening;
}
void OpeningScene::onEnter()
{
	elapsedTime_ = 0.0;
	skipPressTime_ = 0.0;
	isSkipping_ = false;
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
	const double dt = Scene::DeltaTime();
	elapsedTime_ += dt;
	if (KeyF2.down())
	{
		changeScene(SceneType::InGame);
		return;
	}
	if (KeySpace.pressed())
	{
		skipPressTime_ += dt;
		isSkipping_ = true;
		if (skipPressTime_ >= skipRequiredTime_)
		{
			changeScene(SceneType::InGame);
			return;
		}
	}
	else
	{
		skipPressTime_ = 0.0;
		isSkipping_ = false;
	}
	if (elapsedTime_ >= totalDuration_)
	{
		changeScene(SceneType::InGame);
		return;
	}
}
void OpeningScene::draw()
{
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 });
	drawHaiku();
	drawSkipUI();
}
double OpeningScene::getFadeAlpha() const
{
	if (elapsedTime_ < fadeInDuration_)
	{
		return elapsedTime_ / fadeInDuration_;
	}
	else if (elapsedTime_ < fadeInDuration_ + displayDuration_)
	{
		return 1.0;
	}
	else
	{
		const double fadeOutElapsed = elapsedTime_ - (fadeInDuration_ + displayDuration_);
		return 1.0 - (fadeOutElapsed / fadeOutDuration_);
	}
}
void OpeningScene::drawHaiku()
{
	Array<String> haikuLines = {
		U"かつて、あの【愚（おろ）】かなる【蛸（たこ）】は、",
		U"深海（しんかい）のすべての【色】を手にしたと自惚（うぬぼ）れていた。",
		U"身の程も知らず、",
		U"【神】の領域（りょういき）たる「【パレット】」を欲（ほっ）したその【罪（つみ）】。",
		U"我（われ）はやつから【光】を奪（うば）い、",
		U"永遠（えいえん）の【闇（やみ）】をくれてやった。",
		U"今や、やつがその狭（せま）い深海（しんかい）で",
		U"できる唯一（ゆいいつ）のこととは…",
		U"【蛸壺（たこつぼ）】やはかなき【夢（ゆめ）】を【夏（なつ）】の【月（つき）】"
	};
	const Vec2 center = Scene::Center();
	const double lineSpacing = 60;  // 행간격（上から下へ）
	const size_t totalLines = haikuLines.size();
	const double totalHeight = lineSpacing * (totalLines - 1);
	const double startY = center.y - (totalHeight / 2.0) - 100.0;  // 100픽셀 위로 이동
	const double centerX = center.x;
	for (size_t lineIndex = 0; lineIndex < totalLines; ++lineIndex)
	{
		const String& line = haikuLines[lineIndex];
		const double lineY = startY + (lineIndex * lineSpacing);
		const double lineAlpha = getHaikuLineAlpha(lineIndex);
		if (lineAlpha <= 0.0)
		{
			continue;
		}
		const size_t visibleCharCount = getVisibleCharCount(lineIndex);
		drawTextWithRubyPartial(line, Vec2{ centerX, lineY }, lineAlpha, visibleCharCount);
	}
}
size_t OpeningScene::getVisibleCharCount(size_t lineIndex) const
{
	size_t adjustedIndex = lineIndex;
	if (lineIndex >= 3)
	{
		adjustedIndex = lineIndex - 1;
	}
	double lineStartTime = adjustedIndex * haikuLineDelay_;
	if (lineIndex >= 4)
	{
		lineStartTime += 2.0;  // 2초 추가 딜레이
	}
	if (elapsedTime_ < lineStartTime)
	{
		return 0;
	}
	const double timeSinceLineStart = elapsedTime_ - lineStartTime;
	const size_t visibleChars = static_cast<size_t>(timeSinceLineStart / typewriterSpeed_);
	return visibleChars;
}
double OpeningScene::getHaikuLineAlpha(size_t lineIndex) const
{
	size_t adjustedIndex = lineIndex;
	if (lineIndex >= 3)
	{
		adjustedIndex = lineIndex - 1;  // 4번째 줄부터는 한 칸씩 당김
	}
	double lineStartTime = adjustedIndex * haikuLineDelay_;
	if (lineIndex >= 4)
	{
		lineStartTime += 2.0;  // 2초 추가 딜레이
	}

	const double lineEndTime = lineStartTime + haikuLineFadeDuration_;
	if (elapsedTime_ < lineStartTime)
	{
		return 0.0;
	}
	if (elapsedTime_ < lineEndTime)
	{
		return (elapsedTime_ - lineStartTime) / haikuLineFadeDuration_;
	}
	const double totalHaikuDisplayTime = 10.0;  // 모든 줄 표시 후 10초간 유지
	const double fadeOutStartTime = 8 * haikuLineDelay_ + 2.0 + totalHaikuDisplayTime;  // 3,4번째 줄 동시 표시 + 2초 딜레이

	if (elapsedTime_ < fadeOutStartTime)
	{
		return 1.0;
	}
	const double fadeOutProgress = (elapsedTime_ - fadeOutStartTime) / fadeOutDuration_;
	return Math::Max(0.0, 1.0 - fadeOutProgress);
}

void OpeningScene::drawSkipUI()
{
	const double gameWidth = Scene::Size().x;
	const double gameHeight = Scene::Size().y;
	const Vec2 uiPos = Vec2{ gameWidth - 80, gameHeight - 80 };
	const double radius = 30.0;
	Circle{ uiPos, radius }.draw(ColorF{ 0.2, 0.2, 0.2, 0.5 });
	Circle{ uiPos, radius }.drawFrame(2.0, ColorF{ 0.6, 0.6, 0.6, 0.7 });
	if (isSkipping_ && skipPressTime_ > 0.0)
	{
		const double progress = skipPressTime_ / skipRequiredTime_;
		const double startAngle = -Math::HalfPi;  // 12시 방향부터 시작
		const double sweepAngle = Math::TwoPi * progress;
		Circle{ uiPos, radius - 4 }.drawPie(startAngle, sweepAngle, ColorF{ 0.3, 0.8, 1.0, 0.8 });
		Circle{ uiPos, radius - 4 }.drawArc(startAngle, sweepAngle, 4.0, 4.0, ColorF{ 0.5, 1.0, 1.0, 1.0 });
	}
	skipFont_(U"SPACE").drawAt(uiPos, ColorF{ 1.0, 1.0, 1.0, 0.9 });
	const Vec2 skipTextPos = Vec2{ gameWidth - 80, gameHeight - 45 };
	skipFont_(U"スキップ").drawAt(skipTextPos, ColorF{ 0.9, 0.9, 0.9, 0.8 });
	const Vec2 holdTextPos = Vec2{ gameWidth - 80, gameHeight - 25 };
	skipFont_(U"Hold 3s").drawAt(holdTextPos, ColorF{ 0.8, 0.8, 0.8, 0.6 });
}

Array<OpeningScene::RubyText> OpeningScene::parseRubyText(const String& text) const
{
	Array<RubyText> result;
	String currentBase;
	String currentRuby;
	bool inParenthesis = false;
	bool hasRubyForCurrent = false;
	bool isHighlighted = false;  // 【】로 강조된 텍스트인지

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
				ColorF highlightColor = ColorF{ 1.0, 1.0, 0.7 };  // 연노란색 (낮은 채도)
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

void OpeningScene::drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color) const
{
	Array<RubyText> parts = parseRubyText(text);
	double totalWidth = 0.0;
	for (const auto& part : parts)
	{
		totalWidth += haikuFont_(part.baseText).region().w;
	}
	double currentX = basePos.x - (totalWidth / 2.0);
	for (const auto& part : parts)
	{
		const double baseWidth = haikuFont_(part.baseText).region().w;
		if (!part.rubyText.isEmpty())
		{
			const double rubyWidth = rubyFont_(part.rubyText).region().w;
			const double baseCenterX = currentX + (baseWidth / 2.0);
			const double rubyX = baseCenterX - (rubyWidth / 2.0);
			ColorF rubyColor = part.color.withAlpha(color.a);
			rubyFont_(part.rubyText).draw(rubyX, basePos.y - 19.0, rubyColor);
		}
		ColorF textColor = part.color.withAlpha(color.a);
		haikuFont_(part.baseText).draw(currentX, basePos.y, textColor);
		currentX += baseWidth;
	}
}

void OpeningScene::drawTextWithRubyPartial(const String& text, const Vec2& basePos, double alpha, size_t visibleCharCount) const
{
	Array<RubyText> parts = parseRubyText(text);
	double totalWidth = 0.0;
	for (const auto& part : parts)
	{
		totalWidth += haikuFont_(part.baseText).region().w;
	}
	double currentX = basePos.x - (totalWidth / 2.0);
	size_t displayedChars = 0;
	for (const auto& part : parts)
	{
		if (displayedChars >= visibleCharCount)
		{
			break;
		}
		const double baseWidth = haikuFont_(part.baseText).region().w;
		const size_t charsToShow = Min(part.baseText.length(), visibleCharCount - displayedChars);
		const String partialBase = part.baseText.substr(0, charsToShow);
		if (!part.rubyText.isEmpty() && charsToShow == part.baseText.length())
		{
			const double rubyWidth = rubyFont_(part.rubyText).region().w;
			const double baseCenterX = currentX + (baseWidth / 2.0);
			const double rubyX = baseCenterX - (rubyWidth / 2.0);
			ColorF rubyColor = part.color.withAlpha(alpha);
			rubyFont_(part.rubyText).draw(rubyX, basePos.y - 19.0, rubyColor);
		}
		ColorF textColor = part.color.withAlpha(alpha);
		haikuFont_(partialBase).draw(currentX, basePos.y, textColor);
		displayedChars += charsToShow;
		currentX += baseWidth;
	}
}
