#pragma once

#include <SDL.h>
#include "../Headers.h"
#include "../AssetManager.h"
#include "../Audio/Audio.h"
#include "../Text/TextService.h"
#include "../Renderer/Renderer.h"
#include "../EventBus/EventBus.h"
#include "../GUI/PopupService.h"

#include "../Scene/Scene.h"
#include "../Events/Events.h"
#include "../GUI/GUI.h"

#include "json.hpp"
#include <sol/function.hpp>
#include "../Mode7/Camera.h"

enum GameState {
    STATE_TITLE,
    STATE_LOAD,
    STATE_RUN,
    STATE_PAUSED,
    STATE_POPUP,
    STATE_END
};

const uint32_t FPS = 30;
const uint32_t MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
    //operator has a left and right argument.
    //Use friend for complete access
    //left arg is out normal output in a << statement
    //right arg is what we want to debug
    //friend std::ostream &operator<<(std::ostream &out, const Game &game);

private:
    Camera c;
    SDL_Surface *convertedSurface;

    std::vector<float> ys{546, 551, 570, 600, 640, 700, 800};
    sol::state lua;

    //TODO better way?
    EventVariant events;

    bool isFullscreen;
    bool isRunning;
    bool isDebug;

    uint32_t millisecsPreviousFrame = 0;
    double timer = 0;
    uint32_t timerTarget = 0;
    sol::function timerCallback;

    SDL_Window *window;
    SDL_Window *debugWindow;

    SDL_Renderer *debugRenderer;
    SDL_Renderer *renderer;

    std::string nextScene;

    std::unique_ptr<Renderer> animationRenderer;
    std::unique_ptr<GUI> gui;

    std::shared_ptr<AssetManager> assetManager;
    std::shared_ptr<EventBus> eventBus;
    std::shared_ptr<TextService> textService;
    std::shared_ptr<PopupService> popupService;



    bool mouseDown{};
    int startX;
    int startY;
    int mouseX;
    int mouseY;
    std::vector<nlohmann::json> savegames;

public:

    Game();

    ~Game();

    //void HandleEvents();
    void Initialize();

    void Run();


    void RenderGrid() const;

    void OnTransitionDone(TransitionDoneEvent &event);
    void OnStartIdle(StartIdleEvent &event);
    void OnFadeinDone(FadeinDoneEvent &event);
    void OnStartWait(StartWaitEvent &event);
    void OnSave(StartSaveGameEvent &event);
    void OnQuit(StartQuitEvent &event);
    void OnToMenu(StartMenuEvent &event);
    void HandleSaveDoneEvent(SaveDoneEvent &event);
    void HandleDeleteEvent(StartDeleteEvent &event);
    //void OnStartDialogEvent(const StartDialogEvent &event);
    void onPressEnter();

    void Setup();
    void LoadScripts();
    void LoadSaves();

    void ProcessInput();

    void Update();

    void Render() ;

    void Destroy();

    void SwitchFullscreen();

    static int windowWidth;
    static int windowHeight;
    static std::string currentScene;
    static uint8_t saveSlot;
    static GameState gameState;


    void initDebugWindow();

};
