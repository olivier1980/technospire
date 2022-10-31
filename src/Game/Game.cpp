#include "Game.h"
#include <SDL.h>
#include <memory>
#include <random>
#include <utility>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <string>
#include "../Save/SaveService.h"
#include "../base64/base64.h"
#include "../Events/Events.h"
#include <fstream>
#include "../Mode7/function.h"
#include "../Util/md5.h"

//std::ostream& operator<< (std::ostream& out, const Game& game) {
//    out << game.windowWidth;
//    return out;
//}

int Game::windowWidth;
int Game::windowHeight;
std::string Game::currentScene;
GameState Game::gameState;
uint8_t Game::saveSlot;

Game::Game() {
    isDebug = false;
    isRunning = false;
    isFullscreen = false;
    gameState = STATE_TITLE;
    currentScene = "";
    nextScene = "";

    Logger::Log("Game constructor called!");
}

Game::~Game() {

    //Logger::Log("Destruct Game");
    //assetManager->Clean();
    //Audio::GetInstance().Free();
}

void Game::initDebugWindow() {

    debugWindow = SDL_CreateWindow(
            "Debug",
            (1920 - 500),
            SDL_WINDOWPOS_UNDEFINED,
            500,
            800,
            SDL_WINDOW_RESIZABLE
    );

    if (!debugWindow) {
        Logger::Err("Error creating SDL debug window.");
        exit(-1);
    }

    //|SDL_RENDERER_TARGETTEXTURE
    debugRenderer = SDL_CreateRenderer(debugWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!debugRenderer) {
        Logger::Err("Error creating SDL debugRenderer.");
        exit(SDL_NO_VIDEO);
    }
    //init imgui
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(debugWindow, debugRenderer);
    ImGui_ImplSDLRenderer_Init(debugRenderer);

}

//Setup engine
void Game::Initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Logger::Err("Error initializing SDL.");
        exit(SDL_NO_VIDEO);
    }

    if (SDL_Init(TTF_Init()) != 0) {
        Logger::Err("Error initializing SDL.");
        exit(SDL_NO_TEXT);
    }
    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");


    // prefix underscore?
    windowWidth = WINDOW_WIDTH;
    windowHeight = WINDOW_HEIGHT;
    window = SDL_CreateWindow(
            "Tecnospire",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            SDL_WINDOW_OPENGL
    );
    if (!window) {
        Logger::Err("Error creating SDL window.");
        exit(SDL_NO_VIDEO);
    }

    //|SDL_RENDERER_TARGETTEXTURE
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        Logger::Err("Error creating SDL renderer.");
        exit(SDL_NO_VIDEO);
    }

    SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);


    lua.open_libraries(sol::lib::base, sol::lib::table);

    //EventBus::GetInstance();


    eventBus = std::make_shared<EventBus>();
    assetManager = std::make_shared<AssetManager>(renderer, eventBus);
    assetManager->SubscribeToEvents();
    assetManager->Load(lua);


    textService = std::make_shared<TextService>(renderer, eventBus, assetManager);

    popupService = std::make_shared<PopupService>(renderer);



    //TextService component will skip text on any click

    textService->SubscribeToEvents();

    gui = std::make_unique<GUI>(renderer, currentScene, lua, eventBus, assetManager, popupService, textService);
    gui->SubscribeToEvents();

    animationRenderer = std::make_unique<Renderer>(renderer, lua, eventBus);
    animationRenderer->SubscribeToEvents();

    auto &audio = Audio::GetInstance();
    audio.Init();

    if (IS_DEBUG) {
        initDebugWindow();
    }

    SDL_ShowCursor(SDL_DISABLE);

    //init listeners
    eventBus->SubscribeToEvent<TransitionDoneEvent>(this, &Game::OnTransitionDone);
    eventBus->SubscribeToEvent<FadeinDoneEvent>(this, &Game::OnFadeinDone);
    eventBus->SubscribeToEvent<StartIdleEvent>(this, &Game::OnStartIdle);
    eventBus->SubscribeToEvent<StartWaitEvent>(this, &Game::OnStartWait);
    eventBus->SubscribeToEvent<StartSaveGameEvent>(this, &Game::OnSave);
    eventBus->SubscribeToEvent<StartQuitEvent>(this, &Game::OnQuit);
    eventBus->SubscribeToEvent<StartMenuEvent>(this, &Game::OnToMenu);
    eventBus->SubscribeToEvent<SaveDoneEvent>(this, &Game::HandleSaveDoneEvent);
    eventBus->SubscribeToEvent<StartDeleteEvent>(this, &Game::HandleDeleteEvent);

    auto getRekt = [](SDL_Rect input) -> SDL_Rect {
        return SDL_Rect{
                .x = input.x * PIXEL_SIZE,
                .y = input.y * PIXEL_SIZE,
                .w = input.w * PIXEL_SIZE,
                .h = input.h * PIXEL_SIZE,
        };
    };

    gui->guiElements.emplace("menu", getRekt(SDL_Rect{397, 327, 101, 38}));
    gui->guiElements.emplace("quit", getRekt(SDL_Rect{397, 417, 101, 38}));
    gui->guiElements.emplace("open", getRekt(SDL_Rect{181, 296, 82, 42}));
    gui->guiElements.emplace("take", getRekt(SDL_Rect{181, 345, 82, 42}));
    gui->guiElements.emplace("save", getRekt(SDL_Rect{397, 279, 101, 38}));
    gui->guiElements.emplace("use", getRekt(SDL_Rect{181, 393, 82, 42}));
    gui->guiElements.emplace("credits", getRekt(SDL_Rect{397, 371, 101, 38}));

    gui->guiMagicElements.emplace("magic1", getRekt(SDL_Rect{279, 294, 83, 43}));
    gui->guiMagicElements.emplace("magic2", getRekt(SDL_Rect{279, 343, 83, 43}));
    gui->guiMagicElements.emplace("magic3", getRekt(SDL_Rect{279, 392, 83, 43}));

    gui->loadElements.emplace("save1", getRekt(SDL_Rect{192, 141, 100, 46}));
    gui->loadElements.emplace("save2", getRekt(SDL_Rect{192, 189, 100, 46}));
    gui->loadElements.emplace("save3", getRekt(SDL_Rect{192, 238, 100, 46}));

    gui->uriElements.emplace("uri1", getRekt(SDL_Rect{57, 82, 443, 22}));
    gui->uriElements.emplace("uri2", getRekt(SDL_Rect{3, 202, 443, 22}));
    gui->uriElements.emplace("uri3", getRekt(SDL_Rect{34, 326, 443, 22}));

    SDL_Surface *surface = IMG_Load("./assets/grid.png");
    convertedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    audio.bg.emplace("title", Mix_LoadMUS("./assets/mp3/title.mp3"));
    audio.bg.emplace("lobby", Mix_LoadMUS("./assets/mp3/lobby.mp3"));
    audio.bg.emplace("death", Mix_LoadMUS("./assets/mp3/death.mp3"));
    audio.bg.emplace("hallway", Mix_LoadMUS("./assets/mp3/hallway.mp3"));
    audio.bg.emplace("elevator", Mix_LoadMUS("./assets/mp3/elevator.mp3"));
    audio.bg.emplace("boss", Mix_LoadMUS("./assets/mp3/boss.mp3"));
    audio.bg.emplace("old_auto", Mix_LoadMUS("./assets/mp3/old_auto.mp3"));
    audio.bg.emplace("credits", Mix_LoadMUS("./assets/mp3/credits.mp3"));
    audio.bg.emplace("parking",
                     Mix_LoadMUS("./assets/mp3/parking.mp3")); //https://freesound.org/people/kyles/sounds/454254/


    audio.effects.emplace("select", Mix_LoadWAV("./assets/wav/select2.wav"));
    audio.effects.emplace("select3", Mix_LoadWAV("./assets/wav/select3.wav"));
    audio.effects.emplace("select4", Mix_LoadWAV("./assets/wav/select4.wav"));
    audio.effects.emplace("ding", Mix_LoadWAV("./assets/wav/ding.wav"));
    audio.effects.emplace("unlock", Mix_LoadWAV("./assets/wav/unlock.wav"));
    audio.effects.emplace("car", Mix_LoadWAV("./assets/wav/car.wav"));
    audio.effects.emplace("elevator", Mix_LoadWAV("./assets/wav/elevator.wav"));

    Audio::GetInstance().Play("title");

    isRunning = true;
}

void Game::LoadSaves() {

    savegames.clear();
    for (int i = 0; i < 3; ++i) {
        nlohmann::json save;
        try {
            save = SaveService::GetInstance().getSaveGame(i);
            std::string dt = save["datetime"];
            //Logger::Log(dt);
        } catch (nlohmann::json::exception &exc) {

        }
        savegames.emplace_back(save);
    }

}

void Game::LoadScripts() {
    std::vector<std::string> scripts{
            "inventory",
            "game",
            "SceneDeath",
            "SceneTutorial",
            "SceneGameEntrance",
            "SceneLobby",
            "SceneElevator",
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
            "SceneBoss",
            "SceneEnd"
    };

    if (IS_RELEASE) {
        //On release, concat all lua files to 1 file, use online base64 encode, then
        //decode single file here and read.
        auto filename = "./assets/scripts/concatenation.txt";
        std::ifstream ifs(filename);
        std::string content((std::istreambuf_iterator<char>(ifs)),
                            (std::istreambuf_iterator<char>()));

        MD5 h{content};
        auto hashed = h.toStr();

        if (hashed != "f8c9cb07fee8d1cbb29bbd07d01da6a7") {
            exit(666);
        }

        content = base64_decode(content);
        lua.script(content);
    } else {
        for (const auto &script: scripts) {
            auto loaded_script = lua.load_file("./assets/scripts/" + script + ".lua");

            if (!loaded_script.valid()) {
                sol::error err = script;
                Logger::Err(err.what());
                exit(LUA_INVALID_SCRIPT);
            }


            lua.script_file("./assets/scripts/" + script + ".lua");
        }
    }
}

//Setup game specific states and vars
void Game::Setup() {

    Game::LoadScripts();
    Game::LoadSaves();

//    lua.set_function("enableMagic", [&]() {
//        gui->magicInterface = true;
//    });

    lua.set_function("Start", [&](const std::string &a) {
        assetManager->Start(a);
    });
    lua.set_function("StartEnd", [&]() {
        gameState = STATE_END;
        currentScene = "";
    });
    lua.set_function("queueStart", [&](const std::string &a, sol::function cb) {
        events.emplace_back(StartPlayEvent{a, std::move(cb)});
    });
    lua.set_function("PlayWav", [&](const std::string &a) {
        Audio::GetInstance().PlayWav(a);
    });
    lua.set_function("PlayMp3", [&](const std::string &a) {
        Audio::GetInstance().PlayMp3(a);
    });
    lua.set_function("addDialog", [&](std::string a, sol::function cb) {
        eventBus->EmitEvent<StartDialogEvent>(std::move(a), std::move(cb));
    });
    lua.set_function("addCursiveDialog", [&](std::string a, sol::function cb) {
        eventBus->EmitEvent<StartDialogEvent>(std::move(a), std::move(cb), true);
    });
    lua.set_function("queueCursiveDialog", [this](std::string a, sol::function cb) {
        events.emplace_back(StartDialogEvent{std::move(a), std::move(cb), true});
    });
    lua.set_function("queueDialog", [this](std::string a, sol::function cb) {
        events.emplace_back(StartDialogEvent{std::move(a), std::move(cb)});
    });
    lua.set_function("addDialogs", [&](std::vector<std::string> a) {
        eventBus->EmitEvent<StartDialogEvent>(std::move(a));
    });

    //apply target for a duration of milliseconds
    lua.set_function("addMove", [&](std::string sprite, int duration, int x, int y, sol::function cb) {
        eventBus->EmitEvent<StartAniMoveEvent>(std::move(sprite), duration, glm::vec2(x, y), std::move(cb));
    });

    lua.set_function("addWaitCounter", [&](uint32_t wait, sol::function cb) {
        eventBus->EmitEvent<StartWaitEvent>(wait, std::move(cb));
    });

    lua.set_function("moveToDestination", [&](const std::string &destination, bool mainOnly = true) {
        //TODO lock GUI here? not the responsibility of the fader, right?
        //Logger::Log("moveToDestination: " + destination);

        eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEOUT, destination, mainOnly);
    });
    lua.set_function("queueDestination", [this](std::string destination, bool mainOnly = true) {
        events.emplace_back(StartTransitionEvent{TRANSITION_FADEOUT, std::move(destination), mainOnly});
    });

}

void Game::SwitchFullscreen() {
    isFullscreen = !isFullscreen;

    if (isFullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent)) {
        if (IS_DEBUG) {
            if (sdlEvent.window.windowID == SDL_GetWindowID(debugWindow)) {
                ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
                return;
            }
        }

        SDL_GetMouseState(&mouseX, &mouseY);

        auto c = sdlEvent.key.keysym;

        switch (sdlEvent.type) {
            case SDL_MOUSEBUTTONDOWN:
                mouseDown = true;
                SDL_GetMouseState(&startX, &startY);

                switch (gameState) {
                    case STATE_TITLE:
                    case STATE_LOAD:
                        onPressEnter();
                    case STATE_RUN:
                        if (GUI::guiState == GUI_LOCKED) {
                            return;
                        }

                        eventBus->EmitEvent<MousePressedEvent>(sdlEvent.button, startX, startY);
                        break;
                    case STATE_PAUSED:
                        //Skip all mouse input on paused
                        return;
                    case STATE_POPUP:
                        eventBus->EmitEvent<PopupPressedEvent>(sdlEvent.button, startX, startY);
                        break;
                }

                break;
            case SDL_MOUSEBUTTONUP:
                mouseDown = false;
                break;
            case SDL_QUIT:
                gameState = STATE_POPUP;
                popupService->ShowConfirmation("Quit game?", "quit");
                //isRunning = false;
                break;
            case SDL_KEYDOWN:

                //dont accept keypress during transitions
                if (GUI::guiState == GUI_LOCKED) {
                    return;
                }
                if (gameState == STATE_POPUP) {
                    return;
                }

                //Dont handle events for a debug , paused, exit
                if (c.sym != SDLK_g && c.sym != SDLK_ESCAPE && c.sym != SDLK_p) {
                    Logger::Log("Emitting KeyPressed");
                    eventBus->EmitEvent<KeyPressedEvent>(c);
                }

                if (c.sym == SDLK_ESCAPE) {
                    //instant exit
                    //ignore in cutscene
                    if (currentScene != "SceneElevator") {
                        if (gameState == STATE_LOAD || gameState == STATE_TITLE) {
                            isRunning = false;
                            break;
                        }
                        gameState = STATE_POPUP;
                        popupService->ShowConfirmation("Quit game?", "quit");
                    }
                } else if (c.sym == SDLK_f) {
                    SwitchFullscreen();
                } else if (c.sym == SDLK_p) {
                    if (gameState == STATE_RUN) {
                        gameState = STATE_PAUSED;
                    } else {
                        gameState = STATE_RUN;
                    }
                } else if (c.sym == SDLK_g) {
                    //isDebug = !isDebug;
                    //if (IS_RELEASE) {
                    //isDebug = false;
                    //}
                } else if (c.sym == SDLK_RETURN || c.sym == SDLK_KP_ENTER) {
                    onPressEnter();
                }
                break;
        }

    }

}

void Game::Update() {
    // If we are too fast, waste some time until we reach the MILLISECS_PER_FRAME
    uint32_t timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
        SDL_Delay(timeToWait);
    }

    //difference in ticks since the last frame, converted to seconds/
    //if difference is '10', ten seconds have passed since
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
    millisecsPreviousFrame = SDL_GetTicks();


    if (gameState == STATE_TITLE) {
        c.x += deltaTime * 15;
    }

    if (gameState != STATE_PAUSED && gameState != STATE_POPUP) {
        textService->Update(deltaTime);
        assetManager->Update(deltaTime);
        popupService->Update();
        animationRenderer->Update(deltaTime);

        //TODO add onWaitDone event
        if (timerTarget != 0) {
            timer += deltaTime;
            if (timer * 1000 >= timerTarget) {
                GUI::guiState = GUIState::GUI_IDLE;
                timerTarget = 0;
                timerCallback();
            }
        }
        // lua["game"]["update"](lua["game"], deltaTime);
    } else {
        textService->Update(0);
        animationRenderer->Update(0);
        assetManager->Update(0);
    }


    if (gameState == STATE_RUN && GUI::guiState == GUI_IDLE) {
        //Logger::Log("State idle, checking event queue");
        if (!events.empty()) {
            auto &e = events.front();
            if (holds_alternative<StartDialogEvent>(e)) {
                auto zoom = get<StartDialogEvent>(e);
                GUI::guiState = GUI_DIALOG;
                Logger::Log("Emit queued dialog");
                eventBus->EmitEvent<StartDialogEvent>(zoom);
            } else if (holds_alternative<StartTransitionEvent>(e)) {
                auto zoom = get<StartTransitionEvent>(e);
                GUI::guiState = GUI_LOCKED;
                Logger::Log("Emit queued transition");
                eventBus->EmitEvent<StartTransitionEvent>(zoom);
            } else if (holds_alternative<StartPlayEvent>(e)) {
                auto zoom = get<StartPlayEvent>(e);
                GUI::guiState = GUI_LOCKED;
                Logger::Log("Emit queued play");
                eventBus->EmitEvent<StartPlayEvent>(zoom);
            }
            events.pop_front();
        }

    }


    ProcessInput();

}

void Game::RenderGrid() const {
    int x_mask = convertedSurface->w - 1;
    int y_mask = convertedSurface->h - 1;
    float factor = 1;
    // the distance and horizontal scale of the line we are drawing
    float fFoVHalf = M_PI / c.mode7Params.fFoVDivider;
    SDL_Surface *sur = SDL_CreateRGBSurface(0, WINDOW_WIDTH, WINDOW_HEIGHT - 546, 32, 0, 0, 0, 0);
    SDL_Surface *newSurface = SDL_ConvertSurfaceFormat(sur, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(sur);

    for (int y = 0; y < c.mode7Params.winHeight; y++) {

        float angle = c.angle;
        float distance = c.height * c.mode7Params.sampleHeight / (y + c.mode7Params.horizon);

        //Center startX, startY
        float fStartX = (c.x + c.w / 2) + (cosf(-angle + fFoVHalf) * distance);
        float fStartY = (c.y + c.h / 2) - (sinf(-angle + fFoVHalf) * distance);

        //Logger::Log(std::to_string(fStartX) + ", " + std::to_string(fStartY));
        float fEndX = c.x + c.w / 2 + (cosf(-angle - fFoVHalf) * distance);
        float fEndY = c.y + c.h / 2 - (sinf(-angle - fFoVHalf) * distance);

        //Logger::Log(std::to_string(fEndX) + ", " + std::to_string(fEndY));
        //SDL_RenderDrawLine(renderer, fStartX*factor, fStartY*factor, fEndX*factor, fEndY*factor);

        // go through all points in this screen line
        for (int x = 0; x < newSurface->w; x++) {
            float fSampleWidth = (float) x / c.mode7Params.sampleWidth;
            float fSampleX = fStartX + ((fEndX - fStartX) * fSampleWidth);
            float fSampleY = fStartY + ((fEndY - fStartY) * fSampleWidth);

            auto pixel = get_pixel(convertedSurface,
                                   (int) fSampleX & x_mask,
                                   (int) fSampleY & y_mask);

            put_pixel(newSurface, (int) x, (int) y, pixel);
        }
    }

    auto tmpTexture = SDL_CreateTextureFromSurface(renderer, newSurface);

    SDL_Rect rectRight{0, c.mode7Params.winYPos, WINDOW_WIDTH, c.mode7Params.winHeight};
    //Rotation only useful from top-down
    SDL_RenderCopyEx(renderer, tmpTexture, nullptr, &rectRight, 0, nullptr, SDL_FLIP_NONE);

    SDL_FreeSurface(newSurface);
    SDL_DestroyTexture(tmpTexture);

}

void Game::Render() {
    if (IS_DEBUG) {
        SDL_SetRenderDrawColor(debugRenderer, 21, 21, 21, 255);
        SDL_RenderClear(debugRenderer);
    }
    bool isCutscene{};
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    if (gameState == GameState::STATE_TITLE) {
        assetManager->CopySpriteToRenderer("title");
        textService->DrawMyText("CLICK TO START", windowWidth / 2, windowHeight / 2 + (20 * PIXEL_SIZE),
                                1.0f, TEXT_CENTER | TEXT_BLINK);
        //text->DrawMyText("CLICK TO START", 100, 100, 0.7f);

    } else if (gameState == GameState::STATE_LOAD ||
               (gameState == STATE_POPUP && popupService->_id.starts_with("del"))) {
        assetManager->CopySpriteToRenderer("title2");

        for (int i = 0; i < 3; ++i) {

            int yPos = windowHeight / 2 - (80 * PIXEL_SIZE) + (i * 50 * PIXEL_SIZE);
            int subYPos = yPos + 20 * PIXEL_SIZE;
            int xPos = windowWidth / 2;
            textService->DrawMyText("File " + std::to_string(i + 1),
                                    xPos,
                                    yPos,
                                    1.0f, TEXT_CENTER);

            nlohmann::json save = savegames[i];
            if (!save.empty()) {
                std::string dt = save["datetime"];
                textService->DrawMyText(dt, windowWidth / 2, subYPos, 1.0f, TEXT_CENTER);

                SDL_Rect trash{xPos + 50 * PIXEL_SIZE, yPos - 10 * PIXEL_SIZE, 18 * PIXEL_SIZE, 18 * PIXEL_SIZE};
                assetManager->CopySpriteToRenderer("trash", trash);

                gui->loadElements.emplace("del" + std::to_string(i), trash);

            } else {
                if (gui->loadElements.contains("del" + std::to_string(i))) {
                    gui->loadElements.erase("del" + std::to_string(i));
                }
                textService->DrawMyText("- New -", windowWidth / 2, subYPos, 1.0f, TEXT_CENTER);
            }

        }
        //} else if (gameState == STATE_END) {

        //SDL_RenderCopy();


    } else {
        if (!currentScene.empty()) {

            sol::table s = lua[currentScene];
            sol::table actionItems = s["actionItems"];

            //Render parallax sprite
            //Logger::Log("start render");
            sol::optional<sol::table> sceneLayersLua = s["layers"];
            sol::optional<std::string> sceneBGLua = s["bgSprite"];
            sol::optional<std::string> sceneCutscene = s["bgCutscene"];

            if (sceneLayersLua) {
                sol::table sceneLayers = sceneLayersLua.value();
                for (const auto &l: sceneLayers) {
                    std::string content = l.second.as<std::string>();
                    assetManager->RenderSprite(content);
                }

            } else if (sceneCutscene) {
                isCutscene = true;
                SDL_Rect d{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
                assetManager->CopyAnimatedSpriteToRenderer(sceneCutscene.value(), d);

            } else if (sceneBGLua) {
                //copy main window
                assetManager->CopySpriteToWindow(sceneBGLua.value());
            }

            std::list<ActionItem> tmp;

            for (auto &item: actionItems) {
                sol::table row = item.second;
                ActionItem ai = ActionItem{item};
                if (ai.isVisible && !ai.sprite.empty()) {
                    tmp.emplace_back(ai);
                }
            }
            tmp.sort();
            tmp.reverse();

            //window sprites
            for (const auto &ai: tmp) {
                if (!ai.isAnimated) {
                    assetManager->CopySpriteToRenderer(ai.sprite, ai.rect);
                } else {
                    assetManager->CopyAnimatedSpriteToRenderer(ai.sprite, ai.rect, ai.currentFrame);
                }
            }

            if (!sceneCutscene) {
                //Main gui with dialog shown
                assetManager->CopySpriteToRenderer("main");
                gui->Render();
            }
        }
    }

    textService->RenderDialog();
    animationRenderer->Render();
    if (gameState == STATE_POPUP) {
        popupService->Render(textService);
    }


    if (isDebug) {
        if (!currentScene.empty()) {
            gui->debugRect();
        }
    }


    if (IS_DEBUG) {
        //debug
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav |
                                       ImGuiWindowFlags_AlwaysAutoResize;
        if (ImGui::Begin("Debug", nullptr, windowFlags)) {

            if (ImGui::CollapsingHeader("mode7 title")) {
                ImGui::InputInt("Camera w", &c.w);
                ImGui::InputInt("Camera h", &c.h);
                ImGui::DragFloat("Camera x", &c.x);
                ImGui::DragFloat("Camera y", &c.y);
                ImGui::InputInt("Camera height", &c.height);
                ImGui::Separator();
                ImGui::DragInt("winHeight", &c.mode7Params.winHeight);
                ImGui::DragInt("winYPos", &c.mode7Params.winYPos);
                ImGui::DragFloat("Horizon", &c.mode7Params.horizon);
                ImGui::DragFloat("sampleWidth", &c.mode7Params.sampleWidth);
                ImGui::DragFloat("sampleHeight", &c.mode7Params.sampleHeight);
                ImGui::DragFloat("fFoVDivider", &c.mode7Params.fFoVDivider);
                ImGui::DragFloat("angle", &c.angle);
            }

            assetManager->debugMode7();

            ImGui::Separator();
            if (!currentScene.empty()) {
                sol::table luaScene = lua[currentScene];
                Scene scene(luaScene);
                sol::table actionItems = luaScene["actionItems"];

                ImGui::Text("music %s", scene.bgMusic.c_str());
                ImGui::Text("sprite %s", scene.bgSprite.c_str());
                ImGui::Checkbox("visited ", &scene.isVisited);
                ImGui::Separator();

                ImGui::Text("Active item %s", gui->getActiveInventory().c_str());

                for (auto &item: actionItems) {

                    ActionItem ai(item);

                    if (ImGui::CollapsingHeader(item.first.as<std::string>().c_str())) {
                        sol::table sub = item.second;
                        ImGui::Checkbox("Active", &ai.isActive);
                        ImGui::Checkbox("Visible", &ai.isVisible);
                        ImGui::Checkbox("Locked", &ai.isLocked);
                        ImGui::Separator();
                        ImGui::InputInt("x", &ai.rect.x);
                        ImGui::InputInt("y", &ai.rect.y);
                        ImGui::Text("defaultAction %s", ai.defaultAction.c_str());
                        ImGui::Text("sprite %s", ai.sprite.c_str());
                        //ImGui::TextService("sprite %s", item.second.rect);
                        ImGui::Text("destination %s", ai.destination.c_str());
                        ImGui::Separator();
                    }
                }
            }
            auto t = "gameState: " + std::to_string(static_cast<int>(gameState));
            ImGui::Text(t.c_str());
            auto t3 = "guiState: " + std::to_string(static_cast<int>(GUI::guiState));
            ImGui::Text(t3.c_str());

        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());

    }

    if (false && !IS_RELEASE && mouseDown && (mouseX - startX) != 0) {
        //Logger::Log("mouse down");
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect mouseRect{startX, startY, (mouseX - startX), (mouseY - startY)};
        SDL_RenderDrawRect(renderer, &mouseRect);
        textService->DrawMyText(
                "X: " + std::to_string(startX) +
                ", Y: " + std::to_string(startY) +
                ", W: " + std::to_string((mouseX - startX)) +
                ", H: " + std::to_string((mouseY - startY)),
                10, 10, 1.0f, TEXT_BG_BLACK, White, true);
        //Debug to logger
        Logger::Log("{" +
                    std::to_string(startX / PIXEL_SIZE - 25) + "," +
                    std::to_string(startY / PIXEL_SIZE - 25) + "," +
                    std::to_string((mouseX - startX) / PIXEL_SIZE) + "," +
                    std::to_string((mouseY - startY) / PIXEL_SIZE)
                    + "}");
    }


    if (currentScene == "SceneTutorial") {
        assetManager->RenderTutorial();
    }

    //TODO switch to type 2 on clickable
    if (!isCutscene || gameState == STATE_POPUP) {
        gui->RenderCursor(mouseX, mouseY);
    }


    if (gameState == STATE_PAUSED) {
        //popupService->RenderPause();
    }

    if (gameState == STATE_TITLE) {
        RenderGrid();
    }

    SDL_RenderPresent(renderer);
    if (IS_DEBUG) {
        SDL_RenderPresent(debugRenderer);
    }
}

void Game::Run() {
    Setup();

    while (isRunning) {
        Update();
        Render();
    }
}

void Game::Destroy() {
    if (IS_DEBUG) {
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyWindow(debugWindow);
        SDL_DestroyRenderer(debugRenderer);
    }


    Audio::GetInstance().Free();

    Logger::Log("Game destroy");
    SDL_FreeSurface(convertedSurface);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    Logger::Log("Game destroy done");


}
//HANDLERS


void Game::onPressEnter() {
    if (gameState == GameState::STATE_TITLE) {
        GUI::guiState = GUI_LOCKED;
        eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEOUT, "", false);

        //animationRenderer->Fade("out", SDL_Rect{0,0, windowWidth, windowHeight}, FADE_OUT_LENGTH);
    } else if (gameState == GameState::STATE_LOAD) {


        Game::saveSlot = 4; //bogus data for checking

        SDL_Point p{mouseX, mouseY};
        if (SDL_PointInRect(&p, &gui->loadElements["save1"])) {
            Game::saveSlot = 0;
        } else if (SDL_PointInRect(&p, &gui->loadElements["save2"])) {
            Game::saveSlot = 1;
        } else if (SDL_PointInRect(&p, &gui->loadElements["save3"])) {
            Game::saveSlot = 2;
        }

        if (SDL_PointInRect(&p, &gui->loadElements["del0"])) {
            gameState = STATE_POPUP;
            popupService->ShowConfirmation("Delete save 1?", "del0");
            return;
        }
        if (SDL_PointInRect(&p, &gui->loadElements["del1"])) {
            gameState = STATE_POPUP;
            popupService->ShowConfirmation("Delete save 2?", "del1");
            return;
        }
        if (SDL_PointInRect(&p, &gui->loadElements["del2"])) {
            gameState = STATE_POPUP;
            popupService->ShowConfirmation("Delete save 3?", "del2");
            return;
        }


        if (Game::saveSlot != 4) {
            GUI::guiState = GUI_LOCKED;


            if (!savegames[Game::saveSlot].empty()) {
                Audio::GetInstance().FadeOut(250);
                auto slot = savegames[Game::saveSlot];

                SaveService::GetInstance().LoadGame(slot, lua);
                eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEOUT, slot["currentScene"], false);
            } else {
                //Game::LoadScripts(); reset save?
                eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEOUT, "SceneTutorial", false);
            }
        }
    }
}


//AssetManager en scenes pas NA transitie switchen
void Game::OnTransitionDone(TransitionDoneEvent &event) {

    Logger::Log("Transition done");

    if (gameState == STATE_TITLE) {
        gameState = STATE_LOAD;

        //TODO helper functie die alvast gui locked!
        eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEIN, "", false);
    } else if (gameState == STATE_LOAD) {
        //TODO this is probably gone after load/new game mechanic?
        gameState = GameState::STATE_RUN;
    }

    if (gameState == STATE_RUN) {
        bool fadeMain = false;

        //when coming back from animation
        if (GUI::guiState == GUI_LOCKED) {
            GUI::guiState = GUI_IDLE;
        }

        if (!event.nextScene.empty()) {

            GUI::guiState = GUI_LOCKED;

            sol::optional<sol::table> testScene = lua[event.nextScene];
            if (testScene == sol::nullopt) {
                Logger::Err("scene not found: " + event.nextScene);
                exit(SCENE_NOT_FOUND);
            }

            nextScene = event.nextScene;

            //Logger::Log("set transition to true from OnTransitionDone");

            auto &audio = Audio::GetInstance();

            std::string currentMusic{};
            std::string nextMusic = lua[nextScene]["bgMusic"];

            if (!currentScene.empty()) {
                currentMusic = lua[currentScene]["bgMusic"];
                fadeMain = true;
                if (nextScene == "SceneElevator" || currentScene == "SceneElevator") {
                    fadeMain = false;
                }
                if (nextScene == "SceneEnd" || currentScene == "SceneEnd") {
                    fadeMain = false;
                }
                sol::table s = lua[currentScene];
                //Check for animated sprites we need to remove
                sol::table actionItems = s["actionItems"];
                for (auto &actionItem: actionItems) {
                    ActionItem ai(actionItem);
                    if (ai.isAnimated) {
                        assetManager->Stop(ai.sprite);
                    }
                }

                lua["onExit"](s);
            }

            Logger::Log("currentMusic: " + currentMusic);
            Logger::Log("nextMusic: " + nextMusic);
            if (currentMusic != nextMusic && nextScene != "SceneTutorial") {
                audio.Play(nextMusic);
                audio.FadeIn(FADE_IN_LENGTH);
            }

            currentScene = nextScene;
            nextScene = "";

            //sol::table pScene = lua[currentScene];
            //TextService::GetInstance().SetLua(pScene);

            assetManager->currentSprites.clear();

            //Loop scene assets and check for animated sprites we need to start
            sol::table actionItems = lua[currentScene]["actionItems"];
            for (auto &actionItem: actionItems) {
                ActionItem ai(actionItem);

                if (ai.isAnimated) {
                    Logger::Log("Loading " + ai.sprite);
                    assetManager->currentSprites.emplace_back(ai.sprite);
                }

                if (ai.isAnimated && ai.isAutoStart) {
                    Logger::Log("Starting " + ai.sprite);
                    assetManager->Start(ai.sprite);
                }
            }

//            if (currentScene == "SceneElevator") {
//                GUI::guiState = GUI_IDLE;
//                gui->actionState = ACTION_NEUTRAL;
//                lua.set("holdingItem", nullptr);
//            } else {

            Logger::Log("Fade main: " + std::to_string(fadeMain));

            eventBus->EmitEvent<StartTransitionEvent>(TRANSITION_FADEIN, "", fadeMain);
            //}
        }
    }
}

//Call onEnter after fadein, check for scene specific override
void Game::OnFadeinDone(FadeinDoneEvent &event) {
    GUI::guiState = GUI_IDLE;
    gui->actionState = ACTION_NEUTRAL;
    lua.set("holdingItem", nullptr);

    if (!currentScene.empty()) {
        Logger::Log("Fadein done, checking onEnter");
        sol::table s = lua[currentScene];
        sol::optional<sol::function> tmp = lua[currentScene]["onEnter"];
        if (tmp) {
            lua[currentScene]["onEnter"](s);
        } else {
            lua["onEnter"](s);
        }

        //assetManager->Start("cross_fadeout");
    }
}


void Game::OnStartIdle(StartIdleEvent &event) {
    Logger::Log("Setting to idle");
    GUI::guiState = GUI_IDLE;

    //assetManager->Stop("cross_fadeout");
}

void Game::OnStartWait(StartWaitEvent(&event)) {
    GUI::guiState = GUI_LOCKED;
    timerTarget = event.wait;
    timer = 0;
    timerCallback = event.callback;
}

void Game::OnSave(StartSaveGameEvent(&event)) {
    SaveService::GetInstance().SaveGame(Game::saveSlot, currentScene, lua);
    eventBus->EmitEvent<SaveDoneEvent>();
}

void Game::OnQuit(StartQuitEvent(&event)) {
    isRunning = false;
}

void Game::OnToMenu(StartMenuEvent(&event)) {
    Game::LoadScripts();
    assetManager->ResetAnimation();
    Game::LoadSaves();
    Audio::GetInstance().PlayMp3("title");
    gameState = STATE_LOAD;
    currentScene = "";

}

void Game::HandleSaveDoneEvent(SaveDoneEvent &e) {
    popupService->ShowPopup("Game Saved", "gamesaved");
}

void Game::HandleDeleteEvent(StartDeleteEvent &e) {
    //get savegame from delete event
    SaveService::GetInstance().DeleteGame(e.save);
    Game::LoadSaves();
    gameState = STATE_LOAD;
}
