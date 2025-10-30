#include "BossIntroScene.hpp"

BossIntroScene::BossIntroScene()
	: elapsedTime_(0.0), slideFadeInDuration_(1.0), slideDisplayDuration_(9.0), slideFadeOutDuration_(1.0), slideInterval_(11.0), textFont_(FontMethod::MSDF, 28, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, rubyFont_(FontMethod::MSDF, 14, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, skipFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))
	, introBgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"))
	, currentSlideIndex_(0), isFastForward_(false), isFullscreen_(false), ffwdAnimTime_(0.0), textTypingTime_(0.0), visibleCharCount_(0) {
	currentScene_ = SceneType::BossIntro;
	slides_ = {
	{ U"ArtResources/Texture2D/Story/bossIntro_0.png", {
		U"あなたは、ついに最後の色を所定の場所に押し込む。",
	} },
		{ U"ArtResources/Texture2D/Story/bossIntro_1.png", {
		U"...その時、",
		U"背後から神の怒れる声が響き渡る。",
		U"「そこまでだ！」"
	} },
	{ U"ArtResources/Texture2D/Story/bossIntro_2.png", {
		U"貴様、私を侮って色を盗むとは,",
		U"「今すぐたこ焼きにしてくれるぞ！」"
	} },
	};
	for (const auto& slide : slides_) slideTextures_.push_back(Texture(Resource(slide.imagePath)));
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_0.png")));
	ffwdTextures_.push_back(Texture(Resource(U"ArtResources/Texture2D/FFWD/ffwd_1.png")));
}

void BossIntroScene::onEnter() {
	elapsedTime_ = 0.0; isFastForward_ = false; ffwdAnimTime_ = 0.0; textTypingTime_ = 0.0; visibleCharCount_ = 0;
	if (!introBgm_.isEmpty()) { introBgm_.setLoop(true); introBgm_.setVolume(0.3); introBgm_.play(); }
}

void BossIntroScene::onExit() { if (!introBgm_.isEmpty() && introBgm_.isPlaying()) introBgm_.stop(); }

void BossIntroScene::update() {
	double dt = Scene::DeltaTime(); isFullscreen_ = Window::GetState().fullscreen;
	if (KeySpace.pressed()) { isFastForward_ = true; ffwdAnimTime_ += dt; dt *= 3.0; }
	else { isFastForward_ = false; ffwdAnimTime_ = 0.0; }
	elapsedTime_ += dt;
	const size_t prevIndex = currentSlideIndex_; currentSlideIndex_ = static_cast<size_t>(elapsedTime_ / slideInterval_);
	if (prevIndex != currentSlideIndex_) { textTypingTime_ = 0.0; visibleCharCount_ = 0; }
	if (currentSlideIndex_ < slides_.size()) { textTypingTime_ += dt; visibleCharCount_ = static_cast<size_t>(textTypingTime_ * 15); }
	if (KeyF2.down()) { changeScene(SceneType::InGame); return; }
	if (currentSlideIndex_ >= slides_.size()) { changeScene(SceneType::InGame); return; }
}

void BossIntroScene::draw() {
	Scene::SetBackground(ColorF{ 0.0, 0.0, 0.0 }); drawCurrentSlide(); drawSkipUI();
}

double BossIntroScene::getCurrentSlideAlpha() const {
	const double slideTime = elapsedTime_ - (currentSlideIndex_ * slideInterval_);
	if (slideTime < slideFadeInDuration_) return slideTime / slideFadeInDuration_;
	else if (slideTime < slideFadeInDuration_ + slideDisplayDuration_) return 1.0;
	else return Math::Max(0.0, 1.0 - ((slideTime - (slideFadeInDuration_ + slideDisplayDuration_)) / slideFadeOutDuration_));
}

void BossIntroScene::drawCurrentSlide() {
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

void BossIntroScene::drawSkipUI() {
	const double gameWidth = Scene::Size().x, gameHeight = Scene::Size().y; const Vec2 imagePos = Vec2{ gameWidth - 60, gameHeight - 80 };
	if (!ffwdTextures_.isEmpty()) {
		const size_t frameIndex = isFastForward_ ? (static_cast<size_t>(ffwdAnimTime_ * 16.0) % 2) : 0;
		ffwdTextures_[frameIndex].scaled(0.072).draw(Vec2{ imagePos.x - ffwdTextures_[frameIndex].width() * 0.036, imagePos.y - ffwdTextures_[frameIndex].height() * 0.036 }, ColorF{ 1.0, 0.8 });
	}
	skipFont_(U"SPACE").drawAt(Vec2{ gameWidth - 60, gameHeight - 40 }, ColorF{ 1.0, 1.0, 1.0, 0.85 });
}

Array<BossIntroScene::RubyText> BossIntroScene::parseRubyText(const String& text) const {
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

void BossIntroScene::drawTextWithRuby(const String& text, const Vec2& basePos, const ColorF& color, double alpha) const {
	Array<RubyText> parts = parseRubyText(text); double currentX = basePos.x;
	for (const auto& part : parts) {
		const double baseWidth = textFont_(part.baseText).region().w;
		if (!part.rubyText.isEmpty()) { const double rubyWidth = rubyFont_(part.rubyText).region().w, baseCenterX = currentX + (baseWidth / 2.0), rubyX = baseCenterX - (rubyWidth / 2.0); rubyFont_(part.rubyText).draw(rubyX, basePos.y - 20.0, part.color.withAlpha(alpha)); }
		textFont_(part.baseText).draw(currentX, basePos.y, part.color.withAlpha(alpha)); currentX += baseWidth;
	}
}
