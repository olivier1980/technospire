#include "Popup.h"
#include <SDL.h>
#include "../Text/TextService.h"

std::optional<std::string> Popup::checkClick(int x, int y) {
    for (auto &b : buttons) {
        if (b.isInRect(x,y)) {
            //b.handleClick()
            return b.label;
        }
    }

    //AssetBus->EmitEvent(OkEvent)
    return {};
}

void Popup::Render(SDL_Renderer *renderer,
                   const std::shared_ptr<TextService> &textService) {
    if (!this->_message.empty()) {
        //TODO show graphic?
        auto bg = getBgRect();
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderFillRect(renderer, &bg);

        textService->DrawMyText(_message,
                                              (int) position.x + (width / 2),
                                              (int) position.y - (20*PIXEL_SIZE) + (height / 2),
                                              1.0f, TEXT_CENTER
        );

        //Use font to render text
        for (auto &b : buttons) {
            b.Render(renderer, textService);
        }
    }
}


void CreditsPopup::Render(SDL_Renderer *renderer,
                          const std::shared_ptr<TextService> &textService) {

//TODO show graphic?
    auto bg = getBgRect();
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &bg);

//        std::vector<std::string> credits{
//                "Line 1",
//                " Line 2",
//                " ",
//                " Line 3",
//                " ",
//                "Test"
//        };
//
//        int i = 2;
//        for (const auto &line : credits ) {
//
//            textService->DrawMyText(line,
//                                                  (int) position.x + (width / 2),
//                                                  (int) position.y + 20 * i,
//                                                  1.0f, TEXT_CENTER
//            );
//            ++i;
//        }


    textService->DrawMyText("Art and programming",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 20*PIXEL_SIZE,
                                          1.2f, TEXT_CENTER, Header
    );
    textService->DrawMyText("Olivier Droog",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 50*PIXEL_SIZE,
                                          1.0f, TEXT_CENTER
    );
    textService->DrawMyText("https://twitter.com/crazy_descent",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 70*PIXEL_SIZE,
                                          0.8f, TEXT_CENTER|TEXT_UNDERLINE, Link
    );




    textService->DrawMyText("Narrative Design",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 140*PIXEL_SIZE,
                                          1.2f, TEXT_CENTER, Header
    );
    textService->DrawMyText("Dominic Tramontana",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 170*PIXEL_SIZE,
                                          1.0f, TEXT_CENTER
    );
    textService->DrawMyText("https://dominictramontanaportfolio.wordpress.com/",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 190*PIXEL_SIZE,
                                          0.8f, TEXT_CENTER|TEXT_UNDERLINE, Link
    );


    textService->DrawMyText("Soundtrack",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 260*PIXEL_SIZE,
                                          1.2f, TEXT_CENTER, Header
    );
    textService->DrawMyText("Howtocommittaxfraud101 (7withoutthe11)",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 290*PIXEL_SIZE,
                                          1.0f, TEXT_CENTER
    );
    textService->DrawMyText("https://soundcloud.com/7withoutthe11",
                                          (int) position.x + (width / 2),
                                          (int) position.y + 310*PIXEL_SIZE,
                                          0.8f, TEXT_CENTER|TEXT_UNDERLINE , Link
    );


//Use font to render text
    for (auto &b: buttons) {
        b.Render(renderer, textService);
    }

}
