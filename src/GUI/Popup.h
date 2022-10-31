#pragma once
#include "../Headers.h"
#include <glm/vec2.hpp>
#include <utility>
#include <SDL.h>
#include "Button.h"
#include <vector>
#include <optional>
#include "../Util/Helper.h"

struct Popup {
    std::string _message;
    int width = WINDOW_WIDTH/1.5f;
    int height = WINDOW_HEIGHT/3;
    glm::vec2 position{
            WINDOW_WIDTH/2-this->width/2,
            WINDOW_HEIGHT/3
    };
    std::vector<Button> buttons;

    //gui not checking id/results
    //when popup detects button clicked, gui listens to event from popup
    //boolean shouldclose/return value
    //todo check return optional
    std::optional<std::string> checkClick(int x, int y) ;
    virtual void Render(SDL_Renderer *renderer,
                        const std::shared_ptr<TextService> &textService);

    SDL_Rect getBgRect() {

        auto r = SDL_Rect {
                (int) position.x,
                (int) position.y,
                width,
                height
        };

        return r;
    }

    explicit Popup(std::string message) : _message(std::move(message)) {
        buttons.emplace_back(Button{"OK"});
    }
};

struct ConfirmationPopup : Popup {

    explicit ConfirmationPopup(const std::string &message) : Popup(message) {
        buttons.clear();
        buttons.emplace_back(Button{"YES", glm::vec2(
                position.x + (40*PIXEL_SIZE),
                position.y + height - (60*PIXEL_SIZE)
        )});
        buttons.emplace_back(Button{"NO", glm::vec2(
                position.x + (220*PIXEL_SIZE),
                position.y + height - (60*PIXEL_SIZE)
        )});
    }
};

struct CreditsPopup : Popup {

    explicit CreditsPopup(std::string message) : Popup(std::move(message)) {
        width = WINDOW_WIDTH-10;
        height = WINDOW_HEIGHT-100;

        position = {5,50};
        buttons.clear();
        buttons.emplace_back(Button{"OK",  glm::vec2(
                WINDOW_WIDTH/2-(40*PIXEL_SIZE),
                WINDOW_HEIGHT - (100*PIXEL_SIZE)
                )});
    }

    void Render(SDL_Renderer *renderer,
                const std::shared_ptr<TextService> &textService) override;
};