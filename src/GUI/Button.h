#pragma once
#include <utility>
#include <SDL.h>
#include "ScreenElement.h"
#include "../Text/TextService.h"

class Button: public ScreenElement {

public:
    std::string label;

    explicit Button(std::string label,  int width = 80*PIXEL_SIZE, int height = 40*PIXEL_SIZE)
            : label(std::move(label)) {
        this->position = glm::vec2(
                WINDOW_WIDTH/2 - width/2,
                WINDOW_HEIGHT/2 + (10*PIXEL_SIZE)
                );
        this->width = width;
        this->height = height;
    }

    explicit Button(std::string label, glm::vec2 position, int width = 80*PIXEL_SIZE, int height = 40*PIXEL_SIZE)
    : label(std::move(label)) {
        this->position = position;
        this->width = width;
        this->height = height;
    }

    //Update()
    bool isInRect(int x, int y) {
        SDL_Point p{x,y};
        SDL_Rect r {(int)position.x,
                    (int)position.y,
                    width,
                    height
        };

        //EventBus::GetInstance().EmitEvent<ButtonPressedEvent>(label);
        return SDL_PointInRect(&p, &r);
    }

    //void handleClick();

    void Render(SDL_Renderer *renderer, const std::shared_ptr<TextService>& textService) {
        SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
        SDL_Rect bg{(int) position.x,
                    (int) position.y,
                    width,
                    height};
        SDL_RenderFillRect(renderer, &bg);

        textService->DrawMyText(label,
              (int) position.x + width / 2,
              (int) position.y + height / 2,
              1.0f, TEXT_CENTER
        );

    }
};