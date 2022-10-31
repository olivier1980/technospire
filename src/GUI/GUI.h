#pragma once
#include "../Headers.h"
#include "../Logger/Logger.h"
#include "../EventBus/EventBus.h"
#include "../Events/Events.h"
#include "../Scene/Scene.h"
#include "../Text/TextService.h"
#include <SDL.h>
#include <memory>
#include <sol/sol.hpp>
#include <utility>
#include "PopupService.h"


enum GUIState {
    GUI_IDLE,
    GUI_LOCKED,
    GUI_DIALOG,
};


enum EnumActionState {
    ACTION_NEUTRAL,
    ACTION_LOOK,
    ACTION_TAKE,
    ACTION_USE,
    ACTION_OPEN,
    ACTION_DEBUG_MOVE,

    ACTION_MAGIC1,
    ACTION_MAGIC2,
    ACTION_MAGIC3,

};

class GUI {
public:
    EnumActionState actionState = ACTION_NEUTRAL;

    [[nodiscard]] std::string getActionState() const;
    void resetActionState();

    static std::string enum_to_string(EnumActionState t) ;

    GUI(SDL_Renderer *renderer,
        std::string &currentScene,
        sol::state &lua,
        std::shared_ptr<EventBus> eventBus,
        std::shared_ptr<AssetManager> assetManager,
        const std::shared_ptr<PopupService> &popupService,
        const std::shared_ptr<TextService> &textService

    );

    void doInventoryAction(const std::function<void(const std::string, int loop)>& cb) {
        //inventory
        sol::table inv = lua["inventory"];
        int loop = 0;
        for (auto &t : inv) {
            std::string b = t.second.as<std::string>();
            cb(b, loop);
            ++loop;
        }
    }

    [[nodiscard]] std::string getActiveInventory() const;

    void SubscribeToEvents();

    //RenderActions correct buttons active and show GUI
    //TODO refactor
    void Render();
    void RenderActions();
    void RenderInventory();
    void RenderDebugCoords();
    void RenderMinimap();
    void RenderCursor(int x, int y);
    //Set correct action
    //Get mouse coordinates, check which action is clicked, updates state
    void OnMouseClickEvent(MousePressedEvent &e);
    void OnKeyPressedEvent(KeyPressedEvent &e);
    void OnPopupPressedEvent(PopupPressedEvent &e);
    void HandleGUIEvents(MousePressedEvent &e);
    void HandleViewportEvents(MousePressedEvent &e);
    void HandlePopupEvents(PopupPressedEvent &e);
    //void HandleSaveDoneEvent(SaveDoneEvent &e);

    void debugRect();

    static SDL_Rect rectToScreen(SDL_Rect r);

    static GUIState guiState;
    std::unordered_map<std::string, SDL_Rect> guiElements;
    std::unordered_map<std::string, SDL_Rect> guiMagicElements;
    std::unordered_map<std::string, SDL_Rect> loadElements;
    std::unordered_map<std::string, SDL_Rect> uriElements;

private:
    bool mouseClickLock{};
    std::string &currentScene;
    sol::state &lua;
    SDL_Renderer *renderer;
    std::string invSelected{};
    SDL_Rect invBoxSelected{};

    //debug:
    std::string movingItem;
    std::shared_ptr<EventBus> eventBus;
    std::shared_ptr<AssetManager> assetManager;
    std::shared_ptr<PopupService> popupService;
    std::shared_ptr<TextService> textService;
};

