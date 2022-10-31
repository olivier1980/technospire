#pragma once
#include <SDL.h>
#include <string>
#include <utility>
#include "../EventBus/Event.h"
#include <sol/sol.hpp>
#include <glm/vec2.hpp>

enum TransitionType {
    TRANSITION_FADEIN,
    TRANSITION_FADEOUT
};

class DialogDoneEvent: public Event
{
public:
    explicit DialogDoneEvent() = default;
};


class KeyPressedEvent: public Event
{
public:
    SDL_Keysym symbol;
//    void Emit(EventBus& eventBus) override
//    {
//        eventBus.EmitEvent(this);
//    }
    explicit KeyPressedEvent(SDL_Keysym symbol): symbol(symbol) {}
};

class MousePressedEvent: public Event
{
public:
    int x;
    int y;
    SDL_MouseButtonEvent sdlEvent;

    explicit MousePressedEvent(SDL_MouseButtonEvent &m, int x, int y)
    : sdlEvent(m), x(x), y(y){

    }
};

class PopupPressedEvent : public MousePressedEvent {
public:
    PopupPressedEvent(SDL_MouseButtonEvent &m, int x, int y) : MousePressedEvent(m, x, y) {}
};

class StartTransitionEvent: public Event {
public:
    TransitionType type;
    std::string nextScene;
    bool mainWindowOnly{};

    explicit StartTransitionEvent(TransitionType type, bool mainWindowOnly = true)
    : type(type), mainWindowOnly(mainWindowOnly) {}

    StartTransitionEvent(TransitionType type, std::string nextScene, bool mainWindowOnly = true)
    : nextScene(std::move(nextScene)), type(type), mainWindowOnly(mainWindowOnly) {
        int a = 1;
    }
};

class StartDialogEvent: public Event {
public:
    std::vector<std::string> output;
    sol::function callback;
    bool isCursive = false;

    explicit StartDialogEvent(std::string output) {
        this->output = std::vector<std::string>{std::move(output)};
    }
    explicit StartDialogEvent(std::string output, sol::function callback, bool isCursive = false) {
        this->output = std::vector<std::string>{std::move(output)};
        this->callback = std::move(callback);
        this->isCursive = isCursive;
    }

    explicit StartDialogEvent(std::vector<std::string> output) : output(std::move(output)) {}
};

class StartPlayEvent: public Event {
public:

    //wat slaan we op bij een transformation?
    //animatie later, we gaan deuren doen!
    std::string sprite;
    sol::function callback;

    explicit StartPlayEvent(std::string sprite,  sol::function callback) :
    sprite(std::move(sprite)) {
        this->callback = std::move(callback);
    }
};

class StartAnimationEvent: public Event {
public:

    //wat slaan we op bij een transformation?
    //animatie later, we gaan deuren doen!
    std::string sprite;
    float duration;

    StartAnimationEvent(std::string sprite, float duration) : sprite(std::move(sprite)), duration(duration) {}
};

class StartIdleEvent: public Event {

};

class ButtonPressedEvent: public Event {
public:
    std::string label;

    explicit ButtonPressedEvent(std::string label) : label(std::move(label)) {}
};

class StartAniMoveEvent: public StartAnimationEvent {
public:
    glm::vec2 target{};
    sol::function callback;

    StartAniMoveEvent(std::string sprite, float duration, glm::vec2 target, sol::function callback) :
            StartAnimationEvent(std::move(sprite), duration), target(target), callback(std::move(callback)) {};
    //explicit StartAnimationEvent(std::vector<std::string> output) : output(std::move(output)) {}
};

class StartWaitEvent: public Event {
public:
    uint32_t wait;
    sol::function callback;

    explicit StartWaitEvent(uint32_t wait, sol::function callback) : wait(wait), callback(std::move(callback)) {}
};

class StoreInventoryEvent: public Event {
public:
    std::string itemName;

    explicit StoreInventoryEvent(std::string itemName) : itemName(std::move(itemName)) {}

    //StoreInventoryEvent() = default;

};

class TransitionDoneEvent: public Event
{
public:
    std::string nextScene;
    //TODO make const?
    explicit TransitionDoneEvent(const std::string &nextScene){
        this->nextScene = nextScene;
    }
    explicit TransitionDoneEvent() = default;
};

class FadeinDoneEvent: public TransitionDoneEvent
{
public:
    std::string nextScene;
    //TODO make const?

    explicit FadeinDoneEvent() = default;
};

class StartSaveGameEvent: public Event {

};

class SaveDoneEvent : public Event {

};

class StartQuitEvent : public Event {

};
class StartMenuEvent : public Event {

};
class StartDeleteEvent : public Event {
public:
    int save;
    explicit StartDeleteEvent(int save) : save(save) {}
};