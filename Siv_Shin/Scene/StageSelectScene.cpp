#include "StageSelectScene.hpp"
#include "StageData.hpp"

StageSelectScene::StageSelectScene()
	: titleFont_(FontMethod::MSDF, 48, U"ArtResources/Fonts/Tetsubin Gothic.otf")  // ★ 일본어 폰트로 변경
	, stageFont_(FontMethod::MSDF, 24, U"ArtResources/Fonts/Tetsubin Gothic.otf")  // ★ 일본어 폰트로 변경
	, infoFont_(FontMethod::MSDF, 16, U"ArtResources/Fonts/Tetsubin Gothic.otf")   // ★ 일본어 폰트로 변경
	, stageNumberFont_(48, U"ArtResources/Fonts/Tetsubin Gothic.otf")              // ★ 일본어 폰트로 변경
	, bgm_(U"ArtResources/BGM/DeepSea1.mp3", Loop::Yes)
{
	currentScene_ = SceneType::StageSelect;
	loadStageTextures();
	initializeStages();
}

StageSelectScene::StageSelectScene(GameData* gameData)
	: titleFont_(FontMethod::MSDF, 48, U"ArtResources/Fonts/Tetsubin Gothic.otf")  // ★ 일본어 폰트로 변경
	, stageFont_(FontMethod::MSDF, 24, U"ArtResources/Fonts/Tetsubin Gothic.otf")  // ★ 일본어 폰트로 변경
	, infoFont_(FontMethod::MSDF, 16, U"ArtResources/Fonts/Tetsubin Gothic.otf")   // ★ 일본어 폰트로 변경
	, stageNumberFont_(48, U"ArtResources/Fonts/Tetsubin Gothic.otf")              // ★ 일본어 폰트로 변경
	, bgm_(U"ArtResources/BGM/DeepSea1.mp3", Loop::Yes)
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

	// 화면 중앙 Y 위치
	const int32 centerY = 400;

	// 스테이지 개수 동적 결정 (마지막은 finalStage)
	const int32 totalStages = StageData::getTotalStageCount();

	// ★ 일본어 스테이지 이름으로 변경
	Array<String> stageNames = {
		U"チュートリアル1", U"基本", U"複雑",
		U"チュートリアル2", U"魚", U"マスター",
		U"チュートリアル3", U"赤いチェス", U"最終"
	};

	// ★ 일본어 설명으로 변경
	Array<String> descriptions = {
		U"基本的な動き方を学ぶ",
		U"色の混合の基礎",
		U"高度な組み合わせ",
		U"黒ブロックについて学ぶ",
		U"魚だ",
		U"すごい",
		U"アイテムについて学ぶ",
		U"チェス",
		U"最終ボスステージ"
	};

	// 스테이지 버튼 생성 (가로로 나열)
	for (int32 i = 0; i < totalStages; i++)
	{
		StageButton button;

		// 가로로 배치 (스크롤 영역 고려)
		button.rect = Rect{
			i * (STAGE_WIDTH + STAGE_SPACING),
			centerY - STAGE_HEIGHT / 2,
			STAGE_WIDTH, STAGE_HEIGHT
		};

		button.stageNumber = i + 1;

		// 처음부터 모두 해금
		button.isLocked = false;

		// 배열 범위를 넘어가면 기본 이름/설명 사용
		button.stageName = (i < stageNames.size()) ? stageNames[i] : U"ステージ {}"_fmt(i + 1);
		button.description = (i < descriptions.size()) ? descriptions[i] : U"";

		stageButtons_.push_back(button);
	}

	backButton_ = Rect{ 40, 800, 180, 60 };
}

void StageSelectScene::loadStageTextures()
{
    // 스테이지 텍스처 배열 초기화 (8개 스테이지, 각각 3개 프레임)
    stageTextures_.clear();
    const int32 totalStages = StageData::getTotalStageCount();
    stageTextures_.resize(totalStages);
    
    // 각 스테이지별로 3개의 애니메이션 프레임 로드
    for (int32 stageIndex = 0; stageIndex < totalStages; stageIndex++)
    {
        const int32 stageNumber = stageIndex + 1;  // 스테이지 번호는 1부터 시작
        
        for (int32 frameIndex = 0; frameIndex < 3; frameIndex++)
        {
            // 파일 경로: ArtResources/Texture2D/stage/stage_1-0.png, stage_1-1.png, etc.
            const String texturePath = U"ArtResources/Texture2D/stage/stage_{}-{}.png"_fmt(stageNumber, frameIndex);
            
            Texture frameTexture(texturePath);
            
            // 텍스처 로딩이 성공했는지 확인
            if (!frameTexture.isEmpty())
            {
                stageTextures_[stageIndex].push_back(frameTexture);
            }
            else
            {
                // 로딩 실패 시 빈 텍스처 추가 (오류 방지)
                stageTextures_[stageIndex].push_back(Texture{});
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
            // 포커스를 다시 얻었을 때 음악 재생
            bgm_.play();
        }
        else if (!currentFocus && bgm_.isPlaying())
        {
            // 포커스를 잃었을 때 음악 일시정지
            bgm_.pause();
        }
    }
    wasFocused_ = currentFocus;
    
    // 애니메이션 프레임 업데이트
    animationFrameTimer_ += deltaTime;
    if (animationFrameTimer_ >= animationFrameDuration_)
    {
        animationFrameTimer_ -= animationFrameDuration_;
        animationFrameIndex_ = (animationFrameIndex_ + 1) % 3;  // 3프레임 순환
    }
    
    handleDragInput();
    updateScrolling();
    updateStageButtons();
    
    backHovered_ = backButton_.mouseOver();
    if (backButton_.leftClicked() || KeyEscape.down())
    {
        changeScene(SceneType::MainMenu);
    }
}

void StageSelectScene::updateStageButtons()
{
    // 첫 번째 스테이지가 화면 중앙 왼쪽에서 시작하도록 계산
    const double screenCenterX = Scene::Size().x / 2.0; // 화면 중앙 X
    const double firstStageOffset = -(STAGE_WIDTH + STAGE_SPACING); // 첫 스테이지를 중앙 기준 왼쪽에 배치
    const double viewportX = firstStageOffset + scrollOffset_;
    
    for (auto& button : stageButtons_)
    {
        // 스크롤 오프셋을 적용한 버튼 위치 계산
        Rect adjustedRect = button.rect.movedBy(static_cast<int32>(screenCenterX + viewportX), 0);
        
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
	const double titlePulse = 0.95 + 0.05 * Math::Sin(animTimer_ * 2.0);
	titleFont_(U"ステージ選択").drawAt(Scene::Size().x / 2.0, 100, ColorF(titlePulse, titlePulse, 1.0));  // ★ "Select Stage" → "ステージ選択"

	drawStageButtons();

	if (selectedStage_ > 0)
	{
		// drawStageInfo();
	}

	ColorF backColor = backHovered_ ? ColorF{ 0.3, 0.5, 0.7 } : ColorF{ 0.2, 0.4, 0.6 };
	backButton_.draw(backColor);
	backButton_.drawFrame(2, Palette::White);
	stageFont_(U"戻る").drawAt(backButton_.center(),  // ★ "Back" → "戻る"
		backHovered_ ? Palette::Yellow : Palette::White);

	infoFont_(U"ドラッグでスクロール | クリックで選択 | ESC: 戻る")  // ★ 일본어로 변경
		.drawAt(Scene::Size().x / 2.0, 950, ColorF{ 0.6, 0.6, 0.7 });
}

void StageSelectScene::drawStageButtons()
{
	// 첫 번째 스테이지가 화면 중앙 왼쪽에서 시작하도록 계산
	const double screenCenterX = Scene::Size().x / 2.0;
	const double firstStageOffset = -(STAGE_WIDTH + STAGE_SPACING);
	const double viewportX = firstStageOffset + scrollOffset_;

	for (size_t i = 0; i < stageButtons_.size(); i++)
	{
		const auto& button = stageButtons_[i];

		Rect drawRect = button.rect.movedBy(static_cast<int32>(screenCenterX + viewportX), 0);

		if (drawRect.x + drawRect.w < -100 || drawRect.x > 1124)
			continue;

		if (i < stageTextures_.size() && !stageTextures_[i].isEmpty())
		{
			const auto& frames = stageTextures_[i];
			if (animationFrameIndex_ < static_cast<int32>(frames.size()) && !frames[animationFrameIndex_].isEmpty())
			{
				const Texture& currentFrame = frames[animationFrameIndex_];

				if (button.isLocked)
				{
					currentFrame.resized(drawRect.size).draw(drawRect.pos, ColorF(0.3, 0.3, 0.3));
				}
				else
				{
					currentFrame.resized(drawRect.size).draw(drawRect.pos);
				}
			}
		}
		else
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

		if (button.isLocked)
		{
			const Vec2 center = drawRect.center();
			Circle{ center.movedBy(0, -10), 12 }.drawFrame(3, Palette::Gray);
			Rect{ center.movedBy(-8, -2).asPoint(), 16, 18 }.draw(Palette::Gray);
			stageFont_(U"ロック中").drawAt(center.movedBy(0, 40), ColorF{ 0.5, 0.5, 0.5 });  // ★ "Locked" → "ロック中"
		}
	}

	if (getMaxScrollOffset() > 0)
	{
		const double scrollProgress = Math::Abs(scrollOffset_) / getMaxScrollOffset();

		const double centerX = Scene::Size().x / 2.0;
		const double indicatorWidthTotal = 624;
		const double indicatorXStart = centerX - indicatorWidthTotal / 2.0;
		const Rect indicatorBg{ static_cast<int32>(indicatorXStart), 750, static_cast<int32>(indicatorWidthTotal), 8 };
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
    const Vec2 mousePos = Cursor::Pos();
    
    if (MouseL.down())
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
    // 총 8개 스테이지의 전체 너비
    const int32 totalStages = StageData::getTotalStageCount();
    const double totalWidth = (STAGE_WIDTH + STAGE_SPACING) * totalStages - STAGE_SPACING;
    
    // 화면에 3개가 편안하게 보이도록 하는 가시 너비 (여백 포함)
    const double visibleWidth = VISIBLE_STAGES * (STAGE_WIDTH + STAGE_SPACING);
    
    // 마지막 스테이지들이 화면 오른쪽 끝에 여유있게 보이도록 계산
    const double maxOffset = totalWidth - visibleWidth + (STAGE_WIDTH + STAGE_SPACING);
    
    return Math::Max(0.0, maxOffset);
}

void StageSelectScene::clampScrollOffset()
{
    const double maxOffset = getMaxScrollOffset();
    targetScrollOffset_ = Math::Clamp(targetScrollOffset_, -maxOffset, 0.0);
}
