#pragma once

#include <SDL.h>
#include <string>
#include "sol/table.hpp"
#include "../Util/Helper.h"
#include "../AssetManager.h"
struct Scene {

    explicit Scene(const sol::table &scene) {
        sol::optional<std::string> luaMusic = scene["bgMusic"];
        if (luaMusic) {
            bgMusic = luaMusic.value();
        }
        sol::optional<std::string> luaSprite = scene["bgSprite"];
        if (luaSprite) {
            bgSprite = luaSprite.value();
        }
        isVisited = scene["isVisited"].get_or(false);
    }

    bool isVisited{};
    std::string bgMusic{};
    std::string bgSprite{};
};

struct ActionItem {

    ActionItem() = default;

    explicit ActionItem(const std::pair<sol::basic_object<sol::basic_reference<false>>,
               sol::basic_object<sol::basic_reference<false>>> &actionItemTable) {

        name = actionItemTable.first.as<std::string>();
        sol::table actionItem = actionItemTable.second;

        sol::optional<sol::table> luaRect = actionItem["rect"];
        if (luaRect) {
            rect = SDL_Rect{
                    (int)luaRect.value().get<double>(1)+25,
                    (int)luaRect.value().get<double>(2)+25,
                    (int)luaRect.value().get<double>(3),
                    (int)luaRect.value().get<double>(4)
            };

           // Logger::Log("X: " + std::to_string(rect.x));
            Helper::getRekt(rect);
        }

        sol::optional<sol::table> luaRect2 = actionItem["click_rect"];
        if (luaRect2) {
            clickRect = SDL_Rect{
                    (int)(luaRect2.value().get<double>(1)+25),
                    (int)(luaRect2.value().get<double>(2)+25),
                    (int)luaRect2.value().get<double>(3),
                    (int)luaRect2.value().get<double>(4)
            };
            Helper::getRekt(clickRect);
        }

        sol::optional<sol::table> luaMap = actionItem["minimap"];
        if (luaMap) {
            minimap = SDL_Rect{
                    luaMap.value().get<int>(1),
                    luaMap.value().get<int>(2),
                    32,
                    32
            };
        }

        sort = actionItem["sort"].get_or(0);
        currentFrame = actionItem["currentFrame"].get_or(0);
        isActive = actionItem["isActive"].get_or(true);
        isVisible = actionItem["isVisible"].get_or(true);
        isLocked = actionItem["isLocked"].get_or(false);
        isMinimap = actionItem["isMinimap"].get_or(true);
        toDeath = actionItem["toDeath"].get_or(false);
        isAutoStart = actionItem["isAutoStart"].get_or(true);

        sol::optional<std::string> luaDefaultAction = actionItem["defaultAction"];
        if (luaDefaultAction) {
            defaultAction = luaDefaultAction.value();
        }
        sol::optional<std::string> luaDest = actionItem["destination"];
        if (luaDest) {
            destination = luaDest.value();
        }
        sol::optional<std::string> luaDeathDialog = actionItem["deathDialog"];
        if (luaDeathDialog) {
            deathDialog = luaDeathDialog.value();
        }

        sol::optional<std::string> luaAnimatedSprite = actionItem["animatedSprite"];
        sol::optional<std::string> luaSprite = actionItem["sprite"];
        if (luaSprite) {
            sprite = luaSprite.value();
        } else if (luaAnimatedSprite) {
            isAnimated = true;
            sprite = luaAnimatedSprite.value();
        }
    }

    bool operator == (const ActionItem& other) const {
        return name == other.name;
    }
    bool operator != (const ActionItem& other) const {
        return name != other.name;
    }
    bool operator > (const ActionItem& other) const {
        return sort > other.sort;
    }
    bool operator < (const ActionItem& other) const {
        return sort < other.sort;
    }

    //Coordinates on screen
    SDL_Rect rect{};
    SDL_Rect clickRect{};
    SDL_Rect minimap{};

    //graphics
    bool isAnimated{};
    bool isVisible;
    bool isActive;
    bool isMinimap;
    bool isLocked{};
    bool isAutoStart{};
    bool toDeath;

    int currentFrame {};

    int sort{};

    std::string name{};
    std::string sprite{};
    std::string destination{};
    std::string defaultAction = "look";
    std::string deathDialog{};
};

