#include "AssetManager.h"
#include "Headers.h"
#include <sol/sol.hpp>
#include "Logger/Logger.h"
#include "Util/Helper.h"

#include "Game/Game.h"

//#include <imgui/imgui.h>
//#include <imgui/imgui_impl_sdl.h>
//#include <imgui/imgui_impl_sdlrenderer.h>

void AssetManager::Load(sol::state &lua) {
    Logger::Log("init assets");
    SDL_Surface *spriteMap = IMG_Load("./assets/spritemap.png");
    //SDL_Surface *spriteMap2 = IMG_Load("./assets/animation1.png");

    texture = SDL_CreateTextureFromSurface(r, spriteMap);
    //transitionTexture = SDL_CreateTextureFromSurface(r, spriteMap2);

    SDL_FreeSurface(spriteMap);
    //SDL_FreeSurface(spriteMap2);
    IMG_Quit();

    auto script = lua.load_file("./assets/scripts/assets2.lua");

    if (!script.valid()) {
        sol::error err = script;
        Logger::Err(err.what());
        exit(LUA_INVALID_ASSET);
    }

    lua.script_file("./assets/scripts/assets2.lua");

    sol::table table = lua["assets"];

    for (auto &v: table) {
        sol::table subtable = v.second;
        std::string k = v.first.as<std::string>();
        SDL_Rect rect{
                subtable.get<int>(1),
                subtable.get<int>(2),
                subtable.get<int>(3),
                subtable.get<int>(4),
        };
        AddTexture(k, rect);
    }

    //cursor
    auto surface = IMG_Load("./assets/cursor1.png");
    textureCursor = SDL_CreateTextureFromSurface(r, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("./assets/cursor2.png");
    textureCursor2 = SDL_CreateTextureFromSurface(r, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("./assets/tutorial.png");
    textureTutorial = SDL_CreateTextureFromSurface(r, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("./assets/ending.png");
    textureEnding = SDL_CreateTextureFromSurface(r, surface);
    SDL_FreeSurface(surface);





    //load parallax

    sol::optional<sol::table> layers = lua["paraSprites"];
    if (layers) {
        sol::table layersTable = layers.value();
        for (const auto& l: layersTable) {
            std::string id = l.first.as<std::string>();
            sol::table content = l.second;
            //std::string layerSprite = content["sprite"];

            ParaSprite paraSprite{};

            SDL_Rect test{
                    (int)(content["rect"][1])+25,
                    (int)(content["rect"][2])+25,
                    (int)(content["rect"][3]),
                    (int)(content["rect"][4]),
            };
            Helper::getRekt(test);
            paraSprite.rect = test;
            paraSprite.speed = content["speed"];
            paraSprite.isReverse = content["isReverse"].get_or(false);

            paraSprites.emplace(id, paraSprite);
        }
    }

    //load animated sprite info
    table = lua["animatedSprites"];
    for (const auto &tableRow : table) {
        std::string id = tableRow.first.as<std::string>();
        sol::table content = tableRow.second;

        AnimatedSprite newSprite{};
        newSprite.id = id;
        newSprite.isVertical = content["isVertical"].get_or(false);
        newSprite.frameSpeedRate = content["framespeed"].get_or(-1);
        newSprite.numFrames = content["numFrames"].get_or(-1);
        newSprite.loop = content["loop"].get_or(-1);

        sol::optional<std::string> luaPattern = content["pattern"];
        if (luaPattern != sol::nullopt) {
            std::string tmp = luaPattern.value();

            std::vector<std::string> splitResult;

            splitResult = Helper::split(tmp, ',');

            for (const auto& c : splitResult) {

                auto splitResult2 = Helper::split(c, ':');

                std::pair<uint8_t,uint16_t> p;
                p.first = atoi(splitResult2[0].c_str());
                p.second = atoi(splitResult2[1].c_str());
                newSprite.customPattern.emplace_back(p);
            }
        }

        animatedSprites.emplace(id, newSprite);
    }

    //#https://laemeur.sdf.org/fonts/
    font = TTF_OpenFont("./assets/MorePerfectDOSVGA.ttf", 22*PIXEL_SIZE );
    dialogfont = TTF_OpenFont("./assets/MorePerfectDOSVGA.ttf", 18*PIXEL_SIZE );
    fontItalic = TTF_OpenFont("./assets/MorePerfectDOSVGA.ttf", 18*PIXEL_SIZE );
    TTF_SetFontStyle(fontItalic, TTF_STYLE_ITALIC);


//    if(TTF_FontFaceIsFixedWidth(font))
//        printf("The font is fixed width.\n");
//    else
//        printf("The font is not fixed width.\n");

    debugfont = TTF_OpenFont("./assets/FreeSans.ttf", 22*PIXEL_SIZE );
    if (font == nullptr){
        Logger::Err(TTF_GetError());
        exit(-1);
    }

    //load mode7 sprite info
    table = lua["mode7Sprites"];
    for (const auto &tableRow : table) {
        std::string id = tableRow.first.as<std::string>();
        sol::table content = tableRow.second;

        Mode7Sprite newSprite{};

        std::string sprite = content["sprite"];
        std::string path = "./assets/" + sprite;
        SDL_Surface *surface = IMG_Load(path.c_str());
        newSprite.surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(surface);

        newSprite.x = content["x"];
        newSprite.height = content["height"];
        newSprite.horizon = content["horizon"];
        //newSprite.winHeight = content["winHeight"];
        newSprite.angle = content["angle"];
        newSprite.speed = content["speed"];


        mode7Sprites.emplace(id, newSprite);
    }


}

void AssetManager::Start(const std::string &key) {
    if (animatedSprites.contains(key)) {
        auto &s = animatedSprites.at(key);
        s.startTime = SDL_GetTicks();
    } else {
        Logger::Err("Cant start animated sprite " + key);
    }
}

void AssetManager::Start(const std::string &key, Event *event) {
    auto &s = animatedSprites.at(key);
    //s.event = event;
    s.startTime = SDL_GetTicks();
}
//void AssetManager::Start(const std::string &key, std::unique_ptr<Event> event) {
//    auto &s = animatedSprites.at(key);
//    s.event = std::move(event);
//    s.startTime = SDL_GetTicks();
//}

void AssetManager::ResetAnimation() {

    for (auto& spritePair: animatedSprites) {
        auto sprite = &spritePair.second;
        sprite->startTime = 0;
        sprite->currentFrame = 0;
    }

}

void AssetManager::Stop(const std::string &key) {
    auto &s = animatedSprites.at(key);
    s.startTime = 0;
}

void AssetManager::Update(double dt) {


    if (Game::currentScene == "SceneCliff" || Game::currentScene == "SceneDoors2") {
        for (auto &spritePair: paraSprites) {
            auto sprite = &spritePair.second;
            int width = sprite->rect.w;
            if (sprite->isReverse) {
                sprite->x -= dt * sprite->speed;
            } else {
                sprite->x += dt * sprite->speed;
            }
            sprite->rect.x = (int) sprite->x;
            if (abs(sprite->x) >= width) {
                sprite->x = 0;
            }
        }
    }

    if (Game::currentScene == "SceneBoss") {
        for (auto &spritePair: mode7Sprites) {
            auto sprite = &spritePair.second;
            if (sprite->isReverse) {
                sprite->y -= dt * sprite->speed;
            } else {
                sprite->y += dt * sprite->speed;
            }

            //Logger::Log("Update SpriteY: " + std::to_string(sprite->y));

        }
    }


    for (const auto& spritetmp : currentSprites) {

        auto sprite = &animatedSprites[spritetmp];

        //Logger::Log(spritetmp);

        if (sprite->startTime == 0) {
            continue;
        }
        //Logger::Log("Loop entity " + std::to_string(entity.GetId()));

        if (!sprite->customPattern.empty()) {
            //Logger::Log(sprite->id);

            int totalTime{};
            for (const auto &line : sprite->customPattern) {
                totalTime += line.second;
            }

            //Logger::Log(SDL_GetTicks64() - sprite->startTime);

            uint32_t elapsedTime = (
                    (SDL_GetTicks64() - sprite->startTime)
                    ) % totalTime;

            //Logger::Log(elapsedTime);
            int sum = 0;

            for (const auto &line : sprite->customPattern) {

                sum += line.second;
                if (elapsedTime < sum) {
                    sprite->currentFrame = line.first;

                    //Logger::Log(line.first);
                    //Logger::Log((int)line.first);
                    //Logger::Log(sprite->numFrames-1);
                    if ((int)line.first == sprite->numFrames-1) {
                        if (sprite->loop == 1) {
                            sprite->startTime = 0;
                        }
                        if (emitSprites.contains(sprite->id) && emitSprites[sprite->id]) {
                            Logger::Log("emit custom animation done");
                            eventBus->EmitEvent<StartIdleEvent>();
                            if (callback) {
                                callback();
                            }
                            emitSprites[sprite->id] = false;
                        }
                    }

                    break;
                }


            }


        } else {

            int currentFrame{};
            auto frameSpeedRate = (sprite->frameSpeedRate) / 1000.0f;
            uint32_t elapsedTime = (SDL_GetTicks64() - sprite->startTime);



            //auto r = rand() % 2;
            currentFrame = floor(elapsedTime * frameSpeedRate);
            sprite->currentFrame = currentFrame % sprite->numFrames;

            if (sprite->currentFrame == sprite->numFrames-1) {
                ++sprite->played;
            }

            if (sprite->loop == sprite->played) {

                if (emitSprites.contains(sprite->id) && emitSprites[sprite->id]) {

                   eventBus->EmitEvent<StartIdleEvent>();
                    if (callback) {
                        callback();
                    }
                    emitSprites[sprite->id] = false;
                }

                sprite->played = 0;
                sprite->startTime = 0;
            }
        }


    }
}

AnimatedSprite AssetManager::GetAnimatedSprite(const std::string &key) {

    return animatedSprites.at(key);
}

void AssetManager::Clean() {
    Logger::Log("Clean assetManager");

    //lua = nullprs;

    for (const auto& row: mode7Sprites) {
        auto rowContent = row.second;
        SDL_FreeSurface(rowContent.surface);
    }



    //Logger::Log("Destroy textures");
    SDL_DestroyTexture(textureCursor);
    SDL_DestroyTexture(textureCursor2);
    SDL_DestroyTexture(textureTutorial);
    SDL_DestroyTexture(textureEnding);
    //SDL_DestroyTexture(transitionTexture);
    SDL_DestroyTexture(texture);


    //Logger::Log("Close fonts");
    TTF_CloseFont(font);
    TTF_CloseFont(dialogfont);
    TTF_CloseFont(fontItalic);
    TTF_CloseFont(debugfont);
    TTF_Quit();
};

SDL_Rect AssetManager::GetTexture(const std::string &assetId) {

    if (assetId == "ending") {
        return SDL_Rect {0,0, 512/2, 464/2};
    }

    if (!textures.contains(assetId)) {
        Logger::Err("Unknown asset " + assetId);
        exit(UNKNOWN_ASSET);
    }
    return textures[assetId];
}

void AssetManager::AddTexture(const std::string &assetId, SDL_Rect rect) {
    textures.emplace(assetId, rect);
    //Logger::Log("New texture added with _id = " + assetId);
}

//Copies over whole window
void AssetManager::CopySpriteToRenderer(const std::string &spriteName) {
    auto t = GetTexture(spriteName);

    SDL_Rect window = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderCopy(r, texture, &t, &window);
}

//Copies to specific destination
void AssetManager::CopySpriteToRenderer(const std::string &spriteName, const SDL_Rect &target) {
    auto t = GetTexture(spriteName);
    SDL_RenderCopy(r, texture, &t, &target);
}

void AssetManager::CopyParaSpriteToRenderer(const std::string &key)
{
    auto sourceRect = GetTexture(key);
    ParaSprite sprite = paraSprites.at(key);

    SDL_Rect newRect = sprite.rect;
    if (sprite.isReverse) {
        newRect.x = sprite.rect.w + sprite.rect.x;
    } else {
        newRect.x = -sprite.rect.w + sprite.rect.x;
    }

    newRect.x += (PIXEL_SIZE*25);
    sprite.rect.x += (PIXEL_SIZE*25);

    SDL_RenderCopy(r, texture, &sourceRect, &newRect);
    SDL_RenderCopy(r, texture, &sourceRect, &sprite.rect);
}

void AssetManager::CopyAnimatedSpriteToRenderer(const std::string &key, const SDL_Rect &target, int currentFrame) {
    auto sourceRect = GetTexture(key);
    AnimatedSprite sprite = animatedSprites.at(key);

    int frameNumber = currentFrame > 0 ? currentFrame : sprite.currentFrame;

    if (sprite.isVertical) {
        sourceRect.y = sourceRect.y + (frameNumber * sourceRect.h);
        //Logger::Log(sourceRect.y);
    } else {
        sourceRect.x = sourceRect.x + (frameNumber * sourceRect.w);
    }
    //Logger::Log(sourceRect.x);
    if (key == "ending") {
        SDL_RenderCopy(r, textureEnding, &sourceRect, &target);
    } else {
        SDL_RenderCopy(r, texture, &sourceRect, &target);
    }
}

void AssetManager::RenderSprite(const std::string &spriteName) {
    if (paraSprites.contains(spriteName)) {
        CopyParaSpriteToRenderer(spriteName);

    } else if (mode7Sprites.contains(spriteName)) {
        RenderMode7(spriteName);
    } else {
        CopySpriteToWindow(spriteName);
    }
}

void AssetManager::CopyAnimatedSpriteToWindow(const std::string &key) {
    auto sourceRect = GetTexture(key);
    AnimatedSprite sprite = animatedSprites.at(key);
    if (sprite.isVertical) {
        sourceRect.y = sourceRect.y + (sprite.currentFrame * sourceRect.h);
        //Logger::Log(sourceRect.y);
    } else {
        sourceRect.x = sourceRect.x + (sprite.currentFrame * sourceRect.w);
    }
    SDL_Rect d = MAIN_RECT;
    //Logger::Log(sourceRect.x);
    SDL_RenderCopy(r, texture, &sourceRect, &d);
}


void AssetManager::RenderTutorial() {
    SDL_RenderCopy(r, textureTutorial, nullptr, nullptr);
}

//Copies over specific part of window , naming!
void AssetManager::CopySpriteToWindow(const std::string &spriteName) {
    auto t = GetTexture(spriteName);
    SDL_Rect d = MAIN_RECT;
    SDL_RenderCopy(r, texture, &t, &d);
}

//TODO refactor, these functions KNOW their assetManager
//Uses sprite dimension and places at specifc x,y
void AssetManager::CopyGUIToWindow() {
    auto t = GetTexture("main_gui");
    SDL_Rect d = GUI_RECT;
    SDL_RenderCopy(r, texture, &t, &d);
}

void AssetManager::RenderCursor(int x, int y, int type) {
    if (type == 1) {
        SDL_Rect dst{
                x, y, 12, 19
        };
        SDL_RenderCopy(r, textureCursor, nullptr, &dst);
    } else {
        SDL_Rect dst{
                x-5, y, 18, 23
        };
        SDL_RenderCopy(r, textureCursor2, nullptr, &dst);
    }
}


void AssetManager::RenderMode7(const std::string &spriteName) {

    Mode7Sprite &sprite = mode7Sprites.at(spriteName);


    int x_mask = sprite.surface->w - 1;
    int y_mask = sprite.surface->h - 1;

    // the distance and horizontal scale of the line we are drawing
    float fFoVHalf = M_PI / sprite.fFoVDivider;
    SDL_Surface *sur = SDL_CreateRGBSurface (0, WINDOW_WIDTH, WINDOW_HEIGHT, 32,0xff, 0xff00, 0xff0000, 0xff000000);
    SDL_Surface *newSurface = SDL_ConvertSurfaceFormat(sur, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(sur);

    for (int y = 0; y < sprite.winHeight; y++) {

        float angle = sprite.angle;
        float distance = sprite.height * sprite.sampleHeight / (y + sprite.horizon);

        //Center startX, startY
        float fStartX = (sprite.x + sprite.w/2) + (cosf(-angle + fFoVHalf) * distance);
        float fStartY = (sprite.y + sprite.h/2) - (sinf(-angle + fFoVHalf) * distance);

        //Logger::Log(std::to_string(fStartX) + ", " + std::to_string(fStartY));
        float fEndX = sprite.x + sprite.w/2 + (cosf(-angle - fFoVHalf) * distance);
        float fEndY = sprite.y + sprite.h/2 - (sinf(-angle - fFoVHalf) * distance);

        //Logger::Log(std::to_string(fEndX) + ", " + std::to_string(fEndY));
        //SDL_RenderDrawLine(renderer, fStartX*factor, fStartY*factor, fEndX*factor, fEndY*factor);

        // go through all points in this screen line
        for (int x = 0; x < newSurface->w; x++) {
            float fSampleWidth = (float)x / sprite.sampleWidth;
            float fSampleX = fStartX + ((fEndX - fStartX) * fSampleWidth);
            float fSampleY = fStartY + ((fEndY - fStartY) * fSampleWidth);

            auto pixel = Helper::get_pixel(sprite.surface,
                                   (int) fSampleX & x_mask,
                                   (int) fSampleY & y_mask);

            Helper::put_pixel(newSurface, (int) x, (int) y , pixel);
        }
    }

    auto tmpTexture = SDL_CreateTextureFromSurface(r, newSurface);

    SDL_Rect rectRight {0, sprite.winYPos, WINDOW_WIDTH,  sprite.winHeight};
    //Rotation only useful from top-down

    SDL_Rect test = MAIN_RECT ;
    SDL_RenderCopy(r, tmpTexture , nullptr, &test);

    SDL_FreeSurface(newSurface);
    SDL_DestroyTexture(tmpTexture);

}

void AssetManager::SetRenderer(SDL_Renderer *r) {
    this->r = r;
}

void Mode7Sprite::debug() {
//
//
//    ImGui::DragInt("Speed", &speed);
//    ImGui::DragInt("Camera w", &w);
//    ImGui::DragInt("Camera h", &h);
//    ImGui::DragFloat("Camera x", &x);
//    ImGui::DragFloat("Camera y", &y);

//
//    ImGui::DragInt("winHeight", &sprite.winHeight);
//    ImGui::DragInt("winYPos", &sprite.winYPos);
//    ImGui::DragInt("height", &sprite.height);
//    ImGui::DragFloat("Horizon", &sprite.horizon);
//    ImGui::DragFloat("sampleWidth", &sprite.sampleWidth);
//    ImGui::DragFloat("sampleHeight", &sprite.sampleHeight);
//    ImGui::DragFloat("fFoVDivider", &sprite.fFoVDivider);
//    ImGui::DragFloat("angle", &sprite.angle);

}

void AssetManager::debugMode7()
{
//    int i = 0;
//
//
//    if (ImGui::TreeNode("Mode7 layers")) {
//
//        for (auto &mode7: mode7Sprites) {
//            auto id = mode7.first;
//            auto &sprite = mode7.second;
//
//
//            if (ImGui::TreeNode((void*)(intptr_t)i, id.c_str(), i)) {
//                //mode7.second.debug();
//
//                ImGui::DragInt("Speed", &sprite.speed);
//                ImGui::DragInt("Camera w", &sprite.w);
//                ImGui::DragInt("Camera h", &sprite.h);
//                ImGui::DragFloat("Camera x", &sprite.x);
//                ImGui::DragFloat("Camera y", &sprite.y);
//
//
//                ImGui::DragInt("winHeight", &sprite.winHeight);
//                ImGui::DragInt("winYPos", &sprite.winYPos);
//                ImGui::DragInt("height", &sprite.height);
//                ImGui::DragFloat("Horizon", &sprite.horizon);
//                ImGui::DragFloat("sampleWidth", &sprite.sampleWidth);
//                ImGui::DragFloat("sampleHeight", &sprite.sampleHeight);
//                ImGui::DragFloat("fFoVDivider", &sprite.fFoVDivider);
//                ImGui::DragFloat("angle", &sprite.angle);
//                ImGui::TreePop();
//            }
//           // }
//            ++i;
//
//        }
//        ImGui::TreePop();
//
//    }

//    Logger::Log(sprite.y);




}

void AssetManager::OnPlayEvent(StartPlayEvent &e)
{
    if (!emitSprites.contains(e.sprite)) {
        emitSprites[e.sprite] = true;
    }

    emitSprites[e.sprite] = true;

    if (e.callback) {
        callback = e.callback;
    }
    Start(e.sprite);
}

void AssetManager::SubscribeToEvents() {
    eventBus->SubscribeToEvent<StartPlayEvent>(this, &AssetManager::OnPlayEvent);

}