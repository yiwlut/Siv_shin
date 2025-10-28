#include "InGameScene.hpp"
#include "StageData.hpp"

InGameScene::InGameScene()
    : InGameScene(1)  // 기본은 스테이지 1
{
}

InGameScene::InGameScene(int32 stageNumber)
    : InGameScene(stageNumber, nullptr)
{
}

InGameScene::InGameScene(int32 stageNumber, GameData* gameData)
: playerPos_(1, 1)
, playerMoveSpeed_(400.0)  // 픽셀/초 (TILE_SIZE * 5, 타일 크기 증가에 맞춤)
, isPlayerMoving_(false)
, inputCooldown_(0.0)
, moveDelay_(0.3)
, playerColor_(0.2, 0.8, 0.3)
, currentStage_(stageNumber)
, playerHeldItem_(ItemType::None)  // 초기에는 아이템 없음
, tacoDirection_(TacoDirection::Down)
, isFacingLeft_(false)
, tacoAnimFrame_(0)
, tacoAnimTimer_(0.0)
, isPlayingPaintAnimation_(false)
, isPaintAnimMirrored_(false)
, paintAnimTimer_(0.0)
, paintAnimFrame_(0)
, playerAnimTimer_(0.0)
, currentPlayerFrame_(0)
, gameFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 수정
, debugFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))       // ★ 수정
, clearFont_(FontMethod::MSDF, 64, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 수정
, buttonFont_(FontMethod::MSDF, 20, Resource(U"ArtResources/Fonts/TetsubinGothic.otf"))  // ★ 수정
, gameTime_(0.0)
, score_(0)
, moves_(0)
, isCleared_(false)
, showClearButtons_(false)  // 클리어 버튼 표시 상태 초기화
, showHelpScreen_(false)  // 조작법 도움말 화면 초기화
, isFailed_(false)
, showFailedButtons_(false)
, stageBackground_(Resource(U"ArtResources/Texture2D/BG_K.png"))
, fadeTimer_(0.0)
, fadeDuration_(1.0)  // 1초 페이드
, isFading_(true)
, gameData_(gameData)
{
    currentScene_ = SceneType::InGame;
    playerPixelPos_ = tileToPixel(playerPos_);
    targetPixelPos_ = playerPixelPos_;

	if (StageData::isFinalStage(currentStage_))
	{
		currentBossPhase_ = 1;
		bossHitCount_ = 0;
		bossMaxHP_ = 3;
		bossCurrentHP_ = 3;
	}

    loadAssets();
    initializeClearButtons();  // 클리어 버튼 초기화
    
}

void InGameScene::onEnter()
{
	fadeTimer_ = 0.0;
	isFading_ = true;
	gameTime_ = 0.0;
	score_ = 0;
	moves_ = 0;
	isCleared_ = false;
	showClearButtons_ = false;
	showHelpScreen_ = false;
	clearSoundPlayed_ = false;

	isFailed_ = false;
	showFailedButtons_ = false;

	playerHeldItem_ = ItemType::None;
	tacoDirection_ = TacoDirection::Down;
	isFacingLeft_ = false;
	tacoAnimFrame_ = 0;
	isPlayerMoving_ = false;

	isPlayerDead_ = false;
	deathAnimTimer_ = 0.0;
	deathParticles_.clear();

	showClearEffect_ = false;
	clearEffectTimer_ = 0.0;
	clearParticles_.clear();

	loadStage(currentStage_);

	// 픽셀 위치 재설정
	playerPixelPos_ = tileToPixel(playerPos_);
	targetPixelPos_ = playerPixelPos_;

	bossProjectiles_.clear();
	bossExplosionParticles_.clear();
	bossAttackTimer_ = 0.0;
	if (StageData::isFinalStage(currentStage_))
	{
		currentBossPhase_ = 1;
		bossHitCount_ = 0;
		bossCurrentHP_ = 3;

		initBossPhaseSystem();
		initBossAttacks();
		initBossWallSystem();

		if (!bossBgm_.isEmpty())
		{
			bossBgm_.setVolume(0.4);
			bossBgm_.play();
		}
	}
	else
	{
		if (!bgm_.isEmpty())
		{
			bgm_.setVolume(0.33);
			bgm_.play();
		}
	}
	if (StageData::isFinalStage(currentStage_) && KeyF2.down()) {
		isCleared_ = true;
		changeScene(SceneType::Ending);
		return;
	}

	auto& holo = g_Shaders.holographic();
	holo.setRainbowMode(false);
	holo.setScale(0.05f);
	holo.setSpeed(1.5f);
	holo.setHoloColor(ColorF{ 1.0, 1.0, 1.0 });
	holo.setIntensity(0.0f);
}

void InGameScene::onExit()
{
    // 배경음악 정지
    if (!bgm_.isEmpty() && bgm_.isPlaying())
    {
        bgm_.stop();
    }
	// ★ 보스 배경음악 정지
	if (!bossBgm_.isEmpty() && bossBgm_.isPlaying())
	{
		bossBgm_.stop();
	}
}

void InGameScene::loadAssets()
{
    tacoDownFrames_.clear();
    tacoSideFrames_.clear();
    tacoUpFrames_.clear();
    for (int32 i = 0; i < 2; i++)
    {
        Texture downFrame{ Resource(U"ArtResources/Texture2D/Taco/tacoMoveDown_{}.png"_fmt(i)) };
        if (!downFrame.isEmpty()) tacoDownFrames_.push_back(downFrame);
        Texture sideFrame{ Resource(U"ArtResources/Texture2D/Taco/tacoMoveSide_{}.png"_fmt(i)) };
        if (!sideFrame.isEmpty()) tacoSideFrames_.push_back(sideFrame);
        Texture upFrame{ Resource(U"ArtResources/Texture2D/Taco/tacoMoveUp_{}.png"_fmt(i)) };
        if (!upFrame.isEmpty()) tacoUpFrames_.push_back(upFrame);
    }
    
    // 타코 점수 이미지 로드
    tacoScoreTexture_ = Texture{ Resource(U"ArtResources/Texture2D/Menu/Score/tacoScoreOn.png") };
    tacoScoreOffTexture_ = Texture{ Resource(U"ArtResources/Texture2D/Menu/Score/tacoScoreOff.png") };

	heartFullTexture_ = Texture{ Resource(U"ArtResources/Texture2D/Menu/Score/tacoScoreOn.png") };
	heartEmptyTexture_ = Texture{ Resource(U"ArtResources/Texture2D/Menu/Score/tacoScoreOff.png") };
    // 타코 페인트 애니메이션 프레임 로드 (tacoPaint_0.png ~ tacoPaint_4.png)
    tacoPaintFrames_.clear();
    for (int32 i = 0; i < PAINT_ANIM_FRAME_COUNT; i++)
    {
        Texture paintFrame{ Resource(U"ArtResources/Texture2D/Taco/Color/tacoPaint_{}.png"_fmt(i)) };
        if (!paintFrame.isEmpty())
        {
            tacoPaintFrames_.push_back(paintFrame);
        }
        else
        {
            // Print << U"Warning: Failed to load tacoPaint_{}.png"_fmt(i);
        }
    }
    
    // 배경음악 로드
    bgm_ = Audio{ Resource(U"ArtResources/BGM/HappyOcean.mp3"), Loop::Yes };
	bossBgm_ = Audio{ Resource(U"ArtResources/BGM/Boss.mp3"), Loop::Yes };
    
    // 블록 밀기 효과음 로드
    noteE5_ = Audio{ Resource(U"ArtResources/SFX/E5.wav") };
    noteG5_ = Audio{ Resource(U"ArtResources/SFX/G5.wav") };
    noteC6_ = Audio{ Resource(U"ArtResources/SFX/C6.wav") };

	stageClearSound_ = Audio{ Resource(U"ArtResources/SFX/StageClear.wav") };
	bombExplosionSound_ = Audio{ Resource(U"ArtResources/SFX/bomb.wav") };
	bumpSound_ = Audio{ Resource(U"ArtResources/SFX/bump.wav") };  // ★ 추가


	// ★ 보스 등장 애니메이션 로드 (boss_cloak_5.png → boss_cloak_0.png 역순)
	bossIdleFrames_.clear();
	for (int32 i = 5; i >= 0; --i)
	{
		Texture f{ Resource(U"ArtResources/Texture2D/Boss/boss_cloak_{}.png"_fmt(i)) };
		if (!f.isEmpty())
		{
			bossIdleFrames_.push_back(f);
		}
	}
	bossAnimFrame_ = 0;
	bossAnimTimer_ = 0.0;
}

void InGameScene::loadStage(int32 stageNumber)
{
	currentStage_ = stageNumber;

	// StageData에서 맵 텍스트 획득
	const Array<String> mapText = StageData::getStageMap(stageNumber);

	// 가변 크기 파서 호출
	loadStageFromText_VarSize(mapText);

	wallMask_.assign(getMapHeight(), Array<bool>(getMapWidth(), false));

	overlayWarn_.assign(getMapHeight(), Array<OverlayType>(getMapWidth(), OverlayType::None));
	const Array<String> tileOverlay = StageData::getStageTileOverlay(stageNumber);
	applyTileOverlay(tileOverlay, StageData::isFinalStage(currentStage_) ? OverlayApplyMode::OverlayOnly   // 보스전: 오버레이만
										   : OverlayApplyMode::WriteToMap);


	// 맵 크기에 맞춘 고정 카메라 적용
	onStageLoaded_FixedCamera();

}

void InGameScene::loadStageFromText(const Array<String>& mapText)
{
    // 맵 초기화
    for (auto& row : mapData_)
    {
        row.assign(getMapWidth(), TileType::Empty);
    }
    
    boxes_.clear();
    items_.clear();  // 아이템 초기화
    redGoalPositions_.clear();
    yellowGoalPositions_.clear();
    blueGoalPositions_.clear();
    orangeGoalPositions_.clear();
    greenGoalPositions_.clear();
    violetGoalPositions_.clear();
    blackGoalPositions_.clear();
    
    // 텍스트 맵 파싱
    for (int32 y = 0; y < Min((int32)mapText.size(), getMapHeight()); y++)
    {
        const String& line = mapText[y];
        for (int32 x = 0; x < Min((int32)line.length(), getMapWidth()); x++)
        {
            char32 ch = line[x];
            Point pos(x, y);
            
            switch (ch)
            {
            case U'#':  // 벽
                mapData_[y][x] = TileType::Wall;
                break;
            case U'i':  // 얼음
                mapData_[y][x] = TileType::Ice;
                break;
                
            case U'T':  // 플레이어 시작 위치 (Taco)
                playerPos_ = pos;
                playerPixelPos_ = tileToPixel(pos);
                targetPixelPos_ = playerPixelPos_;
                isPlayerMoving_ = false;
                break;
                
            // 대문자 = 상자
			case U'R': // 빨간 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Red, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
			case U'Y': // 노란 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Yellow, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
			case U'B': // 파간 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Blue, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
			case U'O': // 주황 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Orange, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
			case U'G': // 초록 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Green, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
			case U'V': // 보라 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Violet, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
			case U'K': // 검은 상자
				boxes_.push_back(ColorBox{ pos, BoxColor::Black, 0.0, nextBoxUID_++ }); // ★ uid 추가
				break;
                
            // 소문자 = 목표
            case U'r':  // 빨강 목표
                redGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::RedGoal;
                break;
                
            case U'y':  // 노랑 목표
                yellowGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::YellowGoal;
                break;
                
            case U'b':  // 파랑 목표
                blueGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::BlueGoal;
                break;
                
            case U'o':  // 주황 목표
                orangeGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::OrangeGoal;
                break;
                
            case U'g':  // 초록 목표
                greenGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::GreenGoal;
                break;
                
            case U'v':  // 보라 목표
                violetGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::VioletGoal;
                break;
                
            case U'k':  // 검정 목표
                blackGoalPositions_.push_back(pos);
                mapData_[y][x] = TileType::BlackGoal;
                break;
                
            // 아이템들 (숫자)
            case U'2':  // 빨강 아이템
                items_.push_back(GameItem{ pos, ItemType::RedItem });
                mapData_[y][x] = TileType::RedItem;
                break;
                
            case U'4':  // 주황 아이템
                items_.push_back(GameItem{ pos, ItemType::OrangeItem });
                mapData_[y][x] = TileType::OrangeItem;
                break;
                
            case U'6':  // 노랑 아이템
                items_.push_back(GameItem{ pos, ItemType::YellowItem });
                mapData_[y][x] = TileType::YellowItem;
                break;
                
            case U'7':  // 초록 아이템
                items_.push_back(GameItem{ pos, ItemType::GreenItem });
                mapData_[y][x] = TileType::GreenItem;
                break;
                
            case U'8':  // 파랑 아이템
                items_.push_back(GameItem{ pos, ItemType::BlueItem });
                mapData_[y][x] = TileType::BlueItem;
                break;
                
            case U'9':  // 보라 아이템
                items_.push_back(GameItem{ pos, ItemType::VioletItem });
                mapData_[y][x] = TileType::VioletItem;
                break;
                
            case U'.':  // 빈 공간
            case U' ':
            default:
                mapData_[y][x] = TileType::Empty;
                break;
            }
        }
    }
}

static Vec2 impactOriginLocalUVForDir(const Point& pushDir) {
	if (pushDir == Point{ 1, 0 })       return Vec2(0.0, 0.5); // →로 밀면 타깃의 왼쪽 변
	else if (pushDir == Point{ -1, 0 }) return Vec2(1.0, 0.5); // ←로 밀면 타깃의 오른쪽 변
	else if (pushDir == Point{ 0, 1 })  return Vec2(0.5, 0.0); // ↓로 밀면 타깃의 위쪽 변
	else                                 return Vec2(0.5, 1.0); // ↑로 밀면 타깃의 아래쪽 변
}

void InGameScene::triggerMergePaintFX_Directional(Point tile,
	const ColorF& baseColor, const ColorF& resultColor, Point pushDir)
{
	mergeFX_.active = true;
	//mergeFX_.tile = tile;
	mergeFX_.baseColor = baseColor;     // 박스 베이스는 합성 전 타깃 색
	mergeFX_.paintColor = resultColor;   // 퍼질 색은 합성 결과 색
	mergeFX_.originUV = impactOriginLocalUVForDir(pushDir); // 타일 로컬 0~1 원점

	// PaintSpread 파라미터 즉시 세팅 및 시작 (지연 없음)
	g_Shaders.paintSpread().setPaintColor(mergeFX_.paintColor);
	g_Shaders.paintSpread().setOriginPoint(mergeFX_.originUV);
	g_Shaders.paintSpread().setNoiseScale(1.0f);
	g_Shaders.paintSpread().setWaveStrength(0.4f);
	g_Shaders.paintSpread().setSpreadSpeed(0.6f);
	g_Shaders.paintSpread().startAnimation();          // 즉시 0→1 시작
}

// InGameScene.cpp
ColorBox* InGameScene::getBoxByUid(uint64 uid)
{
	for (auto& b : boxes_) {
		if (b.uid == uid) return &b;
	}
	return nullptr;
}



void InGameScene::updateMergePaintFX()
{
	if (!mergeFX_.active) return;

	g_Shaders.paintSpread().updateProgress(Scene::DeltaTime());

	if (g_Shaders.paintSpread().isAnimationComplete()) {
		if (mergeFX_.commitPending) {
			if (ColorBox* b = getBoxByUid(mergeFX_.targetUid)) {
				b->color = mergeFX_.finalColor; // 이 시점에만 논리색 변경
			}
			mergeFX_.commitPending = false;
		}
		mergeFX_.active = false;
	}
}

void InGameScene::forceMergePaintFXCompletion()
{
	if (!mergeFX_.active) return;

	if (mergeFX_.commitPending) {
		if (ColorBox* b = getBoxByUid(mergeFX_.targetUid)) {
			b->color = mergeFX_.finalColor;
		}
		mergeFX_.commitPending = false;
	}

	mergeFX_.active = false;
}

BoxColor InGameScene::getEffectiveBoxColor(uint64 uid) const {
	// 현재 합성 중인 박스라면 최종 색상을 반환
	if (mergeFX_.active && mergeFX_.commitPending && mergeFX_.targetUid == uid) {
		return mergeFX_.finalColor;
	}

	// 아니면 실제 박스 색상 반환
	for (const auto& box : boxes_) {
		if (box.uid == uid) {
			return box.color;
		}
	}

	// 박스를 찾지 못한 경우 (shouldn't happen)
	return BoxColor::Red;
}

bool InGameScene::removeBoxByUid(uint64 uid)
{
	const size_t n0 = boxes_.size();
	boxes_.remove_if([&](const ColorBox& b) { return (b.uid == uid); });
	return boxes_.size() != n0;
}

void InGameScene::triggerBombBoxFXForBlack_Multi(uint64 uid, double durationSec)
{
	BombBoxInstance inst;
	inst.uid = uid;
	inst.effect = std::make_unique<BombBoxEffect>();
	inst.effect->reset();
	inst.params.pulseDuration = 3.0;
	inst.params.pulseCount = 1.0;
	inst.params.explodeTime = 0.5;
	inst.params.pulseAmp = 0.12f;
	inst.params.pulseSpeed = 10.0f;
	inst.params.spread = 100.0f;
	inst.params.seed = static_cast<float>(Random(0.0, 1.0));
	bombFXs_.push_back(std::move(inst));
}

void InGameScene::updateBombBoxFX_Multi(double dt)
{
	double accShakeIntensityWorld = 0.0;
	double maxShakeDuration = 0.0;

	for (auto it = bombFXs_.begin(); it != bombFXs_.end(); )
	{
		auto& fx = *it;
		if (!fx.effect) { it = bombFXs_.erase(it); continue; }

		fx.effect->update(dt);

		if (!fx.params.wallsDestroyed && fx.effect->isPulsing()) {
			const double elapsed = fx.effect->getTime();
			const double threshold = (fx.params.pulseDuration * fx.params.pulseCount);
			if (elapsed >= threshold) {
				fx.effect->trigger();
				if (const ColorBox* b = getBoxByUid(fx.uid)) {
					destroyWalls8(b->pos);

					
					// ★ 폭발 시 플레이어가 8방향 안에 있으면 흰색 파티클로 죽임
					if (isPlayerInExplosionRange(b->pos) && !isPlayerDead_) {
						isPlayerDead_ = true;
						deathAnimTimer_ = 0.0;
						createDeathEffect(true);  // ★ true = 흰색 파티클 사용

						if (!bgm_.isEmpty() && bgm_.isPlaying()) {
							savedMusicPosition_ = bgm_.posSec();  // 현재 위치 저장
							bgm_.stop();
						}
					}
				}
				fx.params.wallsDestroyed = true;
				if (!fx.shakeStarted) {
					if (const ColorBox* b = getBoxByUid(fx.uid)) {
						const Vec2 worldCenter = tileToPixel(b->pos) + Vec2(TILE_SIZE * 0.5, TILE_SIZE * 0.5);
						const Vec2 camC = camera().getCenter();
						const double dist = (worldCenter - camC).length();
						const double scale = camera().getScale();
						const double falloff = Saturate(1.0 - dist / (900.0 / Max(0.001, scale)));
						const double intenWorld = (10.0 * falloff) / Max(0.001, scale);
						accShakeIntensityWorld += intenWorld;
						maxShakeDuration = Max(maxShakeDuration, Min(0.35, fx.params.explodeTime));
						fx.shakeStarted = true;
					}
				}
			}
		}

		if (fx.params.wallsDestroyed && fx.effect->isExploding() && !fx.params.useWallColor) {
			if (const ColorBox* b = getBoxByUid(fx.uid)) destroyWalls8(b->pos);
			fx.params.useWallColor = true;
		}

		if (fx.effect->isInExplode() && (fx.effect->getExplodeT() >= fx.params.explodeTime))
		{
			if (const ColorBox* b = getBoxByUid(fx.uid)) destroyWalls8(b->pos);
			removeBoxByUid(fx.uid);
			it = bombFXs_.erase(it);
			continue;
		}
		++it;
	}

	if (accShakeIntensityWorld > 0.0 && maxShakeDuration > 0.0) {
		const double clampedIntensity = Min(accShakeIntensityWorld, 40.0 / Max(0.001, camera().getScale()));
		camera().shake(maxShakeDuration, clampedIntensity);
	}
}



void InGameScene::drawBombBoxFX_Multi()
{
	if (bombFXs_.isEmpty()) return;

	const Mat3x2 transform = camera().getMat3x2();
	const double scale = camera().getScale();

	Array<RectF> logicRects;    // 분해 기준(기존 inner)
	Array<RectF> drawRects;     // 실제 그릴 AABB(확장)
	Array<BombBoxEffect::Params> batchParams;
	Array<double> batchTimes;
	Array<double> batchExplodeTs;

	logicRects.reserve(bombFXs_.size());
	drawRects.reserve(bombFXs_.size());
	batchParams.reserve(bombFXs_.size());
	batchTimes.reserve(bombFXs_.size());
	batchExplodeTs.reserve(bombFXs_.size());

	for (auto& fx : bombFXs_) {
		if (!fx.effect) continue;
		if (const ColorBox* b = getBoxByUid(fx.uid)) {
			const s3d::Rect boxRect(
				b->pos.x * TILE_SIZE, b->pos.y * TILE_SIZE,
				TILE_SIZE, TILE_SIZE
			);
			const s3d::RectF inner = boxRect.stretched(-8);
			const Vec2 worldCenter = inner.center();
			const Vec2 screenCenter = transform.transformPoint(worldCenter);

			// 논리 반폭(스크린 px)
			const Vec2 logicHalfScreen = (inner.size * 0.5) * scale;

			// 논리 사각형(기존과 동일)
			const RectF logicRect(Arg::center = screenCenter, logicHalfScreen * 2.0);

			// 폭발 진행도
			double explodeT = 0.0;
			if (fx.effect->isInExplode()) {
				explodeT = (fx.effect->getExplodeT() / fx.params.explodeTime);
			}

			// 폭발 중에만 드로우 AABB 확장
			RectF drawRect = logicRect;
			if (explodeT > 0.0) {
				// 안전 계수와 AA 패딩(튜닝 포인트)
				constexpr float kSpreadSafety = 1.30f;   // spread 계수
				constexpr float kAApad = 6.0f;    // 가장자리 여유

				// 파편 중심 후보(pc) 분산 + 논리 반폭 + 이동 여유(spread)
				const Vec2 pcMax = logicHalfScreen * 1.30;       // 후보 교점 분산 상한
				const Vec2 polyMax = logicHalfScreen * 1.00;       // 조각 자체 최대 반폭 근사
				const float spreadMargin = (float)fx.params.spread * kSpreadSafety + kAApad;

				const Vec2 halfDraw = pcMax + polyMax + Vec2{ spreadMargin, spreadMargin };
				drawRect = RectF(Arg::center = screenCenter, halfDraw * 2.0);
			}

			logicRects.push_back(logicRect);
			drawRects.push_back(drawRect);
			batchParams.push_back(fx.params);
			batchTimes.push_back(fx.effect->getTime());
			batchExplodeTs.push_back(explodeT);
		}
	}

	if (!logicRects.isEmpty()) {
		// 확장된 API 사용: 논리Rect(분해 기준) vs 드로우Rect(확장 AABB) 분리
		BombBoxEffect::drawBatchedExpanded(
			bombFXs_[0].effect->getPixelShader(),
			logicRects, drawRects,
			batchParams, batchTimes, batchExplodeTs
		);
	}
}

bool InGameScene::isBombGhost(uint64 uid) const
{
	for (const auto& fx : bombFXs_) {
		if (fx.uid == uid && fx.effect && !fx.effect->exploded()) return true;
	}
	return false;
}

bool InGameScene::isBombHidden(uint64 uid) const
{
	for (const auto& fx : bombFXs_) {
		if (fx.uid == uid && fx.effect && !fx.effect->exploded()) return true;
	}
	return false;
}

bool InGameScene::isBombNonBlocking(uint64 uid) const
{
	for (const auto& fx : bombFXs_) {
		if (fx.uid == uid && fx.effect && fx.effect->isExploding()) return true;
	}
	return false;
}

bool InGameScene::isBombPulsing(uint64 uid) const
{
	for (const auto& fx : bombFXs_) {
		if (fx.uid == uid && fx.effect && fx.effect->isPulsing()) {
			return true;
		}
	}
	return false;
}

bool InGameScene::shouldDrawBox(const ColorBox& box) const
{
	if (box.color == BoxColor::Black && isBombHidden(box.uid)) return false;
	return true;
}

void InGameScene::drawBoxes_RespectBombFX()
{
	for (const auto& box : boxes_) {
		if (!shouldDrawBox(box)) continue;

		const Rect boxRect{ box.pos.x * TILE_SIZE, box.pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
		const bool onGoal = std::any_of(
			getGoalPositionsForColor(box.color).begin(),
			getGoalPositionsForColor(box.color).end(),
			[&](const Point& g) { return g == box.pos; }
		);

		if (onGoal) {
			Circle{ boxRect.center(), 32 }.draw(ColorF{ getBoxColorF(box.color), 0.3 });
		}

		if (mergeFX_.active && (box.uid == mergeFX_.targetUid)) {
			RectF inner = boxRect.stretched(-8);
			constexpr double frameThickness = 3.0;
			RectF centerRect = inner.stretched(-frameThickness);
			const ColorF frameBase = ColorF{
				mergeFX_.baseColor.r * 1.2,
				mergeFX_.baseColor.g * 1.2,
				mergeFX_.baseColor.b * 1.2, 1.0
			};
			g_Shaders.paintSpread().draw(inner, frameBase);
			centerRect.draw(mergeFX_.baseColor);
			g_Shaders.paintSpread().draw(inner, mergeFX_.baseColor);
		}
		else {
			const ColorF c = getBoxColorF(box.color);
			boxRect.stretched(-8).draw(c);
			boxRect.stretched(-8).drawFrame(3, 0, ColorF{ c.r * 1.2, c.g * 1.2, c.b * 1.2 });
			Circle{ boxRect.center(), 8 }.draw(ColorF{ c, 0.8 });
		}
	}

}

bool InGameScene::isBombSpanActive() const
{
	for (const auto& fx : bombFXs_) {
		if (fx.effect && !fx.effect->exploded()) return true;
	}
	return false;
}

void InGameScene::rebuildBombFXFromState_() {
	bombFXs_.clear();

	for (const auto& b : boxes_) {
		if (b.color != BoxColor::Black) continue;
		const double expiry = bombExpiryAbs_.contains(b.uid) ? bombExpiryAbs_[b.uid]
			: (bombClock_ + kTotal);
		const double remaining = Max(0.0, expiry - bombClock_);
		const double elapsed = Clamp(kTotal - remaining, 0.0, kTotal);

		BombBoxInstance inst;
		inst.uid = b.uid;
		inst.effect = std::make_unique<BombBoxEffect>();
		inst.effect->reset();
		inst.params.explodeTime = static_cast<float>(kExpDuration);
		inst.params.spread = 220.0f;
		inst.params.gravity = 800.0f;
		inst.params.seed = static_cast<float>(Random(0.0, 1.0));

		if (elapsed < kPreDuration) {
			inst.effect->update(elapsed);
		}
		else {
			inst.effect->trigger();
			inst.effect->update(elapsed - kPreDuration);
		}
		bombFXs_.push_back(std::move(inst));
	}
}

void InGameScene::destroyWalls8(Point centerTile)
{
	const Array<Point> directions = {
		Point{-1, -1}, Point{0, -1}, Point{1, -1},  // 위쪽 3칸
		Point{-1,  0},               Point{1,  0},  // 좌우
		Point{-1,  1}, Point{0,  1}, Point{1,  1}   // 아래쪽 3칸
	};

	for (const auto& d : directions) {
		Point t = centerTile + d;
		if (!isInsideMap(t)) continue;

		if (!wallMask_.isEmpty() && wallMask_[t.y][t.x]) {
			wallMask_[t.y][t.x] = false;
			spawnWallBreakFXAtTile(t);
			continue;
		}

		if (mapData_[t.y][t.x] == TileType::Wall) {
			mapData_[t.y][t.x] = TileType::Empty;
			spawnWallBreakFXAtTile(t);
		}
	}
}
void InGameScene::spawnWallBreakFXAtTile(Point tile)
{
	WallBreakFX fx;
	fx.tilePos = tile;
	fx.effect = std::make_unique<BombBoxEffect>();
	fx.effect->reset();
	fx.effect->trigger();

	fx.params.pulseDuration = 0.0;
	fx.params.pulseCount = 0.0;
	fx.params.explodeTime = 0.6;
	fx.params.pulseAmp = 0.0f;
	fx.params.pulseSpeed = 0.0f;
	fx.params.spread = 100.0f;
	fx.params.gravity = 600.0f;
	fx.params.seed = static_cast<float>(Random(0.0, 1.0));
	fx.params.useWallColor = true;
	fx.params.wallColor = ColorF{ 0.3, 0.3, 0.35 };
	fx.finished = false;
	fx.shakeStarted = false;

	// 소형 흔들림 즉시 트리거
	const Vec2 worldCenter = tileToPixel(tile) + Vec2(TILE_SIZE * 0.5, TILE_SIZE * 0.5);
	const Vec2 camC = camera().getCenter();
	const double dist = (worldCenter - camC).length();
	const double scale = camera().getScale();

	double basePx = 5.0; // 벽 파괴는 약하게
	double maxR = 700.0 / Max(0.001, scale);
	double falloff = Saturate(1.0 - dist / maxR);
	double intenWorld = (basePx * falloff) / Max(0.001, scale);
	if (intenWorld > 0.0) {
		camera().shake(0.20, Min(intenWorld, 24.0 / Max(0.001, scale)));
		fx.shakeStarted = true;
	}

	wallBreakFXs.push_back(std::move(fx));
}

void InGameScene::updateWallBreakFX()
{
	const double dt = Scene::DeltaTime();

	for (auto& fx : wallBreakFXs)
	{
		// 이펙트 핸들 없으면 바로 종료 표시
		if (!fx.effect)
		{
			fx.finished = true;
			continue;
		}

		// 시간 진행
		fx.effect->update(dt);

		// 현재 폭발 구간 여부
		const bool inExplode = fx.effect->isInExplode();

		// 폭발 구간에 들어간 뒤 아직 벽 파괴를 하지 않았다면 1회만 수행
		// (폭탄 스타일: params.useWallColor == false 라고 가정)
		if (inExplode && !fx.params.useWallColor && !fx.params.wallsDestroyed)
		{
			destroyWalls8(fx.tilePos);   // 중심 8방향 벽 파괴
			fx.params.wallsDestroyed = true;
		}

		// 폭발 경과 시간(초) 조회
		const double expTime = fx.effect->getExplodeT();

		// 지정된 폭발 지속시간을 지나면 FX 종료
		if (inExplode && (expTime >= fx.params.explodeTime))
		{
			fx.finished = true;
		}
	}

	// 종료된 FX 정리
	wallBreakFXs.remove_if([](const WallBreakFX& e) { return e.finished; });
}

void InGameScene::drawWallBreakFX() {
	if (wallBreakFXs.isEmpty()) return;

	const Mat3x2 transform = camera().getMat3x2();
	const double scale = camera().getScale();

	Array<RectF> logicRects;
	Array<RectF> drawRects;
	Array<BombBoxEffect::Params> batchParams;
	Array<double> batchTimes;
	Array<double> batchExplodeTs;

	for (auto& fx : wallBreakFXs) {
		if (!fx.effect || fx.finished) continue;

		const Rect tileRect(
			fx.tilePos.x * TILE_SIZE, fx.tilePos.y * TILE_SIZE,
			TILE_SIZE, TILE_SIZE
		);
		const RectF inner = tileRect.stretched(-8);
		const Vec2 worldCenter = inner.center();
		const Vec2 screenCenter = transform.transformPoint(worldCenter);
		const Vec2 logicHalf = (inner.size * 0.5) * scale;

		// 논리Rect: 기존과 동일
		const RectF logicRect(Arg::center = screenCenter, logicHalf * 2.0);

		// 진행도
		double explodeT = 0.0;
		if (fx.effect->isInExplode()) {
			explodeT = fx.effect->getExplodeT() / fx.params.explodeTime; // 0..1
		}

		// 폭발 중에만 AABB 확장
		RectF drawRect = logicRect;
		if (explodeT > 0.0) {
			constexpr float kSpreadSafety = 1.30f;
			constexpr float kAApad = 6.0f;
			const float spreadMargin = (float)fx.params.spread * kSpreadSafety + kAApad;

			const Vec2 pcMax = logicHalf * 1.30;
			const Vec2 polyMax = logicHalf * 1.00;
			const Vec2 halfDraw = pcMax + polyMax + Vec2{ spreadMargin, spreadMargin };
			drawRect = RectF(Arg::center = screenCenter, halfDraw * 2.0);
		}

		logicRects << logicRect;
		drawRects << drawRect;
		batchParams << fx.params;
		batchTimes << fx.effect->getTime();
		batchExplodeTs << explodeT;
	}

	if (!logicRects.isEmpty() && wallBreakFXs[0].effect) {
		BombBoxEffect::drawBatchedExpanded(
			wallBreakFXs[0].effect->getPixelShader(),
			logicRects, drawRects,
			batchParams, batchTimes, batchExplodeTs
		);
	}
}

void InGameScene::applyHoloFromHeldItem_()
{
	auto& holo = g_Shaders.holographic();
	if (playerHeldItem_ == ItemType::None) {
		holo.setRainbowMode(false);
		holo.setIntensity(0.0f);
		return;
	}

	const ColorF c = getItemColorF(playerHeldItem_);
	holo.setRainbowMode(false);
	holo.setHoloColor(c);
	holo.setIntensity(0.7f);
}

bool InGameScene::canMoveTo(Point pos) const
{
	if (pos.x < 0 || pos.x >= getMapWidth() || pos.y < 0 || pos.y >= getMapHeight())
		return false;

	if (isBlocked(pos)) return false;

	if (getBoxAt(pos) != nullptr)
		return false;

	// 미끄러지는 중인 박스들의 목표 위치 확인
	for (const auto& task : iceSlideTasks_)
	{
		if (!task.active) continue;

		// 현재 박스의 위치와 방향을 기반으로 최종 도착 위치 예측
		if (const ColorBox* box = getBoxByUid_const(task.uid))
		{
			Point currentPos = box->pos;
			Point testPos = currentPos;

			// 박스가 최종적으로 멈출 위치까지 계산
			while (true)
			{
				const Point nextPos = testPos + task.dir; // 다음 후보 [attached_file:7]

				// 맵 밖이거나 "차단 칸"(기존 Wall + 마스크)이면 현재 testPos에서 멈춤
				if (!isInsideMap(nextPos) || isBlocked(nextPos) || getBoxAt(nextPos) != nullptr)
				{
					if (testPos == pos) return false; // 플레이어가 가려는 칸이 그 박스의 최종 정지칸 [attached_file:7]
					break; // 시뮬 종료 [attached_file:7]
				}

				testPos = nextPos; // 전진 [attached_file:7]

				// 얼음이 아니면 여기서 멈춤
				if (!isIce(testPos))
				{
					if (testPos == pos) return false; // 최종 정지칸과 충돌 [attached_file:7]
					break; // 시뮬 종료 [attached_file:7]
				}
			}
		}
	}

	return true;
}

// const 버전의 getBoxByUid 추가 (헬퍼 함수)
const ColorBox* InGameScene::getBoxByUid_const(uint64 uid) const
{
	for (const auto& b : boxes_) {
		if (b.uid == uid) return &b;
	}
	return nullptr;
}

ColorBox* InGameScene::getBoxAt(Point pos)
{
	for (auto& box : boxes_) {
		if (box.pos != pos) continue;
		// 충돌 제외는 '폭발 구간'에서만
		if (box.color == BoxColor::Black && isBombNonBlocking(box.uid)) {
			continue;
		}
		return &box;
	}
	return nullptr;
}

// const 버전
const ColorBox* InGameScene::getBoxAt(Point pos) const
{
	for (const auto& box : boxes_) {
		if (box.pos != pos) continue;
		if (box.color == BoxColor::Black && isBombNonBlocking(box.uid)) {
			continue;
		}
		return &box;
	}
	return nullptr;
}

bool InGameScene::canPushBox(Point playerPos, Point boxPos, Point dir) const {
	const Point next = boxPos + dir;

	if (!isInsideMap(next)) return false;
	if (isBlocked(next)) return false;

	if (const auto* target = getBoxAt(next)) {
		if (const auto* current = getBoxAt(boxPos)) {
			const BoxColor currentColor = getEffectiveBoxColor(current->uid);
			const BoxColor targetColor = getEffectiveBoxColor(target->uid);
			const auto merged = getMergedColor(currentColor, targetColor);
			return merged.has_value();
		}
		return false;
	}
	return true;
}
void InGameScene::pushBox(ColorBox* box, Point direction)
{
    box->pos = box->pos + direction;
    
    // 블록 밀기 효과음 재생
    playBoxSound(box->color);
}

void InGameScene::playBoxSound(BoxColor color)
{
    // 볼륨 설정 (5배 증가: 0.6 * 5 = 3.0)
    const double volume = 2.2;
    
    switch (color)
    {
    case BoxColor::Red:
        // 빨강 = C6
        if (!noteC6_.isEmpty())
            noteC6_.playOneShot(volume);
        break;
        
    case BoxColor::Yellow:
        // 노랑 = E5
        if (!noteE5_.isEmpty())
            noteE5_.playOneShot(volume);
        break;
        
    case BoxColor::Blue:
        // 파랑 = G5
        if (!noteG5_.isEmpty())
            noteG5_.playOneShot(volume);
        break;
        
    case BoxColor::Orange:
        // 주황 = Red(C6) + Yellow(E5)
        if (!noteC6_.isEmpty())
            noteC6_.playOneShot(volume);
        if (!noteE5_.isEmpty())
            noteE5_.playOneShot(volume);
        break;
        
    case BoxColor::Green:
        // 초록 = Yellow(E5) + Blue(G5)
        if (!noteE5_.isEmpty())
            noteE5_.playOneShot(volume);
        if (!noteG5_.isEmpty())
            noteG5_.playOneShot(volume);
        break;
        
    case BoxColor::Violet:
        // 보라 = Blue(G5) + Red(C6)
        if (!noteG5_.isEmpty())
            noteG5_.playOneShot(volume);
        if (!noteC6_.isEmpty())
            noteC6_.playOneShot(volume);
        break;
        
    case BoxColor::Black:
        // 검정 = C6 + E5 + G5 + C6 (C6 중복)
        if (!noteC6_.isEmpty())
            noteC6_.playOneShot(volume);
        if (!noteE5_.isEmpty())
            noteE5_.playOneShot(volume);
        if (!noteG5_.isEmpty())
            noteG5_.playOneShot(volume);
        if (!noteC6_.isEmpty())
            noteC6_.playOneShot(volume * 0.5);  // 두 번째 C6는 볼륨 절반
        break;
        
    default:
        break;
    }
}

void InGameScene::checkBoxMerge(Point pos)
{
    ColorBox* box1 = getBoxAt(pos);
    if (!box1) return;
    
    // 같은 위치에 다른 상자가 있는지 확인 (합성 가능)
    for (auto& box2 : boxes_)
    {
        if (&box2 == box1) continue;
        if (box2.pos != pos) continue;
        
        // 빨강+초록 = 노란색
        if ((box1->color == BoxColor::Red && box2.color == BoxColor::Green) ||
            (box1->color == BoxColor::Green && box2.color == BoxColor::Red))
        {
            // 두 상자를 제거하고 노란색 상자 생성
            BoxColor newColor = BoxColor::Yellow;
            Point newPos = pos;
            
            // 두 상자 제거
            boxes_.remove_if([&](const ColorBox& b) {
                return b.pos == pos && (b.color == BoxColor::Red || b.color == BoxColor::Green);
            });
            
            // 노란색 상자 추가
            boxes_.push_back(ColorBox{ newPos, newColor });
            score_ += 50;  // 합성 보너스
            break;
        }
    }
}
void InGameScene::slideBoxOnIce(ColorBox* box, Point dir)
{
	if (!box || !isIce(box->pos)) return;
	startIceSlideTask_(box, dir);
}
void InGameScene::updateIceSlideTasks_(double dt)
{
	for (auto& task : iceSlideTasks_)
	{
		if (!task.active) continue;

		task.cooldown -= dt;

		if (task.cooldown <= 0.0)
		{
			if (ColorBox* box = getBoxByUid(task.uid))
			{
				const Point nextPos = box->pos + task.dir;

				// 다음 위치에 다른 박스가 있는지 확인
				if (ColorBox* targetBox = getBoxAt(nextPos))
				{
					// 합성 가능한지 확인
					const BoxColor currentColor = getEffectiveBoxColor(box->uid);
					const BoxColor targetColor = getEffectiveBoxColor(targetBox->uid);

					if (Optional<BoxColor> merged = getMergedColor(currentColor, targetColor))
					{
						// 합성 처리 (기존 코드 유지)
						forceMergePaintFXCompletion();

						const uint64 uidA = box->uid;
						const uint64 uidB = targetBox->uid;
						boxes_.remove_if([uidA, uidB](const ColorBox& b) {
							return (b.uid == uidA || b.uid == uidB);
						});

						ColorBox newBox(nextPos, *merged, 0.0, nextBoxUID_++);

						if (*merged == BoxColor::Black)
						{
							newBox.creationTime = gameTime_;
							bombExpiryAbs_[newBox.uid] = bombClock_ + kTotal;
							triggerBombBoxFXForBlack_Multi(newBox.uid, BLACK_BOX_LIFETIME);

							if (!bombExplosionSound_.isEmpty())
							{
								bombExplosionSound_.stop();
								bombExplosionSound_.setVolume(0.6);
								bombExplosionSound_.play();
							}
						}

						boxes_.push_back(newBox);

						triggerMergePaintFX_Directional(nextPos,
							getBoxColorF(targetColor),
							getBoxColorF(*merged),
							task.dir);

						playBoxSound(*merged);

						const ColorTier tier = getColorTier(*merged);
						if (tier == ColorTier::Secondary) score_ += 50;
						else if (tier == ColorTier::Tertiary) score_ += 100;

						// 새로 생성된 박스도 얼음 위라면 계속 미끄러짐
						if (isIce(nextPos))
						{
							if (ColorBox* newBoxPtr = getBoxAt(nextPos))
							{
								slideBoxOnIce(newBoxPtr, task.dir);
							}
						}

						task.active = false;
					}
					else
					{
						// 합성 불가능하면 정지
						task.active = false;
					}
				}
				// 다음 위치가 이동 가능한 빈 공간인지 확인
				else if (!isInsideMap(nextPos) || mapData_[nextPos.y][nextPos.x] == TileType::Wall)
				{
					// 벽이나 맵 밖이면 정지
					task.active = false;
				}
				else
				{
					// 빈 공간으로 이동
					box->pos = nextPos;
					task.cooldown = 0.05;

					// 다음 위치가 얼음이 아니면 미끄러짐 종료
					if (!isIce(nextPos))
					{
						task.active = false;
					}
				}
			}
			else
			{
				task.active = false;
			}
		}
	}

	iceSlideTasks_.remove_if([](const IceSlideTask& t) { return !t.active; });
}

bool InGameScene::isGameClear() const
{
    // 모든 목표 지점에 해당하는 색상의 상자가 있는지 확인
    const Array<BoxColor> allColors = {
        BoxColor::Red, BoxColor::Yellow, BoxColor::Blue,
        BoxColor::Orange, BoxColor::Green, BoxColor::Violet, BoxColor::Black
    };
    
    for (const auto& color : allColors)
    {
        const Array<Point>& goals = getGoalPositionsForColor(color);
        
        // 이 색상의 목표가 있다면
        for (const auto& goal : goals)
        {
            const ColorBox* box = getBoxAt(goal);
            if (!box || box->color != color)
                return false;  // 목표 지점에 해당 색상 상자가 없으면 실패
        }
    }
    
    return true;  // 모든 목표 지점에 해당 색상 상자가 있으면 클리어
}

ColorTier InGameScene::getColorTier(BoxColor color) const
{
    switch (color)
    {
    case BoxColor::Red:
    case BoxColor::Yellow:
    case BoxColor::Blue:
        return ColorTier::Primary;
        
    case BoxColor::Orange:
    case BoxColor::Green:
    case BoxColor::Violet:
        return ColorTier::Secondary;
        
    case BoxColor::Black:
        return ColorTier::Tertiary;
        
    default:
        return ColorTier::Primary;
    }
}

Optional<BoxColor> InGameScene::getMergedColor(BoxColor color1, BoxColor color2) const
{
    // 같은 색은 합성 불가
    if (color1 == color2)
        return none;
    
    ColorTier tier1 = getColorTier(color1);
    ColorTier tier2 = getColorTier(color2);
    
    // Black끼리는 합성 불가
    if (tier1 == ColorTier::Tertiary || tier2 == ColorTier::Tertiary)
        return none;
    
    // 1차 + 1차 = 2차
    if (tier1 == ColorTier::Primary && tier2 == ColorTier::Primary)
    {
        // R + Y = O
        if ((color1 == BoxColor::Red && color2 == BoxColor::Yellow) ||
            (color1 == BoxColor::Yellow && color2 == BoxColor::Red))
            return BoxColor::Orange;
            
        // Y + B = G
        if ((color1 == BoxColor::Yellow && color2 == BoxColor::Blue) ||
            (color1 == BoxColor::Blue && color2 == BoxColor::Yellow))
            return BoxColor::Green;
            
        // B + R = V
        if ((color1 == BoxColor::Blue && color2 == BoxColor::Red) ||
            (color1 == BoxColor::Red && color2 == BoxColor::Blue))
            return BoxColor::Violet;
    }
    
    // 2차 + 2차 = K (무조건)
    if (tier1 == ColorTier::Secondary && tier2 == ColorTier::Secondary)
        return BoxColor::Black;
    
    // 1차 + 2차 = K (단, 2차 색상의 구성 요소가 아닌 경우만)
    if ((tier1 == ColorTier::Primary && tier2 == ColorTier::Secondary) ||
        (tier1 == ColorTier::Secondary && tier2 == ColorTier::Primary))
    {
        BoxColor primary = (tier1 == ColorTier::Primary) ? color1 : color2;
        BoxColor secondary = (tier1 == ColorTier::Secondary) ? color1 : color2;
        
        // O(주황=R+Y) + R 또는 Y = 불가능
        if (secondary == BoxColor::Orange)
        {
            if (primary == BoxColor::Red || primary == BoxColor::Yellow)
                return none;
        }
        
        // G(초록=Y+B) + Y 또는 B = 불가능
        if (secondary == BoxColor::Green)
        {
            if (primary == BoxColor::Yellow || primary == BoxColor::Blue)
                return none;
        }
        
        // V(보라=B+R) + B 또는 R = 불가능
        if (secondary == BoxColor::Violet)
        {
            if (primary == BoxColor::Blue || primary == BoxColor::Red)
                return none;
        }
        
        // 구성 요소가 아닌 경우만 Black 생성
        return BoxColor::Black;
    }
    
    return none;
}

ColorF InGameScene::getBoxColorF(BoxColor color) const
{
    switch (color)
    {
    case BoxColor::Red:
        return ColorF{ 0.66, 0.25, 0.25 };      // 빨강 (채도 낮춤)
    case BoxColor::Yellow:
        return ColorF{ 1.0, 1.0, 0.5 };      // 노랑 (채도 절반)
    case BoxColor::Blue:
        return ColorF{ 0.2, 0.4, 0.9 };      // 파랑 (원래값)
    case BoxColor::Orange:
        return ColorF{ 1.0, 0.5, 0.0 };      // 주황 (원래값)
    case BoxColor::Green:
        return ColorF{ 0.2, 0.8, 0.3 };      // 초록 (원래값)
    case BoxColor::Violet:
        return ColorF{ 0.7, 0.2, 0.8 };      // 보라 (원래값)
    case BoxColor::Black:
        return ColorF{ 0.1, 0.1, 0.1 };      // 검정 (원래값)
    default:
        return ColorF{ 0.1, 0.1, 0.1 };
    }
}
void InGameScene::updateFinalStageTileOverlay()
{
    static double lastCheckedTime = -1.0;
    static int32 lastAppliedPhase = -1;

    const Array<String> overlay = StageData::getFinalStageTileOverlay(gameTime_);

    if (!overlay.isEmpty())
    {
        // ★ 6초 간격으로 변경
        int32 currentPhase = (gameTime_ >= 18.0) ? 3 :  // 18초
                             (gameTime_ >= 12.0) ? 2 :  // 12초
                             (gameTime_ >= 6.0) ? 1 : 0;  // 6초

        if (currentPhase != lastAppliedPhase)
        {
            applyTileOverlay(overlay,StageData::isFinalStage(currentStage_) ? OverlayApplyMode::OverlayOnly : OverlayApplyMode::WriteToMap);
            lastAppliedPhase = currentPhase;

            checkPlayerLavaCollision();
            checkBoxesLavaCollision();
        }
    }
}
void InGameScene::checkBoxesLavaCollision()
{
	Array<uint64> boxesToRemove;

	for (const auto& box : boxes_)
	{
		if (isInsideMap(box.pos) &&
			mapData_[box.pos.y][box.pos.x] == TileType::Lava)
		{
			boxesToRemove.push_back(box.uid);
		}
	}

	// 용암에 닿은 박스 제거
	for (uint64 uid : boxesToRemove)
	{
		removeBoxByUid(uid);
	}
}
// ★ 추가: 플레이어 용암 충돌 확인
void InGameScene::checkPlayerLavaCollision()
{
	if (isInsideMap(playerPos_) &&
		mapData_[playerPos_.y][playerPos_.x] == TileType::Lava)
	{
		if (!isPlayerDead_)
		{
			isPlayerDead_ = true;
			deathAnimTimer_ = 0.0;
			createDeathEffect(true);  // 주황색 파티클

			if (!bossBgm_.isEmpty() && bossBgm_.isPlaying())
			{
				savedMusicPosition_ = bossBgm_.posSec();
				bossBgm_.stop();
			}
		}
	}
}

void InGameScene::update()
{
	const double dt = Scene::DeltaTime();

	camera().update();

	const bool focused = Window::GetState().focused;
	// 페이드 업데이트
	if (isFading_)
	{
		fadeTimer_ += dt;
		if (fadeTimer_ >= fadeDuration_)
		{
			isFading_ = false;
		}
	}
	// ★ Final Stage 시간 기반 타일 오버레이 업데이트
	if (StageData::isFinalStage(currentStage_))
	{
		// 보스 공격 시퀀스 중이 아니고, 모든 골이 채워졌을 때
		if (!isBossAttackSequenceActive_ && !isCleared_ && !isPlayerDead_ && checkAllGoalsFilledForBoss())
		{
			startBossAttackSequence();
		}

		// 보스 공격 시퀀스 업데이트
		if (isBossAttackSequenceActive_)
		{
			updateBossAttackSequence(dt);
			updateMergePaintFX();
			updateBossHitEffect(dt);
			return;
		}

		// 보스 패턴 업데이트
		if (!isBossAttackSequenceActive_ && !isPlayerDead_ && !isCleared_)
		{
			updateBossWallPattern(dt);
		}
	}
	// ★ StageFailed 상태: R키만 동작, Z키는 불가, ESC도 불가
	if (isFailed_ && showFailedButtons_)
	{
		// R키로 리트라이
		if (!isCleared_ && KeyR.down()) {
			isFailed_ = false;
			showFailedButtons_ = false;
			isPlayerDead_ = false;
			deathAnimTimer_ = 0.0;
			deathParticles_.clear();
			gameTime_ = 0.0;
			moves_ = 0;
			score_ = 0;
			playerHealth_ = MAX_HEALTH;
			const auto initialOverlay = StageData::getFinalStageTileOverlay(0.0);
			if (!initialOverlay.isEmpty()) { applyTileOverlay(initialOverlay,StageData::isFinalStage(currentStage_) ? OverlayApplyMode::OverlayOnly: OverlayApplyMode::WriteToMap); }
			

			loadStage(currentStage_);

			if (StageData::isFinalStage(currentStage_))
			{
				const auto ov = StageData::getFinalStageTileOverlay(0.0);
				applyTileOverlay(ov, OverlayApplyMode::OverlayOnly);
				initBossAttacks();
				initBossWallSystem();
				if (!bossBgm_.isEmpty())
				{
					bossBgm_.setVolume(0.4);
					bossBgm_.play();
				}
			}
			else
			{
				if (!bgm_.isEmpty())
				{
					bgm_.setVolume(0.33);
					bgm_.play();
				}
			}

			updateMergePaintFX();
			return;
		}
		// StageFailed 상태에서는 다른 입력 처리 없음
		updateMergePaintFX();
		return;
	}

	if (isPlayerDead_) {
		deathAnimTimer_ += dt;
		updateDeathEffect();
		updateBossExplosions(dt); 

		if (!isCleared_ && KeyR.down()) {
			gameTime_ = 0.0;
			moves_ = 0;
			score_ = 0;
			playerHeldItem_ = ItemType::None;
			isPlayerDead_ = false;
			deathAnimTimer_ = 0.0;
			deathParticles_.clear();

			isFailed_ = false;
			showFailedButtons_ = false;

			bombFXs_.clear();
			wallBreakFXs.clear();
			bombExpiryAbs_.clear();

			loadStage(currentStage_);

			if (StageData::isFinalStage(currentStage_))
			{
				const auto initialOverlay = StageData::getFinalStageTileOverlay(0.0);
				if (!initialOverlay.isEmpty()) { applyTileOverlay(initialOverlay,OverlayApplyMode::OverlayOnly); }

				initBossAttacks();
				initBossWallSystem();

				if (!bossBgm_.isEmpty())
				{
					bossBgm_.setVolume(0.4);
					bossBgm_.play();
				}
			}
			else
			{
				if (!bgm_.isEmpty())
				{
					bgm_.setVolume(0.33);
					bgm_.play();
				}
			}

			updateMergePaintFX();
			return;
		}

		return;
	}
	if (!focused && !showHelpScreen_ && !isCleared_) {
		showHelpScreen_ = true;
		if (!bgm_.isEmpty() && bgm_.isPlaying()) {
			bgm_.pause();
		}
		if (!bombExplosionSound_.isEmpty() && bombExplosionSound_.isPlaying()) {
			bombExplosionSound_.pause();
		}
	}

	// ESC 키 처리
	if (KeyEscape.down()) {
		showHelpScreen_ = !showHelpScreen_;
		if (showHelpScreen_) {
			// 일시정지 시작
			if (!bgm_.isEmpty() && bgm_.isPlaying()) {
				bgm_.pause();
			}
			// ★ 보스 배경음악도 멈춤
			if (!bossBgm_.isEmpty() && bossBgm_.isPlaying()) {
				bossBgm_.pause();
			}
			// ✅ 폭탄 생성 사운드도 멈춤
			if (!bombExplosionSound_.isEmpty() && bombExplosionSound_.isPlaying()) {
				bombExplosionSound_.pause();
			}
		}
		else {
			// 일시정지 해제
			if (!isPlayerDead_)
			{
				if (StageData::isFinalStage(currentStage_))
				{
					if (!bossBgm_.isEmpty() && !bossBgm_.isPlaying())
					{
						bossBgm_.play();
					}
				}
				else
				{
					if (!bgm_.isEmpty() && !bgm_.isPlaying())
					{
						bgm_.play();
					}
				}
			}
			if (!bombExplosionSound_.isEmpty() && bombExplosionSound_.isPaused()) {
				bombExplosionSound_.play();
			}
		}
	}

	// ★★★ 일시정지 중이면 여기서 완전히 멈춤 ★★★
	if (showHelpScreen_) {
		// 일시정지 중에는 비주얼 이펙트만 업데이트
		updateMergePaintFX();
		return;
	}
	bombClock_ += dt;

	if (isInsideMap(playerPos_) && mapData_[playerPos_.y][playerPos_.x] == TileType::Lava) {
		if (!isPlayerDead_) {
			isPlayerDead_ = true;
			deathAnimTimer_ = 0.0;
			createDeathEffect(true);  // ★ true = 흰색 파티클 사용
			const auto initialOverlay = StageData::getFinalStageTileOverlay(0.0);
			if (!initialOverlay.isEmpty()) { applyTileOverlay(initialOverlay,StageData::isFinalStage(currentStage_) ? OverlayApplyMode::OverlayOnly: OverlayApplyMode::WriteToMap); }

			if (StageData::isFinalStage(currentStage_)) {
				if (!bossBgm_.isEmpty() && bossBgm_.isPlaying()) {
					savedMusicPosition_ = bossBgm_.posSec();
					bossBgm_.stop();
					initBossAttacks();
					initBossWallSystem();
				}
			}
			else {
				if (!bgm_.isEmpty() && bgm_.isPlaying()) {
					savedMusicPosition_ = bgm_.posSec();
					bgm_.stop();
				}
			}
		}
	}

	// ★★★ 게임 업데이트 ★★★
	updateMergePaintFX();
	updateBombBoxFX_Multi(dt);
	updateWallBreakFX();
	applyHoloFromHeldItem_();

	// ★ 보스 공격 업데이트
	if (StageData::isFinalStage(currentStage_) && !isPlayerDead_ && !isCleared_)
	{
		//updateBossAttack(dt);
		updateBossHitEffect(dt);
		updateBossAttacks(dt);
		updateBossProjectiles(dt);
		updateEnergyBalls(dt);
	}
	updateBossExplosions(dt);  // 폭발은 항상 업데이트

	if (focused && !isCleared_ && !isPlayerDead_)
	{
		if (StageData::isFinalStage(currentStage_))
		{
			// ★ 보스 스테이지에서는 bossBgm만 재생
			if (!bossBgm_.isEmpty() && !bossBgm_.isPlaying())
			{
				bossBgm_.play();
			}
			// ★ HappyOcean.mp3가 재생 중이면 정지
			if (!bgm_.isEmpty() && bgm_.isPlaying())
			{
				bgm_.stop();
			}
		}
		else
		{
			if (!bgm_.isEmpty() && !bgm_.isPlaying())
			{
				bgm_.play();
			}
		}
	}
	if (StageData::isFinalStage(currentStage_) && KeyF2.down()) {
		isCleared_ = true;
		changeScene(SceneType::Ending);
		return;
	}
	// ★ R키로 즉시 리트라이 (죽음 중에도 가능)
	if (!isCleared_ && KeyR.down()) {
		gameTime_ = 0.0;
		moves_ = 0;
		score_ = 0;
		playerHeldItem_ = ItemType::None;
		isPlayerDead_ = false;
		deathAnimTimer_ = 0.0;
		deathParticles_.clear();

		isFailed_ = false;
		showFailedButtons_ = false;

		bombFXs_.clear();
		wallBreakFXs.clear();
		bombExpiryAbs_.clear();

		bossProjectiles_.clear();
		bossExplosionParticles_.clear();
		bossAttackTimer_ = 0.0;

		const auto initialOverlay = StageData::getFinalStageTileOverlay(0.0);
		if (!initialOverlay.isEmpty()) { applyTileOverlay(initialOverlay, StageData::isFinalStage(currentStage_) ? OverlayApplyMode::OverlayOnly: OverlayApplyMode::WriteToMap); }
		

		loadStage(currentStage_);

		if (StageData::isFinalStage(currentStage_))
		{
			const Array<String> initialOverlay = StageData::getFinalStageTileOverlay(0.0);
			if (!initialOverlay.isEmpty())
			{
				applyTileOverlay(initialOverlay,OverlayApplyMode::OverlayOnly);
				initBossAttacks();
				initBossWallSystem();
			}

			if (!bossBgm_.isEmpty())
			{
				bossBgm_.setVolume(0.4);
				bossBgm_.play();
			}
		}
		else
		{
			if (!bgm_.isEmpty())
			{
				bgm_.setVolume(0.33);
				bgm_.play();
			}
		}

		updateMergePaintFX();
		return;
	}

	// 클리어 상태 처리
	if (isCleared_) {
		if (!showClearButtons_) {
			showClearButtons_ = true;
		}
		gameTime_ += dt;

		if (KeySpace.down() || KeyEnter.down()) {
			if (currentStage_ < StageData::getTotalStageCount()) {
				currentStage_++;
				loadStage(currentStage_);
				isCleared_ = false;
				showClearButtons_ = false;
				showClearEffect_ = false;
				clearParticles_.clear();
				gameTime_ = 0.0;
				moves_ = 0;
			}
			else {
				changeScene(SceneType::StageSelect);
			}
		}

		if (showClearEffect_) {
			updateClearEffect();
		}

		updateMergePaintFX();
		return;
	}

	// 일반 업데이트
	gameTime_ += dt;

	updateIceSlideTasks_(dt);

	if (isSliding_) {
		continueSliding();
		updatePlayer();
		updateAnimations();
		updatePaintAnimation();
		updateMergePaintFX();
		updateBlackBoxes();
		if (!isCleared_ && isGameClear()) {
			isCleared_ = true;
			score_ += 1000;
			showClearEffect_ = true;
			clearEffectTimer_ = 0.0;
			createClearEffect();
			if (gameData_) {
				gameData_->clearStage(currentStage_);
			}
		}
		if (showClearEffect_ && !isCleared_) {
			updateClearEffect();
		}
		return;
	}

	handleInput();
	updatePlayer();
	updateAnimations();
	updatePaintAnimation();
	updateMergePaintFX();
	updateBlackBoxes();

	if (!isCleared_ && isGameClear() && !StageData::isFinalStage(currentStage_)) {
		isCleared_ = true;
		score_ += 1000;
		showClearEffect_ = true;
		clearEffectTimer_ = 0.0;
		createClearEffect();
		if (!stageClearSound_.isEmpty()) {
			stageClearSound_.playOneShot(0.9);
		}
		if (gameData_) {
			gameData_->clearStage(currentStage_);
		}
	}

	if (showClearEffect_ && !isCleared_) {
		updateClearEffect();
	}
}

void InGameScene::bufferInputWhileMoving() {
	// 이번 프레임 새로 눌린 방향만 큐에 저장 (.down 우선)
	if (KeyRight.down()) {
		queuedDir_ = Point{ 1, 0 };
	}
	else if (KeyLeft.down()) {
		queuedDir_ = Point{ -1, 0 };
	}
	else if (KeyUp.down()) {
		queuedDir_ = Point{ 0, -1 };
	}
	else if (KeyDown.down()) {
		queuedDir_ = Point{ 0, 1 };
	}
}

bool InGameScene::pollMoveDirection(Point& outDir, TacoDirection& outTacoDir, bool& outFacingLeft) {
	auto set = [&](const Point& d, TacoDirection td, bool fl) {
		outDir = d;
		outTacoDir = td;
		outFacingLeft = fl;
		return true;
		};

	// 1) 버퍼 우선 소비
	if (queuedDir_) {
		const Point q = *queuedDir_;
		queuedDir_.reset();
		if (q.x > 0) return set(Point{ 1, 0 }, TacoDirection::Side, false);
		if (q.x < 0) return set(Point{ -1, 0 }, TacoDirection::Side, true);
		if (q.y < 0) return set(Point{ 0, -1 }, TacoDirection::Up, isFacingLeft_);
		if (q.y > 0) return set(Point{ 0, 1 }, TacoDirection::Down, isFacingLeft_);
	}

	// 2) 이번 프레임 .down 우선 (단일 선택, 가로→세로 예시 우선순위)
	if (KeyRight.down()) return set(Point{ 1, 0 }, TacoDirection::Side, false);
	if (KeyLeft.down())  return set(Point{ -1, 0 }, TacoDirection::Side, true);
	if (KeyUp.down())    return set(Point{ 0, -1 }, TacoDirection::Up, isFacingLeft_);
	if (KeyDown.down())  return set(Point{ 0, 1 }, TacoDirection::Down, isFacingLeft_);

	// 3) 누르고 있는 키(.pressed) — 쿨다운이 없을 때만
	if (inputCooldown_ <= 0.0) {
		if (KeyRight.pressed()) return set(Point{ 1, 0 }, TacoDirection::Side, false);
		if (KeyLeft.pressed())  return set(Point{ -1, 0 }, TacoDirection::Side, true);
		if (KeyUp.pressed())    return set(Point{ 0, -1 }, TacoDirection::Up, isFacingLeft_);
		if (KeyDown.pressed())  return set(Point{ 0, 1 }, TacoDirection::Down, isFacingLeft_);
	}

	return false;
}

void InGameScene::handleInput()
{
	const double dt = Scene::DeltaTime();

	inputCooldown_ = Max(0.0, inputCooldown_ - dt);
	bumpSoundCooldown_ = Max(0.0, bumpSoundCooldown_ - dt);

	if (isPlayerMoving_ || isPlayingPaintAnimation_ || isSliding_) {
		bufferInputWhileMoving();
		return;
	}

	// 활성화된 얼음 미끄러짐 태스크가 있으면 입력 차단
	bool hasActiveSlideTasks = false;
	for (const auto& task : iceSlideTasks_) {
		if (task.active) {
			hasActiveSlideTasks = true;
			break;
		}
	}

	if (hasActiveSlideTasks) {
		bufferInputWhileMoving();
		return;
	}
	Point dir(0, 0);
	bool moved = false;
	TacoDirection newDir = tacoDirection_;
	bool newFacingLeft = isFacingLeft_;
	auto press = [&](const Input& key, const Point& d, TacoDirection tdir, bool faceLeft = false) {
		if (key.down() || (key.pressed() && inputCooldown_ <= 0.0)) {
			dir = d; moved = true; newDir = tdir;
			newFacingLeft = (tdir == TacoDirection::Side) ? faceLeft : isFacingLeft_;
		}
		};
	// 방향키와 WASD 키 모두 지원
	press(KeyLeft, { -1, 0 }, TacoDirection::Side, true);
	press(KeyA, { -1, 0 }, TacoDirection::Side, true);
	press(KeyRight, { 1, 0 }, TacoDirection::Side, false);
	press(KeyD, { 1, 0 }, TacoDirection::Side, false);
	press(KeyUp, { 0,-1 }, TacoDirection::Up);
	press(KeyW, { 0,-1 }, TacoDirection::Up);
	press(KeyDown, { 0, 1 }, TacoDirection::Down);
	press(KeyS, { 0, 1 }, TacoDirection::Down);
	if (!moved) return;

	const Point newPos = playerPos_ + dir;

	const bool enteringIce = (isIce(newPos) && !isIce(playerPos_));
	const bool leavingIce = (isIce(playerPos_) && !isIce(newPos));

	if (!isInsideMap(newPos) || mapData_[newPos.y][newPos.x] == TileType::Wall) {
		if (!bumpSound_.isEmpty() && bumpSoundCooldown_ <= 0.0) {
			bumpSound_.playOneShot(0.4);
			bumpSoundCooldown_ = 0.25;

		}
		return;
	}

	if (ColorBox* box = getBoxAt(newPos)) {
		if (playerHeldItem_ != ItemType::None) {
			if (tryChangeBoxColor(newPos, dir)) {
				playerHeldItem_ = ItemType::None;
				return;
			}
		}
		const Point next = box->pos + dir;

		if (!canPushBox(playerPos_, box->pos, dir)) {
			if (!bumpSound_.isEmpty() && bumpSoundCooldown_ <= 0.0) {
				bumpSound_.playOneShot(0.4);
				bumpSoundCooldown_ = 0.25;

			}
			return;
		}

		// 합성 경로(타깃 존재)
		if (ColorBox* targetPeek = getBoxAt(next)) {
			bool createsBomb = false;
			const BoxColor curC = getEffectiveBoxColor(box->uid);
			const BoxColor tgtC = getEffectiveBoxColor(targetPeek->uid);
			if (Optional<BoxColor> m = getMergedColor(curC, tgtC)) {
				createsBomb = (*m == BoxColor::Black);
			}
			// 실제 합성
			const BoxColor currentBoxColor = getEffectiveBoxColor(box->uid);
			const BoxColor targetBoxColor = getEffectiveBoxColor(targetPeek->uid);
			if (Optional<BoxColor> merged = getMergedColor(currentBoxColor, targetBoxColor)) {
				forceMergePaintFXCompletion();

				// 두 상자 제거
				const uint64 uidA = box->uid;
				const uint64 uidB = targetPeek->uid;
				boxes_.remove_if([&](const ColorBox& b) {
					return (b.uid == uidA || b.uid == uidB);
				});

				// 새 상자 생성
				ColorBox newBox(next, *merged, 0.0, nextBoxUID_++);
				if (*merged == BoxColor::Black) {
					newBox.creationTime = gameTime_;
					bombExpiryAbs_[newBox.uid] = bombClock_ + kTotal;
					triggerBombBoxFXForBlack_Multi(newBox.uid, BLACK_BOX_LIFETIME);
					// 앵커 진행 중이면 UID 수집
					if (!bombExplosionSound_.isEmpty()) {
						bombExplosionSound_.stop();
						bombExplosionSound_.setVolume(0.6);
						bombExplosionSound_.play();
					}

				}
				boxes_.push_back(newBox);

				// 이펙트/사운드/점수(생략 없이 유지)
				const Vec2 originUV = impactOriginLocalUVForDir(dir);
				const ColorF base = getBoxColorF(targetBoxColor);
				const ColorF result = getBoxColorF(*merged);
				mergeFX_.active = true;
				mergeFX_.baseColor = base;
				mergeFX_.paintColor = result;
				mergeFX_.originUV = originUV;
				mergeFX_.targetUid = newBox.uid;
				mergeFX_.finalColor = *merged;
				mergeFX_.commitPending = true;
				g_Shaders.paintSpread().setPaintColor(result);
				g_Shaders.paintSpread().setOriginPoint(originUV);
				g_Shaders.paintSpread().setSpreadSpeed(2.0f);
				g_Shaders.paintSpread().startAnimation();
				playBoxSound(*merged);
				const ColorTier tier = getColorTier(*merged);
				if (tier == ColorTier::Secondary) score_ += 50;
				else if (tier == ColorTier::Tertiary) score_ += 100;

				if (isIce(next)) {
					if (ColorBox* nb = getBoxAt(next)) slideBoxOnIce(nb, dir);
				}

				movePlayerTo(newPos);
				moves_ += 1;
				collectItem(newPos);

				if (tacoDirection_ != newDir || isFacingLeft_ != newFacingLeft) {
					tacoDirection_ = newDir; isFacingLeft_ = newFacingLeft;
					tacoAnimFrame_ = 0; tacoAnimTimer_ = 0.0;
				}
				if (isIce(newPos)) { isSliding_ = true; slideDir_ = dir; }
				return;
			}
		}

		// 일반 밀기
		pushBox(box, dir);
		slideBoxOnIce(box, dir);
		movePlayerTo(newPos);
		moves_ += 1;
		collectItem(newPos);


		if (tacoDirection_ != newDir || isFacingLeft_ != newFacingLeft) {
			tacoDirection_ = newDir; isFacingLeft_ = newFacingLeft;
			tacoAnimFrame_ = 0; tacoAnimTimer_ = 0.0;
		}
		if (isIce(newPos)) { isSliding_ = true; slideDir_ = dir; }
		return;
	}

	// 아이템/일반 이동
	if (playerHeldItem_ != ItemType::None) {
		if (tryChangeBoxColor(newPos, dir)) { playerHeldItem_ = ItemType::None; return; }
	}
	if (canMoveTo(newPos) || getBoxAt(newPos)) {
		movePlayerTo(newPos);
		inputCooldown_ = moveDelay_;
		moves_ += 1;
		collectItem(newPos);
		if (tacoDirection_ != newDir || isFacingLeft_ != newFacingLeft) {
			tacoDirection_ = newDir; isFacingLeft_ = newFacingLeft;
			tacoAnimFrame_ = 0; tacoAnimTimer_ = 0.0;
		}
		if (isIce(newPos)) { isSliding_ = true; slideDir_ = dir; }
	}
}


bool InGameScene::willCreateBombOnPush(Point boxPos, Point dir) const
{
	const ColorBox* cur = getBoxAt(boxPos);
	const ColorBox* tgt = getBoxAt(boxPos + dir);
	if (!cur || !tgt) return false;

	const BoxColor c0 = getEffectiveBoxColor(cur->uid);
	const BoxColor c1 = getEffectiveBoxColor(tgt->uid);
	const auto merged = getMergedColor(c0, c1);
	return (merged && *merged == BoxColor::Black);
}

void InGameScene::ensureUniqueBoxUIDs() {
	HashSet<uint64> used;
	for (auto& b : boxes_) {
		if (b.uid == 0 || used.contains(b.uid)) {
			b.uid = nextBoxUID_++;
		}
		used.insert(b.uid);
	}
}

void InGameScene::startIceSlideTask_(ColorBox* box, Point dir)
{
	if (!box) return;
	for (auto& t : iceSlideTasks_) {
		if (t.active && t.uid == box->uid) {
			t.dir = dir;
			t.cooldown = 0.0;
			return;
		}
	}
	IceSlideTask t;
	t.uid = box->uid;
	t.dir = dir;
	t.cooldown = 0.0;
	t.active = true;
	iceSlideTasks_ << t;
}

bool InGameScene::canSlideNext_(Point tile, Point dir) const
{
	const Point nxt = tile + dir;
	if (!isInsideMap(nxt)) return false;
	if (mapData_[nxt.y][nxt.x] == TileType::Wall) return false;
	if (getBoxAt(nxt) != nullptr) return false;
	return true;
}

bool InGameScene::isIce(Point pos) const
{
	if (isBlocked(pos)) return false;
    return (mapData_[pos.y][pos.x] == TileType::Ice);
}

void InGameScene::continueSliding()
{
	if (isPlayerMoving_) return;
	if (!isSliding_) return;

	// ★ 용암 체크
	if (isInsideMap(playerPos_) && mapData_[playerPos_.y][playerPos_.x] == TileType::Lava) {
		if (!isPlayerDead_) {
			isPlayerDead_ = true;
			deathAnimTimer_ = 0.0;
			createDeathEffect(false);

			// ★ 보스 스테이지면 bossBgm 저장, 아니면 bgm 저장
			if (StageData::isFinalStage(currentStage_)) {
				if (!bossBgm_.isEmpty() && bossBgm_.isPlaying()) {
					savedMusicPosition_ = bossBgm_.posSec();
					bossBgm_.stop();
				}
			}
			else {
				if (!bgm_.isEmpty() && bgm_.isPlaying()) {
					savedMusicPosition_ = bgm_.posSec();
					bgm_.stop();
				}
			}
		}
		return;
	}

	if (!isIce(playerPos_)) {
		isSliding_ = false;
		return;
	}

	const Point next = playerPos_ + slideDir_;

	if (!isInsideMap(next) || mapData_[next.y][next.x] == TileType::Wall) {
		isSliding_ = false;
		return;
	}

	if (!isIce(next)) {
		if (ColorBox* box = getBoxAt(next)) {
			isSliding_ = false;
			return;
		}

		movePlayerTo(next);
		moves_ += 1;
		collectItem(next);
		isSliding_ = false;
		return;
	}

	if (ColorBox* box = getBoxAt(next)) {
		// 밀 수 있는지 확인
		if (!canPushBox(playerPos_, box->pos, slideDir_)) {
			isSliding_ = false;
			return;
		}

		const Point boxNextPos = box->pos + slideDir_;

		if (ColorBox* targetBox = getBoxAt(boxNextPos)) {
			const BoxColor c0 = getEffectiveBoxColor(box->uid);
			const BoxColor c1 = getEffectiveBoxColor(targetBox->uid);

			if (auto merged = getMergedColor(c0, c1)) {
				const bool createsBomb = (*merged == BoxColor::Black);

				forceMergePaintFXCompletion();

				const uint64 uidA = box->uid;
				const uint64 uidB = targetBox->uid;
				boxes_.remove_if([uidA, uidB](const auto& b) {
					return b.uid == uidA || b.uid == uidB;
				});

				ColorBox newBox(boxNextPos, *merged, 0.0, nextBoxUID_++);

				if (createsBomb) {
					newBox.creationTime = gameTime_;
					bombExpiryAbs_[newBox.uid] = bombClock_ + kTotal;
					triggerBombBoxFXForBlack_Multi(newBox.uid, BLACK_BOX_LIFETIME);

					if (!bombExplosionSound_.isEmpty()) {
						bombExplosionSound_.stop();
						bombExplosionSound_.setVolume(0.6);
						bombExplosionSound_.play();
					}
				}

				boxes_.push_back(newBox);
				triggerMergePaintFX_Directional(boxNextPos, getBoxColorF(c1), getBoxColorF(*merged), slideDir_);
				playBoxSound(*merged);

				const ColorTier tier = getColorTier(*merged);
				if (tier == ColorTier::Secondary) {
					score_ += 50;
				}
				else if (tier == ColorTier::Tertiary) {
					score_ += 100;
				}

				// 플레이어 이동
				movePlayerTo(next);
				moves_ += 1;
				collectItem(next);

				if (isIce(boxNextPos)) {
					if (ColorBox* newBoxPtr = getBoxAt(boxNextPos)) {
						slideBoxOnIce(newBoxPtr, slideDir_);
					}
				}
				if (!isIce(next)) {
					isSliding_ = false;
				}
				return;
			}
		}

		pushBox(box, slideDir_);
		slideBoxOnIce(box, slideDir_);
		movePlayerTo(next);
		moves_ += 1;
		collectItem(next);

		if (!isIce(next)) {
			isSliding_ = false;
		}
		return;
	}


	movePlayerTo(next);
	moves_ += 1;
	collectItem(next);

	if (!isIce(next)) {
		isSliding_ = false;
	}
}

uint8 InGameScene::encodeTile_(InGameScene::TileType t) noexcept {
	return static_cast<s3d::uint8>(t);
}

InGameScene::TileType InGameScene::decodeTile_(s3d::uint8 c) noexcept {
	using T = TileType;
	
	constexpr s3d::uint8 kMax =
		static_cast<s3d::uint8>(T::VioletItem); // 필요 시 실제 마지막 값으로 갱신
	if (c <= kMax) {
		return static_cast<T>(c);
	}
	return T::Empty; // 범위를 벗어나면 안전하게 Empty로
}

Array<Array<s3d::uint8>> InGameScene::exportMapCodes_() const {
	Array<Array<s3d::uint8>> out(mapData_.size());
	for (size_t y = 0; y < mapData_.size(); ++y) {
		out[y].resize(mapData_[y].size());
		for (size_t x = 0; x < mapData_[y].size(); ++x) {
			out[y][x] = encodeTile_(mapData_[y][x]);
		}
	}
	return out;
}

void InGameScene::importMapCodes_(const Array<Array<s3d::uint8>>& codes) {
	mapData_.resize(codes.size());
	for (size_t y = 0; y < codes.size(); ++y) {
		mapData_[y].resize(codes[y].size());
		for (size_t x = 0; x < codes[y].size(); ++x) {
			mapData_[y][x] = decodeTile_(codes[y][x]);
		}
	}
}

void InGameScene::updatePlayer()
{
    // 플레이어 부드러운 이동 처리
    if (isPlayerMoving_)
    {
        const double deltaTime = Scene::DeltaTime();
        Vec2 direction = targetPixelPos_ - playerPixelPos_;
        double distance = direction.length();
        
        if (distance <= playerMoveSpeed_ * deltaTime)
        {
            // 목표 지점에 도달
            playerPixelPos_ = targetPixelPos_;
            isPlayerMoving_ = false;
			inputCooldown_ = 0.0;
        }
        else
        {
            // 목표 지점으로 이동
            Vec2 normalizedDirection = direction.normalized();
            playerPixelPos_ += normalizedDirection * playerMoveSpeed_ * deltaTime;
        }
    }
}

void InGameScene::updateAnimations()
{
    const double deltaTime = Scene::DeltaTime();
    tacoAnimTimer_ += deltaTime;
    if (tacoAnimTimer_ >= 0.4)
    {
        tacoAnimTimer_ -= 0.4;
        tacoAnimFrame_ = (tacoAnimFrame_ + 1) % 2;
        // 보스 애니메이션도 Taco와 동일 속도로 진행
        if (!bossIdleFrames_.isEmpty())
        {
            bossAnimFrame_ = (bossAnimFrame_ + 1) % static_cast<int32>(bossIdleFrames_.size());
        }
    }
}
void InGameScene::draw()
{
	applyFixedCameraFitToMap();
	drawBackground();

	if (StageData::isFinalStage(currentStage_))
	{
		if (!bossIdleFrames_.isEmpty())
		{
			const int32 screenW = Scene::Width();
			const int32 screenH = Scene::Height();
			const int32 topH = screenH / 2;

			const Texture& bossTex = bossIdleFrames_[bossAnimFrame_ % static_cast<int32>(bossIdleFrames_.size())];
			const Size bossSize = bossTex.size();

			if (bossSize.x > 0 && bossSize.y > 0)
			{
				const double sx = static_cast<double>(screenW) / bossSize.x;
				const double sy = static_cast<double>(topH) / bossSize.y;
				const double s = Min(sx, sy) * 0.9;

				const int32 drawW = static_cast<int32>(bossSize.x * s);
				const int32 drawH = static_cast<int32>(bossSize.y * s);
				const int32 x = (screenW - drawW) / 2;
				const int32 y = (topH - drawH) / 2;

				bossTex.resized(drawW, drawH).draw(x, y);
			}
		}
	}



	// 카메라 변환 시작 (월드 렌더링 + 인게임 UI)
	{
		const auto _t = camera().createTransformer();

		drawMap();
		drawPlayer();

		if (isBossAttackSequenceActive_ &&
			currentBossAttackPhase_ != BossAttackPhase::LaunchTowardsBoss &&
			currentBossAttackPhase_ != BossAttackPhase::BossHit)
		{
			drawBossAttackSequence();
		}

	}

	drawBombBoxFX_Multi();
	drawWallBreakFX();
	drawUI();
	drawBossProjectiles();
	drawBossExplosions();
	drawEnergyBalls();
	drawHomingBullets();

	if (isBossAttackSequenceActive_ &&
	(currentBossAttackPhase_ == BossAttackPhase::LaunchTowardsBoss ||
		currentBossAttackPhase_ == BossAttackPhase::BossHit))
	{
		drawBossAttackSequence();
	}

	// Stage 6: 플레이어 주변 80px만 보이도록 화면 어둡게 처리
	if (currentStage_ == 5)
	{
		const Mat3x2 transform = camera().getMat3x2();
		const Vec2 screenPos = transform.transformPoint(playerPixelPos_);
		const double radius = 100.0;
		const double w = static_cast<double>(Scene::Width());
		const double h = static_cast<double>(Scene::Height());
		const double outer = std::sqrt(w * w + h * h);

		Circle spotlight(screenPos, radius);
		spotlight.drawFrame(0.0, Max(0.0, outer - radius), ColorF{ 0, 0, 0, 1.0 });
	}

	// 조작법 도움말 표시
	if (showHelpScreen_)
	{
		drawHelpScreen();
		return;
	}

	// 클리어 화면
	if (isCleared_ && showClearButtons_)
	{
		Rect{ 0, 0, Scene::Size().x, Scene::Size().y }.draw(ColorF{ 0, 0, 0, 0.8 });

		clearFont_(U"ステージクリア")
			.drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 200, ColorF{ 1.0, 1.0, 0.5 });

		gameFont_(U"手数: {}"_fmt(moves_))
			.drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 + 100, ColorF{ 1.0, 1.0, 1.0 });

		int32 starCount = calculateStars(moves_);
		drawStars(starCount, Vec2(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 10));

		String ratingText;
		ColorF ratingColor;
		if (starCount == 3)
		{
			ratingText = U"完璧!";
			ratingColor = ColorF{ 1.0, 1.0, 0.3 };
		}
		else if (starCount == 2)
		{
			ratingText = U"良い!";
			ratingColor = ColorF{ 0.9, 0.9, 0.9 };
		}
		else
		{
			ratingText = U"OK!";
			ratingColor = ColorF{ 0.8, 0.5, 0.3 };
		}
		gameFont_(ratingText).drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 120, ratingColor);
		gameFont_(U"スペースかエンターを押す").drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 + 150, ColorF{ 0.8, 0.8, 0.8 });
	}

	if (isPlayerDead_ && deathAnimTimer_ >= 1.5)
	{
		double alpha = Sin((deathAnimTimer_ - 1.5) * Math::TwoPi) * 0.5 + 0.5;
		clearFont_(U"R?").drawAt(
			Scene::Size().x / 2.0,
			Scene::Size().y / 2.0,
			ColorF{ 1.0, 1.0, 1.0, alpha }
		);
	}
	
	// 클리어 이펙트 그리기
	if (showClearEffect_)
	{
		drawClearEffect();
	}
	if (isFading_)
	{
		const double alpha = 1.0 - (fadeTimer_ / fadeDuration_);
		Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, alpha });
	}
}

void InGameScene::drawBackground()
{
	if (!stageBackground_.isEmpty())
	{
		stageBackground_.resized(Scene::Size()).draw(0, 0);
	}
}

void InGameScene::drawMap()
{
	// 타일 배경
	for (int32 y = 0; y < getMapHeight(); ++y) {
		for (int32 x = 0; x < getMapWidth(); ++x) {
			const Rect tileRect{ x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };

			switch (mapData_[y][x]) {
			case TileType::Empty:
				tileRect.draw(ColorF{ 0.15, 0.15, 0.2, 0.3 });
				break;
			case TileType::Wall:
				tileRect.draw(ColorF{ 0.3, 0.3, 0.35 });
				tileRect.drawFrame(2, 0, ColorF{ 0.5, 0.5, 0.55 });
				break;
			case TileType::Ice:
				// 얼음 타일: 연한 하늘색
				tileRect.draw(ColorF{ 0.615, 0.988, 0.976, 0.6 });
				tileRect.drawFrame(2, 0, ColorF{ 0.8, 1.0, 1.0, 0.8 });
				break;
			case TileType::Lava:
			{
				double pulse = 0.5 + 0.5 * sin(gameTime_ * 3.0);
				ColorF lavaColor{ 0.9, 0.2 + 0.2 * pulse, 0.0, 0.8 };
				tileRect.draw(lavaColor);
				tileRect.drawFrame(2, 0, ColorF{ 1.0, 0.5, 0.0 });
			}
			break;  // ★ break 추가!
			//case TileType::LavaWarning:  // ★ 새로운 케이스 추가
			//{
			//	// 투명한 배경
			//	tileRect.draw(ColorF{ 0.9, 0.2, 0.2, 0.12 });

			//	// 빠르게 깜빡이는 테두리
			//	const double flash = 0.4 + 0.6 * Math::Abs(Math::Sin(gameTime_ * 8.0));
			//	tileRect.drawFrame(4, 0, ColorF{ 1.0, 0.0, 0.0, flash });
			//}
			//break;
			case TileType::RedGoal:
				tileRect.draw(ColorF{ 0.9, 0.2, 0.2, 0.3 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 0.9, 0.2, 0.2 });
				break;
			case TileType::YellowGoal:
				tileRect.draw(ColorF{ 1.0, 0.9, 0.2, 0.3 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 1.0, 0.9, 0.2 });
				break;
			case TileType::BlueGoal:
				tileRect.draw(ColorF{ 0.2, 0.4, 0.9, 0.3 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 0.2, 0.4, 0.9 });
				break;
			case TileType::OrangeGoal:
				tileRect.draw(ColorF{ 1.0, 0.5, 0.0, 0.3 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 1.0, 0.5, 0.0 });
				break;
			case TileType::GreenGoal:
				tileRect.draw(ColorF{ 0.2, 0.8, 0.3, 0.3 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 0.2, 0.8, 0.3 });
				break;
			case TileType::VioletGoal:
				tileRect.draw(ColorF{ 0.7, 0.2, 0.8, 0.3 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 0.7, 0.2, 0.8 });
				break;
			case TileType::BlackGoal:
				tileRect.draw(ColorF{ 0.2, 0.2, 0.2, 0.5 });
				Circle{ tileRect.center(), 12 }.drawFrame(3, ColorF{ 0.4, 0.4, 0.4 });
				break;
			case TileType::RedItem:
			case TileType::OrangeItem:
			case TileType::YellowItem:
			case TileType::GreenItem:
			case TileType::BlueItem:
			case TileType::VioletItem:
				tileRect.draw(ColorF{ 0.15, 0.15, 0.2, 0.3 });
				break;
			default:
				tileRect.draw(ColorF{ 0.15, 0.15, 0.2, 0.3 });
				break;
			}

			tileRect.drawFrame(1, 0, ColorF{ 0.2, 0.2, 0.25, 0.5 });
		}
	}

	// 상자
	drawBoxes_RespectBombFX();

	// 아이템
	for (const auto& item : items_) {
		const Rect r{ item.pos.x * TILE_SIZE, item.pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
		const ColorF ic = getItemColorF(item.type);

		const Vec2 center = r.center();
		const Vec2 top = center + Vec2(0, -20);
		const Vec2 right = center + Vec2(20, 0);
		const Vec2 bottom = center + Vec2(0, 20);
		const Vec2 left = center + Vec2(-20, 0);

		Quad(top, right, bottom, left).draw(ic);
		Quad(top, right, bottom, left).drawFrame(2, ColorF{ ic.r * 1.3, ic.g * 1.3, ic.b * 1.3 });
	}

	//drawBossAttacks();
	drawWallMask();
	drawGoalMarkersTop();
	drawOverlayWarnings();
}



void InGameScene::drawPlayer()
{
	/// 죽음 애니메이션
	if (isPlayerDead_) {
		drawDeathEffect();
		return;
	}
    if (isPlayingPaintAnimation_)
    {
        drawPaintAnimation();
        return;
    }
    
    // 실제 픽셀 위치를 사용하여 플레이어 렌더링
    const Rect playerRect{ 
        static_cast<int32>(playerPixelPos_.x - TILE_SIZE / 2), 
        static_cast<int32>(playerPixelPos_.y - TILE_SIZE / 2), 
        TILE_SIZE, 
        TILE_SIZE 
    };
    
    const Array<Texture>* currentFrames = nullptr;
    
    switch (tacoDirection_)
    {
    case TacoDirection::Down:
        if (tacoDownFrames_.size() > 0)
            currentFrames = &tacoDownFrames_;
        break;
    case TacoDirection::Side:
        if (tacoSideFrames_.size() > 0)
            currentFrames = &tacoSideFrames_;
        break;
    case TacoDirection::Up:
        if (tacoUpFrames_.size() > 0)
            currentFrames = &tacoUpFrames_;
        break;
    }
    
    if (currentFrames && tacoAnimFrame_ < currentFrames->size())
    {
        const Texture& frame = (*currentFrames)[tacoAnimFrame_];
        if (!frame.isEmpty())
        {
			const auto scope = g_Shaders.holographic().scopedTexture(frame);

            if (tacoDirection_ == TacoDirection::Side && isFacingLeft_)
            {
                frame.resized(TILE_SIZE, TILE_SIZE).mirrored().draw(playerRect.pos);
            }
            else
            {
                frame.resized(TILE_SIZE, TILE_SIZE).draw(playerRect.pos);
            }
            return;
        }
    }
    
    Circle{ playerPixelPos_, TILE_SIZE / 2 - 8 }.draw(playerColor_);
    Circle{ playerPixelPos_, TILE_SIZE / 2 - 8 }.drawFrame(3, 0, ColorF{ 1.0, 1.0, 1.0, 0.9 });
    Circle{ playerPixelPos_, 8 }.draw(ColorF{ 1.0, 1.0, 1.0, 0.8 });
}

void InGameScene::drawUI()
{
    Rect{ 0, 0, 150, 180 }.draw(ColorF{ 0, 0, 0, 0.5 });
    gameFont_(U"ステージ: {}"_fmt(currentStage_)).draw(Vec2{ 16, 16 }, ColorF{ 0.8, 0.8, 1.0 });
    gameFont_(U"手数: {}"_fmt(moves_)).draw(Vec2{ 16, 45 }, ColorF{ 1.0, 1.0, 0.5 });
    gameFont_(U"時間: {:.1f}s"_fmt(gameTime_)).draw(Vec2{ 16, 74 }, ColorF{ 0.5, 1.0, 0.5 });
    
    // 목표 진행도 (모든 색상)
    int32 totalGoals = 0;
    int32 cleared = 0;
    
    const Array<BoxColor> allColors = {
        BoxColor::Red, BoxColor::Yellow, BoxColor::Blue,
        BoxColor::Orange, BoxColor::Green, BoxColor::Violet, BoxColor::Black
    };
    
    for (const auto& color : allColors)
    {
        const Array<Point>& goals = getGoalPositionsForColor(color);
        totalGoals += goals.size();
        
        for (const auto& goal : goals)
        {
            const ColorBox* box = getBoxAt(goal);
            if (box && box->color == color)
                cleared++;
        }
    }
    
    gameFont_(U"目標: {}/{}"_fmt(cleared, totalGoals))
        .draw(Vec2{ 16, 103 }, ColorF{ 0.9, 0.9, 0.9 });
    
    // 현재 가진 아이템 표시
    if (playerHeldItem_ != ItemType::None)
    {
        gameFont_(U"アイテム: 持っている").draw(Vec2{ 16, 132 }, getItemColorF(playerHeldItem_));
    }
    else
    {
        gameFont_(U"アイテム: なし").draw(Vec2{ 16, 132 }, ColorF{ 0.6, 0.6, 0.6 });
    }
}

void InGameScene::drawHelpScreen()
{
    // 반투명 배경 오버레이
    Rect{ 0, 0, Scene::Size().x, Scene::Size().y }.draw(ColorF{ 0, 0, 0, 0.8 });
    
    // 제목 (일본어)
    clearFont_(U"中止").drawAt(Scene::Size().x / 2.0, 100, ColorF{ 1.0, 1.0, 0.5 });
    
    // 조작법 설명 (일본어로 변경)
    const double centerX = Scene::Size().x / 2.0;
    const double startY = 200;
    const double lineHeight = 50;
    
    Array<String> helpTexts = {
        U"↑ ↓ ← → : 移動 / ブロックを押す",
        U"R : ステージをやり直す",
        U"Space / Enter : 次のステージへ (クリア時)",
        U"ESC : 再開 / このメニューを表示"
    };
    
    for (size_t i = 0; i < helpTexts.size(); i++)
    {
        ColorF textColor = ColorF{ 0.9, 0.9, 0.9 };
        debugFont_(helpTexts[i]).drawAt(centerX, startY + i * lineHeight, textColor);
    }
    
    // 스테이지 선택으로 돌아가는 버튼
    const double buttonY = startY + helpTexts.size() * lineHeight + 60;
    const Rect backButton{ static_cast<int32>(centerX - 100), static_cast<int32>(buttonY), 200, 50 };
    
    // 버튼 배경
    ColorF buttonColor = backButton.mouseOver() ? ColorF{ 0.3, 0.5, 0.7 } : ColorF{ 0.2, 0.4, 0.6 };
    backButton.draw(buttonColor);
    backButton.drawFrame(3, Palette::White);
    
    // 버튼 텍스트 (일본어)
    ColorF textColor = backButton.mouseOver() ? Palette::Yellow : Palette::White;
    buttonFont_(U"ステージ選択に戻る").drawAt(backButton.center(), textColor);
    
    // 버튼 클릭 처리
    if (backButton.leftClicked())
    {
        changeScene(SceneType::StageSelect);
    }
    
    // 포커스 상태에 따라 다른 메시지 표시 (일본어)
    if (!Window::GetState().focused)
    {
        debugFont_(U"クリックして再開").drawAt(centerX, Scene::Size().y - 100, ColorF{ 1.0, 1.0, 0.5 });
    }
    else
    {
        debugFont_(U"ESCで再開").drawAt(centerX, Scene::Size().y - 100, ColorF{ 1.0, 1.0, 0.5 });
    }
}

Array<Point>& InGameScene::getGoalPositionsForColor(BoxColor color)
{
    switch (color)
    {
    case BoxColor::Red:
        return redGoalPositions_;
    case BoxColor::Yellow:
        return yellowGoalPositions_;
    case BoxColor::Blue:
        return blueGoalPositions_;
    case BoxColor::Orange:
        return orangeGoalPositions_;
    case BoxColor::Green:
        return greenGoalPositions_;
    case BoxColor::Violet:
        return violetGoalPositions_;
    case BoxColor::Black:
        return blackGoalPositions_;
    default:
        return blackGoalPositions_;  // fallback
    }
}

const Array<Point>& InGameScene::getGoalPositionsForColor(BoxColor color) const
{
    switch (color)
    {
    case BoxColor::Red:
        return redGoalPositions_;
    case BoxColor::Yellow:
        return yellowGoalPositions_;
    case BoxColor::Blue:
        return blueGoalPositions_;
    case BoxColor::Orange:
        return orangeGoalPositions_;
    case BoxColor::Green:
        return greenGoalPositions_;
    case BoxColor::Violet:
        return violetGoalPositions_;
    case BoxColor::Black:
        return blackGoalPositions_;
    default:
        return blackGoalPositions_;  // fallback
    }
}

void InGameScene::updateBlackBoxes()
{
	//// 처리할 UID를 임시로 저장할 배열
	//Array<uint64> toTrigger;

	//// 모든 상자를 순회하며 검은 상자를 찾습니다.
	//for (const auto& box : boxes_)
	//{
	//	if (box.color != BoxColor::Black) continue;

	//	// 이미 이 상자에 대한 폭발 효과가 진행 중인지 확인합니다.
	//	// (중복 트리거 방지)
	//	bool hasActiveFX = false;
	//	for (const auto& fx : bombFXs_) {
	//		if (fx.uid == box.uid) {
	//			hasActiveFX = true;
	//			break;
	//		}
	//	}
	//	if (hasActiveFX) continue;

	//	// 생성된 후 3초가 지났는지 확인합니다.
	//	const double elapsed = gameTime_ - box.creationTime;
	//	if (elapsed >= 3.0) {
	//		// 3초가 지났다면, 효과를 발동시킬 목록에 추가합니다.
	//		toTrigger.push_back(box.uid);
	//	}
	//}

	//// 목록에 있는 모든 UID에 대해 폭발 효과를 트리거합니다.
	//for (const uint64 uid : toTrigger) {
	//	triggerBombBoxFXForBlack_Multi(uid, BLACK_BOX_LIFETIME);
	//}
}

//void InGameScene::updateBossAttack(double dt)
//{
//	if (!StageData::isFinalStage(currentStage_)) return;
//
//	bossAttackTimer_ += dt;
//
//	// 3초마다 공 발사
//	if (bossAttackTimer_ >= BOSS_ATTACK_INTERVAL)
//	{
//		bossAttackTimer_ = 0.0;
//		spawnBossProjectile();
//	}
//}

void InGameScene::spawnBossProjectile()
{
	// 보스 이미지 중앙에서 발사
	const int32 screenW = Scene::Width();
	const int32 screenH = Scene::Height();
	const Vec2 bossPos = Vec2(screenW / 2.0, screenH / 4.0);  // 상단 중앙

	// 플레이어 픽셀 위치로 변환 (카메라 변환 적용)
	const Mat3x2 transform = camera().getMat3x2();
	const Vec2 playerScreenPos = transform.transformPoint(playerPixelPos_);

	// 방향 벡터 계산
	Vec2 direction = (playerScreenPos - bossPos).normalized();
	double speed = 400.0;  // 공 속도

	BossProjectile proj;
	proj.pos = bossPos;
	proj.velocity = direction * speed;
	proj.color = ColorF{ 0.8, 0.2, 0.9, 1.0 };  // 보라색
	proj.size = 40.0;  // ⬅️ 20.0 → 40.0으로 변경 (2배)
	proj.active = true;
	proj.exploded = false;

	bossProjectiles_.push_back(proj);
}

void InGameScene::updateBossProjectiles(double dt)
{
	for (auto& proj : bossProjectiles_)
	{
		if (!proj.active) continue;

		proj.pos += proj.velocity * dt;

		const Mat3x2 transform = camera().getMat3x2();
		const Vec2 playerScreenPos = transform.transformPoint(playerPixelPos_);

		const double dist = (proj.pos - playerScreenPos).length();

		if (dist < 40.0)  // 충돌 반경
		{
			// 폭발 생성
			createBossExplosion(proj.pos, proj.color);
			proj.active = false;
			proj.exploded = true;

			// ★ 플레이어 사망 처리 (체력 깎임과 동시에 죽음)
			if (!isPlayerDead_)
			{
				isPlayerDead_ = true;
				deathAnimTimer_ = 0.0;
				createDeathEffect(false);

				// 배경음악 정지
				if (!bossBgm_.isEmpty() && bossBgm_.isPlaying())
				{
					savedMusicPosition_ = bossBgm_.posSec();
					bossBgm_.stop();
				}
			}
		}

		// 화면 밖으로 나가면 제거
		if (proj.pos.x < -100 || proj.pos.x > Scene::Width() + 100 ||
			proj.pos.y < -100 || proj.pos.y > Scene::Height() + 100)
		{
			proj.active = false;
		}
	}

	bossProjectiles_.remove_if([](const BossProjectile& p) { return !p.active; });
}

void InGameScene::createBossExplosion(Vec2 pos, ColorF color)
{
	// 6방향으로 파티클 발사 (육각형 패턴, 중력 없음)
	const Array<double> angles = {
		0.0,                    // 우
		Math::Pi / 3.0,         // 우상
		2.0 * Math::Pi / 3.0,   // 좌상
		Math::Pi,               // 좌
		4.0 * Math::Pi / 3.0,   // 좌하
		5.0 * Math::Pi / 3.0    // 우하
	};

	const Array<ColorF> explosionColors = {
		ColorF{1.0, 0.3, 0.9},  // 보라
		ColorF{1.0, 0.5, 0.9},  // 밝은 보라
		ColorF{0.8, 0.2, 1.0},  // 진보라
		ColorF{1.0, 0.7, 0.9},  // 연보라
	};

	for (const auto& angle : angles)
	{
		BossExplosionParticle particle;

		const double speed = Random(150.0, 250.0);
		particle.pos = pos;
		particle.velocity = Vec2(Math::Cos(angle) * speed, Math::Sin(angle) * speed);
		particle.color = explosionColors[Random(0, (int32)explosionColors.size() - 1)];
		particle.life = particle.maxLife = Random(0.8, 1.5);
		particle.size = Random(8.0, 15.0);

		bossExplosionParticles_.push_back(particle);
	}
}

void InGameScene::updateBossExplosions(double dt)
{
	for (auto& particle : bossExplosionParticles_)
	{
		particle.pos += particle.velocity * dt;
		particle.life -= dt;

		// 중력 없음 (등속 직선 운동)
		// particle.velocity *= 0.98;  // 약간의 감속만 (선택사항)

		// 알파값 조정
		double alpha = particle.life / particle.maxLife;
		particle.color.a = alpha;
	}

	// 수명이 다한 파티클 제거
	bossExplosionParticles_.remove_if([](const BossExplosionParticle& p) { return p.life <= 0; });
}

void InGameScene::drawBossProjectiles()
{
	ScopedRenderStates2D blend{ BlendState::Additive };

	for (const auto& proj : bossProjectiles_)
	{
		if (!proj.active) continue;

		// 공 그리기 (빛나는 효과)
		Circle{ proj.pos, proj.size }.draw(proj.color);
		Circle{ proj.pos, proj.size * 0.6 }.draw(ColorF{ 1.0, 1.0, 1.0, 0.8 });
	}
}

void InGameScene::drawBossExplosions()
{
	ScopedRenderStates2D blend{ BlendState::Additive };

	for (const auto& particle : bossExplosionParticles_)
	{
		Circle{ particle.pos, particle.size }.draw(particle.color);
	}
}

Vec2 InGameScene::tileToPixel(Point tilePos) const
{
    return Vec2(
        tilePos.x * TILE_SIZE + TILE_SIZE / 2.0,
        tilePos.y * TILE_SIZE + TILE_SIZE / 2.0
    );
}

void InGameScene::movePlayerTo(Point newTilePos)
{
    playerPos_ = newTilePos;
    targetPixelPos_ = tileToPixel(newTilePos);
    isPlayerMoving_ = true;
}

BoxColor InGameScene::itemTypeToBoxColor(ItemType item) const
{
    switch (item)
    {
    case ItemType::RedItem:
        return BoxColor::Red;
    case ItemType::OrangeItem:
        return BoxColor::Orange;
    case ItemType::YellowItem:
        return BoxColor::Yellow;
    case ItemType::GreenItem:
        return BoxColor::Green;
    case ItemType::BlueItem:
        return BoxColor::Blue;
    case ItemType::VioletItem:
        return BoxColor::Violet;
    default:
        return BoxColor::Red;  // fallback
    }
}

ItemType InGameScene::tileTypeToItemType(TileType tile) const
{
    switch (tile)
    {
    case TileType::RedItem:
        return ItemType::RedItem;
    case TileType::OrangeItem:
        return ItemType::OrangeItem;
    case TileType::YellowItem:
        return ItemType::YellowItem;
    case TileType::GreenItem:
        return ItemType::GreenItem;
    case TileType::BlueItem:
        return ItemType::BlueItem;
    case TileType::VioletItem:
        return ItemType::VioletItem;
    default:
        return ItemType::None;
    }
}

ColorF InGameScene::getItemColorF(ItemType item) const
{
    switch (item)
    {
    case ItemType::RedItem:
        return ColorF{ 0.6, 0.3, 0.3 };      // 빨강 (채도 낮춤)
    case ItemType::OrangeItem:
        return ColorF{ 1.0, 0.5, 0.0 };      // 주황 (원래값)
    case ItemType::YellowItem:
        return ColorF{ 1.0, 1.0, 0.5 };      // 노랑 (채도 절반)
    case ItemType::GreenItem:
        return ColorF{ 0.2, 0.8, 0.3 };      // 초록 (원래값)
    case ItemType::BlueItem:
        return ColorF{ 0.2, 0.4, 0.9 };      // 파랑 (원래값)
    case ItemType::VioletItem:
        return ColorF{ 0.7, 0.2, 0.8 };      // 보라 (원래값)
    default:
        return ColorF{ 0.1, 0.1, 0.1 };
    }
}

GameItem* InGameScene::getItemAt(Point pos)
{
    for (auto& item : items_)
    {
        if (item.pos == pos)
            return &item;
    }
    return nullptr;
}

const GameItem* InGameScene::getItemAt(Point pos) const
{
    for (const auto& item : items_)
    {
        if (item.pos == pos)
            return &item;
    }
    return nullptr;
}

void InGameScene::collectItem(Point pos)
{
    GameItem* item = getItemAt(pos);
    if (item && playerHeldItem_ == ItemType::None)
    {
        playerHeldItem_ = item->type;
        
        // 아이템을 맵에서 제거
        items_.remove_if([pos](const GameItem& i) {
            return i.pos == pos;
        });
        
        // 타일을 빈 공간으로 변경
        mapData_[pos.y][pos.x] = TileType::Empty;
        
        score_ += 25;  // 아이템 수집 보너스
    }
}

bool InGameScene::tryChangeBoxColor(Point pos, Point direction)
{
    ColorBox* box = getBoxAt(pos);
    if (!box || playerHeldItem_ == ItemType::None)
        return false;
    
	const BoxColor prevColor = box->color;
	const BoxColor newColor = itemTypeToBoxColor(playerHeldItem_);

    // 이미 같은 색상이면 변경하지 않음
    if (box->color == newColor)
        return false;
    
    // 박스 색상 변경
    box->color = newColor;
    score_ += 100;

	mergeFX_.active = true;
	mergeFX_.baseColor = getBoxColorF(prevColor);
	mergeFX_.paintColor = getBoxColorF(newColor);
	mergeFX_.originUV = impactOriginLocalUVForDir(direction);
	mergeFX_.targetUid = box->uid;
	mergeFX_.finalColor = newColor;
	mergeFX_.commitPending = false;

	g_Shaders.paintSpread().setPaintColor(mergeFX_.paintColor);
	g_Shaders.paintSpread().setOriginPoint(mergeFX_.originUV);
	g_Shaders.paintSpread().setNoiseScale(1.0f);
	g_Shaders.paintSpread().setWaveStrength(0.4f);
	g_Shaders.paintSpread().setSpreadSpeed(0.6f);
	g_Shaders.paintSpread().startAnimation();

    if (direction == Point(0, -1))
    {
        // 위쪽 방향: 애니메이션 재생 안 함
        return true;
    }
    else if (direction == Point(-1, 0))
    {
        // 왼쪽 방향: 좌우반전 애니메이션
        startPaintAnimation(true);
    }
    else
    {
        // 오른쪽 또는 아래: 일반 애니메이션
        startPaintAnimation(false);
    }

	//g_Shaders.holographic().setIntensity(0.0f);
    
    return true;
}

void InGameScene::initializeClearButtons()
{
    const double buttonSize = 70;  // 버튼 크기
    const double centerX = Scene::Size().x / 2.0;
    const double centerY = Scene::Size().y / 2.0 + 120; // 버튼을 더 아래로 이동
    const double buttonMargin = buttonSize / 2.0; // 버튼 크기의 1/2만큼 여백
    const double totalButtonSpacing = buttonSize + buttonMargin; // 버튼 + 여백
    
    // 다시하기 버튼 (왼쪽)
    retryButton_.rect = Rect{
        static_cast<int32>(centerX - totalButtonSpacing - buttonSize / 2),
        static_cast<int32>(centerY - buttonSize / 2),
        static_cast<int32>(buttonSize), static_cast<int32>(buttonSize)
    };
    retryButton_.text = U"やり直す";
    retryButton_.normalColor = ColorF{ 0.6, 0.3, 0.2 };
    retryButton_.hoverColor = ColorF{ 0.7, 0.4, 0.3 };
    
    // 스테이지 선택으로 돌아가기 버튼 (중앙)
    stageSelectButton_.rect = Rect{
        static_cast<int32>(centerX - buttonSize / 2),
        static_cast<int32>(centerY - buttonSize / 2),
        static_cast<int32>(buttonSize), static_cast<int32>(buttonSize)
    };
    stageSelectButton_.text = U"メニュー";
    stageSelectButton_.normalColor = ColorF{ 0.2, 0.4, 0.6 };
    stageSelectButton_.hoverColor = ColorF{ 0.3, 0.5, 0.7 };
    
    // 다음 스테이지 버튼 (오른쪽)
    nextStageButton_.rect = Rect{
        static_cast<int32>(centerX + totalButtonSpacing - buttonSize / 2),
        static_cast<int32>(centerY - buttonSize / 2),
        static_cast<int32>(buttonSize), static_cast<int32>(buttonSize)
    };
    nextStageButton_.text = (currentStage_ < StageData::getTotalStageCount()) ? 
        U"次へ" : U"完了";
    nextStageButton_.normalColor = ColorF{ 0.2, 0.6, 0.3 };
    nextStageButton_.hoverColor = ColorF{ 0.3, 0.7, 0.4 };
}

void InGameScene::updateClearButtons()
{
    updateClearButton(retryButton_);
    updateClearButton(stageSelectButton_);
    updateClearButton(nextStageButton_);
    
    // 버튼 클릭 처리
    if (retryButton_.rect.leftClicked())
    {
        // 현재 스테이지 다시 시작
        isCleared_ = false;
        showClearButtons_ = false;
        gameTime_ = 0.0;
        moves_ = 0;
        score_ = 0;
        loadStage(currentStage_);
		if (StageData::isFinalStage(currentStage_)) {
			const auto initialOverlay = StageData::getFinalStageTileOverlay(0.0);
			if (!initialOverlay.isEmpty()) { applyTileOverlay(initialOverlay, StageData::isFinalStage(currentStage_) ? OverlayApplyMode::OverlayOnly: OverlayApplyMode::WriteToMap); }
			initBossAttacks();
			initBossWallSystem();
		}
    }
    else if (stageSelectButton_.rect.leftClicked())
    {
        // 스테이지 선택으로 돌아가기
        changeScene(SceneType::StageSelect);
    }
    else if (nextStageButton_.rect.leftClicked())
    {
        if (currentStage_ < StageData::getTotalStageCount())
        {
            // 다음 스테이지로
            currentStage_++;
            loadStage(currentStage_);
            isCleared_ = false;
            showClearButtons_ = false;
            gameTime_ = 0.0;
            moves_ = 0;
        }
        else
        {
            // 모든 스테이지 클리어 -> 스테이지 선택으로
            changeScene(SceneType::StageSelect);
        }
    }
}

void InGameScene::updateClearButton(ClearButton& button)
{
    button.isHovered = button.rect.mouseOver();
}

void InGameScene::drawClearButtons()
{
    drawClearButton(retryButton_);
    drawClearButton(stageSelectButton_);
    drawClearButton(nextStageButton_);
}

void InGameScene::drawClearButton(const ClearButton& button)
{
    ColorF currentColor = button.isHovered ? button.hoverColor : button.normalColor;
    button.rect.draw(currentColor);
    button.rect.drawFrame(3, Palette::White);
    
    ColorF textColor = button.isHovered ? button.hoverTextColor : button.textColor;
    buttonFont_(button.text).drawAt(button.rect.center(), textColor);
}

int32 InGameScene::calculateStars(int32 moves) const
{
    if (moves <= 50)
        return 3;  // 3개 별
    else if (moves <= 100)
        return 2;  // 2개 별
    else
        return 1;  // 1개 별
}

void InGameScene::drawStars(int32 starCount, Vec2 centerPos) const
{
    if (tacoScoreTexture_.isEmpty() || tacoScoreOffTexture_.isEmpty())
        return;
    
    const double starSize = 160.0;  // 별 크기 (40 * 4)
    const double spacing = 200.0;   // 별 간격 (50 * 4)
    const int32 maxStars = 3;       // 최대 별 개수
    const double totalWidth = (maxStars - 1) * spacing;
    const Vec2 startPos = centerPos - Vec2(totalWidth / 2.0, 0);
    
    for (int32 i = 0; i < maxStars; i++)
    {
        Vec2 starPos = startPos + Vec2(i * spacing, 0);
        
        if (i < starCount)
        {
            // 획득한 점수: 온 상태 이미지
            tacoScoreTexture_.resized(starSize, starSize).drawAt(starPos);
        }
        else
        {
            // 획득하지 못한 점수: 오프 상태 이미지
            tacoScoreOffTexture_.resized(starSize, starSize).drawAt(starPos);
        }
    }
}
bool InGameScene::isPlayerInExplosionRange(Point bombPos) const
{
	const Array<Point> directions = {
		Point{-1, -1}, Point{0, -1}, Point{1, -1},  // 위쪽 3칸
		Point{-1,  0}, Point{0,  0}, Point{1,  0},  // 좌우 + 중앙
		Point{-1,  1}, Point{0,  1}, Point{1,  1}   // 아래쪽 3칸
	};

	for (const auto& dir : directions) {
		Point checkPos = bombPos + dir;
		if (checkPos == playerPos_) {
			return true;
		}
	}
	return false;
}
void InGameScene::createDeathEffect(bool useWhiteParticles)
{
	deathParticles_.clear();
	isWhiteParticleDeath_ = useWhiteParticles;  // ★ 플래그 저장

	const Vec2 center = playerPixelPos_;

	// 정확히 8방향으로 1개씩 파티클 생성 (총 8개)
	const Array<Vec2> directions = {
		Vec2{-1, -1}.normalized(),  // 좌상 (대각선)
		Vec2{ 0, -1},                // 상
		Vec2{ 1, -1}.normalized(),  // 우상 (대각선)
		Vec2{-1,  0},                // 좌
		Vec2{ 1,  0},                // 우
		Vec2{-1,  1}.normalized(),  // 좌하 (대각선)
		Vec2{ 0,  1},                // 하
		Vec2{ 1,  1}.normalized()   // 우하 (대각선)
	};

	// ★ 폭탄 폭발용 흰색 팔레트와 용암용 색상 팔레트 분리
	const Array<ColorF> whiteColors = {
		ColorF{1.0, 1.0, 1.0},      // 순백
		ColorF{0.95, 0.95, 1.0},    // 약간 푸른 흰색
		ColorF{1.0, 0.95, 0.85},    // 약간 노란 흰색
		ColorF{0.9, 0.9, 0.9},      // 밝은 회색
	};

	const Array<ColorF> lavaColors = {
		ColorF{1.0, 0.3, 0.0},  // 주황
		ColorF{1.0, 0.5, 0.0},  // 밝은 주황
		ColorF{1.0, 0.8, 0.0},  // 노랑
		ColorF{0.9, 0.2, 0.0},  // 진한 빨강
	};

	// ★ 사용할 색상 팔레트 선택
	const Array<ColorF>& colorPalette = useWhiteParticles ? whiteColors : lavaColors;

	// 각 방향마다 정확히 1개의 파티클 생성
	for (const auto& dir : directions)
	{
		DeathParticle particle;

		const double speed = Random(200.0, 350.0);  // 속도 범위

		particle.pos = center;
		particle.velocity = dir * speed;  // 정확한 방향으로 발사

		// ★ 색상 팔레트에서 선택
		particle.color = colorPalette[Random(0, (int32)colorPalette.size() - 1)];
		particle.life = particle.maxLife = Random(0.7, 1.3);  // 수명
		particle.size = Random(6.0, 12.0);  // 크기

		deathParticles_.push_back(particle);
	}
}
void InGameScene::createClearEffect()
{
    clearParticles_.clear();
    
    // 화면 중앙에서 폭죽 효과 생성
    const Vec2 centerPos = Vec2(Scene::Size().x / 2.0, Scene::Size().y / 2.0);
    
    // 여러 개의 폭죽을 다른 위치에서 생성
    Array<Vec2> fireworkPositions = {
        centerPos + Vec2(-200, -100),
        centerPos + Vec2(200, -100),
        centerPos + Vec2(0, -150),
        centerPos + Vec2(-100, 50),
        centerPos + Vec2(100, 50)
    };
    
    for (const auto& pos : fireworkPositions)
    {
        // 각 폭죽당 30~50개의 파티클 생성
        int32 particleCount = Random(30, 50);
        
        for (int32 i = 0; i < particleCount; i++)
        {
            ClearParticle particle;
            
            // 방사형으로 퍼지도록 각도 설정
            double angle = Random(0.0, 2 * Math::Pi);
            double speed = Random(100.0, 400.0);
            
            particle.pos = pos;
            particle.velocity = Vec2(Math::Cos(angle) * speed, Math::Sin(angle) * speed);
            
            // 다채로운 색상 (황금, 빨강, 파랑, 초록, 보라)
            Array<ColorF> colors = {
                ColorF{1.0, 0.8, 0.2},  // 황금
                ColorF{1.0, 0.3, 0.3},  // 빨강
                ColorF{0.3, 0.5, 1.0},  // 파랑
                ColorF{0.3, 1.0, 0.3},  // 초록
                ColorF{0.8, 0.3, 1.0},  // 보라
                ColorF{1.0, 0.5, 0.0},  // 주황
                ColorF{1.0, 1.0, 0.3}   // 노랑
            };
            
            particle.color = colors[Random(0, (int32)colors.size() - 1)];
            particle.life = particle.maxLife = Random(1.5, 3.0);
            particle.size = Random(3.0, 8.0);
            particle.rotation = Random(0.0, 2 * Math::Pi);
            particle.rotationSpeed = Random(-5.0, 5.0);
            
            clearParticles_.push_back(particle);
        }
    }
}
void InGameScene::updateDeathEffect()
{
	const double deltaTime = Scene::DeltaTime();

	for (auto& particle : deathParticles_)
	{
		particle.pos += particle.velocity * deltaTime;
		particle.life -= deltaTime;
		particle.velocity *= 0.95;  // 천천히 감속
		double alpha = particle.life / particle.maxLife;
		particle.color.a = alpha;
	}
	// 수명이 다한 파티클 제거
	deathParticles_.remove_if([](const DeathParticle& p) { return p.life <= 0; });
}
void InGameScene::updateClearEffect()
{
    const double deltaTime = Scene::DeltaTime();
    clearEffectTimer_ += deltaTime;
    
    // 5초 후 이펙트 종료
    if (clearEffectTimer_ > 5.0)
    {
        showClearEffect_ = false;
        clearParticles_.clear();
        return;
    }
    
    // 2초마다 새로운 폭죽 추가
    if (clearEffectTimer_ > 1.0 && (int)(clearEffectTimer_ * 2) % 2 == 0 && clearParticles_.size() < 300)
    {
        // 추가 폭죽 생성
        Vec2 randomPos = Vec2(Random(100, (int)Scene::Size().x - 100), Random(100, 400));
        
        for (int32 i = 0; i < 20; i++)
        {
            ClearParticle particle;
            
            double angle = Random(0.0, 2 * Math::Pi);
            double speed = Random(80.0, 250.0);
            
            particle.pos = randomPos;
            particle.velocity = Vec2(Math::Cos(angle) * speed, Math::Sin(angle) * speed);
            particle.color = HSV{Random(0, 360), Random(0.6, 1.0), 1.0}.toColorF();
            particle.life = particle.maxLife = Random(1.0, 2.5);
            particle.size = Random(2.0, 6.0);
            particle.rotation = Random(0.0, 2 * Math::Pi);
            particle.rotationSpeed = Random(-8.0, 8.0);
            
            clearParticles_.push_back(particle);
        }
    }
    
    // 파티클 업데이트
    for (auto& particle : clearParticles_)
    {
        particle.pos += particle.velocity * deltaTime;
        particle.life -= deltaTime;
        particle.rotation += particle.rotationSpeed * deltaTime;
        
        // 중력 효과
        particle.velocity.y += 200.0 * deltaTime;
        
        // 공기 저항
        particle.velocity *= 0.98;
        
        // 알파값 조정 (생명이 줄어들수록 투명해짐)
        double alpha = particle.life / particle.maxLife;
        particle.color.a = alpha;
    }
    
    // 수명이 다한 파티클 제거
    clearParticles_.remove_if([](const ClearParticle& p) { return p.life <= 0; });
}
void InGameScene::drawDeathEffect()
{
	// Additive 블렌드로 빛나는 효과
	ScopedRenderStates2D blend{ BlendState::Additive };

	for (const auto& particle : deathParticles_)
	{
		Circle{ particle.pos, particle.size }.draw(particle.color);
	}
}
void InGameScene::drawClearEffect()
{
    // Additive 블렌드 상태로 설정하여 빛나는 효과
    ScopedRenderStates2D blend{ BlendState::Additive };
    
    for (const auto& particle : clearParticles_)
    {
        // 파티클 크기와 색상 적용
        if (particle.size < 2.0)
        {
            // 작은 파티클은 점으로 표시
            Circle{particle.pos, particle.size}.draw(particle.color);
        }
        else
        {
            // 큰 파티클은 회전하는 별 모양
            const double halfSize = particle.size * 0.5;
            const Vec2 center = particle.pos;
            
            // 별 모양 그리기 (5개 점)
            Array<Vec2> starPoints;
            for (int i = 0; i < 10; i++)
            {
                double angle = particle.rotation + (i * Math::Pi / 5.0);
                double radius = (i % 2 == 0) ? halfSize : halfSize * 0.5;
                starPoints.push_back(center + Vec2(Math::Cos(angle) * radius, Math::Sin(angle) * radius));
            }
            
            // 별 모양 폴리곤 그리기
            if (starPoints.size() >= 3)
            {
                Polygon{starPoints}.draw(particle.color);
            }
        }
    }
}
void InGameScene::drawFailedScreen()
{
	// 배경 오버레이 표시
	Rect{ 0, 0, Scene::Size().x, Scene::Size().y }.draw(ColorF{ 0, 0, 0, 0.8 });

	// "STAGE FAILED!" 텍스트 (빨간색)
	clearFont_(U"STAGE FAILED!")
		.drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 200, ColorF{ 1.0, 0.3, 0.3 });

	// Move 횟수 표시
	gameFont_(U"Moves: {}"_fmt(moves_))
		.drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0, ColorF{ 0.8, 0.8, 0.8 });

	// 버튼 영역
	const double centerX = Scene::Size().x / 2.0;
	const double centerY = Scene::Size().y / 2.0 + 120;
	const double buttonWidth = 200;
	const double buttonHeight = 50;
	const double buttonMargin = 20;

	// Retry 버튼 (왼쪽)
	Rect retryButtonRect{
		static_cast<int32>(centerX - buttonWidth - buttonMargin / 2),
		static_cast<int32>(centerY - buttonHeight / 2),
		static_cast<int32>(buttonWidth),
		static_cast<int32>(buttonHeight)
	};

	ColorF retryColor = retryButtonRect.mouseOver() ? ColorF{ 0.7, 0.4, 0.3 } : ColorF{ 0.6, 0.3, 0.2 };
	retryButtonRect.draw(retryColor);
	retryButtonRect.drawFrame(3, Palette::White);

	ColorF retryTextColor = retryButtonRect.mouseOver() ? Palette::Yellow : Palette::White;
	buttonFont_(U"やり直す").drawAt(retryButtonRect.center(), retryTextColor);

	// Stage Select 버튼 (오른쪽)
	Rect stageSelectButtonRect{
		static_cast<int32>(centerX + buttonMargin / 2),
		static_cast<int32>(centerY - buttonHeight / 2),
		static_cast<int32>(buttonWidth),
		static_cast<int32>(buttonHeight)
	};

	ColorF stageSelectColor = stageSelectButtonRect.mouseOver() ? ColorF{ 0.3, 0.5, 0.7 } : ColorF{ 0.2, 0.4, 0.6 };
	stageSelectButtonRect.draw(stageSelectColor);
	stageSelectButtonRect.drawFrame(3, Palette::White);

	ColorF stageSelectTextColor = stageSelectButtonRect.mouseOver() ? Palette::Yellow : Palette::White;
	buttonFont_(U"ステージ選択").drawAt(stageSelectButtonRect.center(), stageSelectTextColor);

	// 버튼 클릭 처리
	if (retryButtonRect.leftClicked())
	{
		// 현재 스테이지 다시 시작
		isFailed_ = false;
		showFailedButtons_ = false;
		isPlayerDead_ = false;  // ★ 추가
		deathAnimTimer_ = 0.0;   // ★ 추가
		deathParticles_.clear(); // ★ 추가
		gameTime_ = 0.0;
		moves_ = 0;
		score_ = 0;


		loadStage(currentStage_);

		// ★ 배경음악 재생 (Final Stage 구분) ★
		if (StageData::isFinalStage(currentStage_))
		{
			if (!bossBgm_.isEmpty())
			{
				bossBgm_.setVolume(0.4);
				bossBgm_.play();
			}
		}
		else
		{
			if (!bgm_.isEmpty())
			{
				bgm_.setVolume(0.33);
				bgm_.play();
			}
		}
	}
	else if (stageSelectButtonRect.leftClicked())
	{
		changeScene(SceneType::StageSelect);
	}

	gameFont_(U"Rキーでやり直す")
		.drawAt(Scene::Size().x / 2.0, Scene::Size().y - 100, ColorF{ 0.7, 0.7, 0.7 });
}

void InGameScene::startPaintAnimation(bool mirrored)
{
    isPlayingPaintAnimation_ = true;
    isPaintAnimMirrored_ = mirrored;
    paintAnimTimer_ = 0.0;
    paintAnimFrame_ = 0;
}

void InGameScene::updatePaintAnimation()
{
    if (!isPlayingPaintAnimation_)
        return;
    
    const double deltaTime = Scene::DeltaTime();
    paintAnimTimer_ += deltaTime;
    
    // 각 프레임당 표시 시간 계산 (1초 / 5프레임 = 0.2초)
    const double frameDuration = PAINT_ANIM_DURATION / PAINT_ANIM_FRAME_COUNT;
    
    if (paintAnimTimer_ >= frameDuration)
    {
        paintAnimTimer_ -= frameDuration;
        paintAnimFrame_++;
        
        // 모든 프레임 재생 완료
        if (paintAnimFrame_ >= PAINT_ANIM_FRAME_COUNT)
        {
            isPlayingPaintAnimation_ = false;
            paintAnimFrame_ = 0;
            paintAnimTimer_ = 0.0;
        }
    }
}

void InGameScene::drawPaintAnimation()
{
    if (!isPlayingPaintAnimation_ || tacoPaintFrames_.isEmpty())
        return;
    
    // 현재 프레임 인덱스가 유효한지 확인
    if (paintAnimFrame_ < 0 || paintAnimFrame_ >= static_cast<int32>(tacoPaintFrames_.size()))
        return;
    
    const Texture& currentFrame = tacoPaintFrames_[paintAnimFrame_];
    if (currentFrame.isEmpty())
        return;
    
    // 플레이어 위치에 페인트 애니메이션 그리기
    const Rect animRect{
        static_cast<int32>(playerPixelPos_.x - TILE_SIZE / 2),
        static_cast<int32>(playerPixelPos_.y - TILE_SIZE / 2),
        TILE_SIZE,
        TILE_SIZE
    };
    
    // 좌우 반전 여부에 따라 그리기
    if (isPaintAnimMirrored_)
    {
        currentFrame.resized(TILE_SIZE, TILE_SIZE).mirrored().draw(animRect.pos);
    }
    else
    {
        currentFrame.resized(TILE_SIZE, TILE_SIZE).draw(animRect.pos);
    }
}
CustomCamera2D& InGameScene::camInstance() {
	static CustomCamera2D s_cam{ Vec2{ 0, 0 }, 1.0 };
	return s_cam;
}
CustomCamera2D& InGameScene::camera() { return camInstance(); }
const CustomCamera2D& InGameScene::camera() const { return camInstance(); }

int32 InGameScene::getMapWidth() const {
	if (!mapData_.isEmpty()) {
		return static_cast<int32>(mapData_.front().size());
	}
	return 0;
}

int32 InGameScene::getMapHeight() const {
	if (!mapData_.isEmpty()) {
		return static_cast<int32>(mapData_.size());
	}
	return 0;
}

// 2) 스테이지 인덱스로 로드 (가변 파서 경로 사용)
void InGameScene::loadStageByIndex(int32 stageNumber)
{
	currentStage_ = stageNumber;
	const Array<String> mapText = StageData::getStageMap(stageNumber);
	loadStageFromText_VarSize(mapText); // 가변 파서 호출
}

// 3) 가변 맵 파서 (텍스트 길이로 크기 산출 → 리사이즈 → 전체 범위 파싱)
void InGameScene::loadStageFromText_VarSize(const Array<String>& mapText)
{
	// 크기 산출
	const int32 H = static_cast<int32>(mapText.size());
	int32 W = 0;
	for (const auto& line : mapText) {
		W = Max(W, static_cast<int32>(line.size()));
	}
	const int32 width = Max(1, W);
	const int32 height = Max(1, H);

	// mapData_ 리사이즈 및 초기화
	mapData_.clear();
	mapData_.resize(height);
	for (auto& row : mapData_) {
		row.assign(width, TileType::Empty);
	}

	// 엔티티/목표 초기화
	boxes_.clear();
	items_.clear();
	redGoalPositions_.clear();
	yellowGoalPositions_.clear();
	blueGoalPositions_.clear();
	orangeGoalPositions_.clear();
	greenGoalPositions_.clear();
	violetGoalPositions_.clear();
	blackGoalPositions_.clear();

	// 전체 범위 파싱 (기존 로직 유지, addBox 없이 push_back 사용)
	for (int32 y = 0; y < height; ++y) {
		const String line = (y < mapText.size() ? mapText[y] : U"");
		for (int32 x = 0; x < width; ++x) {
			const char32 ch = (x < static_cast<int32>(line.size()) ? line[x] : U'.');
			const Point pos{ x, y };

			switch (ch) {
			case U'#': mapData_[y][x] = TileType::Wall; break;
            case U'i': mapData_[y][x] = TileType::Ice; break;
			case U'L': mapData_[y][x] = TileType::Lava; break;

			case U'!': mapData_[y][x] = TileType::LavaWarning; break;
			case U'T':
				playerPos_ = pos;
				playerPixelPos_ = tileToPixel(pos);
				targetPixelPos_ = playerPixelPos_;
				isPlayerMoving_ = false;
				break;

				// 박스(대문자)
			case U'R': boxes_.push_back(ColorBox{ pos, BoxColor::Red,    0.0, nextBoxUID_++ }); break;
			case U'Y': boxes_.push_back(ColorBox{ pos, BoxColor::Yellow, 0.0, nextBoxUID_++ }); break;
			case U'B': boxes_.push_back(ColorBox{ pos, BoxColor::Blue,   0.0, nextBoxUID_++ }); break;
			case U'O': boxes_.push_back(ColorBox{ pos, BoxColor::Orange, 0.0, nextBoxUID_++ }); break;
			case U'G': boxes_.push_back(ColorBox{ pos, BoxColor::Green,  0.0, nextBoxUID_++ }); break;
			case U'V': boxes_.push_back(ColorBox{ pos, BoxColor::Violet, 0.0, nextBoxUID_++ }); break;
			case U'K': boxes_.push_back(ColorBox{ pos, BoxColor::Black,  0.0, nextBoxUID_++ }); break;

				// 목표(소문자)
			case U'r': mapData_[y][x] = TileType::RedGoal;    redGoalPositions_.push_back(pos);    break;
			case U'y': mapData_[y][x] = TileType::YellowGoal; yellowGoalPositions_.push_back(pos); break;
			case U'b': mapData_[y][x] = TileType::BlueGoal;   blueGoalPositions_.push_back(pos);   break;
			case U'o': mapData_[y][x] = TileType::OrangeGoal; orangeGoalPositions_.push_back(pos); break;
			case U'g': mapData_[y][x] = TileType::GreenGoal;  greenGoalPositions_.push_back(pos);  break;
			case U'v': mapData_[y][x] = TileType::VioletGoal; violetGoalPositions_.push_back(pos); break;
			case U'k': mapData_[y][x] = TileType::BlackGoal;  blackGoalPositions_.push_back(pos);  break;

				// 아이템(숫자)
			case U'2': items_.push_back(GameItem{ pos, ItemType::RedItem }); mapData_[y][x] = TileType::RedItem;    break;
			case U'4': items_.push_back(GameItem{ pos, ItemType::OrangeItem }); mapData_[y][x] = TileType::OrangeItem; break;
			case U'6': items_.push_back(GameItem{ pos, ItemType::YellowItem }); mapData_[y][x] = TileType::YellowItem; break;
			case U'7': items_.push_back(GameItem{ pos, ItemType::GreenItem }); mapData_[y][x] = TileType::GreenItem;  break;
			case U'8': items_.push_back(GameItem{ pos, ItemType::BlueItem }); mapData_[y][x] = TileType::BlueItem;   break;
			case U'9': items_.push_back(GameItem{ pos, ItemType::VioletItem }); mapData_[y][x] = TileType::VioletItem; break;

				// 빈 공간
			case U'.':
			case U' ':
			default:
				mapData_[y][x] = TileType::Empty;
				break;
			}
		}
	}

	// 픽셀 좌표 및 고정 카메라 정렬
	playerPixelPos_ = tileToPixel(playerPos_);
	onStageLoaded_FixedCamera(); // 맵 크기에 맞게 스케일/센터 고정
}

// 4) 경계/이동 체크 (동적 크기 기반)
bool InGameScene::isInsideMap(Point pos) const {
	return (pos.x >= 0 && pos.y >= 0
		 && pos.x < getMapWidth()
		 && pos.y < getMapHeight());
}

bool InGameScene::canMoveToPoint(Point pos) const {
	if (isBlocked(pos)) return false;
	if (mapData_[pos.y][pos.x] == TileType::Wall) return false;
	if (getBoxAt(pos) != nullptr) return false;
	return true;
}

double InGameScene::computeFitScaleToMap(double margin) const {
	const double vw = static_cast<double>(Scene::Width());
	const double vh = static_cast<double>(Scene::Height());
	const double worldW = static_cast<double>(getMapWidth()) * TILE_SIZE;
	const double worldH = static_cast<double>(getMapHeight()) * TILE_SIZE;
	if (worldW <= 0.0 || worldH <= 0.0) return 1.0;
	double s = Min(vw / worldW, vh / worldH) * margin;
	return Clamp(s, 0.25, 5.0);
}

void InGameScene::applyFixedCameraFitToMap()
{
	if (StageData::isFinalStage(currentStage_)) {
		const double vw = Scene::Width();
		const double vh = Scene::Height();
		const double worldW = getMapWidth() * TILE_SIZE;
		const double worldH = getMapHeight() * TILE_SIZE;

		double s = Min(vw / worldW, (vh * 0.5) / worldH) * 0.9;
		s = Clamp(s, 0.25, 5.0);

		const Vec2 center(worldW * 0.5, worldH * 0.5);
		auto& cam = camera();
		cam.jumpToScale(s);

		const Vec2 offsetCenter(center.x, center.y - (vh / s) * 0.25);
		cam.jumpToPos(offsetCenter);
		return;
	}

	const double s = computeFitScaleToMap(0.95);
	const Vec2 center(getMapWidth() * TILE_SIZE * 0.5,
					  getMapHeight() * TILE_SIZE * 0.5);
	auto& cam = camera();
	cam.jumpToScale(s);
	cam.jumpToPos(center);
}

void InGameScene::applyFixedCameraFitToRect(const Rect& tileRect, double margin) {
	const double vw = static_cast<double>(Scene::Width());
	const double vh = static_cast<double>(Scene::Height());
	const double worldW = static_cast<double>(tileRect.w) * TILE_SIZE;
	const double worldH = static_cast<double>(tileRect.h) * TILE_SIZE;
	if (worldW <= 0.0 || worldH <= 0.0) return;

	double s = Min(vw / worldW, vh / worldH) * margin;
	s = Clamp(s, 0.25, 5.0);

	const Vec2 center(
		(static_cast<double>(tileRect.x) + tileRect.w * 0.5) * TILE_SIZE,
		(static_cast<double>(tileRect.y) + tileRect.h * 0.5) * TILE_SIZE
	);

	auto& cam = camera();
	cam.jumpToScale(s);
	cam.jumpToPos(center);
}

void InGameScene::updateFixedCameraZoomByWheelInput() {
	const double minScale = computeFitScaleToMap(0.95);
	const double maxScale = 3.0;

	auto& cam = camera();
	double s = cam.getScale();
	const double wheel = Mouse::Wheel();
	if (wheel != 0.0) {
		s = Clamp(s * (1.0 + wheel * 0.1), minScale, maxScale);
		cam.jumpToScale(s);
	}
	const Vec2 center(
		static_cast<double>(getMapWidth()) * TILE_SIZE * 0.5,
		static_cast<double>(getMapHeight()) * TILE_SIZE * 0.5
	);
	cam.jumpToPos(center);
}

void InGameScene::onStageLoaded_FixedCamera() {
	playerPixelPos_ = tileToPixel(playerPos_);
	
    // Final stage (보스 스테이지)는 플레이 영역을 화면 하단에 배치
    if (StageData::isFinalStage(currentStage_)) {
		const double vw = static_cast<double>(Scene::Width());
		const double vh = static_cast<double>(Scene::Height());
		const double worldW = static_cast<double>(getMapWidth()) * TILE_SIZE;
		const double worldH = static_cast<double>(getMapHeight()) * TILE_SIZE;
		
		// 플레이 영역(10x5)을 화면에 맞게 스케일 계산
		double s = Min(vw / worldW, (vh * 0.5) / worldH) * 0.9;  // 화면 하단 절반 영역에 맞춤
		s = Clamp(s, 0.25, 5.0);
		
		// 플레이 영역 중심을 화면 하단에 배치 (상단 절반은 보스용)
		const Vec2 center(
			worldW * 0.5,
			worldH * 0.5
		);
		
		auto& cam = camera();
		cam.jumpToScale(s);
		
        // 카메라를 위로 이동 (화면 하단에 플레이 영역 표시)
		const Vec2 offsetCenter = Vec2(
			center.x,
            center.y - (vh / s) * 0.25  // 화면 하단으로 이동
		);
		cam.jumpToPos(offsetCenter);
	}
	else {
		applyFixedCameraFitToMap();
	}
}

Rect InGameScene::getVisibleTileRect() const
{
	const auto& cam = camera();
	const double s = cam.getScale();
	const Vec2 c = cam.getCenter();
	const double vw = static_cast<double>(Scene::Width()) / s;
	const double vh = static_cast<double>(Scene::Height()) / s;
	const RectF view(c.x - vw * 0.5, c.y - vh * 0.5, vw, vh);

	const int32 W = getMapWidth();
	const int32 H = getMapHeight();

	const int32 minX = Max(0, static_cast<int32>(Floor(view.x / TILE_SIZE)) - 1);
	const int32 minY = Max(0, static_cast<int32>(Floor(view.y / TILE_SIZE)) - 1);
	const int32 maxX = Min(W - 1, static_cast<int32>(Floor((view.x + view.w) / TILE_SIZE)) + 1);
	const int32 maxY = Min(H - 1, static_cast<int32>(Floor((view.y + view.h) / TILE_SIZE)) + 1);

	return Rect(minX, minY, Max(0, maxX - minX + 1), Max(0, maxY - minY + 1));
}

void InGameScene::drawWorldWithCamera(const std::function<void()>& worldDraw,
									  const std::function<void()>& uiDraw)
{
	const auto _t = camera().createTransformer();
	worldDraw();
	uiDraw();
}

void InGameScene::setTileAt(Point pos, TileType type) {
	if (isBlocked(pos)) return;
	mapData_[pos.y][pos.x] = type;
}

void InGameScene::setTileAt(int32 x, int32 y, TileType type) {
	setTileAt(Point(x, y), type);
}

InGameScene::TileType InGameScene::getTileAt(Point pos) const {
	if (isBlocked(pos)){
		return TileType::Empty;
	}
	return mapData_[pos.y][pos.x];
}

InGameScene::TileType InGameScene::getTileAt(int32 x, int32 y) const {
	return getTileAt(Point(x, y));
}

void InGameScene::applyTileOverlay(const Array<String>& overlayData, OverlayApplyMode mode) {
	const int H = getMapHeight();
	const int W = getMapWidth();

	if ((int)overlayWarn_.size() != H || (H > 0 && (int)overlayWarn_[0].size() != W)) {
		overlayWarn_.assign(H, Array<OverlayType>(W, OverlayType::None));
	}
	else {
		for (int y = 0; y < H; ++y)
			for (int x = 0; x < W; ++x)
				overlayWarn_[y][x] = OverlayType::None;
	}

	const int h = Min((int)overlayData.size(), H);
	for (int y = 0; y < h; ++y) {
		const String& line = overlayData[y];
		const int w = Min((int)line.size(), W);
		for (int x = 0; x < w; ++x) {
			const char32 ch = line[x];
			const Point p{ x, y };

			if (mode == OverlayApplyMode::OverlayOnly) {
				if (ch == U'!') {
					overlayWarn_[y][x] = OverlayType::Warning;
				}
				continue;
			}

			switch (ch) {
			case U'#': setTileAt(p, TileType::Wall);        break;
			case U' ': setTileAt(p, TileType::Empty);       break;
			case U'i': case U'I': setTileAt(p, TileType::Ice);  break;
			case U'l': case U'L': setTileAt(p, TileType::Lava); break;
			case U'!': setTileAt(p, TileType::LavaWarning); break;

			default: break;
			}
		}
	}
}

void InGameScene::initBossWallSystem()
{
	overlayWarn_.assign(getMapHeight(), Array<OverlayType>(getMapWidth(), OverlayType::None));
	wallMask_.assign(getMapHeight(), Array<bool>(getMapWidth(), false));

	initBossWallPatternSystem();
}

void InGameScene::setOverlayWarning(int32 x, int32 y, bool on) {
	if (x < 0 || x >= getMapWidth() || y < 0 || y >= getMapHeight()) return;
	overlayWarn_[y][x] = on ? OverlayType::Warning : OverlayType::None;
}

void InGameScene::clearOverlayWarningCol(int32 x) {
	if (x < 0 || x >= getMapWidth()) return;
	for (int y = 0; y < getMapHeight(); ++y) {
		overlayWarn_[y][x] = OverlayType::None;
	}
}

void InGameScene::drawOverlayWarnings() {
	const int H = getMapHeight();
	const int W = getMapWidth();

	if ((int)overlayWarn_.size() != H || H == 0) return;
	for (int y = 0; y < H; ++y) {
		if ((int)overlayWarn_[y].size() != W) return;
	}

	const double t = Math::Fmod(gameTime_, 1.0);
	const double flash = 0.6 + 0.4 * Sin(2.0 * Math::Pi * t);

	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			if (overlayWarn_[y][x] != OverlayType::Warning) continue;
			const Rect r{ x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
			r.drawFrame(4, 0, ColorF{ 1.0, 0.15, 0.15, flash });
		}
	}
}

void InGameScene::updateBossWallFilling(double dt) {
	if (!StageData::isFinalStage(currentStage_)) return;
	if (isCleared_ || isPlayerDead_) return;

	if (gameTime_ < wallNextTime_) return;

	const bool hasLeft = (nextColumnL_ <= nextColumnR_);
	const bool hasRight = (nextColumnR_ >= nextColumnL_);

	if (!hasLeft) return;

	if (fillFromBothSides_ && hasLeft && hasRight && nextColumnL_ < nextColumnR_) {
		const int xL = nextColumnL_;
		const int xR = nextColumnR_;
		spawnWallColumn(xL);
		if (xR != xL) spawnWallColumn(xR);

		clearOverlayWarningCol(xL);
		clearOverlayWarningCol(xR);

		nextColumnL_++;
		nextColumnR_--;

		if (nextColumnL_ <= nextColumnR_) {
			if (isInsideMap(Point{ nextColumnL_, 0 })) {
				for (int y = 0; y < getMapHeight(); ++y) setOverlayWarning(nextColumnL_, y, true);
			}
			if (nextColumnR_ != nextColumnL_ && isInsideMap(Point{ nextColumnR_, 0 })) {
				for (int y = 0; y < getMapHeight(); ++y) setOverlayWarning(nextColumnR_, y, true);
			}
		}
	}
	else {
		const int x = nextColumnL_;
		spawnWallColumn(x);
		clearOverlayWarningCol(x);
		nextColumnL_++;
		if (nextColumnL_ <= nextColumnR_) {
			if (isInsideMap(Point{ nextColumnL_, 0 })) {
				for (int y = 0; y < getMapHeight(); ++y) setOverlayWarning(nextColumnL_, y, true);
			}
		}
	}

	wallNextTime_ = gameTime_ + wallStepInterval_;
}

void InGameScene::spawnWallColumn(int32 x) {
	if (x < 0 || x >= getMapWidth()) return;

	if (!isPlayerDead_ && playerPos_.x == x) {
		isPlayerDead_ = true;
		deathAnimTimer_ = 0.0;
		createDeathEffect(true);


		if (StageData::isFinalStage(currentStage_)) {
			if (!bossBgm_.isEmpty() && bossBgm_.isPlaying()) {
				savedMusicPosition_ = bossBgm_.posSec();
				bossBgm_.stop();
			}
		}
		else {
			if (!bgm_.isEmpty() && bgm_.isPlaying()) {
				savedMusicPosition_ = bgm_.posSec();
				bgm_.stop();
			}
		}
	}


	for (int y = 0; y < getMapHeight(); ++y) {
		if (!wallMask_.isEmpty()) wallMask_[y][x] = true;
		spawnWallBreakFXAtTile(Point{ x,y });
	}

	const double scale = camera().getScale();
	const double shakeIntensityWorld = Min(34.0 / Max(0.001, scale), 40.0 / Max(0.001, scale));
	camera().shake(0.25, shakeIntensityWorld);
}

bool InGameScene::isBlocked(Point pos) const {
	if (pos.x < 0 || pos.x >= getMapWidth() || pos.y < 0 || pos.y >= getMapHeight())
		return true;
	if (mapData_[pos.y][pos.x] == TileType::Wall) return true;
	if (!wallMask_.isEmpty() && wallMask_[pos.y][pos.x]) return true;
	return false;
}

void InGameScene::drawWallMask() {
	if (wallMask_.isEmpty()) return;
	for (int y = 0; y < getMapHeight(); ++y) {
		for (int x = 0; x < getMapWidth(); ++x) {
			if (!wallMask_[y][x]) continue;
			const Rect r{ x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
			r.draw(ColorF{ 0.3, 0.3, 0.35 });
			r.drawFrame(2, 0, ColorF{ 0.5, 0.5, 0.55 });
		}
	}
}

bool InGameScene::isGoalTile(TileType t) {
	switch (t) {
	case TileType::RedGoal:
	case TileType::YellowGoal:
	case TileType::BlueGoal:
	case TileType::OrangeGoal:
	case TileType::GreenGoal:
	case TileType::VioletGoal:
	case TileType::BlackGoal:
		return true;
	default:
		return false;
	}
}

void InGameScene::drawGoalMarkersTop() {
	// 바닥 타일 스위치에서 사용한 파라미터와 동일
	constexpr double ringRadius = 12.0;
	constexpr int ringThickness = 3;

	auto shouldOverlay = [&](Point p) -> bool {
		if (p.x < 0 || p.x >= getMapWidth() || p.y < 0 || p.y >= getMapHeight()) return false;
		// 이미 바닥 타일로 Goal이 그려질 상황이면 상단에서 중복 그리지 않음
		if (isGoalTile(mapData_[p.y][p.x])) return false;
		// 그 외(용암/얼음/벽/마스크 등으로 가려진 경우)는 상단에서 그려서 보이게 함
		return true;
		};

	auto drawGoalTop = [&](Point p, const ColorF& cFill, const ColorF& cRing) {
		if (!shouldOverlay(p)) return;
		const Rect r{ p.x * TILE_SIZE, p.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
		r.draw(cFill);
		Circle{ r.center(), ringRadius }.drawFrame(ringThickness, cRing);
		};

	// 바닥 타일과 동일 색상으로 최상단 오버레이
	for (const auto& p : redGoalPositions_)
		drawGoalTop(p, ColorF{ 0.9, 0.2, 0.2, 0.3 }, ColorF{ 0.9, 0.2, 0.2 });
	for (const auto& p : yellowGoalPositions_)
		drawGoalTop(p, ColorF{ 1.0, 0.9, 0.2, 0.3 }, ColorF{ 1.0, 0.9, 0.2 });
	for (const auto& p : blueGoalPositions_)
		drawGoalTop(p, ColorF{ 0.2, 0.4, 0.9, 0.3 }, ColorF{ 0.2, 0.4, 0.9 });
	for (const auto& p : orangeGoalPositions_)
		drawGoalTop(p, ColorF{ 1.0, 0.5, 0.0, 0.3 }, ColorF{ 1.0, 0.5, 0.0 });
	for (const auto& p : greenGoalPositions_)
		drawGoalTop(p, ColorF{ 0.2, 0.8, 0.3, 0.3 }, ColorF{ 0.2, 0.8, 0.3 });
	for (const auto& p : violetGoalPositions_)
		drawGoalTop(p, ColorF{ 0.7, 0.2, 0.8, 0.3 }, ColorF{ 0.7, 0.2, 0.8 });
	for (const auto& p : blackGoalPositions_)
		drawGoalTop(p, ColorF{ 0.2, 0.2, 0.2, 0.5 }, ColorF{ 0.4, 0.4, 0.4 });
}

void InGameScene::initBossAttacks()
{
	pendingAttacks_.clear();;
	nextAttackTime_ = gameTime_;
	scheduleNextBossAttack();
}

bool InGameScene::isSpawnableTile(Point p) const {
	if (!isInsideMap(p)) return false;
	if (mapData_[p.y][p.x] == TileType::Wall) return false;
	if (!wallMask_.isEmpty() && wallMask_[p.y][p.x]) return false;
	if (getBoxAt(p) != nullptr) return false;
	return true;
}

bool InGameScene::chooseRandomSpawnTile(Point& out) {
	Array<Point> candidates;
	for (int y = 0; y < getMapHeight(); ++y) {
		for (int x = 0; x < getMapWidth(); ++x) {
			const Point p{ x, y };
			if (isSpawnableTile(p)) candidates << p;
		}
	}
	if (candidates.isEmpty()) return false;
	out = candidates[Random(candidates.size() - 1)];
	return true;
}

ColorF InGameScene::randomSixColor() const {
	static const BoxColor pool[] = {
		BoxColor::Red, BoxColor::Yellow, BoxColor::Blue,
		BoxColor::Orange, BoxColor::Green, BoxColor::Violet
	};

	switch (pool[Random(5)])
	{
		case BoxColor::Red:    return Palette::Red;
		case BoxColor::Yellow: return Palette::Yellow;
		case BoxColor::Blue:   return Palette::Blue;
		case BoxColor::Orange: return Palette::Orange;
		case BoxColor::Green:  return Palette::Green;
		case BoxColor::Violet: return Palette::Violet;
		default:               return Palette::White;
	}

}

//void InGameScene::spawnColorAttack() {
//	Point tile;
//	if (!chooseRandomSpawnTile(tile)) return;
//
//	
//	ColorBox box;
//	box.pos = tile;
//	box.color = randomSixColor();
//	box.uid = nextBoxUID_++;
//	box.creationTime = gameTime_;
//	boxes_ << box;
//
//	
//	spawnWallBreakFXAtTile(tile);
//	camera().shake(0.12, 10.0 / Max(0.001, camera().getScale()));
//
//	lastAttackTile_ = tile;
//}

// InGameScene.cpp
void InGameScene::updateBossAttacks(double dt)
{
	if (!StageData::isFinalStage(currentStage_)) return;

	// 주기적으로 다음 공격 예약
	if (gameTime_ >= nextAttackTime_)
	{
		scheduleNextBossAttack();           // nextAttackTime은 함수 내부에서 갱신
	}

	// 이하 기존 텔레그래프 처리/발사 처리 유지
	for (auto& p : pendingAttacks_)
	{
		if (p.phase != AttackPhase::Telegraph) continue;
		p.telegraphTime -= dt;
		if (p.telegraphTime <= 0.0)
		{
			if (p.type == BossAttackType::ColorSpawn)
			{
				fireColorAttack(p);
			}
			p.phase = AttackPhase::Done;
		}
	}
	pendingAttacks_.remove_if([](const PendingAttack& a) { return a.phase == AttackPhase::Done; });
}


void InGameScene::spawnBlackHomingAttack() {
	Point source = lastAttackTile_;
	if (!isInsideMap(source) || !isSpawnableTile(source)) {
		if (!chooseRandomSpawnTile(source)) return;
	}

	spawnWallBreakFXAtTile(source);
	camera().shake(0.08, 8.0 / Max(0.001, camera().getScale()));

	HomingBullet b;
	b.sourceTile = source;
	b.pos = Vec2{ (source.x + 0.5) * TILE_SIZE, (source.y + 0.5) * TILE_SIZE };
	b.armed = false;
	b.armTime = 0.25;
	b.life = 5.0;
	b.speed = 420.0;

	b.targetTile = playerPos_;

	b.vel = Vec2{ 0, 0 };
	homingBullets_ << b;

	lastAttackTile_ = source;
}

void InGameScene::drawBossAttacks()
{
	// 텔레그래프 마커 그리기 (기존 코드)
	for (const auto& p : pendingAttacks_)
	{
		if (p.phase != AttackPhase::Telegraph) continue;

		const double t = gameTime_;
		const double pulse = 0.5 + 0.5 * Math::Sin(t * 12.0);

		if (p.type == BossAttackType::ColorSpawn)
		{
			// 타겟 타일 마커
			drawTelegraphMarker(p.targetTile, p.telegraphColor, t, pulse);

			const Vec2 bossPos = getBossStartUIPos();
			const Vec2 targetPos = Vec2(
				p.targetTile.x * TILE_SIZE + TILE_SIZE * 0.5,
				p.targetTile.y * TILE_SIZE + TILE_SIZE * 0.5
			);

			// 보스 -> 타겟 라인 (예고 빔)
			const double beamAlpha = 0.35 + 0.15 * pulse;
			Line(bossPos, targetPos).draw(5, ColorF(
				p.telegraphColor.r,
				p.telegraphColor.g,
				p.telegraphColor.b,
				beamAlpha
			));

			// 보스 위치에 발사 준비 표시
			const double chargePulse = 1.0 - (p.telegraphTime / 2.0); // 준비 진행도
			const double chargeSize = 15.0 + 10.0 * chargePulse;

			Circle(bossPos, chargeSize * pulse)
				.draw(ColorF(p.telegraphColor.r, p.telegraphColor.g, p.telegraphColor.b, 0.5 * pulse));
			Circle(bossPos, chargeSize)
				.drawFrame(3, ColorF(p.telegraphColor.r, p.telegraphColor.g, p.telegraphColor.b, 0.8 * pulse));
		}
		else // BossAttackType::BlackHoming
		{
			// 소스 타일 마커
			drawTelegraphMarker(p.sourceTile, ColorF(0.0, 0.0, 0.0, 1.0), t, pulse);

			const Vec2 bossPos = getBossStartUIPos();
			const Vec2 sourcePos = Vec2(
				p.sourceTile.x * TILE_SIZE + TILE_SIZE * 0.5,
				p.sourceTile.y * TILE_SIZE + TILE_SIZE * 0.5
			);

			Line(bossPos, sourcePos).draw(5, ColorF(0.9, 0.9, 0.9, 0.25 + 0.15 * pulse));

			const double chargePulse = 1.0 - (p.telegraphTime / 2.0);
			const double chargeSize = 15.0 + 10.0 * chargePulse;

			Circle(bossPos, chargeSize * pulse)
				.draw(ColorF(0.9, 0.9, 0.9, 0.4 * pulse));
			Circle(bossPos, chargeSize)
				.drawFrame(3, ColorF(0.95, 0.95, 0.95, 0.7 * pulse));
		}
	}
}


Vec2 InGameScene::getBossStartUIPos() const
{
	if (!StageData::isFinalStage(currentStage_) || bossIdleFrames_.isEmpty())
	{
		return Vec2(Scene::Width() * 0.72, Scene::Height() * 0.18);
	}

	const int32 screenW = Scene::Width();
	const int32 screenH = Scene::Height();
	const int32 topH = screenH / 2;

	const Texture& bossTex = bossIdleFrames_[bossAnimFrame_ % static_cast<int32>(bossIdleFrames_.size())];
	const Size bossSize = bossTex.size();

	if (bossSize.x == 0 || bossSize.y == 0)
	{
		return Vec2(screenW * 0.72, screenH * 0.18);
	}

	const double sx = static_cast<double>(screenW) / bossSize.x;
	const double sy = static_cast<double>(topH) / bossSize.y;
	const double s = Min(sx, sy) * 0.9;

	const int32 drawW = static_cast<int32>(bossSize.x * s);
	const int32 drawH = static_cast<int32>(bossSize.y * s);

	const int32 x = (screenW - drawW) / 2;
	const int32 y = (topH - drawH) / 2;

	return Vec2(x + drawW / 2.0, y + drawH * 0.85);
}

void InGameScene::scheduleNextBossAttack()
{
	PendingAttack p;
	p.type = BossAttackType::ColorSpawn;

	Point t;
	if (!chooseRandomSpawnTile(t))
		return;

	p.targetTile = t;

	static const BoxColor pool[6] = {
		BoxColor::Red, BoxColor::Yellow, BoxColor::Blue,
		BoxColor::Orange, BoxColor::Green, BoxColor::Violet
	};
	const BoxColor bc = pool[Random(0, 5)];
	p.boxColor = bc;
	p.telegraphColor = getBoxColorF(bc);

	p.telegraphTime = 2.0;
	p.phase = AttackPhase::Telegraph;

	pendingAttacks_ << p;

	// 다음 예약 시간 갱신(기존 간격 사용)
	nextAttackTime_ = gameTime_ + attackInterval_;
}

void InGameScene::fireColorAttack(const PendingAttack& p)
{
	const Vec2 bossScreenPos = getBossStartUIPos();
	createEnergyBallEffect(
		bossScreenPos,
		p.targetTile,
		p.telegraphColor,
		1.2,
		true,
		p.boxColor
	);
}

void InGameScene::fireBlackHoming(const PendingAttack& p)
{

}

void InGameScene::drawTelegraphMarker(Point tile, const ColorF& col, double t, double pulse01) const
{
	const Vec2 center(tile.x * TILE_SIZE + TILE_SIZE * 0.5, tile.y * TILE_SIZE + TILE_SIZE * 0.5);

	const double baseRadius = TILE_SIZE * 0.45;
	const double pulseRadius = baseRadius * (0.85 + 0.15 * pulse01);
	const double alpha = 0.6 + 0.4 * pulse01;

	Circle(center, pulseRadius).draw(ColorF(col.r, col.g, col.b, 0.25 * alpha));

	Circle(center, pulseRadius).drawFrame(4, ColorF(col.r, col.g, col.b, alpha));

	const double innerRadius = 8.0 + 4.0 * pulse01;
	Circle(center, innerRadius).draw(ColorF(col.r, col.g, col.b, 0.6 * alpha));

	const double crossSize = 15.0;
	Line(center - Vec2(crossSize, 0), center + Vec2(crossSize, 0))
		.draw(3, ColorF(col.r, col.g, col.b, alpha * 0.8));
	Line(center - Vec2(0, crossSize), center + Vec2(0, crossSize))
		.draw(3, ColorF(col.r, col.g, col.b, alpha * 0.8));
}

void InGameScene::createEnergyBallEffect(Vec2 screenStart, Point targetTile, const ColorF& color,
										 double duration, bool spawnBoxOnArrive, BoxColor boxColor)
{
	EnergyBall b;
	b.startPosScreen = screenStart;
	b.targetTile = targetTile;
	b.currentPosScreen = screenStart;
	b.color = color;
	b.duration = duration;
	b.elapsedTime = 0.0;
	b.progress = 0.0;
	b.active = true;
	b.spawnBoxOnArrive = spawnBoxOnArrive;
	b.spawnBoxColor = boxColor;
	b.arrivalHandled = false;

	energyBalls << b;
}

void InGameScene::updateEnergyBalls(double dt)
{
	const Mat3x2 transform = camera().getMat3x2();

	for (auto& b : energyBalls)
	{
		if (!b.active) continue;

		b.elapsedTime += dt;
		b.progress = Clamp(b.elapsedTime / Max(0.001, b.duration), 0.0, 1.0);

		const double eased = 1.0 - Math::Pow(1.0 - b.progress, 3.0);

		const Vec2 targetWorld(
			b.targetTile.x * TILE_SIZE + TILE_SIZE * 0.5,
			b.targetTile.y * TILE_SIZE + TILE_SIZE * 0.5
		);
		const Vec2 targetScreen = transform.transformPoint(targetWorld);

		b.currentPosScreen = Math::Lerp(b.startPosScreen, targetScreen, eased);

		// 도착 처리: 한 번만 실행
		if (!b.arrivalHandled && b.progress >= 1.0)
		{
			b.arrivalHandled = true;

			if (b.spawnBoxOnArrive)
			{
				// 박스 생성 지연 실행
				ColorBox box;
				box.pos = b.targetTile;
				box.color = b.spawnBoxColor;
				box.uid = nextBoxUID_++;
				box.creationTime = gameTime_;
				boxes_ << box;

				spawnWallBreakFXAtTile(b.targetTile);
				camera().shake(0.12, 10.0 / Max(0.001, camera().getScale()));
				lastAttackTile_ = b.targetTile;
			}

			b.active = false;
		}
	}

	energyBalls.remove_if([](const EnergyBall& e) { return !e.active; });
}

void InGameScene::drawEnergyBalls()
{
	if (energyBalls.isEmpty()) return;

	// Additive 블렌딩으로 발광 효과
	ScopedRenderStates2D blend(BlendState::Additive);

	for (const auto& ball : energyBalls)
	{
		if (!ball.active) continue;

		const double t = ball.progress;

		const double sizeProgress = Math::Sin(t * Math::Pi);
		const double baseSize = 25.0;
		const double size = baseSize * (0.5 + sizeProgress * 0.8);

		const double alpha = t < 0.9 ? 1.0 : (1.0 - (t - 0.9) / 0.1);

		Circle(ball.currentPosScreen, size * 1.5)
			.draw(ColorF(ball.color.r, ball.color.g, ball.color.b, 0.3 * alpha));

		Circle(ball.currentPosScreen, size * 1.2)
			.draw(ColorF(ball.color.r, ball.color.g, ball.color.b, 0.5 * alpha));

		Circle(ball.currentPosScreen, size)
			.draw(ColorF(ball.color.r, ball.color.g, ball.color.b, 0.8 * alpha));

		Circle(ball.currentPosScreen, size * 0.4)
			.draw(ColorF(1.0, 1.0, 1.0, 0.9 * alpha));

		const Vec2 trailDir = (ball.currentPosScreen - ball.startPosScreen).normalized();
		const double trailLength = 30.0 * sizeProgress;
		for (int i = 0; i < 5; ++i)
		{
			const double offset = (i + 1) * (trailLength / 5.0);
			const Vec2 trailPos = ball.currentPosScreen - trailDir * offset;
			const double trailAlpha = (1.0 - i / 5.0) * 0.4 * alpha;
			const double trailSize = size * (1.0 - i / 5.0) * 0.6;

			Circle(trailPos, trailSize)
				.draw(ColorF(ball.color.r, ball.color.g, ball.color.b, trailAlpha));
		}
	}
}

// InGameScene.cpp - 새 함수 추가
void InGameScene::drawHomingBullets()
{
}

void InGameScene::drawBossChargeAtStart(const ColorF& col, double remain, double total)
{
	const Vec2 center = getBossStartUIPos();      // 화면 좌표(보스 시작점)
	const double t01 = Saturate(1.0 - remain / Max(0.001, total));
	const double pulse = 0.5 + 0.5 * Math::Sin(gameTime_ * 10.0);
	const double base = 18.0 + 10.0 * t01;
	const double rCore = base * (0.7 + 0.3 * pulse);
	const double rMid = rCore * 1.45;
	const double rGlow = rCore * 2.0;

	ScopedRenderStates2D blend(BlendState::Additive);

	Circle(center, rGlow).draw(ColorF(col.r, col.g, col.b, 0.20 + 0.15 * pulse));
	Circle(center, rMid).draw(ColorF(col.r, col.g, col.b, 0.35 + 0.25 * pulse));
	Circle(center, rCore).draw(ColorF(col.r, col.g, col.b, 0.7 + 0.25 * pulse));
	Circle(center, rCore * 0.4).draw(ColorF(1.0, 1.0, 1.0, 0.8));

	const int sparkCount = 6;
	const double orbit = rGlow * (0.55 + 0.25 * t01);
	for (int i = 0; i < sparkCount; ++i)
	{
		const double ang = gameTime_ * 4.0 + i * Math::TwoPi / sparkCount;
		const Vec2 p = center + Vec2(Math::Cos(ang), Math::Sin(ang)) * orbit;
		Circle(p, 4.0 + 2.0 * pulse).draw(ColorF(col.r, col.g, col.b, 0.5));
	}
}

void InGameScene::drawBossChargeTelegraphs()
{
	bool any = false;
	for (const auto& p : pendingAttacks_)
		if (p.phase == AttackPhase::Telegraph) { any = true; break; }
	if (!any) return;

	for (const auto& p : pendingAttacks_)
	{
		if (p.phase != AttackPhase::Telegraph) continue;

		const ColorF c = (p.type == BossAttackType::ColorSpawn)
			? p.telegraphColor
			: ColorF(0.9, 0.9, 0.9, 1.0);

		drawBossChargeAtStart(c, p.telegraphTime, 2.0);
	}
}

void InGameScene::spawnBombExplosionFXAtTile(Point tile)
{
	WallBreakFX fx;
	fx.tilePos = tile;
	fx.effect = std::make_unique<BombBoxEffect>();
	fx.effect->reset();
	fx.effect->trigger();
	fx.params.pulseDuration = 0.0f;
	fx.params.pulseCount = 0.0f;
	fx.params.explodeTime = 0.6f;
	fx.params.pulseAmp = 0.0f;
	fx.params.pulseSpeed = 0.0f;
	fx.params.spread = 140.0f;
	fx.params.gravity = 700.0f;
	fx.params.seed = static_cast<float>(Random(0.0, 1.0));
	fx.params.useWallColor = false;
	fx.params.wallColor = ColorF(0.1, 0.1, 0.1);
	fx.finished = false;
	fx.shakeStarted = false;

	const Vec2 worldCenter = tileToPixel(tile) + Vec2(TILE_SIZE * 0.5, TILE_SIZE * 0.5);
	const Vec2 camC = camera().getCenter();
	const double dist = (worldCenter - camC).length();
	const double scale = camera().getScale();
	const double falloff = Saturate(1.0 - dist / (900.0 / Max(0.001, scale)));
	const double inten = 16.0 * falloff / Max(0.001, scale);
	camera().shake(0.25, Min(inten, 40.0));

	wallBreakFXs << std::move(fx);

}

void InGameScene::initBossWallPatternSystem()
{
	patternHistory_.clear();
	loadRandomPattern();

	currentPatternFrame_ = 0;
	patternFrameTimer_ = 0.0;
	isPlayingWallPattern_ = true;

	if (!currentWallPattern_.frames.isEmpty())
	{
		applyWallPatternFrame(currentWallPattern_.frames[0]);
	}
}


void InGameScene::updateBossWallPattern(double dt)
{
	if (!StageData::isFinalStage(currentStage_)) return;
	if (!isPlayingWallPattern_) return;
	if (isCleared_ || isPlayerDead_) return;
	if (currentWallPattern_.frames.isEmpty()) return;

	patternFrameTimer_ += dt;

	const auto& currentFrame = currentWallPattern_.frames[currentPatternFrame_];
	if (patternFrameTimer_ >= currentFrame.duration)
	{
		patternFrameTimer_ = 0.0;
		currentPatternFrame_++;

		if (currentPatternFrame_ >= static_cast<int32>(currentWallPattern_.frames.size()))
		{
			if (currentWallPattern_.looping)
			{
				loadRandomPattern();
				currentPatternFrame_ = 0;
			}
			else
			{
				isPlayingWallPattern_ = false;
				return;
			}
		}

		applyWallPatternFrame(currentWallPattern_.frames[currentPatternFrame_]);
	}
}

void InGameScene::applyWallPatternFrame(const BossWallPatternFrame& frame)
{
	const int32 mapW = getMapWidth();
	const int32 mapH = getMapHeight();
	const int32 patternH = static_cast<int32>(frame.pattern.size());

	for (int32 y = 0; y < Min(patternH, mapH); ++y)
	{
		const String& row = frame.pattern[y];
		const int32 patternW = static_cast<int32>(row.size());

		for (int32 x = 0; x < Min(patternW, mapW); ++x)
		{
			const char32 ch = row[x];
			const Point tile{ x, y };

			if (!isInsideMap(tile)) continue;

			switch (ch)
			{
			case U'!':
				setWarningAtTile(tile, true);
				break;

			case U'W':
				setWarningAtTile(tile, false);
				spawnWallAtTile(tile);
				break;

			case U'X':
				setWarningAtTile(tile, false);
				destroyWallAtTile(tile);
				break;

			case U'.':
			default:
				setWarningAtTile(tile, false);
				break;
			}
		}
	}
}

void InGameScene::spawnWallAtTile(Point tile)
{
	if (!isInsideMap(tile)) return;

	if (mapData_[tile.y][tile.x] == TileType::Wall) return;
	if (!wallMask_.isEmpty() && wallMask_[tile.y][tile.x]) return;

	if (!wallMask_.isEmpty())
	{
		wallMask_[tile.y][tile.x] = true;
	}

	spawnWallBreakFXAtTile(tile);

	if (playerPos_ == tile && !isPlayerDead_)
	{
		isPlayerDead_ = true;
		deathAnimTimer_ = 0.0;
		createDeathEffect(true);

		if (StageData::isFinalStage(currentStage_))
		{
			if (!bossBgm_.isEmpty() && bossBgm_.isPlaying())
			{
				savedMusicPosition_ = bossBgm_.posSec();
				bossBgm_.stop();
			}
		}
	}

	const double scale = camera().getScale();
	const double shakeIntensity = Min(5.0 / Max(0.001, scale), 15.0 / Max(0.001, scale));
	camera().shake(0.15, shakeIntensity);
}

void InGameScene::destroyWallAtTile(Point tile)
{
	if (!isInsideMap(tile)) return;

	bool wasWall = false;

	if (!wallMask_.isEmpty() && wallMask_[tile.y][tile.x])
	{
		wallMask_[tile.y][tile.x] = false;
		wasWall = true;
	}

	if (mapData_[tile.y][tile.x] == TileType::Wall)
	{
		mapData_[tile.y][tile.x] = TileType::Empty;
		wasWall = true;
	}

	if (wasWall)
	{
		spawnWallBreakFXAtTile(tile);

		const double scale = camera().getScale();
		const double shakeIntensity = Min(3.0 / Max(0.001, scale), 10.0 / Max(0.001, scale));
		camera().shake(0.1, shakeIntensity);
	}
}

void InGameScene::setWarningAtTile(Point tile, bool on)
{
	if (!isInsideMap(tile)) return;

	if (static_cast<int32>(overlayWarn_.size()) != getMapHeight() ||
		overlayWarn_.isEmpty() ||
		static_cast<int32>(overlayWarn_[0].size()) != getMapWidth())
	{
		overlayWarn_.assign(getMapHeight(), Array<OverlayType>(getMapWidth(), OverlayType::None));
	}

	overlayWarn_[tile.y][tile.x] = on ? OverlayType::Warning : OverlayType::None;
}

void InGameScene::loadRandomPattern()
{
	Array<int32> availablePatterns = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	if (patternHistory_.size() >= 2)
	{
		availablePatterns.remove_if([this](int32 pattern) {
			return patternHistory_.contains(pattern);
		});
	}

	if (availablePatterns.isEmpty())
	{
		availablePatterns = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
		patternHistory_.clear();
	}

	const int32 selectedPattern = availablePatterns.choice();

	patternHistory_.push_back(selectedPattern);
	if (patternHistory_.size() > 2)
	{
		patternHistory_.pop_front();
	}

	switch (selectedPattern)
	{
	case 0: currentWallPattern_ = BossWallPatternData::getVerticalFillPattern(); break;
	case 1: currentWallPattern_ = BossWallPatternData::getHorizontalWavePattern(); break;
	case 2: currentWallPattern_ = BossWallPatternData::getVerticalWavePattern(); break;
	case 3: currentWallPattern_ = BossWallPatternData::getCrossPattern(); break;
	case 4: currentWallPattern_ = BossWallPatternData::getDiagonalXPattern(); break;
	case 5: currentWallPattern_ = BossWallPatternData::getSpikePattern(); break;
	case 6: currentWallPattern_ = BossWallPatternData::getAlternatingPattern(); break;
	case 7: currentWallPattern_ = BossWallPatternData::getMazePattern(); break;
	case 8: currentWallPattern_ = BossWallPatternData::getCheckerboardPattern(); break;
	case 9: currentWallPattern_ = BossWallPatternData::getRandomBulletPattern(); break;
	case 10: currentWallPattern_ = BossWallPatternData::getRotatingPattern(); break;
	case 11: currentWallPattern_ = BossWallPatternData::getBoxShrinkPattern(); break;
	default: currentWallPattern_ = BossWallPatternData::getVerticalFillPattern(); break;
	}
}

void InGameScene::initBossPhaseSystem()
{
	currentBossPhase_ = 1;
	bossHitCount_ = 0;
	bossCurrentHP_ = 3;
	bossHitEffectTimer_ = 0.0;
	showBossHitEffect_ = false;

	loadBossPhase(1);
}

void InGameScene::loadBossPhase(int32 phase)
{
	currentBossPhase_ = phase;

	const Array<String> phaseMap = StageData::getFinalStageMapForPhase(phase);

	boxes_.clear();
	items_.clear();
	redGoalPositions_.clear();
	yellowGoalPositions_.clear();
	blueGoalPositions_.clear();
	orangeGoalPositions_.clear();
	greenGoalPositions_.clear();
	violetGoalPositions_.clear();
	blackGoalPositions_.clear();

	const int32 mapHeight = static_cast<int32>(phaseMap.size());
	int32 mapWidth = 0;
	for (const auto& line : phaseMap)
	{
		mapWidth = Max(mapWidth, static_cast<int32>(line.size()));
	}

	mapData_.assign(mapHeight, Array<TileType>(mapWidth, TileType::Empty));

	Point playerStart(1, 1);
	bool foundPlayer = false;

	for (int32 y = 0; y < mapHeight; ++y)
	{
		const String& line = phaseMap[y];
		for (int32 x = 0; x < static_cast<int32>(line.size()); ++x)
		{
			const char32 ch = line[x];
			const Point pos(x, y);

			switch (ch)
			{
			case U' ':
				mapData_[y][x] = TileType::Empty;
				break;

			case U'#':
				mapData_[y][x] = TileType::Wall;
				break;

			case U'T':
				mapData_[y][x] = TileType::Empty;
				playerStart = pos;
				foundPlayer = true;
				break;

			case U'R':
				mapData_[y][x] = TileType::Empty;
				boxes_.push_back(ColorBox{ pos, BoxColor::Red, 0.0, nextBoxUID_++ });
				break;
			case U'r':
				mapData_[y][x] = TileType::RedGoal;
				redGoalPositions_.push_back(pos);
				break;

			case U'Y':
				mapData_[y][x] = TileType::Empty;
				boxes_.push_back(ColorBox{ pos, BoxColor::Yellow, 0.0, nextBoxUID_++ });
				break;
			case U'y':
				mapData_[y][x] = TileType::YellowGoal;
				yellowGoalPositions_.push_back(pos);
				break;

			case U'B':
				mapData_[y][x] = TileType::Empty;
				boxes_.push_back(ColorBox{ pos, BoxColor::Blue, 0.0, nextBoxUID_++ });
				break;
			case U'b':
				mapData_[y][x] = TileType::BlueGoal;
				blueGoalPositions_.push_back(pos);
				break;

			case U'O':
				mapData_[y][x] = TileType::Empty;
				boxes_.push_back(ColorBox{ pos, BoxColor::Orange, 0.0, nextBoxUID_++ });
				break;
			case U'o':
				mapData_[y][x] = TileType::OrangeGoal;
				orangeGoalPositions_.push_back(pos);
				break;

			case U'G':
				mapData_[y][x] = TileType::Empty;
				boxes_.push_back(ColorBox{ pos, BoxColor::Green, 0.0, nextBoxUID_++ });
				break;
			case U'g':
				mapData_[y][x] = TileType::GreenGoal;
				greenGoalPositions_.push_back(pos);
				break;

			case U'V':
				mapData_[y][x] = TileType::Empty;
				boxes_.push_back(ColorBox{ pos, BoxColor::Violet, 0.0, nextBoxUID_++ });
				break;
			case U'v':
				mapData_[y][x] = TileType::VioletGoal;
				violetGoalPositions_.push_back(pos);
				break;

			default:
				mapData_[y][x] = TileType::Empty;
				break;
			}
		}
	}
	if (foundPlayer)
	{
		playerPos_ = playerStart;
	}
	else
	{
		bool placed = false;
		for (int32 y = 0; y < mapHeight && !placed; ++y)
		{
			for (int32 x = 0; x < mapWidth && !placed; ++x)
			{
				if (mapData_[y][x] == TileType::Empty && !getBoxAt(Point(x, y)))
				{
					playerPos_ = Point(x, y);
					placed = true;
				}
			}
		}

		if (!placed)
		{
			playerPos_ = Point(1, 1);
		}
	}

	playerPixelPos_ = tileToPixel(playerPos_);
	targetPixelPos_ = playerPixelPos_;
	isPlayerMoving_ = false;

	wallMask_.assign(mapHeight, Array<bool>(mapWidth, false));
	overlayWarn_.assign(mapHeight, Array<OverlayType>(mapWidth, OverlayType::None));

	isPlayerDead_ = false;
	deathAnimTimer_ = 0.0;
	deathParticles_.clear();

	initBossWallPatternSystem();

}

void InGameScene::advanceToNextBossPhase()
{
	if (currentBossPhase_ >= 3)
	{
		isCleared_ = true;
		score_ += 1000;
		showClearEffect_ = true;
		clearEffectTimer_ = 0.0;
		createClearEffect();

		if (!stageClearSound_.isEmpty())
		{
			stageClearSound_.playOneShot(0.9);
		}

		if (gameData_)
		{
			gameData_->clearStage(currentStage_);
		}

		return;
	}

	currentBossPhase_++;
	loadBossPhase(currentBossPhase_);
}

bool InGameScene::checkAllGoalsFilledForBoss() const
{
	if (!StageData::isFinalStage(currentStage_)) return false;

	const Array<BoxColor> allColors = {
		BoxColor::Red, BoxColor::Yellow, BoxColor::Blue,
		BoxColor::Orange, BoxColor::Green, BoxColor::Violet
	};

	for (const auto& color : allColors)
	{
		const Array<Point>& goals = getGoalPositionsForColor(color);
		for (const auto& goal : goals)
		{
			const ColorBox* box = getBoxAt(goal);
			if (!box || box->color != color)
			{
				return false;
			}
		}
	}

	return true;
}

Point InGameScene::getMapCenterTile() const
{
	return Point(getMapWidth() / 2, getMapHeight() / 2);
}

Vec2 InGameScene::getBossPositionForAttack() const
{
	return getBossStartUIPos();
}

void InGameScene::startBossAttackSequence()
{
	if (isBossAttackSequenceActive_) return;

	isBossAttackSequenceActive_ = true;
	bossAttackSequenceTimer_ = 0.0;
	currentBossAttackPhase_ = BossAttackPhase::GatherBoxes;

	isPlayingWallPattern_ = false;

	gatheringBoxes_.clear();

	const Array<BoxColor> allColors = {
		BoxColor::Red, BoxColor::Yellow, BoxColor::Blue,
		BoxColor::Orange, BoxColor::Green, BoxColor::Violet
	};

	for (const auto& color : allColors)
	{
		const Array<Point>& goals = getGoalPositionsForColor(color);
		for (const auto& goal : goals)
		{
			ColorBox* box = getBoxAt(goal);
			if (box && box->color == color)
			{
				BoxGatherData data;
				data.originalPos = box->pos;
				data.currentPixelPos = tileToPixel(box->pos);
				data.color = box->color;
				data.gathered = false;
				gatheringBoxes_.push_back(data);

				removeBoxByUid(box->uid);
			}
		}
	}

	const Point centerTile = getMapCenterTile();
	mergedBoxPixelPos_ = tileToPixel(centerTile);
	mergedBoxCreated_ = false;

}

void InGameScene::updateBossAttackSequence(double dt)
{
	if (!isBossAttackSequenceActive_) return;

	bossAttackSequenceTimer_ += dt;

	auto& holo = g_Shaders.holographic();

	switch (currentBossAttackPhase_)
	{
	case BossAttackPhase::GatherBoxes:
	{
		const double gatherDuration = 1.5;
		const double t = Min(bossAttackSequenceTimer_ / gatherDuration, 1.0);
		const double eased = 1.0 - Math::Pow(1.0 - t, 3.0);

		const Point centerTile = getMapCenterTile();
		const Vec2 targetPos = tileToPixel(centerTile);

		bool allGathered = true;
		for (auto& boxData : gatheringBoxes_)
		{
			if (!boxData.gathered)
			{
				boxData.currentPixelPos = Math::Lerp(
					tileToPixel(boxData.originalPos),
					targetPos,
					eased
				);

				if (eased >= 0.99)
				{
					boxData.gathered = true;
				}
				else
				{
					allGathered = false;
				}
			}
		}

		if (allGathered && t >= 1.0)
		{
			currentBossAttackPhase_ = BossAttackPhase::MergeEffect;
			bossAttackSequenceTimer_ = 0.0;

			const double scale = camera().getScale();
			camera().shake(0.3, 15.0 / Max(0.001, scale));
		}
		break;
	}

	case BossAttackPhase::MergeEffect:
	{
		const double mergeDuration = 0.5;

		if (bossAttackSequenceTimer_ >= mergeDuration)
		{
			mergedBoxCreated_ = true;
			currentBossAttackPhase_ = BossAttackPhase::ChargeRainbow;
			bossAttackSequenceTimer_ = 0.0;

			holo.setRainbowMode(true);
			holo.setScale(0.08f);
			holo.setSpeed(2.5f);
			holo.setIntensity(0.0f);
		}
		break;
	}

	case BossAttackPhase::ChargeRainbow:
	{
		const double chargeDuration = 1.0;
		const double t = Min(bossAttackSequenceTimer_ / chargeDuration, 1.0);

		holo.setIntensity(static_cast<float>(t * 1.2f));

		if (t >= 1.0)
		{
			currentBossAttackPhase_ = BossAttackPhase::LaunchTowardsBoss;
			bossAttackSequenceTimer_ = 0.0;
		}
		break;
	}

	case BossAttackPhase::LaunchTowardsBoss:
	{
		const double launchDuration = 0.8;
		const double t = Min(bossAttackSequenceTimer_ / launchDuration, 1.0);
		const double eased = Math::Pow(t, 2.0);

		const Point centerTile = getMapCenterTile();
		const Vec2 startPos = tileToPixel(centerTile);
		const Vec2 bossPos = getBossPositionForAttack();

		const Mat3x2 transform = camera().getMat3x2();
		const Vec2 startScreen = transform.transformPoint(startPos);

		mergedBoxPixelPos_ = Math::Lerp(startScreen, bossPos, eased);


		if (t >= 1.0)
		{
			currentBossAttackPhase_ = BossAttackPhase::BossHit;
			bossAttackSequenceTimer_ = 0.0;

			const double scale = camera().getScale();
			camera().shake(0.5, 30.0 / Max(0.001, scale));

			bossCurrentHP_--;
			showBossHitEffect_ = true;
			bossHitEffectTimer_ = 0.0;
		}
		break;
	}

	case BossAttackPhase::BossHit:
	{
		const double hitDuration = 0.5;

		if (bossAttackSequenceTimer_ >= hitDuration)
		{
			currentBossAttackPhase_ = BossAttackPhase::Complete;
			bossAttackSequenceTimer_ = 0.0;
		}
		break;
	}

	case BossAttackPhase::Complete:
	{
		if (bossAttackSequenceTimer_ >= 0.5)
		{
			isBossAttackSequenceActive_ = false;

			holo.setRainbowMode(false);
			holo.setIntensity(0.0f);

			// ★ 보스 HP 확인
			bossHitCount_++;

			if (bossHitCount_ >= 3)
			{
				// 최종 클리어
				isCleared_ = true;
				score_ += 1000;
				showClearEffect_ = true;
				clearEffectTimer_ = 0.0;
				createClearEffect();

				if (!stageClearSound_.isEmpty())
				{
					stageClearSound_.playOneShot(0.9);
				}

				if (gameData_)
				{
					gameData_->clearStage(currentStage_);
				}

			}
			else
			{
				// 다음 페이즈로
				advanceToNextBossPhase();
			}
		}
		break;
	}

	default:
		break;
	}
}

void InGameScene::damageBoss()
{
	bossCurrentHP_--;
	showBossHitEffect_ = true;
	bossHitEffectTimer_ = 0.0;

}

void InGameScene::updateBossHitEffect(double dt)
{
	if (!showBossHitEffect_) return;

	bossHitEffectTimer_ += dt;

	if (bossHitEffectTimer_ >= 1.0)
	{
		showBossHitEffect_ = false;
		bossHitEffectTimer_ = 0.0;
	}
}

void InGameScene::drawBossAttackSequence()
{
	if (!isBossAttackSequenceActive_) return;

	auto& holo = g_Shaders.holographic();
	const ScopedRenderStates2D blend(BlendState::Additive);

	switch (currentBossAttackPhase_)
	{
	case BossAttackPhase::GatherBoxes:
	{
		for (const auto& boxData : gatheringBoxes_)
		{
			const ColorF color = getBoxColorF(boxData.color);
			const RectF boxRect = RectF(Arg::center = boxData.currentPixelPos, TILE_SIZE - 16, TILE_SIZE - 16);

			boxRect.draw(color);
			boxRect.drawFrame(3, 0, ColorF(color.r * 1.2, color.g * 1.2, color.b * 1.2));
			Circle(boxData.currentPixelPos, 8).draw(ColorF(color, 0.8));
		}
		break;
	}

	case BossAttackPhase::MergeEffect:
	{
		const double t = bossAttackSequenceTimer_ / 0.5;
		const double pulseSize = 50.0 * (1.0 + Math::Sin(t * Math::TwoPi * 3.0) * 0.3);

		Circle(mergedBoxPixelPos_, pulseSize).draw(ColorF(1.0, 1.0, 1.0, 0.3 * (1.0 - t)));

		for (const auto& boxData : gatheringBoxes_)
		{
			const ColorF color = getBoxColorF(boxData.color);
			const RectF boxRect = RectF(Arg::center = boxData.currentPixelPos, TILE_SIZE - 16, TILE_SIZE - 16);

			boxRect.draw(ColorF(color, 1.0 - t));
		}
		break;
	}

	case BossAttackPhase::ChargeRainbow:
	{
		const RectF boxRect = RectF(Arg::center = mergedBoxPixelPos_, TILE_SIZE - 8, TILE_SIZE - 8);

		const auto scope = holo.scopedTexture(Texture());
		boxRect.draw(ColorF(1.0, 1.0, 1.0));

		const double t = bossAttackSequenceTimer_ / 1.0;
		const double pulse = 0.5 + 0.5 * Math::Sin(t * Math::TwoPi * 4.0);
		const double radius = 30.0 + 20.0 * t;

		Circle(mergedBoxPixelPos_, radius * pulse).drawFrame(3, ColorF(1.0, 1.0, 1.0, 0.5 * pulse));
		break;
	}

	case BossAttackPhase::LaunchTowardsBoss:
	{
		const double t = bossAttackSequenceTimer_ / 0.8;
		const double size = TILE_SIZE - 8 + t * 20.0;

		const RectF boxRect = RectF(Arg::center = mergedBoxPixelPos_, size, size);

		const auto scope = holo.scopedTexture(Texture());
		boxRect.draw(ColorF(1.0, 1.0, 1.0));

		const Point centerTile = getMapCenterTile();
		const Vec2 startPos = tileToPixel(centerTile);
		const Mat3x2 transform = camera().getMat3x2();
		const Vec2 startScreen = transform.transformPoint(startPos);
		const Vec2 bossPos = getBossPositionForAttack();

		const int trailCount = 8;
		for (int i = 0; i < trailCount; ++i)
		{
			const double trailT = Max(0.0, t - i * 0.1);
			if (trailT <= 0.0) continue;

			const Vec2 trailPos = Math::Lerp(startScreen, bossPos, Math::Pow(trailT, 2.0));
			const double trailAlpha = (1.0 - static_cast<double>(i) / trailCount) * 0.5;
			const double trailSize = size * (1.0 - static_cast<double>(i) / trailCount * 0.5);

			Circle(trailPos, trailSize * 0.5).draw(ColorF(1.0, 1.0, 1.0, trailAlpha));
		}
		break;
	}

	case BossAttackPhase::BossHit:
	{
		const double t = bossAttackSequenceTimer_ / 0.5;
		const Vec2 bossPos = getBossPositionForAttack();

		const double explosionRadius = t * 200.0;
		Circle(bossPos, explosionRadius).drawFrame(10, ColorF(1.0, 1.0, 1.0, 1.0 - t));

		const double flashSize = 50.0 * (1.0 - t);
		Circle(bossPos, flashSize).draw(ColorF(1.0, 1.0, 1.0, 1.0 - t));

		for (int i = 0; i < 20; ++i)
		{
			const double angle = i * Math::TwoPi / 20.0;
			const Vec2 dir = Vec2(Math::Cos(angle), Math::Sin(angle));
			const double dist = t * 150.0;
			const Vec2 particlePos = bossPos + dir * dist;

			Circle(particlePos, 5.0 * (1.0 - t)).draw(ColorF(1.0, 1.0, 1.0, 0.8 * (1.0 - t)));
		}
		break;
	}

	default:
		break;
	}
}

void InGameScene::drawBossHP()
{
	if (!StageData::isFinalStage(currentStage_)) return;

	const double centerX = Scene::Width() / 2.0;
	const double y = 50.0;

	const String hpText = U"BOSS HP: {} / {}";
	gameFont_(Format(hpText, bossCurrentHP_, bossMaxHP_)).drawAt(centerX, y, ColorF(1.0, 0.3, 0.3));

	const double barWidth = 400.0;
	const double barHeight = 30.0;
	const RectF barBg(Arg::center(centerX, y + 40), barWidth, barHeight);
	const double hpRatio = static_cast<double>(bossCurrentHP_) / bossMaxHP_;
	const RectF barFill(barBg.pos, barWidth * hpRatio, barHeight);

	barBg.draw(ColorF(0.2, 0.2, 0.2, 0.8));
	barFill.draw(ColorF(1.0, 0.2, 0.2, 0.9));
	barBg.drawFrame(3, ColorF(0.8, 0.8, 0.8));

	if (showBossHitEffect_)
	{
		const double t = bossHitEffectTimer_ / 1.0;
		const double alpha = (1.0 - t) * 0.5;
		Rect(0, 0, Scene::Width(), Scene::Height()).draw(ColorF(1.0, 1.0, 1.0, alpha));
	}
}
