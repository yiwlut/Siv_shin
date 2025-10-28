#include "EndingScene.hpp"

EndingScene::EndingScene()
	: elapsedTime_(0.0), slideFadeInDuration_(0.7), slideDisplayDuration_(6.5), slideFadeOutDuration_(0.8), slideInterval_(8.0)
	, textFont_(FontMethod::MSDF, 28, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, rubyFont_(FontMethod::MSDF, 14, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, skipFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, endingBgm_(Resource(U"ArtResources/BGM/HappyOcean.mp3"))
	, currentSlideIndex_(0), isFastForward_(false), isFullscreen_(false), ffwdAnimTime_(0.0), textTypingTime_(0.0), visibleCharCount_(0) {
	currentScene_ = SceneType::Ending;
	slides_ = {
	{ U"ArtResources/Texture2D/Story/ed_0.png", { U"彼は、力を失い意識のない神のそばに落ちていた鍵を見つける。「これこそが、檻の鍵に違いない！」" } },
	{ U"ArtResources/Texture2D/Story/ed_1.png", { U"彼は鍵を頭に乗せて、懐かしい我が家へと急いで戻った。" } },
	{ U"ArtResources/Texture2D/Story/ed_2.png", { U"彼が鍵を扉に差し込むと……カチッ！ 鍵は扉を開き、すべての兄弟たちを解放した。" } },
	{ U"ArtResources/Texture2D/Story/ed_3.png", { U"彼らは抱き合った。すると、眩い色彩の渦が彼らの周りで弾けた。彼の旅路で得た色合いが、彼と兄弟たちを光り輝くスペクトルに染め上げる。それこそが、彼らが深海（シンかい）の新（あら）たな守護者となった証だった。" } },
	};
	for (const auto& slide : slides_) slideTextures_.push_back(Texture(Resource(slide.imagePath)));
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_0.png")));
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_1.png")));
}

void EndingScene::onEnter() {
	elapsedTime_ = 0.0; isFastForward_ = false; ffwdAnimTime_ = 0.0; textTypingTime_ = 0.0; visibleCharCount_ = 0;
	if (!endingBgm_.isEmpty()) { endingBgm_.setLoop(true); endingBgm_.setVolume(0.3); endingBgm_.play(); }
}

void EndingScene::onExit() { if (!endingBgm_.isEmpty() && endingBgm_.isPlaying()) endingBgm_.stop(); }

void EndingScene::update() {
	double dt = Scene::DeltaTime(); isFullscreen_ = Window::GetState().fullscreen;
	if (KeySpace.pressed()) { isFastForward_ = true; ffwdAnimTime_ += dt; dt *= 3.0; }
	else { isFastForward_ = false; ffwdAnimTime_ = 0.0; }
	elapsedTime_ += dt;
	const size_t prevIndex = currentSlideIndex_; currentSlideIndex_ = static_cast<size_t>(elapsedTime_ / slideInterval_);
	if (prevIndex != currentSlideIndex_) { textTypingTime_ = 0.0; visibleCharCount_ = 0; }
	if (currentSlideIndex_ < slides_.size()) { textTypingTime_ += dt; visibleCharCount_ = static_cast<size_t>(textTypingTime_ * 12.5); }
	if (KeyF2.down()) { changeScene(SceneType::MainMenu); return; }
	if (currentSlideIndex_ >= slides_.size()) { changeScene(SceneType::MainMenu); return; }
}

void EndingScene::draw() {
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 }); drawCurrentSlide(); drawSkipUI();
}

double EndingScene::getCurrentSlideAlpha() const {
	const double slideTime = elapsedTime_ - (currentSlideIndex_ * slideInterval_);
	if (slideTime < slideFadeInDuration_) return slideTime / slideFadeInDuration_;
	else if (slideTime < slideFadeInDuration_ + slideDisplayDuration_) return 1.0;
	else return Math::Max(0.0, 1.0 - ((slideTime - (slideFadeInDuration_ + slideDisplayDuration_)) / slideFadeOutDuration_));
}

void EndingScene::drawCurrentSlide() {
	if (currentSlideIndex_ >= slides_.size()) return;
	const double alpha = getCurrentSlideAlpha(); if (alpha <= 0.0) return;
	const Texture& texture = slideTextures_[currentSlideIndex_]; const Vec2 center = Scene::Center();
	const double scale = isFullscreen_ ? 0.88 : 0.66, yOffset = isFullscreen_ ? -70 : -140;
	const Vec2 imagePos = Vec2{ center.x - texture.width() * scale / 2.0, center.y - texture.height() * scale / 2.0 + yOffset };
	texture.scaled(scale).draw(imagePos, ColorF{ 1.0, alpha });
	const Array<String>& textLines = slides_[currentSlideIndex_].textLines;
	const double lineHeight = 40.0, startY = Scene::Height() - 260.0, leftMargin = isFullscreen_ ? (Scene::Width() - 1280.0) / 2.0 + 80.0 : 80.0;
	size_t charCount = 0;
	for (size_t i = 0; i < textLines.size(); ++i) {
		const String& line = textLines[i]; const size_t lineLength = line.length();
		if (charCount < visibleCharCount_) {
			const size_t visibleInLine = std::min(visibleCharCount_ - charCount, lineLength);
			drawTextWithRuby(line.substr(0, visibleInLine), Vec2{ leftMargin, startY + i * lineHeight }, ColorF{ 0.95, 0.95, 0.9 }, alpha);
		}
		charCount += lineLength;
	}
}

void EndingScene::drawSkipUI() {
	const double gameWidth = Scene::Size().x, gameHeight = Scene::Size().y; const Vec2 imagePos = Vec2{ gameWidth - 60, gameHeight - 80 };
	if (!ffwdTextures_.isEmpty()) {
		const size_t frameIndex = isFastForward_ ? (static_cast<size_t>(ffwdAnimTime_ * 16.0) % 2) : 0;
		ffwdTextures_[frameIndex].scaled(0.072).draw(Vec2{ imagePos.x - ffwdTextures_[frameIndex].width() * 0.036, imagePos.y - ffwdTextures_[frameIndex].height() * 0.036 }, ColorF{ 1.0, 0.8 });
	}
	skipFont_(U"SPACE").drawAt(Vec2{ gameWidth - 60, gameHeight - 40 }, ColorF{ 1.0, 1.0, 1.0, 0.85 });
}

Array<EndingScene::RubyText> EndingScene::parseRubyText(const String& text) const {
	Array<RubyText> result; String currentBase, currentRuby; bool inParenthesis = false, hasRubyForCurrent = false, isHighlighted = false;
	for (size_t i = 0; i < text.length(); ++i) {
		const char32 ch = text[i];
		if (ch == U'【') isHighlighted = true;
		else if (ch == U'】') { if (!currentBase.isEmpty()) result.push_back(RubyText{ currentBase, currentRuby, ColorF{1.0, 1.0, 0.7} }); currentBase.clear(); currentRuby.clear(); isHighlighted = false; hasRubyForCurrent = false; }
		else if (ch == U'（') { inParenthesis = true; hasRubyForCurrent = true; currentRuby.clear(); }
		else if (ch == U'）') { if (!currentBase.isEmpty()) result.push_back(RubyText{ currentBase, currentRuby, isHighlighted ? ColorF{1.0, 1.0, 0.7} : ColorF{0.95, 0.95, 0.9} }); currentBase.clear(); currentRuby.clear(); inParenthesis = false; hasRubyForCurrent = false; }
		else if (inParenthesis) currentRuby.push_back(ch);
		else { bool nextIsRuby = (i + 1 < text.length() && text[i + 1] == U'（'), nextIsHighlightEnd = (i + 1 < text.length() && text[i + 1] == U'】'); if (!nextIsRuby && !currentBase.isEmpty() && !hasRubyForCurrent && !isHighlighted) { result.push_back(RubyText{ currentBase, U"", ColorF{0.95, 0.95, 0.9} }); currentBase.clear(); } currentBase.push_back(ch); if (!nextIsRuby && !hasRubyForCurrent && !isHighlighted && !nextIsHighlightEnd) { result.push_back(RubyText{ currentBase, U"", ColorF{0.95, 0.95, 0.9} }); currentBase.clear(); } }
	}
	if (!currentBase.isEmpty()) result.push_back(RubyText{ currentBase, U"", isHighlighted ? ColorF{1.0, 1.0, 0.7} : ColorF{0.95, 0.95, 0.9} });
	return result;
}

void EndingScene::drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color, double alpha) const {
	Array<RubyText> parts = parseRubyText(text); double currentX = basePos.x;
	for (const auto& part : parts) {
		const double baseWidth = textFont_(part.baseText).region().w;
		if (!part.rubyText.isEmpty()) { const double rubyWidth = rubyFont_(part.rubyText).region().w, baseCenterX = currentX + (baseWidth / 2.0), rubyX = baseCenterX - (rubyWidth / 2.0); rubyFont_(part.rubyText).draw(rubyX, basePos.y - 20.0, part.color.withAlpha(alpha)); }
		textFont_(part.baseText).draw(currentX, basePos.y, part.color.withAlpha(alpha)); currentX += baseWidth;
	}
}
