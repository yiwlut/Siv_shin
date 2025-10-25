#pragma once
#include "SceneManager.hpp"
#include "StageData.hpp"  // 스테이지 데이터 포함
#include "../Shader/Manager/ShaderManager.hpp"
#include "../Camera/CustomCamera2D.hpp"

// 상자 색상
enum class BoxColor
{
    // 1차 색상 (Primary)
    Red,        // 빨강 (R)
    Yellow,     // 노랑 (Y) 
    Blue,       // 파랑 (B)
    
    // 2차 색상 (Secondary)
    Orange,     // 주황 (O) = Red + Yellow
    Green,      // 초록 (G) = Yellow + Blue
    Violet,     // 보라 (V) = Blue + Red
    
    // 3차 색상 (Tertiary)
    Black       // 검정 (K) = 2차+2차 또는 1차+2차
};

// 아이템 타입
enum class ItemType
{
    None,       // 아이템 없음
    RedItem,    // ㅃ - 빨강 아이템
    OrangeItem, // ㅈ - 주황 아이템
    YellowItem, // ㄴ - 노랑 아이템
    GreenItem,  // ㅊ - 초록 아이템
    BlueItem,   // ㅍ - 파랑 아이템
    VioletItem  // ㅂ - 보라 아이템
};

// 아이템 정보
struct GameItem
{
    Point pos;
    ItemType type;
};

// 색상 등급
enum class ColorTier
{
    Primary,    // 1차
    Secondary,  // 2차
    Tertiary    // 3차 (Black)
};

// 상자 정보
struct ColorBox
{
    Point pos;
    BoxColor color;
    double creationTime = 0.0;  // 생성 시간 (검은색 블록용)
	uint64 uid = 0;
};

class InGameScene : public GameScene
{
private:
    static constexpr int32 TILE_SIZE = 80;  // 타일 크기 증가 (1024/11 ≈ 93, 여백 고려하여 80)
    static constexpr int32 MAP_WIDTH = 11;  // 11x11 맵
    static constexpr int32 MAP_HEIGHT = 11;
    static constexpr double BLACK_BOX_LIFETIME = 3.5;  // 검은색 블록 수명 (초)
	bool clearSoundPlayed_ = false;  

	bool isPlayerDead_ = false;           
	double deathAnimTimer_ = 0.0;       
	static constexpr double DEATH_ANIM_DURATION = 1.5;

	uint64 nextBoxUID_ = 1;

    // 소코반 타일 타입
    enum class TileType
    {
        Empty,     
        Wall,      
        RedGoal,     
        YellowGoal,  
        BlueGoal,  
        OrangeGoal,  
        GreenGoal,  
        VioletGoal, 
        BlackGoal,  
        Ice,
		Lava,
        RedItem,    
        OrangeItem, 
        YellowItem,  
        GreenItem,   
        BlueItem,    
        VioletItem 
    };
    
    // Taco 플레이어 방향
    enum class TacoDirection
    {
        Down,
        Side,
        Up
    };


    // 배경 이미지
    Texture stageBackground_;
    // 보스 이미지 (Final stage 전용) - 3프레임 애니메이션
    Array<Texture> bossIdleFrames_;
    int32 bossAnimFrame_ = 0;
	double bossAnimTimer_;
    
    // 플레이어 - 부드러운 이동 추가
    Point playerPos_;           // 논리적 타일 위치
    Vec2 playerPixelPos_;       // 실제 픽셀 위치 (부드러운 이동용)
    Vec2 targetPixelPos_;       // 목표 픽셀 위치
    double playerMoveSpeed_;    // 이동 속도 (픽셀/초)
    bool isPlayerMoving_;       // 플레이어가 이동 중인지
    double inputCooldown_;
    double moveDelay_;
    ColorF playerColor_;

	Optional<Point> queuedDir_; // 방향 전환 입력 버퍼
	bool pollMoveDirection(Point& outDir, TacoDirection& outTacoDir, bool& outFacingLeft);
	void bufferInputWhileMoving();

    // 소코반 맵 데이터
    int32 currentStage_;
    Array<Array<TileType>> mapData_;  // 11x11 맵
    Array<ColorBox> boxes_;           // 상자들 (색상 포함)
    Array<GameItem> items_;           // 아이템들
    ItemType playerHeldItem_;         // 플레이어가 가진 아이템
    
    // 각 색상별 목표 위치
    Array<Point> redGoalPositions_;
    Array<Point> yellowGoalPositions_;
    Array<Point> blueGoalPositions_;
    Array<Point> orangeGoalPositions_;
    Array<Point> greenGoalPositions_;
    Array<Point> violetGoalPositions_;
    Array<Point> blackGoalPositions_;
    
    // Taco 애니메이션
    Array<Texture> tacoDownFrames_;
    Array<Texture> tacoSideFrames_;
    Array<Texture> tacoUpFrames_;
    TacoDirection tacoDirection_;
    bool isFacingLeft_;
    int32 tacoAnimFrame_;
    double tacoAnimTimer_;
    
    // Taco Paint 애니메이션 (아이템 사용 시)
    Array<Texture> tacoPaintFrames_;  // tacoPaint_0.png ~ tacoPaint_4.png (5장)
    bool isPlayingPaintAnimation_;    // 페인트 애니메이션 재생 중인지
    bool isPaintAnimMirrored_;        // 페인트 애니메이션 좌우반전 여부
    double paintAnimTimer_;           // 페인트 애니메이션 타이머
    int32 paintAnimFrame_;            // 현재 페인트 애니메이션 프레임
    static constexpr double PAINT_ANIM_DURATION = 1.0;  // 1초 동안 재생
    static constexpr int32 PAINT_ANIM_FRAME_COUNT = 5;  // 5개 프레임
    
    // 기존 플레이어 애니메이션 (fallback)
    Array<Texture> playerFrames_;
    double playerAnimTimer_;
    int32 currentPlayerFrame_;
    
    // UI
    Font gameFont_;
    Font debugFont_;
    Font pauseFont_;
    Font clearFont_;  // 클리어 메시지용 폰트
    Font buttonFont_; // 클리어 버튼용 폰트
    
    // 클리어 화면 버튼 구조체
    struct ClearButton
    {
        Rect rect;
        String text;
        bool isHovered = false;
        ColorF normalColor = ColorF{ 0.2, 0.4, 0.6 };
        ColorF hoverColor = ColorF{ 0.3, 0.5, 0.7 };
        ColorF textColor = Palette::White;
        ColorF hoverTextColor = Palette::Yellow;
    };
    
    // 게임 상태
    bool isPaused_;
    bool wasFocused_;  // 이전 프레임의 포커스 상태 추적
    double gameTime_;
    int32 score_;
    int32 moves_;  // 이동 횟수
    bool isCleared_;  // 클리어 여부
    bool showClearButtons_;  // 클리어 버튼 표시 여부
    bool showHelpScreen_;  // 조작법 도움말 화면 표시 여부

	bool isFailed_;          // 실패 여부 (체력 0)
	bool showFailedButtons_; // 실패 화면 버튼 표시 여부
    
	bool willCreateBombOnPush(Point boxPos, Point dir) const;
	void ensureUniqueBoxUIDs(); //초기 UID 보정
	static uint8 encodeTile_(TileType t) noexcept; 	// TileType -> 코드 변환
	static TileType decodeTile_(uint8 c) noexcept; 	// 코드 -> TileType 변환

	// 스냅샷 내보내기 / 가져오기
	Array<Array<uint8>> exportMapCodes_() const;
	void importMapCodes_(const Array<Array<uint8>>& codes);
    
    // 클리어 화면 버튼들
    ClearButton retryButton_;      // 다시하기
    ClearButton stageSelectButton_; // 스테이지 선택으로
    ClearButton nextStageButton_;   // 다음 스테이지
    
    // 게임 데이터 참조
    GameData* gameData_ = nullptr;

	// PaintSpreadShader helper
	struct MergePaintFX {
		bool     active = false;
		ColorF   baseColor;
		ColorF   paintColor;
		Vec2     originUV;         // 타일 로컬 UV (0~1)
		uint64   targetUid = 0;    // ← 추가: 이펙트 대상 박스 UID
		BoxColor finalColor;       // 완료 시 커밋할 논리색
		bool     commitPending = false;
	};
	MergePaintFX mergeFX_;

	ColorBox* getBoxByUid(uint64 uid);
	void triggerMergePaintFX_Directional(Point tile, const ColorF& baseColor,
										 const ColorF& resultColor, Point pushDir);
	void updateMergePaintFX();
	void forceMergePaintFXCompletion(); // 진행 중인 이펙트를 즉시 완료
	BoxColor getEffectiveBoxColor(uint64 uid) const; // 합성 중이면 최종 색상 반환

	bool removeBoxByUid(uint64 uid);

	// BombBox Shader helper (동시 다발 폭발용)
	struct BombBoxInstance {
		uint64 uid = 0;
		std::unique_ptr<BombBoxEffect> effect;
		BombBoxEffect::Params params;
		bool shakeStarted = false;
	};

	// 배칭 렌더링용 구조체
	struct BombRenderBatch {
		RectF screenRect;
		BombBoxEffect::Params params;
		double time;
		double explodeT;
	};


	Array<BombBoxInstance> bombFXs_;

	double bombClock_ = 0.0; // Undo 불변 시계
	HashTable<uint64, double> bombExpiryAbs_; // uid -> 절대 만료시각(Undo 불변)
	// undo 시 bombExpiryAbs_ 재구성 필요
	static constexpr double kPreDuration = 3.0;
	static constexpr double kExpDuration = 0.9;
	static constexpr double kTotal = kPreDuration + kExpDuration;

	void triggerBombBoxFXForBlack_Multi(uint64 uid, double durationSec = 1.5);
	void updateBombBoxFX_Multi(double dt);
	void drawBombBoxFX_Multi();
	

	bool isBombGhost(uint64 uid) const;        // bomb FX 진행 중인 박스인가
	bool isBombHidden(uint64 uid) const;        // 렌더 숨김: pulse+explode 모두
	bool isBombNonBlocking(uint64 uid) const;   // 충돌 제외: explode만
	bool isBombPulsing(uint64 uid) const;
	bool shouldDrawBox(const ColorBox& box) const; // 유령 상태면 그리지 않음
	void drawBoxes_RespectBombFX();
	bool isBombSpanActive() const;

	void rebuildBombFXFromState_();

	// 벽 파괴 효과용
	struct WallBreakFX {
		Point tilePos;                              // 부서지는 벽 타일 위치
		std::unique_ptr<BombBoxEffect> effect;      // 파편 효과용
		BombBoxEffect::Params params;
		bool finished = false;
		bool shakeStarted = false;
	};

	Array<WallBreakFX> wallBreakFXs;  // 벽 파괴 효과 목록

	void destroyWalls8(Point centerTile);
	void spawnWallBreakFXAtTile(Point tile);
	void updateWallBreakFX();
	void drawWallBreakFX();

	//holographic
	void applyHoloFromHeldItem_();



    bool isSliding_ = false;      
    Point slideDir_{ 0, 0 };
	void startIceSlideTask_(ColorBox* box, Point dir);
	bool canSlideNext_(Point tile, Point dir) const;
    bool isIce(Point pos) const;
    void continueSliding();
    void slideBoxOnIce(ColorBox* box, Point dir);

	void applyTileOverlay(const Array<String>& overlayData);

public:
    InGameScene();
    InGameScene(int32 stageNumber);
    InGameScene(int32 stageNumber, GameData* gameData);
    ~InGameScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;
    
    // 게임 데이터 설정
    void setGameData(GameData* gameData) { gameData_ = gameData; }

	void setTileAt(Point pos, TileType type);
	void setTileAt(int32 x, int32 y, TileType type);
	TileType getTileAt(Point pos) const;
	TileType getTileAt(int32 x, int32 y) const;

private:
    void loadAssets();
    void loadStage(int32 stageNumber);
    void loadStageFromText(const Array<String>& mapText);  // 텍스트 맵 로더
	//void loadStageFromText(const StageData::StageMap& stageMap);

    bool canMoveTo(Point pos) const;
    ColorBox* getBoxAt(Point pos);
    const ColorBox* getBoxAt(Point pos) const;
    bool canPushBox(Point playerPos, Point boxPos, Point direction) const;
    void pushBox(ColorBox* box, Point direction);
    void checkBoxMerge(Point pos);
    void updateBlackBoxes();  // 검은색 블록 업데이트
    bool isGameClear() const;
    ColorF getBoxColorF(BoxColor color) const;
    ColorTier getColorTier(BoxColor color) const;
    Optional<BoxColor> getMergedColor(BoxColor color1, BoxColor color2) const;
    Array<Point>& getGoalPositionsForColor(BoxColor color);
    const Array<Point>& getGoalPositionsForColor(BoxColor color) const;
    
    // 아이템 시스템
    BoxColor itemTypeToBoxColor(ItemType item) const;
    ItemType tileTypeToItemType(TileType tile) const;
    ColorF getItemColorF(ItemType item) const;
    GameItem* getItemAt(Point pos);
    const GameItem* getItemAt(Point pos) const;
    void collectItem(Point pos);
    bool tryChangeBoxColor(Point pos, Point direction);

	void updateFinalStageTileOverlay();
	void checkPlayerLavaCollision();
	void checkBoxesLavaCollision();
    // 페인트 애니메이션
    void startPaintAnimation(bool mirrored);  // 페인트 애니메이션 시작
    void updatePaintAnimation(); // 페인트 애니메이션 업데이트
    void drawPaintAnimation();   // 페인트 애니메이션 그리기
    
    // 플레이어 이동 관련
    Vec2 tileToPixel(Point tilePos) const;  // 타일 좌표를 픽셀 좌표로 변환
    void movePlayerTo(Point newTilePos);    // 플레이어를 새 타일로 이동
    
    void updatePlayer();
    void updateAnimations();
    void drawBackground();
    void drawMap();
    void drawPlayer();
    void drawUI();
	void drawHealthUI();  
    void handleInput();
    
    // 조작법 도움말 관련
    void drawHelpScreen();
    
    // Undo 시스템
    
    // 클리어 화면 관련
    void initializeClearButtons();
    void updateClearButtons();
    void drawClearButtons();
    void updateClearButton(ClearButton& button);
    void drawClearButton(const ClearButton& button);

	void drawFailedScreen();      // 실패 화면 그리기
	void updateFailedButtons();   // 실패 버튼 업데이트
    // 점수 시스템
    int32 calculateStars(int32 moves) const;  // Move 횟수에 따른 별점 계산
    void drawStars(int32 starCount, Vec2 centerPos) const;  // 별점 그리기
    
    // 점수 텍스처
    Texture tacoScoreTexture_;  // 타코 점수 이미지 (온)
    Texture tacoScoreOffTexture_;  // 타코 점수 이미지 (오프)
    
    // 배경음악
    Audio bgm_;  // 배경음악
	Audio bossBgm_;
	double savedMusicPosition_ = 0.0;

    // 블록 밀기 효과음
    Audio noteE5_;  // 노랑 (E5)
    Audio noteG5_;  // 파랑 (G5)
    Audio noteC6_;  // 빨강 및 검정 (C6)

	Audio stageClearSound_;  // 스테이지 클리어 사운드
	Audio bombExplosionSound_;
    // 블록 색상에 따라 음악 재생
    void playBoxSound(BoxColor color);

	struct IceSlideTask {
		uint64 uid = 0;
		Point dir{ 0, 0 };
		double cooldown = 0.0;
		bool active = false;
	};
    // 클리어 이펙트
    struct ClearParticle
    {
        Vec2 pos;
        Vec2 velocity;
        ColorF color;
        double life;
        double maxLife;
        double size;
        double rotation;
        double rotationSpeed;
    };

	struct DeathParticle
	{
		Vec2 pos;
		Vec2 velocity;
		ColorF color;
		double life;
		double maxLife;
		double size;
	};

	// ★ 보스 공 발사 구조체
	struct BossProjectile
	{
		Vec2 pos;
		Vec2 velocity;
		ColorF color;
		double size;
		bool active;
		bool exploded;
	};

	// ★ 보스 공 폭발 파티클
	struct BossExplosionParticle
	{
		Vec2 pos;
		Vec2 velocity;
		ColorF color;
		double life;
		double maxLife;
		double size;
	};

	Array<IceSlideTask> iceSlideTasks_;

	Array<DeathParticle> deathParticles_;
	bool isWhiteParticleDeath_ = false;

	Array<BossProjectile> bossProjectiles_;
	Array<BossExplosionParticle> bossExplosionParticles_;
	double bossAttackTimer_ = 0.0;
	const double BOSS_ATTACK_INTERVAL = 3.0;  // 3초마다 공격

	int32 playerHealth_ = 3;                    // 현재 체력 (Final Stage 전용)
	static constexpr int32 MAX_HEALTH = 3;      // 최대 체력
	Texture heartFullTexture_;                  // 가득 찬 하트 텍스처
	Texture heartEmptyTexture_;

    Array<ClearParticle> clearParticles_;
    double clearEffectTimer_ = 0.0;
    bool showClearEffect_ = false;

	void updateIceSlideTasks_(double dt);

	const ColorBox* getBoxByUid_const(uint64 uid) const;
    // 클리어 이펙트 관련 메서드
    void createClearEffect();
    void updateClearEffect();
    void drawClearEffect();

	void createDeathEffect(bool useWhiteParticles = false);
	void updateDeathEffect();
	void drawDeathEffect();

	void updateBossAttack(double dt);
	void spawnBossProjectile();
	void updateBossProjectiles(double dt);
	void createBossExplosion(Vec2 pos, ColorF color);
	void updateBossExplosions(double dt);
	void drawBossProjectiles();
	void drawBossExplosions();
	bool isPlayerInExplosionRange(Point bombPos) const;

	static CustomCamera2D& camInstance();                    
	CustomCamera2D& camera();           
	const CustomCamera2D& camera() const;                   

	// 동적 맵 크기 질의
	int32 getMapWidth() const;                     
	int32 getMapHeight() const;

	// 가변 맵 파서 (텍스트 기반)
	void loadStageFromText_VarSize(const Array<String>& mapText);

	// 스테이지 인덱스 로드 래퍼
	void loadStageByIndex(int32 stageNumber);           

	// 경계/이동 체크(가변)
	bool isInsideMap(Point pos) const;                 
	bool canMoveToPoint(Point pos) const;         

	// 고정 카메라 유틸(추적 없음)
	double computeFitScaleToMap(double margin = 0.95) const;
	void applyFixedCameraFitToMap();                         
	void applyFixedCameraFitToRect(const Rect& tileRect, double margin = 0.95); 
	void updateFixedCameraZoomByWheelInput();              
	void onStageLoaded_FixedCamera();                      

	// 뷰 컬링
	Rect getVisibleTileRect() const;         

	// 월드/UI 드로잉 래핑(선택)
	void drawWorldWithCamera(const std::function<void()>& worldDraw,
							 const std::function<void()>& uiDraw);  
};
