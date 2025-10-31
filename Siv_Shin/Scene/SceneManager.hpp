#pragma once
#include <Siv3D.hpp>
#include <memory>

enum class SceneType
{
    Logo,            
    MainMenu,
    Opening,         
    StageSelect,      
    InGame,
    Settings,
    GameOver,
    Pause,
    Ending,
    BossIntro
};

struct GameData
{
	Array<bool> stageUnlocked = { true, false, false, false, false, false, false, false, false, false };    
	bool finalStageUnlocked = false;       
	int32 currentStage = 1;
	int32 totalScore = 0;
	bool finalStageCleared = false;
	bool startPausedNextInGame = false;

	void unlockStage(int32 stageNumber)
	{
		if (stageNumber == 11)     
		{
			finalStageUnlocked = true;
		}
		else if (stageNumber > 0 && stageNumber <= static_cast<int32>(stageUnlocked.size()))
		{
			stageUnlocked[stageNumber - 1] = true;
		}
	}

	bool isStageUnlocked(int32 stageNumber) const
	{
		if (stageNumber == 11)     
		{
			return finalStageUnlocked;
		}
		if (stageNumber > 0 && stageNumber <= static_cast<int32>(stageUnlocked.size()))
		{
			return stageUnlocked[stageNumber - 1];
		}
		return false;
	}

	void clearStage(int32 stageNumber)
	{
		unlockStage(stageNumber);     

		if (stageNumber == 6)         
		{
			finalStageUnlocked = true;
		}
		else if (stageNumber == 11)      
		{
			finalStageCleared = true;
			finalStageUnlocked = true;    
			unlockStage(7);     
		}
		else if (stageNumber < 10)    
		{
			unlockStage(stageNumber + 1);
		}
	}

	void unlockAllStages()       
	{
		for (int32 i = 0; i < stageUnlocked.size(); ++i)
		{
			stageUnlocked[i] = true;
		}
		finalStageUnlocked = true;
		finalStageCleared = true;
	}

	void initializeForStageCount(int32 totalStages)
	{
		stageUnlocked.assign(10, false);     
		stageUnlocked[0] = true;    
		finalStageUnlocked = false;
		finalStageCleared = false;
	}
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
