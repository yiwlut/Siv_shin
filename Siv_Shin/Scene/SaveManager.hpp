#pragma once
#include <Siv3D.hpp>

class SaveManager
{
public:
	struct SaveData
	{
		int32 currentStage = 1;
		HashSet<int32> unlockedStages;
		bool openingSeen = false;
		bool finalStageCleared = false;

		void save(const FilePath& path) const
		{
			JSON json;
			json[U"currentStage"] = currentStage;
			json[U"openingSeen"] = openingSeen;
			json[U"finalStageCleared"] = finalStageCleared;

			Array<int32> stageArray;
			for (const auto& stage : unlockedStages)
			{
				stageArray.push_back(stage);
			}
			json[U"unlockedStages"] = stageArray;

			json.save(path);
		}

		bool load(const FilePath& path)
		{
			const JSON json = JSON::Load(path);
			if (!json)
			{
				return false;
			}

			currentStage = json[U"currentStage"].get<int32>();
			openingSeen = json[U"openingSeen"].get<bool>();
			finalStageCleared = json[U"finalStageCleared"].get<bool>();

			unlockedStages.clear();
			if (json[U"unlockedStages"].isArray())
			{
				for (const auto& stage : json[U"unlockedStages"].arrayView())
				{
					unlockedStages.insert(stage.get<int32>());
				}
			}

			return true;
		}
	};

private:
	SaveData m_saveData;
	FilePath m_saveFilePath;

public:
	SaveManager()
		: m_saveFilePath(FileSystem::GetFolderPath(SpecialFolder::LocalAppData) + U"TacoAdventure/savegame.json")
	{
		const FilePath saveDir = FileSystem::ParentPath(m_saveFilePath);
		if (!FileSystem::Exists(saveDir))
		{
			FileSystem::CreateDirectories(saveDir);
		}

		if (!m_saveData.load(m_saveFilePath))
		{
			m_saveData.currentStage = 1;
			m_saveData.unlockedStages.insert(1);
			m_saveData.openingSeen = false;
			m_saveData.finalStageCleared = false;
		}
	}


	int32 getCurrentStage() const
	{
		return m_saveData.currentStage;
	}

	void setCurrentStage(int32 stage)
	{
		m_saveData.currentStage = stage;
		save();
	}

	void unlockStage(int32 stage)
	{
		m_saveData.unlockedStages.insert(stage);
		save();
	}

	bool isStageUnlocked(int32 stage) const
	{
		return m_saveData.unlockedStages.contains(stage);
	}

	void clearStage(int32 stage)
	{
		m_saveData.currentStage = stage;

		unlockStage(stage + 1);

		save();
	}

	bool hasSeenOpening() const
	{
		return m_saveData.openingSeen;
	}

	void markOpeningSeen()
	{
		m_saveData.openingSeen = true;
		save();
	}

	bool isFinalStageCleared() const
	{
		return m_saveData.finalStageCleared;
	}

	void setFinalStageCleared(bool cleared)
	{
		m_saveData.finalStageCleared = cleared;
		save();
	}

	void save()
	{
		m_saveData.save(m_saveFilePath);
	}

	void resetSaveData()
	{
		m_saveData.currentStage = 1;
		m_saveData.unlockedStages.clear();
		m_saveData.unlockedStages.insert(1);
		m_saveData.openingSeen = false;
		m_saveData.finalStageCleared = false;
		save();
	}
};
