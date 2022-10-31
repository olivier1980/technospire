#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <map>
#include "Events/Events.h"
#include "Logger/Logger.h"
#include "EventBus/EventBus.h"
#include <sol/sol.hpp>
#include <SDL_ttf.h>
#include <glm/vec2.hpp>

struct AnimatedSprite {

    //AnimatedSprite();
   // AnimatedSprite(AnimatedSprite const &sprite);

    std::string id{};

    int numFrames{};
    std::vector<std::pair<uint8_t,uint16_t>> customPattern{};
    float frameSpeedRate{};

    //loop logic, loop is -1 for infinite
    int loop = -1;
    int played = 0;

    uint32_t startTime{};
    bool isVertical{};
    bool autoStart{};
    int currentFrame{};

};

struct ParaSprite {
    glm::vec2 position{};
    int width{};
    int height{};
    SDL_Rect rect{};
    float x{};
    int speed{};
    bool isReverse{};
};

struct Mode7Sprite {
    float x{-104.0f};
    float y{};
    int w{};
    int h{};

    int speed{290};
    bool isReverse{};

    float angle{4};
    float fFoVDivider{11.6f};
    float sampleHeight{1383.0f};
    float sampleWidth{239.0f};
    float horizon = -184.0f; //5

    int winHeight = 329; //animate this
    int winYPos{1};

    int height{9};

    SDL_Surface *surface;

    void debug();
};


//make singleton
//usualy problem is init and destruct
class AssetManager {
//public:
//    AssetManager(const AssetManager&) = delete;
//    AssetManager(AssetManager&&) = delete;
//    AssetManager& operator=(const AssetManager&) = delete;
//    AssetManager& operator=(AssetManager&&) = delete;

public:
    TTF_Font* font;
    TTF_Font* dialogfont;
    TTF_Font* fontItalic;
    TTF_Font* debugfont;

//    static AssetManager &GetInstance() {
//        static AssetManager INSTANCE;
//        return INSTANCE;
//    }

    AssetManager(SDL_Renderer *r, std::shared_ptr<EventBus> eventBus)
        : r(r), eventBus(std::move(eventBus)) {};

    ~AssetManager()  {
        Logger::Log("Destruct assetManager");
        Clean();
    };

//public:
    //explicit AssetManager(SDL_Renderer *r, std::shared_ptr<EventBus> eventBus);
    //~AssetManager();

    SDL_Rect GetTexture(const std::string &assetId);

    void AddTexture(const std::string& assetId, SDL_Rect rect);
    void Load(sol::state &lua);
    void Update(double dt);
    void Start(const std::string &key);
    void Start(const std::string &key, Event *event);
    //void Start(const std::string &key, std::unique_ptr<Event> event);

    void Stop(const std::string &key);
    AnimatedSprite GetAnimatedSprite(const std::string &key);
    void CopySpriteToRenderer(const std::string &spriteName);
    void RenderSprite(const std::string &spriteName);
    void RenderMode7(const std::string &spriteName);
    void RenderTutorial();
    void CopySpriteToRenderer(const std::string &spriteName, const SDL_Rect &target);
    void CopyAnimatedSpriteToRenderer(const std::string &key, const SDL_Rect &target, int currentFrame = 0);
    void CopyParaSpriteToRenderer(const std::string &key);
    void CopyAnimatedSpriteToWindow(const std::string &key);
    void CopySpriteToWindow(const std::string &spriteName);
    void CopyGUIToWindow();
    void RenderCursor(int x, int y, int type = 1);
    void Clean();
    void SubscribeToEvents();
    void OnPlayEvent(StartPlayEvent &e);
    void SetRenderer(SDL_Renderer *r);

    void ResetAnimation();

    void debugMode7();
    std::unordered_map<std::string, AnimatedSprite> animatedSprites;
    std::unordered_map<std::string, bool> emitSprites;

    std::vector<std::string> currentSprites;

//    ~AssetManager() {
//        Clean();
//    };
private:
    std::unordered_map<std::string, ParaSprite> paraSprites;
    std::unordered_map<std::string, Mode7Sprite> mode7Sprites;
    std::map<std::string, SDL_Rect> textures;
    SDL_Renderer *r;
    SDL_Texture *texture;
    SDL_Texture *textureTutorial;
    SDL_Texture *textureEnding;
    SDL_Texture *textureCursor;
    SDL_Texture *textureCursor2;
    bool emitEvent{};
    sol::function callback;
    std::shared_ptr<EventBus> eventBus;


};
