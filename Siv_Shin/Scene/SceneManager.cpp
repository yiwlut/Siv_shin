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
	std::unique_ptr<GameScene> result;

	switch (sceneType)
	{
	case SceneType::Logo:
		result = std::make_unique<LogoScene>();
		break;

	case SceneType::MainMenu:
	{
		auto s = std::make_unique<MainMenuScene>();
		s->gameData_ = &gameData_;
		result = std::move(s);
		break;
	}

	case SceneType::Opening:
	{
		auto s = std::make_unique<OpeningScene>();
		s->setGameData(&gameData_);
		result = std::move(s);
		break;
	}

	case SceneType::Ending:
	{
		auto s = std::make_unique<EndingScene>();
		s->gameData_ = &gameData_;
		result = std::move(s);
		break;
	}

	case SceneType::StageSelect:
		result = std::make_unique<StageSelectScene>(&gameData_);
		break;

	case SceneType::InGame:
		result = std::make_unique<InGameScene>(gameData_.currentStage, &gameData_);
		break;

	case SceneType::Settings:
		result = std::make_unique<SettingsScene>();
		break;

	case SceneType::GameOver:
		result = std::make_unique<GameOverScene>();
		break;

	case SceneType::BossIntro:
		result = std::make_unique<BossIntroScene>();
		break;

	default:
		result = std::make_unique<MainMenuScene>();
		break;
	}

	return result;
}

void GameSceneManager::changeScene(SceneType sceneType)
{
    if (currentScene_)
    {
        currentScene_->onExit();
    }
	if (currentSceneType_ == SceneType::Opening && sceneType == SceneType::InGame)
	{
		const int32 saved = gameData_.saveManager.getCurrentStage();
		gameData_.currentStage = (saved >= 1 ? saved : 1);
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


void GameData::initializeForStageCount(int32 totalStages)
{
	if (!saveManager.isStageUnlocked(1)) {
		saveManager.unlockStage(1);
	}

	int32 s = saveManager.getCurrentStage();
	if (s < 1) s = 1;
	if (s > totalStages) s = totalStages;

	saveManager.setCurrentStage(s);
	currentStage = s;
}
