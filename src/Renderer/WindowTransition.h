#pragma once

#include <SDL.h>
#include <functional>
#include <memory>
#include "../Logger/Logger.h"
#include "../EventBus/EventBus.h"
#include "../Events/Events.h"
#include "../Headers.h"
#include <glm/glm.hpp>


class WindowTransition {
public:


    void Update(double deltaTime);
    void Fade(StartTransitionEvent &event);

    void SubscribeToEvents();

private:
    std::shared_ptr<EventBus> eventBus;

    bool runAnimation = false;
    bool fadeActive = false;

    std::string nextScene{};

    double dt; //milliseconde verstreken per frame

};
