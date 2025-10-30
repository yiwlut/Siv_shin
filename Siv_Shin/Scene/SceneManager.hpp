#pragma once
#include <Siv3D.hpp>
#include "SaveManager.hpp"
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
    Ending,
    BossIntro
};

class GameData
{
public:
	SaveManager saveManager;
	int32 currentStage = 1;
	bool startPausedNextInGame = false;

	void clearStage(int32 stage)
	{
		saveManager.clearStage(stage);
		currentStage = stage;
	}

	bool isStageUnlocked(int32 stage) const
	{
		return saveManager.isStageUnlocked(stage);
	}

	bool finalStageCleared() const
	{
		return saveManager.isFinalStageCleared();
	}

	void setFinalStageCleared(bool cleared)
	{
		saveManager.setFinalStageCleared(cleared);
	}

	void initializeForStageCount(int32 totalStages);
};

class GameScene
{
public:
    virtual ~GameScene() = default;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual SceneType getNextScene() const { return nextScene_; }
    virtual bool shouldChangeScene() const { return shouldChange_; }
    virtual void onEnter() {}
    virtual void onExit() {}

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


class GameSceneManager
{
private:
    std::unique_ptr<GameScene> currentScene_;
    SceneType currentSceneType_;
    bool initialized_ = false;
    GameData gameData_;

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
