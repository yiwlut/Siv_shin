#pragma once
#include "SceneManager.hpp"
#include "StageData.hpp"     
#include "../Shader/Manager/ShaderManager.hpp"
#include "../Camera/CustomCamera2D.hpp"
#include "BossWallPatternData.hpp"

enum class BoxColor
{
    Red,          
    Yellow,        
    Blue,         
    
    Orange,           
    Green,            
    Violet,           
    
    Black             
};

enum class ItemType
{
    None,         
    RedItem,        
    OrangeItem,     
    YellowItem,     
    GreenItem,      
    BlueItem,       
    VioletItem      
};

struct GameItem
{
    Point pos;
    ItemType type;
};

enum class ColorTier
{
    Primary,     
    Secondary,   
    Tertiary      
};

struct ColorBox
{
    Point pos;
    BoxColor color;
    double creationTime = 0.0;      
	uint64 uid = 0;
};

class InGameScene : public GameScene
{
private:
    static constexpr int32 TILE_SIZE = 80;           
    static constexpr int32 MAP_WIDTH = 11;    
    static constexpr int32 MAP_HEIGHT = 11;
    static constexpr double BLACK_BOX_LIFETIME = 3.5;      
	bool clearSoundPlayed_ = false;  

	bool isPlayerDead_ = false;           
	double deathAnimTimer_ = 0.0;       
	static constexpr double DEATH_ANIM_DURATION = 1.5;

	uint64 nextBoxUID_ = 1;

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
        VioletItem,
		LavaWarning,
    };
    
    enum class TacoDirection
    {
        Down,
        Side,
        Up
    };

	const Texture* getBossCurrentFrame() const;
    Point playerPos_;              
    Vec2 playerPixelPos_;            
    Vec2 targetPixelPos_;          
    double playerMoveSpeed_;       
    bool isPlayerMoving_;          
    double inputCooldown_;
    double moveDelay_;
    ColorF playerColor_;

	double bumpSoundCooldown_ = 0.0;       

	Optional<Point> queuedDir_;     
	bool pollMoveDirection(Point& outDir, TacoDirection& outTacoDir, bool& outFacingLeft);
	void bufferInputWhileMoving();

    int32 currentStage_;
    Array<Array<TileType>> mapData_;    
    Array<ColorBox> boxes_;              
    Array<GameItem> items_;            
    ItemType playerHeldItem_;            
    
    Array<Point> redGoalPositions_;
    Array<Point> yellowGoalPositions_;
    Array<Point> blueGoalPositions_;
    Array<Point> orangeGoalPositions_;
    Array<Point> greenGoalPositions_;
    Array<Point> violetGoalPositions_;
    Array<Point> blackGoalPositions_;
    
    Array<Texture> tacoDownFrames_;
    Array<Texture> tacoSideFrames_;
    Array<Texture> tacoUpFrames_;
    TacoDirection tacoDirection_;
    bool isFacingLeft_;
    int32 tacoAnimFrame_;
    double tacoAnimTimer_;
    
    Array<Texture> tacoPaintFrames_;      
    bool isPlayingPaintAnimation_;        
    bool isPaintAnimMirrored_;            
    double paintAnimTimer_;              
    int32 paintAnimFrame_;                
    static constexpr double PAINT_ANIM_DURATION = 1.0;     
    static constexpr int32 PAINT_ANIM_FRAME_COUNT = 5;    
    
    Array<Texture> playerFrames_;
    double playerAnimTimer_;
    int32 currentPlayerFrame_;
    
    Font gameFont_;
    Font debugFont_;
    Font pauseFont_;
    Font clearFont_;     
    Font buttonFont_;    
    
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
    
    bool isPaused_;
    bool wasFocused_;       
    double gameTime_;
    int32 score_;
    int32 moves_;    
    bool isCleared_;    
    bool showClearButtons_;      
    bool showHelpScreen_;       

	bool isFailed_;              
	bool showFailedButtons_;      
    
	bool willCreateBombOnPush(Point boxPos, Point dir) const;
	void ensureUniqueBoxUIDs();   
	static uint8 encodeTile_(TileType t) noexcept; 	    
	static TileType decodeTile_(uint8 c) noexcept; 	    

	Array<Array<uint8>> exportMapCodes_() const;
	void importMapCodes_(const Array<Array<uint8>>& codes);
    
    ClearButton retryButton_;       
    ClearButton stageSelectButton_;   
    ClearButton nextStageButton_;     
    
    GameData* gameData_ = nullptr;

	struct MergePaintFX {
		bool     active = false;
		ColorF   baseColor;
		ColorF   paintColor;
		Vec2     originUV;             
		uint64   targetUid = 0;          
		BoxColor finalColor;           
		bool     commitPending = false;
	};
	MergePaintFX mergeFX_;

	ColorBox* getBoxByUid(uint64 uid);
	void triggerMergePaintFX_Directional(Point tile, const ColorF& baseColor,
										 const ColorF& resultColor, Point pushDir);
	void updateMergePaintFX();
	void forceMergePaintFXCompletion();      
	BoxColor getEffectiveBoxColor(uint64 uid) const;      

	bool removeBoxByUid(uint64 uid);

	struct BombBoxInstance {
		uint64 uid = 0;
		std::unique_ptr<BombBoxEffect> effect;
		BombBoxEffect::Params params;
		bool shakeStarted = false;
	};

	struct BombRenderBatch {
		RectF screenRect;
		BombBoxEffect::Params params;
		double time;
		double explodeT;
	};


	Array<BombBoxInstance> bombFXs_;

	double bombClock_ = 0.0;    
	HashTable<uint64, double> bombExpiryAbs_;      
	static constexpr double kPreDuration = 3.0;
	static constexpr double kExpDuration = 0.9;
	static constexpr double kTotal = kPreDuration + kExpDuration;

	void triggerBombBoxFXForBlack_Multi(uint64 uid, double durationSec = 1.5);
	void updateBombBoxFX_Multi(double dt);
	void drawBombBoxFX_Multi();
	

	bool isBombGhost(uint64 uid) const;             
	bool isBombHidden(uint64 uid) const;            
	bool isBombNonBlocking(uint64 uid) const;      
	bool isBombPulsing(uint64 uid) const;
	bool shouldDrawBox(const ColorBox& box) const;     
	void drawBoxes_RespectBombFX();
	bool isBombSpanActive() const;

	void rebuildBombFXFromState_();

	struct WallBreakFX {
		Point tilePos;                                  
		std::unique_ptr<BombBoxEffect> effect;        
		BombBoxEffect::Params params;
		bool finished = false;
		bool shakeStarted = false;
	};

	Array<WallBreakFX> wallBreakFXs;      

	void destroyWalls8(Point centerTile);
	void spawnWallBreakFXAtTile(Point tile);
	void updateWallBreakFX();
	void drawWallBreakFX();

	void applyHoloFromHeldItem_();

	enum class OverlayApplyMode { OverlayOnly, WriteToMap };

    bool isSliding_ = false;      
    Point slideDir_{ 0, 0 };
	void startIceSlideTask_(ColorBox* box, Point dir);
	bool canSlideNext_(Point tile, Point dir) const;
    bool isIce(Point pos) const;
    void continueSliding();
    void slideBoxOnIce(ColorBox* box, Point dir);

	void applyTileOverlay(const Array<String>& overlayData, OverlayApplyMode mode);
	void drawGoalMarkersTop();
	bool isGoalTile(TileType t);

	enum class OverlayType : uint8 { None = 0, Warning = 1 };

	Array<Array<OverlayType>> overlayWarn_;
	double wallStepInterval_ = 6.0;
	double wallNextTime_ = 0.0;
	int32 nextColumnL_ = 0;
	int32 nextColumnR_ = 0;
	bool fillFromBothSides_ = true;

	void initBossWallSystem();
	void updateBossWallFilling(double dt);
	void spawnWallColumn(int32 x);
	void setOverlayWarning(int32 x, int32 y, bool on);
	void clearOverlayWarningCol(int32 x);
	void drawOverlayWarnings();

	Array<Array<bool>> wallMask_;

	bool isBlocked(Point pos) const;

	void clearWallMask();
	void setWallMaskCol(int x, bool on);

	void drawWallMask();

	enum class BossAttackType : uint8 { ColorSpawn, BlackHoming };
	enum class AttackPhase : uint8 { Telegraph, Fire, Done };

	struct HomingBullet {
		Vec2 pos;
		Vec2 vel;
		Point targetTile;
		double speed = 420.0;
		double armTime = 0.25;
		double life = 5.0;
		bool armed = false;
		bool alive = true;
		Point sourceTile{ -1, -1 };
	};

	struct PendingAttack {
		BossAttackType type;
		AttackPhase phase = AttackPhase::Telegraph;
		Point targetTile{ -1, -1 };
		Point sourceTile{ -1, -1 };
		ColorF telegraphColor = ColorF{ 1,1,1,1 };
		BoxColor boxColor = BoxColor::Red;
		double telegraphTime = 1.0;
	};

	double attackInterval_ = 3;
	double nextAttackTime_ = 0.0;
	double attackRatioColor_ = 1.0;
	double attackRatioBlack_ = 0;
	Point lastAttackTile_{ -1, -1 };

	int32 currentBossPhase_ = 1;
	int32 bossHitCount_ = 0;

	BossWallPattern currentWallPattern_;
	int32 currentPatternFrame_ = 0;
	double patternFrameTimer_ = 0.0;
	bool isPlayingWallPattern_ = false;
	Array<int32> patternHistory_;

	bool isBossAttackSequenceActive_ = false;
	double bossAttackSequenceTimer_ = 0.0;

	struct BoxGatherData
	{
		Point originalPos;
		Vec2 currentPixelPos;
		BoxColor color;
		bool gathered = false;
	};
	Array<BoxGatherData> gatheringBoxes_;

	Vec2 mergedBoxPixelPos_;
	bool mergedBoxCreated_ = false;

	enum class BossAttackPhase
	{
		None,
		GatherBoxes,
		MergeEffect,
		ChargeRainbow,
		LaunchTowardsBoss,
		BossHit,
		Complete
	};
	BossAttackPhase currentBossAttackPhase_ = BossAttackPhase::None;

	int32 bossMaxHP_ = 3;
	int32 bossCurrentHP_ = 3;
	double bossHitEffectTimer_ = 0.0;
	bool showBossHitEffect_ = false;

	void initBossPhaseSystem();
	void loadBossPhase(int32 phase);
	void advanceToNextBossPhase();

	void initBossWallPatternSystem();
	void updateBossWallPattern(double dt);
	void applyWallPatternFrame(const BossWallPatternFrame& frame);
	void spawnWallAtTile(Point tile);
	void destroyWallAtTile(Point tile);
	void setWarningAtTile(Point tile, bool on);
	void loadRandomPattern();

	void startBossAttackSequence();
	void updateBossAttackSequence(double dt);
	void drawBossAttackSequence();
	bool checkAllGoalsFilledForBoss() const;
	Point getMapCenterTile() const;
	Vec2 getBossPositionForAttack() const;

	void damageBoss();
	void drawBossHP();
	void updateBossHitEffect(double dt);

	Array<HomingBullet> homingBullets_;
	Array<PendingAttack> pendingAttacks_;

	Vec2 getBossStartUIPos() const;
	void initBossAttacks();
	void scheduleNextBossAttack();
	void updateBossAttacks(double dt);
	void drawBossAttacks();
	void spawnColorAttack();
	void spawnBlackHomingAttack();
	bool chooseRandomSpawnTile(Point& out);
	bool isSpawnableTile(Point p) const;
	ColorF randomSixColor() const;

	void fireColorAttack(const PendingAttack& p);
	void fireBlackHoming(const PendingAttack& p);
	void drawTelegraphMarker(Point tile, const ColorF& col, double t, double pulse01) const;

	struct EnergyBall
	{
		Vec2 startPosScreen;
		Point targetTile;
		Vec2 currentPosScreen;
		ColorF color;
		double progress = 0.0;
		double duration = 0.5;
		double elapsedTime = 0.0;
		bool active = true;
		bool spawnBoxOnArrive = false;
		BoxColor spawnBoxColor = BoxColor::Red;
		bool arrivalHandled = false;
	};

	Array<EnergyBall> energyBalls;

	void createEnergyBallEffect(Vec2 screenStart, Point targetTile, const ColorF& color, double duration, bool spawnBoxOnArrive, BoxColor boxColor);
	void updateEnergyBalls(double dt);
	void drawEnergyBalls();
	void drawHomingBullets();

	void drawBossChargeTelegraphs();
	void drawBossChargeAtStart(const ColorF& col, double remain, double total);

	void spawnBombExplosionFXAtTile(Point tile);

public:
	Texture stageBackground_;
	Array<Texture> bossIdleFrames_;
	Array<Texture> bossAtkFrames_;
	Array<Texture> bossCloakFrames_;
	Array<Texture> bossConfusedFrames_;
	Array<Texture> bossSummonFrames_;
	Texture bossKOFrame_;
	enum class BossAnimState { None, Cloak, Idle, Summon, Attack, Confused, KO };
	BossAnimState bossAnimState_ = BossAnimState::None;
	int32 bossAnimFrame_ = 0;
	double bossAnimTimer_ = 0.0;
	double bossFrameDuration_ = 0.12;
	double bossAtkFrameDuration_ = 0.18;
	bool bossAnimLoop_ = true;
	void setBossState(BossAnimState s, bool loop = true, double frameDuration = -1);
	void updateBossAnimation(double dt);
    InGameScene();
    InGameScene(int32 stageNumber);
    InGameScene(int32 stageNumber, GameData* gameData);
    ~InGameScene() override = default;

    void update() override;
    void draw() override;
    void onEnter() override;
    void onExit() override;
    
    void setGameData(GameData* gameData) { gameData_ = gameData; }

	void setTileAt(Point pos, TileType type);
	void setTileAt(int32 x, int32 y, TileType type);
	TileType getTileAt(Point pos) const;
	TileType getTileAt(int32 x, int32 y) const;

private:
    void loadAssets();
    void loadStage(int32 stageNumber);
    void loadStageFromText(const Array<String>& mapText);     
    bool canMoveTo(Point pos) const;
    ColorBox* getBoxAt(Point pos);
    const ColorBox* getBoxAt(Point pos) const;
    bool canPushBox(Point playerPos, Point boxPos, Point direction) const;
    void pushBox(ColorBox* box, Point direction);
    void checkBoxMerge(Point pos);
    void updateBlackBoxes();     
    bool isGameClear() const;
    ColorF getBoxColorF(BoxColor color) const;
    ColorTier getColorTier(BoxColor color) const;
    Optional<BoxColor> getMergedColor(BoxColor color1, BoxColor color2) const;
    Array<Point>& getGoalPositionsForColor(BoxColor color);
    const Array<Point>& getGoalPositionsForColor(BoxColor color) const;
    
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
    void startPaintAnimation(bool mirrored);     
    void updatePaintAnimation();    
    void drawPaintAnimation();      
    
    Vec2 tileToPixel(Point tilePos) const;       
    void movePlayerTo(Point newTilePos);        
    
    void updatePlayer();
    void updateAnimations();
    void drawBackground();
    void drawMap();
    void drawPlayer();
    void drawUI();
	void drawHealthUI();  
    void handleInput();
    
    void drawHelpScreen();
    
    void initializeClearButtons();
    void updateClearButtons();
    void drawClearButtons();
    void updateClearButton(ClearButton& button);
    void drawClearButton(const ClearButton& button);

	void drawFailedScreen();         
	void updateFailedButtons();      
    int32 calculateStars(int32 moves) const;       
    void drawStars(int32 starCount, Vec2 centerPos) const;    
    
    Texture tacoScoreTexture_;      
    Texture tacoScoreOffTexture_;      
    
    Audio bgm_;   
	Audio bossBgm_;
	double savedMusicPosition_ = 0.0;
	double bossBgmFadeTimer_ = 0.0;
	double bossBgmFadeDuration_ = 1.5;
	double bossBgmTargetVolume_ = 0.5;
	bool bossBgmFadingIn_ = false;
	void startBossBgmFadeIn_();
	void updateBossBgmFade_(double dt);

    Audio noteE5_;    
    Audio noteG5_;    
    Audio noteC6_;      

	Audio stageClearSound_;     
	Audio bombExplosionSound_;
	Audio bumpSound_;        
    void playBoxSound(BoxColor color);

	struct IceSlideTask {
		uint64 uid = 0;
		Point dir{ 0, 0 };
		double cooldown = 0.0;
		bool active = false;
	};
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

	struct BossProjectile
	{
		Vec2 pos;
		Vec2 velocity;
		ColorF color;
		double size;
		bool active;
		bool exploded;
	};

	struct BossExplosionParticle
	{
		Vec2 pos;
		Vec2 velocity;
		ColorF color;
		double life;
		double maxLife;
		double size;
	};

	double fadeTimer_;
	double fadeDuration_;
	bool isFading_;

	Array<IceSlideTask> iceSlideTasks_;

	Array<DeathParticle> deathParticles_;
	bool isWhiteParticleDeath_ = false;

	Array<BossProjectile> bossProjectiles_;
	Array<BossExplosionParticle> bossExplosionParticles_;
	double bossAttackTimer_ = 0.0;
	const double BOSS_ATTACK_INTERVAL = 3.0;    

	int32 playerHealth_ = 3;                         
	static constexpr int32 MAX_HEALTH = 3;        
	Texture heartFullTexture_;                      
	Texture heartEmptyTexture_;

    Array<ClearParticle> clearParticles_;
    double clearEffectTimer_ = 0.0;
    bool showClearEffect_ = false;

	void updateIceSlideTasks_(double dt);

	const ColorBox* getBoxByUid_const(uint64 uid) const;
    void createClearEffect();
    void updateClearEffect();
    void drawClearEffect();

	void createDeathEffect(bool useWhiteParticles = false);
	void updateDeathEffect();
	void drawDeathEffect();

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

	int32 getMapWidth() const;                     
	int32 getMapHeight() const;

	void loadStageFromText_VarSize(const Array<String>& mapText);

	void loadStageByIndex(int32 stageNumber);           

	bool isInsideMap(Point pos) const;                 
	bool canMoveToPoint(Point pos) const;         

	double computeFitScaleToMap(double margin = 0.95) const;
	void applyFixedCameraFitToMap();                         
	void applyFixedCameraFitToRect(const Rect& tileRect, double margin = 0.95); 
	void updateFixedCameraZoomByWheelInput();              
	void onStageLoaded_FixedCamera();                      

	Rect getVisibleTileRect() const;         

	void drawWorldWithCamera(const std::function<void()>& worldDraw,
							 const std::function<void()>& uiDraw);  
};
