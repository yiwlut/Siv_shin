#pragma once
#include <Siv3D.hpp>
#include <memory>

// 씬 타입 열거형
enum class SceneType
{
    Logo,         // 로고 씬 추가
    MainMenu,
    Opening,      // 오프닝 씬 추가
    StageSelect,  // 스테이지 선택 씬 추가
    InGame,
    Settings,
    GameOver,
    Pause,
	Ending
};

// 게임 데이터 저장 구조체
struct GameData
{
	Array<bool> stageUnlocked = { true, true, true, true, true, true, true, true };
	int32 currentStage = 3;
	int32 totalScore = 0;
	bool finalStageCleared = false;
	bool startPausedNextInGame = false;
	void unlockStage(int32 stageNumber)
	{
		if (stageNumber > 0 && stageNumber <= static_cast<int32>(stageUnlocked.size()))
		{
			stageUnlocked[stageNumber - 1] = true;
		}
	}
	bool isStageUnlocked(int32 stageNumber) const
	{
		if (stageNumber > 0 && stageNumber <= static_cast<int32>(stageUnlocked.size()))
		{
			return stageUnlocked[stageNumber - 1];
		}
		return false;
	}
	void clearStage(int32 stageNumber)
	{
		if (stageNumber < static_cast<int32>(stageUnlocked.size()))
		{
			unlockStage(stageNumber + 1);
		}
	}
	void initializeForStageCount(int32 totalStages)
	{
		stageUnlocked.clear();
		stageUnlocked.resize(totalStages, true);
	}
};

// 게임 씬 기본 인터페이스
class GameScene
{
public:
    virtual ~GameScene() = default;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual SceneType getNextScene() const { return nextScene_; }  // 수정: nextScene_ 반환
    virtual bool shouldChangeScene() const { return shouldChange_; }
    virtual void onEnter() {}  // 씬 시작시 호출
    virtual void onExit() {}   // 씬 종료시 호출

protected:
    SceneType currentScene_;
    bool shouldChange_ = false;
    SceneType nextScene_;

    void changeScene(SceneType scene) 
    {
        nextScene_ = scene;
        shouldChange_ = true;
    }
};

// 게임 씬 매니저 클래스
class GameSceneManager
{
private:
    std::unique_ptr<GameScene> currentScene_;
    SceneType currentSceneType_;
    bool initialized_ = false;
    GameData gameData_;  // 전역 게임 데이터

public:
    GameSceneManager();
    ~GameSceneManager() = default;

    void initialize(SceneType startScene = SceneType::MainMenu);
    void update();
    void draw();
    
    SceneType getCurrentScene() const { return currentSceneType_; }
    GameData& getGameData() { return gameData_; }
    const GameData& getGameData() const { return gameData_; }
    
private:
    std::unique_ptr<GameScene> createScene(SceneType sceneType);
    void changeScene(SceneType sceneType);
};
