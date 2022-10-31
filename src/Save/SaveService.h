#pragma once
#include "sol/state.hpp"
#include <json.hpp>

//SaveService logic as a singleton class
//different save classes, cloudstorage, local etc.
class SaveService {
public:
    SaveService(const SaveService&) = delete;
    SaveService(SaveService&&) = delete;
    SaveService& operator=(const SaveService&) = delete;
    SaveService& operator=(SaveService&&) = delete;
public:

    static SaveService &GetInstance() {
        static SaveService INSTANCE;
        return INSTANCE;
    }

    std::vector<std::string> levels {
            "SceneGameEntrance",
            "SceneLobby",
            "SceneOldHallway",
            "SceneOldGorm",
            "SceneStaircase",
            "SceneDoors",
            "SceneDoors2",
            "SceneBasement",
            "SceneShop",
            "SceneArmory",
            "SceneDining",
            "SceneCliff",
            "SceneFountain",
            "SceneBoss"
    };

    std::string path = "./";

    void LoadGame(const nlohmann::json &slot, sol::state &lua);

    void DeleteGame(uint8_t saveSlot);
    
    void LoadScene(const nlohmann::json &slot, sol::state &lua, const std::string &sceneName);

    nlohmann::json getSaveGame(uint8_t saveSlot);

    void SaveGame(uint8_t saveSlot, const std::string &currentScene, const sol::state &lua);

    //https://stackoverflow.com/questions/10873382/write-and-read-string-to-binary-file-c
    void convertActionItems(const sol::state &lua, const std::string& scene, nlohmann::json &data);

private:
    SaveService() = default;
    static std::string getDateString();
};

