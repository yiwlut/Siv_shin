#include "SceneManager.hpp"
#include "LogoScene.hpp"      // 로고 씬 추가
#include "MainMenuScene.hpp"
#include "OpeningScene.hpp"  // 오프닝 씬 추가
#include "StageSelectScene.hpp"
#include "InGameScene.hpp"
#include "SettingsScene.hpp"
#include "GameOverScene.hpp"
#include "StageData.hpp"  // 스테이지 데이터 포함
#include "EndingScene.hpp"
#include "BossIntroScene.hpp"

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
	{
		auto scene = std::make_unique<MainMenuScene>();
		scene->gameData_ = &gameData_;  // ★ 추가
		return scene;
	}
	case SceneType::Opening:
		return std::make_unique<OpeningScene>();
	case SceneType::Ending:
	{
		auto scene = std::make_unique<EndingScene>();
		scene->gameData_ = &gameData_;  // ★ 추가
		return scene;
	}
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
    case SceneType::BossIntro:  // 보스 인트로 씬 추가
        return std::make_unique<BossIntroScene>();
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
    if (currentSceneType_ == SceneType::Opening && sceneType == SceneType::InGame)
    {
        gameData_.currentStage = 1;
		gameData_.startPausedNextInGame = true;
    }

    // BossIntro → InGame 전환 시 보스 스테이지로 진입하도록 설정
    if (currentSceneType_ == SceneType::BossIntro && sceneType == SceneType::InGame)
    {
        gameData_.currentStage = 11;
        gameData_.startPausedNextInGame = true;
    }

    currentSceneType_ = sceneType;
    currentScene_ = createScene(sceneType);
    
    if (currentScene_)
    {
        currentScene_->onEnter();
    }
}
