#include "TextService.h"
#include "../Util/Helper.h"
#include "../GUI/GUI.h"
#include "../Audio/Audio.h"
#define BLINK_SPEED 2

void TextService::SetRenderer(SDL_Renderer *renderer) {
    this->r = renderer;
}

//void TextService::SetLua(sol::table lua) {
//    //this->lua = lua;
//}

void TextService::Update(double dt) {
    this->dt = dt;
    timer += dt;
}

//TODO split to DrawTextCenter? DrawBlinkingText?
void TextService::DrawMyText(const std::string& message, int x, int y,
                             float scale, int flags,
                             const SDL_Color &color,
                             bool debug
) {

    if ((flags & TEXT_BLINK) == TEXT_BLINK) {
        if ((int) (timer * BLINK_SPEED) % 2 == 0) {
            return;
        }
    }

    SDL_Surface *textSurface;

    if ((flags & TEXT_UNDERLINE) == TEXT_UNDERLINE) {
        TTF_SetFontStyle(assetManager->font, TTF_STYLE_UNDERLINE);
    } else {
        TTF_SetFontStyle(assetManager->font, TTF_STYLE_NORMAL);
    }

    if (debug) {
        textSurface = TTF_RenderText_Solid(assetManager->debugfont, message.c_str(), color);
    } else {
        textSurface = TTF_RenderText_Solid(assetManager->font, message.c_str(), color);
    }
    int text_width = textSurface->w;
    int text_height = textSurface->h;

    SDL_Rect textLocation = {
            (int) (x),
            (int) (y),
            (int) (text_width * scale),
            (int) (text_height * scale)
    };

    if ((flags & TEXT_CENTER) == TEXT_CENTER) {
        textLocation = {
                (int) (x - (text_width * scale) / 2),
                (int) (y - (text_height * scale) / 2),
                (int) (text_width * scale),
                (int) (text_height * scale)
        };
    }

    //textLocation.x *= 2;
    //textLocation.y *= 2;
    //Helper::RectToScreen(textLocation);

    SDL_Texture *guiTexture = SDL_CreateTextureFromSurface(r, textSurface);
    SDL_FreeSurface(textSurface);
    if ((flags & TEXT_BG_BLACK) == TEXT_BG_BLACK) {
        SDL_SetRenderDrawColor(r, 0, 0, 0, 200);
        SDL_RenderFillRect(r, &textLocation);
    }
    SDL_SetRenderDrawColor(r, 255, 0, 0, 255);//TODO make arg
    SDL_RenderCopy(r, guiTexture, nullptr, &textLocation);
    SDL_DestroyTexture(guiTexture);
}

std::vector<std::string> TextService::BuildLines(const std::string &txt) {
    int pos = 0;
    std::string line, lineNext;
    std::vector<std::string> returnLines;

    while (pos < txt.length()) {
        int prePos = pos;
        lineNext = line;

        while (!std::isspace(txt[pos]) && pos < txt.length()) {
            lineNext += txt[pos];
            pos++;
        }

        if (pos < txt.length()) {
            lineNext += txt[pos];
            pos++;
        }

        int w, h;
        if (TTF_SizeUTF8(assetManager->dialogfont, lineNext.c_str(), &w, &h) == 0) {
            if (w > TEXT_WIDTH) {
                //spilled over
                returnLines.push_back(line);
                lineNext = "";
                pos = prePos;
            }
        } else {
            exit(-1);
            returnLines.push_back(lineNext);
        }

        line = lineNext;
    }
    returnLines.push_back(lineNext);
    return returnLines;
}

void TextService::RenderLines(DialogLine &dl) {
    if (dl.lines.empty()) {
        return;
    }

    int x = TEXT_START_X;
    int y = TEXT_START_Y;
    int linesLen = 0;
    std::string str{};
    std::string loopString{};
    auto addition = (dt * renderSpeed);
    //Logger::Log(addition);

    //Dialog loop is stored with dialog line, and steadily incremented
    dl.dialogLoop += addition;

    //Logger::Log(dl.dialogLoop);

    int lineY = 20*PIXEL_SIZE;

    //Counter for chars
    int i = 0;

    //Counter for lines
    int j = 0;

    for (auto &line: dl.lines) {

        str = "";
        //linesLen = total length so far
        int textPos = dl.dialogLoop - linesLen;

        if (textPos >= 0) {
            if (textPos < line.length()) {
                //grab each character while time is incrementing
                str += line.substr(0, textPos+1);

                if (std::isspace(line[textPos]) == 0) {
                    if (str.length() > dl.lastLength)
                        Audio::GetInstance().PlayWav("select4", 1);
                    dl.lastLength = (int)str.length();
                }
            } else {
                //if time has advanced much, just grab whole line
                str = line;
            }
        }

        linesLen += line.length();

        if (!str.empty()) {

            auto font = dl.isCursive? assetManager->fontItalic :
                    assetManager->dialogfont;

            SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, str.c_str(), Black);

            int text_width = textSurface->w;
            int text_height = textSurface->h;

            SDL_Rect textLocation = {x, y + (lineY * j),
                                     (int) (text_width),
                                     (int) (text_height)
            };
            SDL_Texture *guiTexture = SDL_CreateTextureFromSurface(r, textSurface);
            SDL_FreeSurface(textSurface);

            SDL_RenderCopy(r, guiTexture, nullptr, &textLocation);
            SDL_DestroyTexture(guiTexture);
        }
        ++j;

    }
}

//TODO should I always render and check if active,
// or let the renderer decide what components to render?
void TextService::RenderDialog() {
    if (internalString.empty()) {
        return;
    }
    bool allRead = true;
    for (auto &dl: internalString) {
        if (!dl.isRead) {
            currentLine = &dl;
            dialogActive = true;
            RenderLines(dl);
            allRead = false;
            break;
        }
    }

    if (allRead) {
        // Logger::Log("Dialog completed, emitting event");
        Logger::Log("Emit dialog done");
        eventBus->EmitEvent<StartIdleEvent>();
        dialogActive = false;
        currentLine = nullptr;
        internalString.clear();
        if (callback) {
            callback();
        }
    }

}

void TextService::SubscribeToEvents() {

    eventBus->SubscribeToEvent<KeyPressedEvent>(this, &TextService::onKeyPressed);
    eventBus->SubscribeToEvent<MousePressedEvent>(this, &TextService::onMousePressed);
    eventBus->SubscribeToEvent<StartDialogEvent>(this, &TextService::PrepareDialog);
}

void TextService::onKeyPressed(KeyPressedEvent &event) {
    OnUserAction();
}

void TextService::onMousePressed(MousePressedEvent &event) {
    //https://wiki.libsdl.org/SDL_MouseButtonEvent?highlight=%28%5CbCategoryStruct%5Cb%29%7C%28SDLStructTemplate%29
    OnUserAction();
}

void TextService::OnUserAction() {
    if (dialogActive) {

        // Logger::Log(currentLine->dialogLoop);
        if (currentLine->dialogLoop >= currentLine->length) {
            // Logger::Log("set "+ currentLine->lines[0].substr(0, 10)+ " to read");
            currentLine->isRead = true;
        } else if (currentLine->dialogLoop < 999) {
            // Logger::Log("set "+ currentLine->lines[0].substr(0, 10)+" to 999");
            currentLine->dialogLoop = 999;
        }
    }
}

void TextService::PrepareDialog(StartDialogEvent &event) {
    GUI::guiState = GUI_DIALOG;
    dialogActive = false;
    internalString.clear();
    callback = sol::nil;
    if (event.callback) {
        callback = event.callback;
    }

    for (const auto &inputText: event.output) {
        DialogLine dialogLines;
        dialogLines.isCursive = event.isCursive;
        dialogLines.lines = BuildLines(inputText);
        dialogLines.length = (int) inputText.length();
        internalString.emplace_back(dialogLines);
    }

}
