#include "InGameScene.hpp"

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
, gameFont_(20)  // 폰트 크기 증가
, debugFont_(16)  // 폰트 크기 증가
, clearFont_(64, Typeface::Bold)  // 폰트 크기 증가
, buttonFont_(24)  // 클리어 버튼용 폰트 추가
, gameTime_(0.0)
, score_(0)
, moves_(0)
, isCleared_(false)
, showClearButtons_(false)  // 클리어 버튼 표시 상태 초기화
, showHelpScreen_(false)  // 조작법 도움말 화면 초기화
, stageBackground_(U"ArtResources/Texture2D/BG_K.png")
, gameData_(gameData)
, undoHoldTime_(0.0)  // Undo 홀드 시간 초기화
, undoCooldown_(0.0)  // Undo 쿨다운 초기화
{
    currentScene_ = SceneType::InGame;
    // Print << U"InGameScene created with stage number: {}"_fmt(stageNumber);
    
    // 초기 픽셀 위치 설정
    playerPixelPos_ = tileToPixel(playerPos_);
    targetPixelPos_ = playerPixelPos_;
    
    loadAssets();
    loadStage(currentStage_);
    initializeClearButtons();  // 클리어 버튼 초기화
    
    // 초기 게임 상태 저장
    saveGameState();
}

void InGameScene::onEnter()
{
    gameTime_ = 0.0;
    score_ = 0;
    moves_ = 0;
    isCleared_ = false;
    showClearButtons_ = false;  // 클리어 버튼 숨김
    showHelpScreen_ = false;  // 조작법 도움말 숨김
    playerHeldItem_ = ItemType::None;  // 아이템 초기화
    tacoDirection_ = TacoDirection::Down;
    isFacingLeft_ = false;
    tacoAnimFrame_ = 0;
    isPlayerMoving_ = false;
    
    // 클리어 이펙트 완전 초기화
    showClearEffect_ = false;
    clearEffectTimer_ = 0.0;
    clearParticles_.clear();
    
    loadStage(currentStage_);
    
    // 픽셀 위치 재설정
    playerPixelPos_ = tileToPixel(playerPos_);
    targetPixelPos_ = playerPixelPos_;
    
    // 게임 상태 기록 초기화
    gameStateHistory_.clear();
    saveGameState();
    
    // 배경음악 재생
    if (!bgm_.isEmpty())
    {
        bgm_.setVolume(0.5);  // 볼륨 설정 (0.0 ~ 1.0)
        bgm_.play();
    }
}

void InGameScene::onExit()
{
    // 배경음악 정지
    if (!bgm_.isEmpty() && bgm_.isPlaying())
    {
        bgm_.stop();
    }
}

void InGameScene::loadAssets()
{
    tacoDownFrames_.clear();
    tacoSideFrames_.clear();
    tacoUpFrames_.clear();
    for (int32 i = 0; i < 2; i++)
    {
        Texture downFrame{ U"ArtResources/Texture2D/Taco/tacoMoveDown_{}.png"_fmt(i) };
        if (!downFrame.isEmpty()) tacoDownFrames_.push_back(downFrame);
        Texture sideFrame{ U"ArtResources/Texture2D/Taco/tacoMoveSide_{}.png"_fmt(i) };
        if (!sideFrame.isEmpty()) tacoSideFrames_.push_back(sideFrame);
        Texture upFrame{ U"ArtResources/Texture2D/Taco/tacoMoveUp_{}.png"_fmt(i) };
        if (!upFrame.isEmpty()) tacoUpFrames_.push_back(upFrame);
    }
    
    // 타코 점수 이미지 로드
    tacoScoreTexture_ = Texture{ U"ArtResources/Texture2D/Menu/Score/tacoScoreOn.png" };
    tacoScoreOffTexture_ = Texture{ U"ArtResources/Texture2D/Menu/Score/tacoScoreOff.png" };
    
    // 타코 페인트 애니메이션 프레임 로드 (tacoPaint_0.png ~ tacoPaint_4.png)
    tacoPaintFrames_.clear();
    for (int32 i = 0; i < PAINT_ANIM_FRAME_COUNT; i++)
    {
        Texture paintFrame{ U"ArtResources/Texture2D/Taco/Color/tacoPaint_{}.png"_fmt(i) };
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
    bgm_ = Audio{ U"ArtResources/BGM/HappyOcean.mp3", Loop::Yes };
}

void InGameScene::loadStage(int32 stageNumber)
{
    currentStage_ = stageNumber;
    // Print << U"Loading stage: {}"_fmt(stageNumber);
    
    // 맵 초기화
    mapData_.clear();
    mapData_.resize(MAP_HEIGHT);
    for (auto& row : mapData_)
    {
        row.resize(MAP_WIDTH, TileType::Empty);
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
    
    // StageData에서 맵 데이터 로드
    Array<String> mapText = StageData::getStageMap(stageNumber);
    // Print << U"Map data loaded, lines: {}"_fmt(mapText.size());
    loadStageFromText(mapText);
}

void InGameScene::loadStageFromText(const Array<String>& mapText)
{
    // 맵 초기화
    for (auto& row : mapData_)
    {
        row.assign(MAP_WIDTH, TileType::Empty);
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
    for (int32 y = 0; y < Min((int32)mapText.size(), MAP_HEIGHT); y++)
    {
        const String& line = mapText[y];
        for (int32 x = 0; x < Min((int32)line.length(), MAP_WIDTH); x++)
        {
            char32 ch = line[x];
            Point pos(x, y);
            
            switch (ch)
            {
            case U'#':  // 벽
                mapData_[y][x] = TileType::Wall;
                break;
                
            case U'T':  // 플레이어 시작 위치 (Taco)
                playerPos_ = pos;
                playerPixelPos_ = tileToPixel(pos);
                targetPixelPos_ = playerPixelPos_;
                isPlayerMoving_ = false;
                break;
                
            // 대문자 = 상자
            case U'R':  // 빨간 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Red });
                break;
                
            case U'Y':  // 노란 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Yellow });
                break;
                
            case U'B':  // 파란 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Blue });
                break;
                
            case U'O':  // 주황 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Orange });
                break;
                
            case U'G':  // 초록 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Green });
                break;
                
            case U'V':  // 보라 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Violet });
                break;
                
            case U'K':  // 검은 상자
                boxes_.push_back(ColorBox{ pos, BoxColor::Black });
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

	g_Shaders.paintSpread().updateProgress(Scene::DeltaTime()); // 즉시 누적[attached_file:36]

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


bool InGameScene::removeBoxByUid(uint64 uid)
{
	const size_t n0 = boxes_.size();
	boxes_.remove_if([&](const ColorBox& b) { return (b.uid == uid); });
	return boxes_.size() != n0;
}

void InGameScene::triggerBombBoxFXForBlack_Multi(uint64 uid, double durationSec)
{
	BombBoxInst inst;
	inst.uid = uid;
	inst.t = 0.0;
	inst.dur = Max(0.05, durationSec);
	inst.seed = static_cast<uint32>(RandomUint32()); // 선택
	bombFXs_.push_back(inst);
}

void InGameScene::updateBombBoxFX_Multi()
{
	if (bombFXs_.isEmpty()) return;

	const double dt = Scene::DeltaTime();

	// 시간 누적
	for (auto& fx : bombFXs_) {
		fx.t += dt;
	}

	// 만료된 인스턴스 정리: 박스 제거 후 리스트에서 삭제
	bombFXs_.remove_if([&](const BombBoxInst& fx) {
		if (fx.t < fx.dur) return false;
		removeBoxByUid(fx.uid); // 폭발 타이밍과 동시에 블럭 제거
		return true;
	});
}

void InGameScene::drawBombBoxFX_Multi() {
	for (const auto& fx : bombFXs_) {
		if (const ColorBox* b = getBoxByUid(fx.uid)) {
			const Rect boxRect(b->pos.x * TILE_SIZE, b->pos.y * TILE_SIZE,
							   TILE_SIZE, TILE_SIZE);
			RectF inner = boxRect.stretched(-8);

			const float progress = static_cast<float>(Clamp(fx.t / fx.dur, 0.0, 1.0));

			BombBoxEffect::Params p;
			p.progress = progress;
			p.timeSeconds = static_cast<float>(fx.t);
			p.seed = fx.seed;
			p.baseColor = ColorF(0.9, 0.3, 0.3);

			p.pulseAmp = 0.08f;
			p.pulseSpeed = 5.0f;
			p.pulseCount = 3.0f;

			// *** 수정: 박스 크기에 비례하도록 조정 ***
			float boxSize = static_cast<float>(inner.w); // 또는 inner.h
			p.spread = boxSize * 3.0f;  // 220.0f 대신 비례값 (64 * 3 = 192)
			p.gravity = 800.0f;

			g_Shaders.bombBox().drawInst(inner, p);
		}
	}
}


bool InGameScene::canMoveTo(Point pos) const
{
    if (pos.x < 0 || pos.x >= MAP_WIDTH || pos.y < 0 || pos.y >= MAP_HEIGHT)
        return false;
    
    if (mapData_[pos.y][pos.x] == TileType::Wall)
        return false;
    
    if (getBoxAt(pos) != nullptr)
        return false;
    
    return true;
}

ColorBox* InGameScene::getBoxAt(Point pos)
{
    for (auto& box : boxes_)
    {
        if (box.pos == pos)
            return &box;
    }
    return nullptr;
}

const ColorBox* InGameScene::getBoxAt(Point pos) const
{
    for (const auto& box : boxes_)
    {
        if (box.pos == pos)
            return &box;
    }
    return nullptr;
}

bool InGameScene::canPushBox(Point playerPos, Point boxPos, Point direction) const
{
    Point nextBoxPos = boxPos + direction;
    
    if (nextBoxPos.x < 0 || nextBoxPos.x >= MAP_WIDTH || 
        nextBoxPos.y < 0 || nextBoxPos.y >= MAP_HEIGHT)  // nextBox -> nextBoxPos 수정
        return false;
    
    if (mapData_[nextBoxPos.y][nextBoxPos.x] == TileType::Wall)
        return false;
    
    // 목표 위치에 상자가 있는지 확인
    const ColorBox* targetBox = getBoxAt(nextBoxPos);
    if (targetBox)
    {
        // 합성 가능한 경우만 true 반환
        const ColorBox* currentBox = getBoxAt(boxPos);
        if (currentBox)
        {
            Optional<BoxColor> mergedColor = getMergedColor(currentBox->color, targetBox->color);
            if (mergedColor)
            {
                return true;  // 합성 가능!
            }
        }
        // 합성 불가능하면 밀 수 없음
        return false;
    }
    
    return true;
}

void InGameScene::pushBox(ColorBox* box, Point direction)
{
    box->pos = box->pos + direction;
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
        return ColorF{ 0.6, 0.3, 0.3 };      // 빨강 (채도 낮춤)
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

void InGameScene::update()
{
	// 0) 페인트 스프레드 진행을 '항상' 최우선 갱신 (지연 제거 핵심)
	updateMergePaintFX(); // ★★

	updateBombBoxFX_Multi();

	// 포커스/도움말 토글
	const bool focused = Window::GetState().focused;
	if (!focused && !showHelpScreen_ && !isCleared_) {
		showHelpScreen_ = true;
	}
	if (KeyEscape.down()) {
		showHelpScreen_ = !showHelpScreen_;
	}
	if (showHelpScreen_) {
		if (!bgm_.isEmpty() && bgm_.isPlaying()) {
			bgm_.pause();
		}
		// 조기 return 직전에도 1회 더 보장 호출
		updateMergePaintFX(); // ★★
		return;
	}
	if (!bgm_.isEmpty() && !bgm_.isPlaying() && focused && !isCleared_) {
		bgm_.play();
	}

	const double dt = Scene::DeltaTime();

	// R로 즉시 리트라이
	if (KeyR.down() && !isCleared_) {
		gameTime_ = 0.0;
		moves_ = 0;
		score_ = 0;
		playerHeldItem_ = ItemType::None;
		gameStateHistory_.clear();
		undoHoldTime_ = 0.0;
		undoCooldown_ = 0.0;
		loadStage(currentStage_);
		// 조기 return 보호
		updateMergePaintFX(); // ★★
		return;
	}

	// Undo 흐름
	if (!isCleared_ && canUndo()) {
		const bool undoPressed = KeyZ.pressed() || KeyBackspace.pressed();
		const bool undoDown = KeyZ.down() || KeyBackspace.down();
		if (undoPressed) {
			undoHoldTime_ += dt;
			if (undoDown) {
				undoLastMove();
				undoCooldown_ = UNDO_REPEAT_DELAY;
				// 조기 return 보호
				updateMergePaintFX(); // ★★
				return;
			}
			if (undoHoldTime_ >= UNDO_HOLD_THRESHOLD) {
				undoCooldown_ -= dt;
				if (undoCooldown_ <= 0.0) {
					undoLastMove();
					undoCooldown_ = UNDO_REPEAT_DELAY;
					// 조기 return 보호
					updateMergePaintFX(); // ★★
					return;
				}
			}
		}
		else {
			undoHoldTime_ = 0.0;
			undoCooldown_ = 0.0;
		}
	}
	else {
		undoHoldTime_ = 0.0;
		undoCooldown_ = 0.0;
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
				gameStateHistory_.clear();
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

		// 조기 return 보호
		updateMergePaintFX(); // ★★
		return;
	}

	// 일반 업데이트
	gameTime_ += dt;

	// 입력 → 합성 성공 분기에서 즉시 setOriginPoint + startAnimation 호출되어야 함
	handleInput();

	// 플레이어 및 일반 애니메이션
	updatePlayer();
	updateAnimations();

	// 별도 타코 페인트 애니메이션 (UI/연출용)
	updatePaintAnimation();

	// 진행 갱신을 한 번 더 보장 (다중 호출 안전)
	updateMergePaintFX(); // ★★

	// 블랙 박스 등 기타 업데이트
	updateBlackBoxes();

	// 클리어 판정
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
}


void InGameScene::handleInput()
{
	const double dt = Scene::DeltaTime();
	inputCooldown_ = Max(0.0, inputCooldown_ - dt);
	if (isPlayerMoving_ || isPlayingPaintAnimation_) return;

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
	press(KeyLeft, { -1, 0 }, TacoDirection::Side, true);
	press(KeyRight, { 1, 0 }, TacoDirection::Side, false);
	press(KeyUp, { 0,-1 }, TacoDirection::Up);
	press(KeyDown, { 0, 1 }, TacoDirection::Down);
	if (!moved) return;

	const Point newPos = playerPos_ + dir;
	if (ColorBox* box = getBoxAt(newPos)) {
		if (playerHeldItem_ != ItemType::None) {
			if (tryChangeBoxColor(newPos, dir)) {
				playerHeldItem_ = ItemType::None;
				if (tacoDirection_ != newDir || isFacingLeft_ != newFacingLeft) {
					tacoDirection_ = newDir; isFacingLeft_ = newFacingLeft;
					tacoAnimFrame_ = 0; tacoAnimTimer_ = 0.0;
				}
				saveGameState();
			}
			return;
		}

		const Point next = box->pos + dir;
		if (!canPushBox(playerPos_, box->pos, dir)) return;

		if (ColorBox* target = getBoxAt(next)) {
			if (Optional<BoxColor> merged = getMergedColor(box->color, target->color)) {
				// 1) 이펙트 파라미터 준비
				Vec2 originUV = (dir == Point{ 1, 0 }) ? Vec2(0.0, 0.5)
					: (dir == Point{ -1, 0 }) ? Vec2(1.0, 0.5)
					: (dir == Point{ 0, 1 }) ? Vec2(0.5, 0.0)
					: Vec2(0.5, 1.0);
				const ColorF base = getBoxColorF(target->color);
				const ColorF result = getBoxColorF(*merged);

				// 2) 실제 합성: 두 상자 제거 후 '타깃 기존색'으로 결과 위치에 새 박스 생성(UID 부여)
				const Point resultPos = next;
				const BoxColor visualColor = target->color; // 효과 중에는 시각적으로 기존색 유지
				boxes_.remove_if([&](const ColorBox& b) {
					return (b.pos == box->pos || b.pos == target->pos);
				});
				ColorBox newBox{ resultPos, visualColor, 0.0, nextBoxUID_++ }; // uid 부여
				if (*merged == BoxColor::Black) {
					//newBox.creationTime = gameTime_;
					triggerBombBoxFXForBlack_Multi(newBox.uid, 1.5);
				}
				boxes_.push_back(newBox);

				// 3) 이펙트 바인딩(UID 기준) + 즉시 시작
				mergeFX_.active = true;
				mergeFX_.baseColor = base;
				mergeFX_.paintColor = result;
				mergeFX_.originUV = originUV;
				mergeFX_.targetUid = newBox.uid;       // ← 타일이 아닌 UID로 바인딩
				mergeFX_.finalColor = *merged;
				mergeFX_.commitPending = true;
				g_Shaders.paintSpread().setPaintColor(result);
				g_Shaders.paintSpread().setOriginPoint(originUV);
				g_Shaders.paintSpread().setNoiseScale(1.0f);
				g_Shaders.paintSpread().setWaveStrength(0.4f);
				g_Shaders.paintSpread().setSpreadSpeed(2.0f);  // 0.6 → 2.0 (더 빠르게)
				g_Shaders.paintSpread().startAnimation();

				// 4) 점수/이동/저장
				const ColorTier tier = getColorTier(*merged);
				if (tier == ColorTier::Secondary) score_ += 50;
				else if (tier == ColorTier::Tertiary) score_ += 100;

				movePlayerTo(newPos);
				inputCooldown_ = moveDelay_;
				moves_ += 1;
				collectItem(newPos);
				saveGameState();

				if (tacoDirection_ != newDir || isFacingLeft_ != newFacingLeft) {
					tacoDirection_ = newDir; isFacingLeft_ = newFacingLeft;
					tacoAnimFrame_ = 0;     tacoAnimTimer_ = 0.0;
				}
				return;
			}
			return;
		}

		// 비어 있으면 일반 밀기
		pushBox(box, dir);
	}

	if (canMoveTo(newPos) || getBoxAt(newPos)) {
		movePlayerTo(newPos);
		inputCooldown_ = moveDelay_;
		moves_ += 1;
		collectItem(newPos);
		saveGameState();
		if (tacoDirection_ != newDir || isFacingLeft_ != newFacingLeft) {
			tacoDirection_ = newDir; isFacingLeft_ = newFacingLeft;
			tacoAnimFrame_ = 0;     tacoAnimTimer_ = 0.0;
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
    }
}

void InGameScene::draw()
{
    drawBackground();
    drawMap();
    drawPlayer();
    drawUI();
    
    // 조작법 도움말 표시 (포커스 잃었을 때도 표시)
    if (showHelpScreen_)
    {
        drawHelpScreen();
        return;  // 도움말이 표시 중이면 다른 UI는 그리지 않음
    }
    
    // 클리어 효과 - 버튼과 Move 횟수 표시
    if (isCleared_ && showClearButtons_)
    {
        // 배경 오버레이 표시
        Rect{ 0, 0, Scene::Size().x, Scene::Size().y }.draw(ColorF{ 0, 0, 0, 0.8 });
        
        // "STAGE CLEAR!" 텍스트
        clearFont_(U"STAGE CLEAR!")
            .drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 200, ColorF{ 1.0, 1.0, 0.5 });
        
        // Move 횟수 표시 (별점 이미지 아래)
        gameFont_(U"Moves: {}"_fmt(moves_))
            .drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 + 100, ColorF{ 1.0, 1.0, 1.0 });
        
        // 별점 계산 및 표시 (중앙)
        int32 starCount = calculateStars(moves_);
        drawStars(starCount, Vec2(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 10));
        
        // 별점에 따른 평가 텍스트 (별점 이미지 위)
        String ratingText;
        ColorF ratingColor;
        if (starCount == 3)
        {
            ratingText = U"Perfect!";
            ratingColor = ColorF{ 1.0, 1.0, 0.3 };  // 황금색
        }
        else if (starCount == 2)
        {
            ratingText = U"Great!";
            ratingColor = ColorF{ 0.9, 0.9, 0.9 };  // 은색
        }
        else
        {
            ratingText = U"Good!";
            ratingColor = ColorF{ 0.8, 0.5, 0.3 };  // 동색
        }
        
        gameFont_(ratingText).drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 - 120, ratingColor);
        
        // 버튼 표시
        // drawClearButtons();
        
        gameFont_(U"Press Space or Enter").drawAt(Scene::Size().x / 2.0, Scene::Size().y / 2.0 + 150, ColorF{ 0.8, 0.8, 0.8 });
    }
    
    // 클리어 이펙트 그리기 (가장 마지막에 - 모든 UI 위에)
    if (showClearEffect_)
    {
        drawClearEffect();
    }
}

void InGameScene::drawBackground()
{
    if (!stageBackground_.isEmpty())
    {
        stageBackground_.resized(1024, 1024).draw(0, 0);  // 1024x1024로 크기 조정
    }
    else
    {
        Scene::SetBackground(ColorF{ 0.1, 0.1, 0.15 });
    }
}

void InGameScene::drawMap()
{
	// 타일 배경
	for (int32 y = 0; y < MAP_HEIGHT; ++y) {
		for (int32 x = 0; x < MAP_WIDTH; ++x) {
			const Rect tileRect{ x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE };

			switch (mapData_[y][x]) {
			case TileType::Empty:
				tileRect.draw(ColorF{ 0.15, 0.15, 0.2, 0.3 });
				break;
			case TileType::Wall:
				tileRect.draw(ColorF{ 0.3, 0.3, 0.35 });
				tileRect.drawFrame(2, 0, ColorF{ 0.5, 0.5, 0.55 });
				break;
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
	for (const auto& box : boxes_) {
		const Rect boxRect{ box.pos.x * TILE_SIZE, box.pos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE };
		const bool isFX = (mergeFX_.active && (box.uid == mergeFX_.targetUid));

		// 목표 위 빛남
		bool onGoal = false;
		const Array<Point>& goals = getGoalPositionsForColor(box.color);
		for (const auto& g : goals) {
			if (box.pos == g) { onGoal = true; break; }
		}
		if (onGoal) {
			Circle{ boxRect.center(), 32 }.draw(ColorF{ getBoxColorF(box.color), 0.3 });
		}

		if (isFX)
		{
			g_Shaders.paintSpread().setPaintColor(mergeFX_.paintColor);
			g_Shaders.paintSpread().setOriginPoint(mergeFX_.originUV);

			RectF inner = boxRect.stretched(-8);          // 기준 사각형(원래 채움과 동일 크기)
			constexpr double frameThickness = 3.0;              // 기존 drawFrame 두께와 동일하게
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
		else
		{
			// 기존 일반 렌더 (채움 + 프레임 + 중앙 점 등)
			const ColorF c = getBoxColorF(box.color);
			boxRect.stretched(-8).draw(c);
			boxRect.stretched(-8).drawFrame(3, 0, ColorF{ c.r * 1.2, c.g * 1.2, c.b * 1.2 });
			Circle{ boxRect.center(), 8 }.draw(ColorF{ c, 0.8 });
		}
	}

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
	//폭발 이펙트(모든 인스턴스) 호출
	drawBombBoxFX_Multi();
}



void InGameScene::drawPlayer()
{
    // 페인트 애니메이션이 재생 중이면 페인트 애니메이션을 그림
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
    
    // Fallback - 픽셀 위치의 중심에 그리기
    Circle{ playerPixelPos_, TILE_SIZE / 2 - 8 }.draw(playerColor_);
    Circle{ playerPixelPos_, TILE_SIZE / 2 - 8 }.drawFrame(3, 0, ColorF{ 1.0, 1.0, 1.0, 0.9 });
    Circle{ playerPixelPos_, 8 }.draw(ColorF{ 1.0, 1.0, 1.0, 0.8 });
}

void InGameScene::drawUI()
{
    // UI 패널 크기 반으로 축소 (300 -> 150)
    Rect{ 0, 0, 150, 180 }.draw(ColorF{ 0, 0, 0, 0.5 });
    gameFont_(U"Stage: {}"_fmt(currentStage_)).draw(Vec2{ 16, 16 }, ColorF{ 0.8, 0.8, 1.0 });
    gameFont_(U"Moves: {}"_fmt(moves_)).draw(Vec2{ 16, 45 }, ColorF{ 1.0, 1.0, 0.5 });
    gameFont_(U"Time: {:.1f}s"_fmt(gameTime_)).draw(Vec2{ 16, 74 }, ColorF{ 0.5, 1.0, 0.5 });
    
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
    
    gameFont_(U"Goals: {}/{}"_fmt(cleared, totalGoals))
        .draw(Vec2{ 16, 103 }, ColorF{ 0.9, 0.9, 0.9 });
    
    // 현재 가진 아이템 표시
    if (playerHeldItem_ != ItemType::None)
    {
        gameFont_(U"Item: Held").draw(Vec2{ 16, 132 }, getItemColorF(playerHeldItem_));
    }
    else
    {
        gameFont_(U"Item: None").draw(Vec2{ 16, 132 }, ColorF{ 0.6, 0.6, 0.6 });
    }
    
    // 하단 정보 패널 위치 조정
    Rect{ 0, 940, 1024, 84 }.draw(ColorF{ 0, 0, 0, 0.5 });
    debugFont_(U"Arrow: Move/Push | ESC: Menu | R: Retry | Z/Backspace: Undo").draw(Vec2{ 16, 950 }, ColorF{ 0.8, 0.8, 0.9 });
    debugFont_(U"ROYGBVK: Match colors to goals (lowercase)! | Undo: {}/{}"_fmt(gameStateHistory_.size() - 1, MAX_UNDO_STEPS)).draw(Vec2{ 16, 975 }, ColorF{ 0.9, 0.9, 0.9 });
}

void InGameScene::drawHelpScreen()
{
    // 반투명 배경 오버레이
    Rect{ 0, 0, Scene::Size().x, Scene::Size().y }.draw(ColorF{ 0, 0, 0, 0.8 });
    
    // 제목
    clearFont_(U"PAUSED").drawAt(Scene::Size().x / 2.0, 100, ColorF{ 1.0, 1.0, 0.5 });
    
    // 조작법 설명
    const double centerX = Scene::Size().x / 2.0;
    const double startY = 200;
    const double lineHeight = 50;
    
    Array<String> helpTexts = {
        U"↑ ↓ ← → : Move / Push boxes",
        U"R : Restart current stage",
        U"Z / Backspace : Undo last move",
        U"Space / Enter : Next stage (when cleared)",
        U"ESC : Resume / Show this menu"
    };
    
    for (size_t i = 0; i < helpTexts.size(); i++)
    {
        ColorF textColor = ColorF{ 0.9, 0.9, 0.9 };
        gameFont_(helpTexts[i]).drawAt(centerX, startY + i * lineHeight, textColor);
    }
    
    // 스테이지 선택으로 돌아가는 버튼
    const double buttonY = startY + helpTexts.size() * lineHeight + 60;
    const Rect backButton{ static_cast<int32>(centerX - 100), static_cast<int32>(buttonY), 200, 50 };
    
    // 버튼 배경
    ColorF buttonColor = backButton.mouseOver() ? ColorF{ 0.3, 0.5, 0.7 } : ColorF{ 0.2, 0.4, 0.6 };
    backButton.draw(buttonColor);
    backButton.drawFrame(3, Palette::White);
    
    // 버튼 텍스트
    ColorF textColor = backButton.mouseOver() ? Palette::Yellow : Palette::White;
    gameFont_(U"Back to Stage Select").drawAt(backButton.center(), textColor);
    
    // 버튼 클릭 처리
    if (backButton.leftClicked())
    {
        changeScene(SceneType::StageSelect);
    }
    
    // 포커스 상태에 따라 다른 메시지 표시
    if (!Window::GetState().focused)
    {
        clearFont_(U"Click to resume").drawAt(centerX, Scene::Size().y - 100, ColorF{ 1.0, 1.0, 0.5 });
    }
    else
    {
        clearFont_(U"Press ESC to resume").drawAt(centerX, Scene::Size().y - 100, ColorF{ 1.0, 1.0, 0.5 });
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
    // 1.5초가 지난 검은색 블록 제거
    boxes_.remove_if([this](const ColorBox& box) {
        if (box.color == BoxColor::Black)
        {
            double elapsed = gameTime_ - box.creationTime;
            return elapsed >= BLACK_BOX_LIFETIME;
        }
        return false;
    });
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

void InGameScene::saveGameState()
{
    GameState currentState;
    currentState.playerPos = playerPos_;
    currentState.boxes = boxes_;
    currentState.items = items_;  // 아이템 상태 저장
    currentState.playerHeldItem = playerHeldItem_;  // 플레이어가 가진 아이템 저장
    currentState.moves = moves_;
    currentState.score = score_;
    
    gameStateHistory_.push_back(currentState);
    
    // 최대 저장 개수 제한
    if (gameStateHistory_.size() > MAX_UNDO_STEPS + 1)
    {
        gameStateHistory_.erase(gameStateHistory_.begin());
    }
}

void InGameScene::undoLastMove()
{
    if (!canUndo()) return;
    
    // 현재 상태 제거 (가장 마지막)
    gameStateHistory_.pop_back();
    
    // 이전 상태로 복원
    if (!gameStateHistory_.isEmpty())
    {
        const GameState& previousState = gameStateHistory_.back();
        
        // 플레이어 위치 즉시 복원 (사선 이동 방지)
        playerPos_ = previousState.playerPos;
        playerPixelPos_ = tileToPixel(playerPos_);
        targetPixelPos_ = playerPixelPos_;
        isPlayerMoving_ = false;
        
        boxes_ = previousState.boxes;
        items_ = previousState.items;  // 아이템 상태 복원
        playerHeldItem_ = previousState.playerHeldItem;  // 플레이어가 가진 아이템 복원
        moves_ = previousState.moves;
        score_ = previousState.score;
    }
}

bool InGameScene::canUndo() const
{
    // 최소 2개의 상태가 있어야 함 (현재 + 이전)
    return gameStateHistory_.size() >= 2;
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
    
    BoxColor newColor = itemTypeToBoxColor(playerHeldItem_);
    
    // 이미 같은 색상이면 변경하지 않음
    if (box->color == newColor)
        return false;
    
    // 박스 색상 변경
    box->color = newColor;
    score_ += 100;  // 색상 변경 보너스
    
    // 방향에 따라 페인트 애니메이션 설정
    // 오른쪽(1,0) 또는 아래(0,1): 일반 재생
    // 왼쪽(-1,0): 좌우반전 재생
    // 위(0,-1): 재생 안 함
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
    retryButton_.text = U"Retry";
    retryButton_.normalColor = ColorF{ 0.6, 0.3, 0.2 };
    retryButton_.hoverColor = ColorF{ 0.7, 0.4, 0.3 };
    
    // 스테이지 선택으로 돌아가기 버튼 (중앙)
    stageSelectButton_.rect = Rect{
        static_cast<int32>(centerX - buttonSize / 2),
        static_cast<int32>(centerY - buttonSize / 2),
        static_cast<int32>(buttonSize), static_cast<int32>(buttonSize)
    };
    stageSelectButton_.text = U"Menu";
    stageSelectButton_.normalColor = ColorF{ 0.2, 0.4, 0.6 };
    stageSelectButton_.hoverColor = ColorF{ 0.3, 0.5, 0.7 };
    
    // 다음 스테이지 버튼 (오른쪽)
    nextStageButton_.rect = Rect{
        static_cast<int32>(centerX + totalButtonSpacing - buttonSize / 2),
        static_cast<int32>(centerY - buttonSize / 2),
        static_cast<int32>(buttonSize), static_cast<int32>(buttonSize)
    };
    nextStageButton_.text = (currentStage_ < StageData::getTotalStageCount()) ? 
        U"Next" : U"Done";
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
        gameStateHistory_.clear();  // Undo 기록 초기화
        loadStage(currentStage_);
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
            gameStateHistory_.clear();  // Undo 기록 초기화
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
    
    // 추가로 반짝이는 작은 파티클들
    for (int32 i = 0; i < 100; i++)
    {
        ClearParticle sparkle;
        sparkle.pos = Vec2(Random(0, (int)Scene::Size().x), Random(0, (int)Scene::Size().y));
        sparkle.velocity = Vec2(Random(-50.0, 50.0), Random(-100.0, 50.0));
        sparkle.color = ColorF{1.0, 1.0, 1.0, 0.8};
        sparkle.life = sparkle.maxLife = Random(2.0, 4.0);
        sparkle.size = Random(1.0, 3.0);
        sparkle.rotation = 0;
        sparkle.rotationSpeed = Random(-10.0, 10.0);
        
        clearParticles_.push_back(sparkle);
    }
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




