#include "SceneManager.hpp"
#include "LogoScene.hpp"      // 로고 씬 추가
#include "MainMenuScene.hpp"
#include "OpeningScene.hpp"  // 오프닝 씬 추가
#include "StageSelectScene.hpp"
#include "InGameScene.hpp"
#include "SettingsScene.hpp"
#include "GameOverScene.hpp"
#include "StageData.hpp"  // 스테이지 데이터 포함

GameSceneManager::GameSceneManager() 
    : currentScene_(nullptr), currentSceneType_(SceneType::Logo)  // 로고부터 시작
{
    // 스테이지 데이터에 맞춰 해금 배열 초기화
    gameData_.initializeForStageCount(StageData::getTotalStageCount());
}

void GameSceneManager::initialize(SceneType startScene)
{
    currentSceneType_ = startScene;
    currentScene_ = createScene(startScene);
    if (currentScene_)
    {
        currentScene_->onEnter();
    }
    initialized_ = true;
}

void GameSceneManager::update()
{
    if (!initialized_ || !currentScene_)
        return;

    currentScene_->update();

    // 씬 전환 체크
    if (currentScene_->shouldChangeScene())
    {
        SceneType nextScene = currentScene_->getNextScene();
        changeScene(nextScene);
    }
}

void GameSceneManager::draw()
{
    if (!initialized_ || !currentScene_)
        return;

    currentScene_->draw();
}

std::unique_ptr<GameScene> GameSceneManager::createScene(SceneType sceneType)
{
    switch (sceneType)
    {
    case SceneType::Logo:  // 로고 씬 추가
        return std::make_unique<LogoScene>();
    case SceneType::MainMenu:
        return std::make_unique<MainMenuScene>();
    case SceneType::Opening:  // 오프닝 씬 추가
        return std::make_unique<OpeningScene>();
    case SceneType::StageSelect:
        {
            auto scene = std::make_unique<StageSelectScene>(&gameData_);
            return scene;
        }
    case SceneType::InGame:
        {
            // 선택된 스테이지 번호와 GameData로 InGameScene 생성
            auto scene = std::make_unique<InGameScene>(gameData_.currentStage, &gameData_);
            return scene;
        }
    case SceneType::Settings:
        return std::make_unique<SettingsScene>();
    case SceneType::GameOver:
        return std::make_unique<GameOverScene>();
    default:
        return std::make_unique<MainMenuScene>();
    }
}

void GameSceneManager::changeScene(SceneType sceneType)
{
    if (currentScene_)
    {
        currentScene_->onExit();
    }

    // Opening 씬에서 InGame으로 전환될 때 Stage 1로 설정
    if (currentSceneType_ == SceneType::Opening && sceneType == SceneType::InGame)
    {
        gameData_.currentStage = 1;
    }

    currentSceneType_ = sceneType;
    currentScene_ = createScene(sceneType);
    
    if (currentScene_)
    {
        currentScene_->onEnter();
    }
}
