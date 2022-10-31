#pragma once
#include <SDL_ttf.h>
#include <memory>
#include "../Logger/Logger.h"
#include "../Color.h"
#include "../Headers.h"
#include "../EventBus/EventBus.h"
#include "../Events/Events.h"
#include "../AssetManager.h"

enum TEXT_FLAGS {
    TEXT_BLINK = 1 << 0,
    TEXT_CENTER = 1 << 1,
    TEXT_BG_BLACK = 1 << 2,
    TEXT_UNDERLINE = 1 << 3
};

struct DialogLine {
    std::vector<std::string> lines;
    bool isRead{};
    float dialogLoop{};
    int length{};
    bool isCursive = false;
    int lastLength{};
};

class TextService {
public:


    explicit TextService(SDL_Renderer *renderer,
                         std::shared_ptr<EventBus> eventBus,
                         const std::shared_ptr<AssetManager> &assetManager
                         )
    : r(renderer),
        eventBus(std::move(eventBus)),
        assetManager(assetManager)
        {

    }
//    ~TextService() {
//        Logger::Log("destroy text");
//        lua = nullptr;
//        //delete(lua);
//    }

    void Update(double dt);
    void DrawMyText(const std::string &message, int x, int y,
                    float scale = 1.0f,
                    int flags = 0,
                    const SDL_Color &color = White,
                    bool debug = false);

    void PrepareDialog(StartDialogEvent &event);
    void RenderDialog();
    std::vector<std::string> BuildLines(const std::string &txt);

    void SubscribeToEvents();

    //TODO single event with interface?
    void onKeyPressed(KeyPressedEvent& event);
    void onMousePressed(MousePressedEvent &event);

    void OnUserAction();
    void RenderLines(DialogLine &dl);

    void SetRenderer(SDL_Renderer *renderer);



private:

    std::shared_ptr<EventBus> eventBus;
    std::shared_ptr<AssetManager> assetManager;

    double dt;
    double timer{};
    bool dialogActive{};
    sol::function callback;
    SDL_Renderer *r;

    //float dialogLoop{};
    std::vector<DialogLine> internalString;
    int renderSpeed = 20;
    //sol::table lua;
    DialogLine *currentLine;

};