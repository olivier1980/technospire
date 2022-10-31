#include "SaveService.h"
#include <filesystem>
#include <fstream>
#include "../Logger/Logger.h"
#include "../AssetManager.h"

void SaveService::DeleteGame(uint8_t saveSlot) {
    std::string searchfilename = "savegame" + std::to_string(saveSlot-'0') + ".json";
    try {
        if (std::filesystem::remove(searchfilename))
           Logger::Log("file " + searchfilename + " deleted.");
        else
            Logger::Err("Not found");
    } catch(const std::filesystem::filesystem_error& err) {
        Logger::Err(err.what());
    }
}

void SaveService::LoadGame(const nlohmann::json &slot, sol::state &lua)
{
    Logger::Log("Start Loading");
    bool magicInterface = slot["magicInterface"];
    lua["magicInterface"] = magicInterface;

    std::vector<std::string> values = slot["inventory"];
    for (const auto &i: values) {
        lua["insertInventory"](i);
    }

    for (const auto &l : levels) {
        LoadScene(slot, lua, l);
    }

    Logger::Log("Loaded");

}

void SaveService::LoadScene(const nlohmann::json &slot, sol::state &lua, const std::string &sceneName)
{
    bool sceneVisited = slot[sceneName]["isVisited"];
    bool isPlayed = slot[sceneName]["isPlayed"];
    lua[sceneName]["isVisited"] = sceneVisited;
    lua[sceneName]["isPlayed"] = isPlayed;

    if (slot[sceneName].contains("bgSprite")) {
        lua[sceneName]["bgSprite"] = std::string{slot[sceneName]["bgSprite"]};
    }
    //TODO load bgsprite

    sol::table luaScene = lua[sceneName]["actionItems"];

    if (!slot[sceneName]["actionItems"].empty()) {
        for (const auto &row: slot[sceneName]["actionItems"].items()) {

            for (const auto &test: row.value().items()) {
                if (test.key().starts_with("is")) {
                    bool jsonVal = test.value();
                    lua[sceneName]["actionItems"][row.key()][test.key()] = jsonVal;
                } else if (test.key() == "currentFrame") {
                    int jsonVal = test.value();
                    lua[sceneName]["actionItems"][row.key()][test.key()] = jsonVal;
                } else if (test.key() == "rect") {
                    float rectx = test.value()[0];
                    float recty = test.value()[1];

                    lua[sceneName]["actionItems"][row.key()]["rect"][1] = rectx;
                    lua[sceneName]["actionItems"][row.key()]["rect"][2] = recty;
                }

            }

        }
    }

}


nlohmann::json SaveService::getSaveGame(uint8_t saveSlot) {

    std::string file_name = "savegame" + std::to_string(saveSlot);

    nlohmann::json data;
    std::ifstream file(path + "/" + file_name + ".json");
    file >> data;
    return data;
}

void SaveService::SaveGame(
        uint8_t saveSlot,
        const std::string &currentScene,
        const sol::state &lua

) {
    // std::string path = "./";
    std::string file_name = "savegame" + std::to_string(saveSlot);
    nlohmann::json data;

    std::vector<std::string> inventory;
    sol::table inv = lua["inventory"];
    for (auto &t : inv) {
        std::string b = t.second.as<std::string>();
        inventory.emplace_back(b);
    }

    data["inventory"] = inventory;
    data["datetime"] = getDateString();
    data["currentScene"] = currentScene;
    data["magicInterface"] = lua["magicInterface"].get_or(false);

    for (const auto &l : levels) {
        convertActionItems(lua, l, data);
    }




    std::ofstream file(path + "/" + file_name + ".json");
    file << std::setw(4) << data << std::endl;

}

//https://stackoverflow.com/questions/10873382/write-and-read-string-to-binary-file-c
void SaveService::convertActionItems(const sol::state &lua, const std::string& scene, nlohmann::json &data) {
    //grab scene sprite, isvisited
    data[scene]["isVisited"] = lua[scene]["isVisited"].get_or(false);


    sol::optional<std::string> sprite = lua[scene]["bgSprite"];
    if (sprite) {
        data[scene]["bgSprite"] = sprite.value();
    }

    data[scene]["isPlayed"] = lua[scene]["isPlayed"].get_or(false);

    sol::table luaScene = lua[scene]["actionItems"];
    for (auto &actionItem : luaScene) {
        std::string key = actionItem.first.as<std::string>();
        //Logger::Log(key);
        sol::table rows = actionItem.second;

        for (auto &itemRows : rows) {
            std::string property = itemRows.first.as<std::string>();

            if (property.starts_with("is")) {
                bool val = itemRows.second.as<bool>();
                data[scene]["actionItems"][key][property] = val;
            } else if (property == "currentFrame") {
                int val = itemRows.second.as<int>();
                data[scene]["actionItems"][key][property] = val;
            } else  if (property == "rect") {
                std::vector<int> rect;
                sol::table luaRect = itemRows.second;
                rect.emplace_back(luaRect.get<double>(1));
                rect.emplace_back(luaRect.get<double>(2));
                rect.emplace_back(luaRect.get<double>(3));
                rect.emplace_back(luaRect.get<double>(4));
                data[scene]["actionItems"][key][property] = rect;
            }
        }


    }
}

std::string SaveService::getDateString() {
    time_t curr_time;
    tm *curr_tm;
    char date_string[100];

    time(&curr_time);
    curr_tm = localtime(&curr_time);

    strftime(date_string, 50, "%d-%m-%Y %H:%M", curr_tm);
    return date_string;
}
