#include "StageSelectScene.hpp"
#include "StageData.hpp"

StageSelectScene::StageSelectScene()
	: titleFont_(FontMethod::MSDF, 48, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 일본어 폰트로 변경
	, stageFont_(FontMethod::MSDF, 24, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 일본어 폰트로 변경
	, infoFont_(FontMethod::MSDF, 16, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))   // ★ 일본어 폰트로 변경
	, stageNumberFont_(48, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))              // ★ 일본어 폰트로 변경
	, stageTitleFont_(FontMethod::MSDF, 32, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 큰 폰트 (28 + 4 = 32)
	, bgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"), Loop::Yes)
{
	currentScene_ = SceneType::StageSelect;
	loadStageTextures();
	initializeStages();
}

StageSelectScene::StageSelectScene(GameData* gameData)
	: titleFont_(FontMethod::MSDF, 48, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 일본어 폰트로 변경
	, stageFont_(FontMethod::MSDF, 24, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 일본어 폰트로 변경
	, infoFont_(FontMethod::MSDF, 16, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))   // ★ 일본어 폰트로 변경
	, stageNumberFont_(48, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))              // ★ 일본어 폰트로 변경
	, stageTitleFont_(FontMethod::MSDF, 32, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 큰 폰트 (28 + 4 = 32)
	, bgm_(Resource(U"ArtResources/BGM/DeepSea1.mp3"), Loop::Yes)
	, gameData_(gameData)
{
	currentScene_ = SceneType::StageSelect;
	loadStageTextures();
	initializeStages();
}

void StageSelectScene::onEnter()
{
	animTimer_ = 0.0;
	selectedStage_ = -1;

	// 처음부터 모두 선택 가능하도록 강제 해금
	for (auto& button : stageButtons_)
	{
		button.isLocked = false;
	}

	// 배경음악 재생
	if (!bgm_.isEmpty())
	{
		bgm_.setVolume(0.2);
		bgm_.play();
	}
}

void StageSelectScene::onExit()
{
	// 배경음악 정지
	if (!bgm_.isEmpty() && bgm_.isPlaying())
	{
		bgm_.stop();
	}
}

void StageSelectScene::initializeStages()
{
	stageButtons_.clear();

	// ★ 논리적 해상도
	constexpr int32 LOGICAL_WIDTH = 880;
	constexpr int32 LOGICAL_HEIGHT = 880;

	const int32 centerY = LOGICAL_HEIGHT / 2;
	const int32 totalStages = StageData::getTotalStageCount();

	Array<String> stageNames = {
	U"神罰", U"ここはどこ", U"爆発",
	U"灼熱海流", U"深淵の門", U"歪む色彩",
	U"裁きの間", U"氷結の迷路", U"上昇海流",
	U"心臓部"
	};

	// 화면 중앙 기준으로 스테이지 버튼 배치
	const int32 totalWidth = (STAGE_WIDTH + STAGE_SPACING) * totalStages - STAGE_SPACING;
	const int32 startX = (LOGICAL_WIDTH - totalWidth) / 2;

	for (int32 i = 0; i < totalStages; i++)
	{
		StageButton button;

		button.rect = Rect{
			startX + i * (STAGE_WIDTH + STAGE_SPACING),
			centerY - STAGE_HEIGHT / 2,
			STAGE_WIDTH, STAGE_HEIGHT
		};
		button.stageNumber = i + 1;
		button.isLocked = false;
		button.stageName = (i < stageNames.size()) ? stageNames[i] : U"ステージ {}"_fmt(i + 1);

		stageButtons_.push_back(button);
	}

	// 뒤로가기 버튼을 화면 왼쪽 하단에 배치 (비율 기준)
	const int32 backButtonMargin = static_cast<int32>(LOGICAL_WIDTH * 0.04);
	const int32 backButtonBottom = static_cast<int32>(LOGICAL_HEIGHT * 0.92);
	backButton_ = Rect{ backButtonMargin, backButtonBottom, 180, 60 };
}

void StageSelectScene::loadStageTextures()
{
	// 스테이지 텍스처 배열 초기화
	stageTextures_.clear();
	const int32 totalStages = StageData::getTotalStageCount();
	stageTextures_.resize(totalStages);


	// 각 스테이지별로 애니메이션 프레임 로드
	for (int32 stageIndex = 0; stageIndex < totalStages; stageIndex++)
	{
		const int32 stageNumber = stageIndex + 1;

		if (stageNumber == 11)
		{
			// 보스 스테이지 (11번): 11개의 프레임 (0~10)
			for (int32 frameIndex = 0; frameIndex <= 10; frameIndex++)
			{
				const String texturePath = Resource(U"ArtResources/Texture2D/stage/stage_boss-{}.png"_fmt(frameIndex));

				Texture frameTexture(texturePath);

				if (!frameTexture.isEmpty())
				{
					stageTextures_[stageIndex].push_back(frameTexture);
				}
				else
				{
					stageTextures_[stageIndex].push_back(Texture{});
				}
			}
		}
		else
		{
			// 일반 스테이지 (1~10번): 각 2개의 프레임 (0~1)
			for (int32 frameIndex = 0; frameIndex < 2; frameIndex++)
			{
				const String texturePath = Resource(U"ArtResources/Texture2D/stage/stage_{}-{}.png"_fmt(stageNumber, frameIndex));
				Texture frameTexture(texturePath);

				if (!frameTexture.isEmpty())
				{
					stageTextures_[stageIndex].push_back(frameTexture);
				}
				else
				{
					stageTextures_[stageIndex].push_back(Texture{});
				}
			}
		}
	}
}

void StageSelectScene::update()
{
	const double deltaTime = Scene::DeltaTime();
	animTimer_ += deltaTime;

	// 포커스 상태 확인 및 음악 제어
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

	// 일반 스테이지 애니메이션 프레임 업데이트
	animationFrameTimer_ += deltaTime;
	if (animationFrameTimer_ >= animationFrameDuration_)
	{
		animationFrameTimer_ -= animationFrameDuration_;
		animationFrameIndex_ = (animationFrameIndex_ + 1) % 2;
	}

	// ★ 보스 스테이지 애니메이션 업데이트 (0→10→0 순환)
	bossAnimationFrameTimer_ += deltaTime;
	if (bossAnimationFrameTimer_ >= bossAnimationFrameDuration_)
	{
		bossAnimationFrameTimer_ -= bossAnimationFrameDuration_;

		if (!bossAnimationReverse_)
		{
			// 정방향: 0 → 10
			bossAnimationFrameIndex_++;
			if (bossAnimationFrameIndex_ >= 10)
			{
				bossAnimationFrameIndex_ = 10;
				bossAnimationReverse_ = true;
			}
		}
		else
		{
			// 역방향: 10 → 0
			bossAnimationFrameIndex_--;
			if (bossAnimationFrameIndex_ <= 0)
			{
				bossAnimationFrameIndex_ = 0;
				bossAnimationReverse_ = false;
			}
		}
	}

	handleDragInput();
	updateScrolling();
	updateStageButtons();

	// 실제 화면 크기 대비 논리적 크기의 오프셋 계산
	const Vec2 screenCenter = Scene::CenterF();
	const Vec2 logicalCenter = Vec2(880 / 2.0, 880 / 2.0);
	const Vec2 offset = screenCenter - logicalCenter;

	backHovered_ = backButton_.movedBy(offset.asPoint()).mouseOver();
	if (backButton_.movedBy(offset.asPoint()).leftClicked() || KeyEscape.down())
	{
		changeScene(SceneType::MainMenu);
	}
}

void StageSelectScene::updateStageButtons()
{
	// 실제 화면 크기 대비 논리적 크기의 오프셋 계산
	const Vec2 screenCenter = Scene::CenterF();
	const Vec2 logicalCenter = Vec2(880 / 2.0, 880 / 2.0);
	const Vec2 offset = screenCenter - logicalCenter;

	for (auto& button : stageButtons_)
	{
		Rect adjustedRect = button.rect.movedBy(static_cast<int32>(scrollOffset_ + offset.x), static_cast<int32>(offset.y));

		button.isHovered = adjustedRect.mouseOver();

		// 클릭 처리 - leftClicked()로 변경 (down + up 완료 시에만 반응)
		if (adjustedRect.leftClicked())
		{
			if (!button.isLocked)
			{
				selectedStage_ = button.stageNumber;

				// 선택된 스테이지를 GameData에 저장
				if (gameData_)
				{
					gameData_->currentStage = button.stageNumber;
				}

				changeScene(SceneType::InGame);
			}
			else
			{
				// 잠긴 스테이지를 클릭했을 때 피드백
			}
		}

		// 해금된 스테이지에만 선택 효과 적용
		if (button.isHovered && !button.isLocked)
		{
			selectedStage_ = button.stageNumber;
		}
	}
}

void StageSelectScene::draw()
{
	// ★ 논리적 해상도
	constexpr int32 LOGICAL_WIDTH = 880;
	constexpr int32 LOGICAL_HEIGHT = 880;

	// 실제 화면 크기 대비 논리적 크기의 오프셋 계산
	const Vec2 screenCenter = Scene::CenterF();
	const Vec2 logicalCenter = Vec2(LOGICAL_WIDTH / 2.0, LOGICAL_HEIGHT / 2.0);
	const Vec2 offset = screenCenter - logicalCenter;

	const double titlePulse = 0.95 + 0.05 * Math::Sin(animTimer_ * 2.0);
	const int32 titleY = static_cast<int32>(LOGICAL_HEIGHT * 0.11);
	titleFont_(U"ステージ選択").drawAt(screenCenter.x, titleY + offset.y, ColorF(titlePulse, titlePulse, 1.0));

	drawStageButtons();

	if (selectedStage_ > 0)
	{
		// drawStageInfo();
	}

	ColorF backColor = backHovered_ ? ColorF{ 0.3, 0.5, 0.7 } : ColorF{ 0.2, 0.4, 0.6 };
	Rect adjustedBackButton = backButton_.movedBy(offset.asPoint());
	adjustedBackButton.draw(backColor);
	adjustedBackButton.drawFrame(2, Palette::White);
	stageFont_(U"戻る").drawAt(adjustedBackButton.center(),  // ★ "Back" → "戻る"
		backHovered_ ? Palette::Yellow : Palette::White);

}

void StageSelectScene::drawStageButtons()
{
	// ★ 논리적 해상도
	constexpr int32 LOGICAL_WIDTH = 880;
	constexpr int32 LOGICAL_HEIGHT = 880;

	// 실제 화면 크기 대비 논리적 크기의 오프셋 계산
	const Vec2 screenCenter = Scene::CenterF();
	const Vec2 logicalCenter = Vec2(LOGICAL_WIDTH / 2.0, LOGICAL_HEIGHT / 2.0);
	const Vec2 offset = screenCenter - logicalCenter;

	for (size_t i = 0; i < stageButtons_.size(); i++)
	{
		const auto& button = stageButtons_[i];
		Rect drawRect = button.rect.movedBy(static_cast<int32>(scrollOffset_ + offset.x), static_cast<int32>(offset.y));

		// 화면 밖에 있는 버튼은 그리지 않음
		if (drawRect.x + drawRect.w < -100 || drawRect.x > Scene::Width() + 100)
			continue;

		// 텍스처 렌더링
		bool textureRendered = false;
		if (i < stageTextures_.size() && !stageTextures_[i].isEmpty())
		{
			const auto& frames = stageTextures_[i];

			const bool isBossStage = (button.stageNumber == 11);
			const int32 frameIndex = isBossStage ? bossAnimationFrameIndex_ : animationFrameIndex_;

			if (frameIndex < static_cast<int32>(frames.size()) && !frames[frameIndex].isEmpty())
			{
				const Texture& currentFrame = frames[frameIndex];

				if (button.isLocked)
				{
					currentFrame.resized(drawRect.size).draw(drawRect.pos, ColorF(0.3, 0.3, 0.3));
				}
				else
				{
					currentFrame.resized(drawRect.size).draw(drawRect.pos);
				}
				textureRendered = true;
			}
		}

		// 텍스처가 없거나 실패한 경우 폴백
		if (!textureRendered)
		{
			ColorF buttonColor;

			if (button.isLocked)
				buttonColor = button.lockedColor;
			else if (button.isHovered)
				buttonColor = button.hoverColor;
			else
				buttonColor = button.normalColor;

			drawRect.draw(buttonColor);

			if (!button.isLocked)
			{
				stageNumberFont_(button.stageNumber)
					.drawAt(drawRect.center().movedBy(0, -30), Palette::White);
				stageFont_(button.stageName)
					.drawAt(drawRect.center().movedBy(0, 30), Palette::White);
			}
		}

		// ★ 텍스처 위에 텍스트 렌더링 (스테이지 1~10만 표시, 11번은 제외)
		if (!button.isLocked && button.stageNumber != 11)
		{
			const Vec2 center = drawRect.center();
			const double offsetY = drawRect.h * 0.25 - 5.0;  // 중앙에서 아래로 1/4, 위로 5px 이동
			const Vec2 textPos = center.movedBy(0, offsetY);

			// 텍스트에 외곽선 추가하여 가독성 향상 (28px 폰트 사용)
			stageTitleFont_(button.stageName).drawAt(textPos.movedBy(2, 2), ColorF(0.0, 0.0, 0.0, 0.7));  // 그림자
			stageTitleFont_(button.stageName).drawAt(textPos, Palette::White);  // 본문
		}

		// 잠금 아이콘
		if (button.isLocked)
		{
			const Vec2 center = drawRect.center();
			Circle{ center.movedBy(0, -10), 12 }.drawFrame(3, Palette::Gray);
			Rect{ center.movedBy(-8, -2).asPoint(), 16, 18 }.draw(Palette::Gray);
			stageFont_(U"ロック中").drawAt(center.movedBy(0, 40), ColorF{ 0.5, 0.5, 0.5 });
		}
	}

	// 스크롤 인디케이터
	const int32 totalStages = StageData::getTotalStageCount();
	const double totalWidth = (STAGE_WIDTH + STAGE_SPACING) * totalStages - STAGE_SPACING;
	const double firstStageX = (LOGICAL_WIDTH - totalWidth) / 2;
	const double maxRightScroll = Math::Max(0.0, -firstStageX + (LOGICAL_WIDTH - STAGE_WIDTH) / 2.0);
	const double maxLeftScroll = getMaxScrollOffset();
	const double totalScrollRange = maxRightScroll + maxLeftScroll;

	if (totalScrollRange > 0)
	{
		// 현재 스크롤 위치를 0~1 범위로 정규화
		const double scrollProgress = (scrollOffset_ + maxLeftScroll) / totalScrollRange;

		const double centerX = screenCenter.x;
		const double indicatorWidthTotal = Math::Min(624.0, LOGICAL_WIDTH * 0.6);
		const double indicatorXStart = centerX - indicatorWidthTotal / 2.0;
		const int32 indicatorY = static_cast<int32>(LOGICAL_HEIGHT * 0.83 + offset.y);
		const Rect indicatorBg{ static_cast<int32>(indicatorXStart), indicatorY, static_cast<int32>(indicatorWidthTotal), 8 };
		indicatorBg.draw(ColorF{ 0.3, 0.3, 0.3, 0.5 });

		const double indicatorWidth = 80;
		const double indicatorX = indicatorBg.x + scrollProgress * (indicatorBg.w - indicatorWidth);

		Rect{ static_cast<int32>(indicatorX), indicatorBg.y, static_cast<int32>(indicatorWidth), indicatorBg.h }
		.draw(ColorF{ 0.7, 0.7, 1.0, 0.8 });
	}
}

void StageSelectScene::drawStageInfo()
{
	const Rect infoPanel{ 200, 700, 624, 60 };  // 패널 크기와 위치 조정
	infoPanel.draw(ColorF{ 0, 0, 0, 0.7 });
	infoPanel.drawFrame(2, ColorF{ 0.5, 0.8, 1.0 });

	for (const auto& button : stageButtons_)
	{
		if (button.stageNumber == selectedStage_ && !button.isLocked)
		{
			infoFont_(button.description)
				.drawAt(infoPanel.center(), ColorF{ 0.9, 0.9, 1.0 });
			break;
		}
	}
}

void StageSelectScene::handleDragInput()
{
	const Vec2 mousePos = Cursor::PosF();

	// 실제 화면 크기 대비 논리적 크기의 오프셋 계산
	const Vec2 screenCenter = Scene::CenterF();
	const Vec2 logicalCenter = Vec2(880 / 2.0, 880 / 2.0);
	const Vec2 offset = screenCenter - logicalCenter;

	const bool isOverBackButton = backButton_.movedBy(offset.asPoint()).mouseOver();

	if (MouseL.down() && !isOverBackButton)
	{
		if (!isDragging_)
		{
			isDragging_ = true;
			dragStartPos_ = mousePos;
			dragStartOffset_ = scrollOffset_;
		}
	}
	else if (MouseL.up())
	{
		isDragging_ = false;
	}

	if (isDragging_)
	{
		const double dragDistance = mousePos.x - dragStartPos_.x;
		targetScrollOffset_ = dragStartOffset_ + dragDistance;
		clampScrollOffset();

		// 드래그 중일 때는 즉시 스크롤 적용
		scrollOffset_ = targetScrollOffset_;
	}
}

void StageSelectScene::updateScrolling()
{
	const double deltaTime = Scene::DeltaTime();

	if (!isDragging_)
	{
		// 부드러운 스크롤 애니메이션
		const double lerpSpeed = 8.0;
		scrollOffset_ = Math::Lerp(scrollOffset_, targetScrollOffset_, lerpSpeed * deltaTime);

		// 목표에 가까워지면 정확히 맞춤
		if (Math::Abs(scrollOffset_ - targetScrollOffset_) < 0.1)
		{
			scrollOffset_ = targetScrollOffset_;
		}
	}
}

double StageSelectScene::getMaxScrollOffset() const
{
	// ★ 고정 해상도 사용
	constexpr int32 LOGICAL_WIDTH = 880;

	const int32 totalStages = StageData::getTotalStageCount();

	// 전체 스테이지 너비 계산
	const double totalWidth = (STAGE_WIDTH + STAGE_SPACING) * totalStages - STAGE_SPACING;

	// 화면 중앙 배치를 고려한 최대 오프셋 계산
	const double startX = (LOGICAL_WIDTH - totalWidth) / 2;
	const double lastStageEndX = startX + totalWidth;

	// 마지막 스테이지가 화면 중앙에 오도록 하는 데 필요한 오프셋
	const double maxOffset = lastStageEndX - (LOGICAL_WIDTH + STAGE_WIDTH) / 2.0;

	return Math::Max(0.0, maxOffset);
}

void StageSelectScene::clampScrollOffset()
{
	constexpr int32 LOGICAL_WIDTH = 880;
	const int32 totalStages = StageData::getTotalStageCount();
	const double totalWidth = (STAGE_WIDTH + STAGE_SPACING) * totalStages - STAGE_SPACING;
	const double firstStageX = (LOGICAL_WIDTH - totalWidth) / 2;

	// 첫 번째 스테이지가 화면 중앙에 오도록 하는 최대 오른쪽 스크롤
	const double maxRightScroll = Math::Max(0.0, -firstStageX + (LOGICAL_WIDTH - STAGE_WIDTH) / 2.0);

	// 마지막 스테이지가 화면 중앙에 오도록 하는 최대 왼쪽 스크롤
	const double maxLeftScroll = getMaxScrollOffset();

	// 스크롤 범위: maxRightScroll(양수) ~ -maxLeftScroll(음수)
	targetScrollOffset_ = Math::Clamp(targetScrollOffset_, -maxLeftScroll, maxRightScroll);
}
