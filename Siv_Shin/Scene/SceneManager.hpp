#pragma once
#include <Siv3D.hpp>
#include <memory>

// 씬 타입 열거형
enum class SceneType
{
    MainMenu,
    Opening,      // 오프닝 씬 추가
    StageSelect,  // 스테이지 선택 씬 추가
    InGame,
    Settings,
    GameOver,
    Pause
};

// 게임 데이터 저장 구조체
struct GameData
{
    Array<bool> stageUnlocked = { true, true, true, true, true, true, true, true };  // Stage 1~8 모두 해금 (총 8개)
    int32 currentStage = 3;  // 스테이지 3부터 시작
    int32 totalScore = 0;
    
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
    
    // 스테이지 클리어 시 다음 스테이지 해금
    void clearStage(int32 stageNumber)
    {
        if (stageNumber < static_cast<int32>(stageUnlocked.size()))
        {
            unlockStage(stageNumber + 1);  // 다음 스테이지 해금
        }
    }
    
    // 스테이지 개수에 따라 동적으로 해금 배열 조정
    void initializeForStageCount(int32 totalStages)
    {
        stageUnlocked.clear();
        // 처음부터 모든 스테이지 해금
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
