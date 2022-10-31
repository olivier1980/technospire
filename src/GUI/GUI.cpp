#include "GUI.h"
#include <SDL.h>
#include "PopupService.h"
#include "../Game/Game.h"
//class Game;


//https://stackoverflow.com/questions/17347950/how-do-i-open-a-url-from-c
#include <windows.h>
#include <shellapi.h>

#include <utility>


GUIState GUI::guiState = GUI_IDLE;

GUI::GUI(SDL_Renderer *renderer,
        std::string &currentScene,
        sol::state &lua,
         std::shared_ptr<EventBus> eventBus,
         std::shared_ptr<AssetManager> assetManager,
         const std::shared_ptr<PopupService> &popupService,
         const std::shared_ptr<TextService> &textService

        
        ) :
        
        renderer(renderer),
        currentScene(currentScene),
        lua(lua),
        eventBus(std::move(eventBus)),
        assetManager(std::move(assetManager)),
        popupService(popupService),
        textService(textService)

        
        {

}


void GUI::HandleGUIEvents(MousePressedEvent &e) {

    Logger::Log("handle click event GUI");

    int normalizedX = e.x;
    int normalizedY = e.y;

    //Logger::Log(e.sdlEvent.x);

    SDL_Point p{normalizedX, normalizedY};

    if (currentScene.empty()) {
        return;
    }

    //check menu box
    if (SDL_PointInRect(&p, &guiElements["menu"])) {
        Audio::GetInstance().PlayWav("select");
        Game::gameState = STATE_POPUP;
        popupService->ShowConfirmation("Quit to menu?", "to_menu");
        return;
    }


    //check Credits box
    if (SDL_PointInRect(&p, &guiElements["credits"])) {
        Audio::GetInstance().PlayWav("select");
        Game::gameState = STATE_POPUP;
        popupService->ShowCredits();
        return;
    }

    if (lua["magicInterface"] && currentScene != "SceneDeath") {
        if (SDL_PointInRect(&p, &guiMagicElements["magic1"])) {
            Audio::GetInstance().PlayWav("select");
            if (actionState == ACTION_MAGIC1) {
                actionState = ACTION_NEUTRAL;
            } else {
                actionState = ACTION_MAGIC1;
            }

            return;
        }
        if (SDL_PointInRect(&p, &guiMagicElements["magic2"])) {
            Audio::GetInstance().PlayWav("select");
            if (actionState == ACTION_MAGIC2) {
                actionState = ACTION_NEUTRAL;
            } else {
                actionState = ACTION_MAGIC2;
            }

            return;
        }
        if (SDL_PointInRect(&p, &guiMagicElements["magic3"])) {
            Audio::GetInstance().PlayWav("select");
            if (actionState == ACTION_MAGIC3) {
                actionState = ACTION_NEUTRAL;
            } else {
                actionState = ACTION_MAGIC3;
            }

            return;
        }
    }


    //check save box
    if (SDL_PointInRect(&p, &guiElements["save"]) && currentScene != "SceneDeath") {
        //save to Game::saveSlot
        Audio::GetInstance().PlayWav("select");
        /*
         * save
         * -inventory
         * -scene visited
         * -scene actionitems
         * -current scene
         *
         * emit save game event?
         */
        Game::gameState = STATE_POPUP;
        popupService->ShowConfirmation("Save game?", "save_info");

        return;
    }

    //check quit button
    if (SDL_PointInRect(&p, &guiElements["quit"])) {
        Audio::GetInstance().PlayWav("select");
        Game::gameState = STATE_POPUP;
        popupService->ShowConfirmation("Quit game?", "quit");

        return;
    }

    //Handle minimap click
    sol::table actionItems = lua[currentScene]["actionItems"];

    std::list<ActionItem> tmp;

    for (auto &item: actionItems) {
        sol::table row = item.second;
        ActionItem ai = ActionItem{item};
        if (ai.isVisible) {
            tmp.emplace_back(ai);
        }
    }
    tmp.sort();

    for (const auto &ai: tmp) {
        auto mp = ai.minimap;
        //Logger::Log("Handling minimap " + ai.name);
        Helper::getRekt(mp);
        if (SDL_PointInRect(&p, &mp) && ai.isMinimap) {

            resetActionState();

            if (ai.toDeath) {
                eventBus->EmitEvent<StartDialogEvent>(ai.deathDialog, lua["gotoDeath"]);
            } else {
                if (ai.destination == "SceneElevator" || ai.destination == "SceneEnd") {
                    eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEOUT, ai.destination, false);
                } else {
                    eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEOUT, ai.destination);
                }

            }
            return;
        }
    }


    auto switchv = [&](EnumActionState v) {
        Audio::GetInstance().PlayWav("select");
        if (actionState == v) {
            actionState = ACTION_NEUTRAL;
        } else {
            actionState = v;
        }
    };

//    SDL_Rect boxOpen{185, 296, 82, 63};
//    if (SDL_PointInRect(&p, &boxOpen)) {
//        switchv(ACTION_LOOK);
//    }

    if (currentScene != "SceneDeath") {
        SDL_Rect boxOpen = {181, 296, 82, 42};
        Helper::getRekt(boxOpen);
        if (SDL_PointInRect(&p, &boxOpen)) {
            switchv(ACTION_OPEN);
        }

        boxOpen = {181, 393, 82, 42};
        Helper::getRekt(boxOpen);
        if (SDL_PointInRect(&p, &boxOpen)) {
            switchv(ACTION_USE);
        }
        boxOpen = {181, 345, 82, 42};
        Helper::getRekt(boxOpen);
        if (SDL_PointInRect(&p, &boxOpen)) {
            switchv(ACTION_TAKE);
        }
    }

//    boxOpen = {144, 180, 40, 9};
//    if (SDL_PointInRect(&p, &boxOpen)) {
//        switchv(ACTION_DEBUG_MOVE);
//    }


    auto currentItem = lua["holdingItem"];
    //if (actionState == ACTION_USE) {
    doInventoryAction([&](const std::string &b, int loop) {
        SDL_Rect boxInv = {301, (61) + loop * 27, 162, 24};
        Helper::getRekt(boxInv);
        if (SDL_PointInRect(&p, &boxInv)) {

            if (!currentItem.valid() || currentItem != b) {
                lua.set("holdingItem", b);
            } else if (currentItem == b) {
                lua.set("holdingItem", nullptr);
            }

            if (currentItem.valid()) {
                actionState = ACTION_USE;
            } else {
                actionState = ACTION_NEUTRAL;
            }
            return;
        }
    });

    //if (actionState) {
    Logger::Log("Actionstate after GUIevents: " + enum_to_string(actionState));
    //}

}

void GUI::HandlePopupEvents(PopupPressedEvent &e) {
    int normalizedX = e.x;
    int normalizedY = e.y;
    SDL_Point p{e.x, e.y};

    auto result = popupService->checkClick(normalizedX, normalizedY);
    //hasValue ? value or else


    if (popupService->_id == "credits") {

        //SDL_Rect uri {34,301,443,22};
        if (SDL_PointInRect(&p, &uriElements["uri1"])) {
            ShellExecute(0, 0, "https://twitter.com/crazy_descent", 0, 0, SW_SHOW);
            return;
        } else if (SDL_PointInRect(&p, &uriElements["uri2"])) {
            ShellExecute(0, 0, "https://dominictramontanaportfolio.wordpress.com/", 0, 0, SW_SHOW);
            return;
        } else if (SDL_PointInRect(&p, &uriElements["uri3"])) {
            ShellExecute(0, 0, "https://soundcloud.com/7withoutthe11", 0, 0, SW_SHOW);
            return;
        }
    }

    if (result.has_value()) {
        if (popupService->_id == "save_info" && result.value() == "YES") {
            popupService->Clear();
            //guiState = GUI_LOCKED;
            //TODO have save singleton handle this directly?
            eventBus->EmitEvent<StartSaveGameEvent>();
        } else if (popupService->_id == "quit" && result.value() == "YES") {
            popupService->Clear();
            //guiState = GUI_LOCKED;
            eventBus->EmitEvent<StartQuitEvent>();
        } else if (popupService->_id == "to_menu" && result.value() == "YES") {
            popupService->Clear();
            //guiState = GUI_LOCKED;
            eventBus->EmitEvent<StartMenuEvent>();
        } else if (popupService->_id.starts_with("del")) {
            if (result.value() == "YES") {

                auto last = popupService->_id.back();
                eventBus->EmitEvent<StartDeleteEvent>(int(last));
            } else {
                Game::gameState = STATE_LOAD;
            }
            popupService->Clear();
        } else {
            popupService->Clear();
            Game::gameState = STATE_RUN;
        }
    }
}

void GUI::HandleViewportEvents(MousePressedEvent &e) {

    if (currentScene.empty()) {
        return;
    }

    SDL_Point p{e.x, e.y};
    //compare screen pixels
    SDL_Rect d = MAIN_RECT;

    if (!SDL_PointInRect(&p, &d)) {
        return;
    }

    Logger::Log("handle click event viewport");

    sol::table scene = lua[currentScene];
    sol::table actionItems = scene["actionItems"];

    std::list<ActionItem> tmp;

    for (auto &item: actionItems) {
        sol::table row = item.second;
        ActionItem ai = ActionItem{item};
        if (ai.isVisible) {
            tmp.emplace_back(ai);
        }
    }
    tmp.sort();

    for (const auto &ai: tmp) {
        if (!ai.isActive) {
            continue;
        }
        Logger::Log("Handling " + ai.name);

        SDL_Rect r = ai.rect;
        //Optional override with click-only rect
        if (ai.clickRect.w > 0) {
            r = ai.clickRect;
        }

        std::string localAction = getActionState().empty() ? ai.defaultAction : getActionState();

        if (SDL_PointInRect(&p, &r)) {
            //Logger::Log("calling on" + Helper::UCFirst(localAction));
            sol::optional<sol::function> callbackTmp = actionItems[ai.name]["on" + Helper::UCFirst(localAction)];
            if (callbackTmp) {
                Logger::Log("calling on" + Helper::UCFirst(localAction));
                sol::function &callback = callbackTmp.value();
                callback(actionItems[ai.name], scene);
            } else if (actionState == ACTION_DEBUG_MOVE) {
                Logger::Log("moving " + ai.name);
                movingItem = ai.name;
            } else {
                //default text for action
                std::string itemDialog = lua["findDialog"](actionItems[ai.name], localAction);
                eventBus->EmitEvent<StartDialogEvent>(itemDialog);

                //events.emplace_back(StartDialogEvent{itemDialog});
                Logger::Log("no callback found, default text for " + localAction);
            }

            resetActionState();
            //Don't continue checking items after first match!
            return;
        }

    }
}

void GUI::OnKeyPressedEvent(KeyPressedEvent &e) {

//    auto x = lua[currentScene]["actionItems"][movingItem]["rect"][1];
//    auto y = lua[currentScene]["actionItems"][movingItem]["rect"][2];
//
//    switch (e.symbol.sym) {
//        case SDLK_LEFT:
//            x.set((int) x - 1);
//            break;
//        case SDLK_RIGHT:
//            x.set((int) x + 1);
//            break;
//        case SDLK_UP:
//            y.set((int) y - 1);
//            break;
//        case SDLK_DOWN:
//            y.set((int) y + 1);
//            break;
//    }
}

//Set correct action
//Get mouse coordinates, check which action is clicked, updates state
void GUI::OnMouseClickEvent(MousePressedEvent &e) {
    if (guiState == GUI_IDLE) {
        if (currentScene == "SceneTutorial") {

            std::string next = "SceneGameEntrance";
            StartTransitionEvent event {TRANSITION_FADEOUT, next};
            eventBus->EmitEvent<StartTransitionEvent>(event);
            return;
        }
        HandleGUIEvents(e);
        HandleViewportEvents(e);
    }
};


void GUI::OnPopupPressedEvent(PopupPressedEvent &e) {
    //if (guiState == GUI_POPUP) {
    HandlePopupEvents(e);
    //}
}

void GUI::Render() {

    if (guiState != GUI_DIALOG) {
        //render gui actions
        assetManager->CopyGUIToWindow();

        if (lua["magicInterface"]) {
            SDL_Rect dest = {279, 294, 88, 145};
            Helper::getRekt(dest);
            assetManager->CopySpriteToRenderer("gui_magic", dest);
        }

        //renders minimap, inv
        //TODO refactor to seperate renders?
        RenderActions();
        RenderMinimap();
    }

    //Draw inventory names in list
    if (currentScene != "SceneDeath") {
        RenderInventory();
    }
    RenderDebugCoords();

    //Logger::Log(currentScene);
    if (currentScene == "SceneDeath") {
        SDL_SetRenderDrawColor(renderer, 156,156,156,255);
        SDL_Rect d = {177,292,191,147};
        Helper::getRekt(d);
        SDL_RenderFillRect(renderer, &d);

        SDL_SetRenderDrawColor(renderer, 0,0,0,155);
        d = {398,281,98,35};
        Helper::getRekt(d);
        SDL_RenderFillRect(renderer, &d);
    }
}

void GUI::RenderCursor(int x, int y) {

    if (currentScene == "SceneTutorial") {
        assetManager->RenderCursor(x, y, 2);
        return;
    }

    SDL_Point p{x, y};

    if (Game::gameState == STATE_LOAD) {
        for (const auto &el: loadElements) {
            auto rect = el.second;
            if (SDL_PointInRect(&p, &rect)) {
                assetManager->RenderCursor(x, y, 2);
                return;
            }
        }

    } else if (Game::gameState == STATE_POPUP && popupService->_id == "credits") {
        for (const auto &el: uriElements) {
            auto rect = el.second;
            if (SDL_PointInRect(&p, &rect)) {
                assetManager->RenderCursor(x, y, 2);
                return;
            }
        }

    } else if (!currentScene.empty() && Game::gameState == STATE_RUN &&
        guiState != GUI_DIALOG
    ) {

        if (currentScene == "SceneDeath" ) {

            for (const auto &el: guiElements) {
                auto rect = el.second;
                if (el.first == "credits" || el.first == "quit"  || el.first == "menu") {
                if (SDL_PointInRect(&p, &rect)) {
                    assetManager->RenderCursor(x, y, 2);
                    return;
                }
                }
            }
            assetManager->RenderCursor(x, y, 1);
            return;
        }

        for (const auto &el: guiElements) {
            auto rect = el.second;
            if (SDL_PointInRect(&p, &rect)) {
                assetManager->RenderCursor(x, y, 2);
                return;
            }
        }

        if (lua["magicInterface"]) {
            for (const auto &el: guiMagicElements) {
                auto rect = el.second;
                if (SDL_PointInRect(&p, &rect)) {
                    assetManager->RenderCursor(x, y, 2);
                    return;
                }
            }
        }

        doInventoryAction([&](const std::string &b, int loop) {
            SDL_Rect boxInv = {301, (61) + loop * 27, 162, 24};
            Helper::getRekt(boxInv);
            if (SDL_PointInRect(&p, &boxInv)) {
                assetManager->RenderCursor(x, y, 2);
                return;
            }
        });

        sol::table actionItems = lua[currentScene]["actionItems"];
        for (auto &item: actionItems) {

            sol::table row = item.second;
            ActionItem ai(item);

            if (!ai.isActive) {
                continue;
            }

            if (ai.minimap.x > 0 && ai.isMinimap) {
                SDL_Rect bg = ai.minimap;
                Helper::getRekt(bg);
                if (SDL_PointInRect(&p, &bg)) {
                    assetManager->RenderCursor(x, y, 2);
                    return;
                }
            }

            if (!ai.isVisible) {
                //Logger::Log("Skip invisible");
                continue;
            }

            SDL_Rect r = ai.rect;

            //Optional override with click-only rect
            if (ai.clickRect.w > 0) {
                r = ai.clickRect;
            }

            if (SDL_PointInRect(&p, &r)) {
                assetManager->RenderCursor(x, y, 2);
                return;
            }
        }
    }


    assetManager->RenderCursor(x, y, 1);

}

void GUI::RenderDebugCoords() {

    if (actionState == ACTION_DEBUG_MOVE && !movingItem.empty()) {
        auto x = lua[currentScene]["actionItems"][movingItem]["rect"][1];
        auto y = lua[currentScene]["actionItems"][movingItem]["rect"][2];
        textService->DrawMyText(
                "X: " + std::to_string((int) x) + ", Y: " + std::to_string((int) y),
                10, 10, 1.0f, TEXT_BG_BLACK, White, true);
    }
}

void GUI::RenderInventory() {
    auto currentItem = lua["holdingItem"];

    doInventoryAction([&](const std::string &b, int loop) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        textService->DrawMyText(b,
                                              301 * PIXEL_SIZE,
                                              (63 + (loop * 27)) * PIXEL_SIZE,
                                              1.0f,
                                              0,
                                              Black
        );

        if (currentItem == b) {
            invBoxSelected = {
                    288 * PIXEL_SIZE,
                    (66 + (loop * 27)) * PIXEL_SIZE,
                    10 * PIXEL_SIZE,
                    10 * PIXEL_SIZE
            };
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &invBoxSelected);
            return;
        }
    });

}

void GUI::SubscribeToEvents() {

    eventBus->SubscribeToEvent<MousePressedEvent>(this, &GUI::OnMouseClickEvent);
    eventBus->SubscribeToEvent<KeyPressedEvent>(this, &GUI::OnKeyPressedEvent);
    //eb.SubscribeToEvent<SaveDoneEvent>(this, &GUI::HandleSaveDoneEvent);
    eventBus->SubscribeToEvent<PopupPressedEvent>(this, &GUI::OnPopupPressedEvent);
}

std::string GUI::getActionState() const {
    return enum_to_string(actionState);
}

void GUI::resetActionState() {
    actionState = ACTION_NEUTRAL;
    lua.set("holdingItem", nullptr);
}

std::string GUI::enum_to_string(EnumActionState t) {
    switch (t) {
        case ACTION_OPEN:
            return "open";
        case ACTION_LOOK:
            return "look";
        case ACTION_TAKE:
            return "take";
        case ACTION_USE:
            return "use";
        case ACTION_NEUTRAL:
            return "";
        case ACTION_MAGIC1:
            return "magicLight";
        case ACTION_MAGIC2:
            return "magicShield";
        case ACTION_MAGIC3:
            return "magicOrb";
        case ACTION_DEBUG_MOVE:
            return "debugmove";
        default:
            Logger::Err("Invalid enum in enum_to_string");
            exit(-1);
    }
}


//RenderActions correct buttons active and show GUI
//TODO render function was const, but didn't work with lambda
void GUI::RenderActions() {

    bool render1 = true;
    bool render2 = true;
    bool render3 = true;

    //RenderActions GUI checkboxes
    if (actionState != ACTION_NEUTRAL) {
        SDL_Rect d;
        switch (actionState) {
//            case ACTION_LOOK:
//                d = {217, 336, 17,17};
//                break;
            case ACTION_TAKE:
                d = {191, 355, 66, 21};
                Helper::getRekt(d);
                assetManager->CopySpriteToRenderer("gui_take", d);
                break;
            case ACTION_USE:
                d = {198, 405, 49, 21};
                Helper::getRekt(d);
                assetManager->CopySpriteToRenderer("gui_use", d);
                break;
            case ACTION_OPEN:
                d = {191, 307, 66, 21};
                Helper::getRekt(d);
                assetManager->CopySpriteToRenderer("gui_open", d);
                break;

        }
    }

    if (lua["magicInterface"]) {
        switch (actionState) {
            case ACTION_MAGIC1:
                render1 = false;
                break;
            case ACTION_MAGIC2:
                render2 = false;
                break;
            case ACTION_MAGIC3:
                render3 = false;
                break;
        }

        SDL_Rect d{};
        if (render1) {
            d = {283, 298, 78, 36};
            Helper::getRekt(d);
            assetManager->CopySpriteToRenderer("magic_light", d);
        }
        if (render2) {
            d = {295, 347, 52, 37};
            Helper::getRekt(d);
            assetManager->CopySpriteToRenderer("magic_shield", d);
        }
        if (render3) {
            d = {299, 396, 43, 37};
            Helper::getRekt(d);
            assetManager->CopySpriteToRenderer("magic_orb", d);
        }
    }

    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderFillRect(renderer, &d);



}

void GUI::RenderMinimap() {

    sol::table scene = lua[currentScene];

    //RenderActions minimap
    sol::table actionItems = scene["actionItems"];

    for (auto &item: actionItems) {
        sol::table row = item.second;
        ActionItem ai(item);


        if (ai.minimap.x > 0 && ai.isMinimap) {

            SDL_Rect fg{ai.minimap.x + 4, ai.minimap.y + 4, 24, 24};
            Helper::getRekt(fg);
            SDL_Rect bg = ai.minimap;
            Helper::getRekt(bg);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, &bg);
            SDL_SetRenderDrawColor(renderer, 255, 166, 0, 255);
            SDL_RenderFillRect(renderer, &fg);
        }
    }
}

void GUI::debugRect() {
    sol::table scene = lua[currentScene];
    sol::table actionItems = scene["actionItems"];

//    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
//    SDL_Rect save{192, 214, 42, 8};
//    SDL_RenderFillRect(renderer, &save);

    for (auto &item: actionItems) {
        ActionItem ai(item);
        if (ai.rect.w > 0) {
            SDL_RenderDrawRect(renderer, &ai.rect);
        }
    }
}

std::string GUI::getActiveInventory() const {
    return invSelected;
}