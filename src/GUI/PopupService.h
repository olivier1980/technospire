#pragma once
#include <SDL.h>
#include <glm/vec2.hpp>
#include <string>
#include <utility>
#include "ScreenElement.h"
#include "../Headers.h"
#include "../Text/TextService.h"
#include "Button.h"
#include "Popup.h"

//TODO inject AssetManager everywhere to use the font in button, popup etc?
class PopupService {
public:

    std::unique_ptr<Popup> instance;
    std::string _id;

    explicit PopupService(SDL_Renderer *renderer):
        renderer(renderer) {
    }

    void Clear() {
        _id = "";
    }


    void ShowPopup(const std::string& message, std::string id) {
        this->_id = std::move(id);
        instance = std::make_unique<Popup>(message);

    }

    void ShowCredits() {
        this->_id = "credits";
        instance = std::make_unique<CreditsPopup>("");
    }


    //std::optional
    //sol::optional
    void ShowConfirmation(const std::string& message, std::string id) {
    //void ShowConfirmation(std::string message, std::string id, OkEvent) {
    //void ShowConfirmation(std::string message, std::string id, OkEvent, SecondButtonEvent) {
        this->_id = std::move(id);
        instance = std::make_unique<ConfirmationPopup>(message);

    }

    void Update()
    {

    }

    void Render(std::shared_ptr<TextService> textService)
    {
        if (!_id.empty()) {
            instance->Render(renderer, textService);
        }
    }

    std::optional<std::string> checkClick(int x, int y) {
        if (!_id.empty()) {
            return instance->checkClick(x,y);
        }
        return {};
    }



    void SetRenderer(SDL_Renderer *r) {
        this->renderer = r;
    }


private:
    //std::shared_ptr<TextService> textService;
    SDL_Renderer *renderer;

};
