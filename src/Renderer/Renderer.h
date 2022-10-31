#pragma once

#include <SDL.h>
#include <functional>
#include <memory>
#include "../Logger/Logger.h"
#include "../EventBus/EventBus.h"
#include "../Events/Events.h"
#include "../Headers.h"
#include <glm/glm.hpp>
#include <utility>
//#include "Game/Game.h"
//256x232

//How to make this part of a list of effects? which I can assign to a variable, or maybe function pointer

enum AnimateType {
    ANIMATE_MOVE,
    AnimateRotate,
    AnimateScale
};

struct AnimationContainer {
    std::string scene{};
    std::string spriteName{};
    glm::vec2 timer{};
    glm::vec2 target;
    //glm::highp_vec2 pos;
    sol::function callback;

    //transform properties
    glm::vec2 velocity{};
    double angle{};
    glm::vec2 scale {1.0f, 1.0f};

};


class Renderer {
public:
    explicit Renderer(SDL_Renderer *renderer, sol::state &lua, std::shared_ptr<EventBus> eventBus)
    : renderer(renderer), lua(lua), eventBus(std::move(eventBus)) {

    }

    void Update(double deltaTime);

    void Render();

    //void Fade(const std::string &effect, SDL_Rect rect, int duration = 250) {
    void Fade(StartTransitionEvent &event);
    void AnimateMove(StartAniMoveEvent &event);

    void SubscribeToEvents();

private:

    sol::state &lua;

    AnimateType animationType;

    float fadeValue {};
    float secondeSteps{};



    bool runAnimation = false;
    sol::table luaCoords;
    AnimationContainer testContainer;

    bool fadeActive = false;
    bool fadeout = false;
    bool fadein = false;
    std::string nextScene{};
    SDL_Rect screen;
    double dt; //milliseconde verstreken per frame
    std::shared_ptr<EventBus> eventBus;
    SDL_Renderer *renderer;
};
