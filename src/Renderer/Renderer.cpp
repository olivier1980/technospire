#include "Renderer.h"
#include "../GUI/GUI.h"
#include "../Game/Game.h"
#include <SDL.h>

void Renderer::Update(double deltaTime) {
    this->dt = deltaTime;
};

void Renderer::Render() {
    if (fadein || fadeout) {
        if (fadein) {
            fadeValue -= (secondeSteps * dt);

            if (fadeValue < 0) {
                fadeValue = 0;
                fadein = false;
                fadeActive = false;
                eventBus->EmitEvent<FadeinDoneEvent>();
            }
        } else if (fadeout) {
            fadeValue += secondeSteps * dt;

            if (fadeValue > 255) {
                fadeValue = 255;
                fadeout = false;
                fadeActive = false;
                eventBus->EmitEvent<TransitionDoneEvent>(nextScene);
            }
        }

        //Logger::Log(std::to_string(fadeValue) + " 0 " + std::to_string((int) fadeValue));
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, (int) fadeValue);
        SDL_RenderFillRect(renderer, &screen);
    } else if (runAnimation) {
        //TODO split to animator
        switch (animationType) {
            case ANIMATE_MOVE:
                auto x = lua[testContainer.scene]["actionItems"][testContainer.spriteName]["rect"][1];
                auto y = lua[testContainer.scene]["actionItems"][testContainer.spriteName]["rect"][2];
                glm::vec2 tmp {x,y};
                tmp += (testContainer.velocity * float(dt));
                x.set(tmp.x);
                y.set(tmp.y);

                testContainer.timer += testContainer.velocity* (float)dt;

                if ((testContainer.timer.x != 0 && abs(testContainer.timer.x) >= abs(testContainer.target.x)) ||
                        (testContainer.timer.y != 0 && abs(testContainer.timer.y) >= abs(testContainer.target.y))
                ) {
                    Logger::Log("Finished at y " + std::to_string((float)y));
                    runAnimation = false;
                    eventBus->EmitEvent<StartIdleEvent>();
                    testContainer.callback();
                    testContainer = AnimationContainer{};
                }
//
//                //Apply target during the duration
//                auto x = lua[testContainer.scene]["actionItems"][testContainer.spriteName]["rect"][1];
//                auto y = lua[testContainer.scene]["actionItems"][testContainer.spriteName]["rect"][2];
//
//                x.set((double)x + dt * testContainer.target.x);
//                y.set((double)y + dt * testContainer.target.y);
//
//                testContainer.timer += dt;
//
//                if (testContainer.timer*1000 > testContainer.duration) {
//                    Logger::Log("Finished at y " + std::to_string((int)testContainer.pos.y));
//                    runAnimation = false;
//                    eventBus->EmitEvent<StartIdleEvent>();
//                    testContainer.callback();
//                }

                break;
        }



    }
};

void Renderer::AnimateMove(StartAniMoveEvent &event) {
    GUI::guiState = GUI_LOCKED;
    runAnimation = true;
    animationType = ANIMATE_MOVE;

    testContainer.scene = Game::currentScene;
    testContainer.velocity.x = (event.target.x / event.duration) * 1000.0f; // change per seconde
    testContainer.velocity.y = (event.target.y / event.duration) * 1000.0f; // change per seconde
    testContainer.spriteName = event.sprite;
    testContainer.callback = event.callback;
    testContainer.target = event.target;
}

//void Fade(const std::string &effect, SDL_Rect rect, int duration = 250) {
void Renderer::Fade(StartTransitionEvent &event) {

//    if (event.nextScene == "SceneElevator") {
//        eventBus->EmitEvent<TransitionDoneEvent>(event.nextScene);
//        return;
//    }

    int duration;
    if (!fadeActive) {
        GUI::guiState = GUI_LOCKED;
        fadeActive = true;
    } else {
        return;
    }

    if (event.type == TRANSITION_FADEIN) {
        fadein = true;
        fadeValue = 255.0f;
        duration = FADE_IN_LENGTH;
    } else {
        duration = FADE_OUT_LENGTH;
        fadeout = true;
        fadeValue = 0;
    }

    //dt = aantal seconde per frame
    secondeSteps = (255.0f / duration) * 1000; // opacity change per seconde

    if (!event.nextScene.empty()) {
        //main window switch
        nextScene = event.nextScene;
    }

    if (event.mainWindowOnly) {
        Logger::Log("Fade viewport");
        screen = MAIN_RECT;
    } else {
        Logger::Log("Fade window");
        screen = SDL_Rect{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    }
}

void Renderer::SubscribeToEvents() {

    eventBus->SubscribeToEvent<StartTransitionEvent>(this, &Renderer::Fade);
    eventBus->SubscribeToEvent<StartAniMoveEvent>(this, &Renderer::AnimateMove);
}
