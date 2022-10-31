#pragma once
#include <variant>
#include <deque>
#include <string>

#define IS_RELEASE false
#define IS_DEBUG true

#define PIXEL_SIZE 2

#define WINDOW_WIDTH (512 * PIXEL_SIZE)
#define WINDOW_HEIGHT (464 * PIXEL_SIZE)

#define FADE_IN_LENGTH 500
#define FADE_OUT_LENGTH 250

#define TEXT_START_X (30 * PIXEL_SIZE)
#define TEXT_START_Y (290 * PIXEL_SIZE)
#define TEXT_WIDTH (340 * PIXEL_SIZE)

//main viewport
#define MAIN_RECT {25 *PIXEL_SIZE, 25 *PIXEL_SIZE, 224*PIXEL_SIZE, 224*PIXEL_SIZE }
#define GUI_RECT {15*PIXEL_SIZE , 278*PIXEL_SIZE , 370*PIXEL_SIZE, 177*PIXEL_SIZE }


enum ERROR_CODE {
    SCENE_NOT_FOUND = 100,
    SDL_NO_TEXT = 101,
    SDL_NO_VIDEO = 102,
    LUA_INVALID_ASSET = 103,
    LUA_INVALID_SCRIPT = 104,
    UNKNOWN_ASSET = 105,
    MISSING_LUA_ONACTION = 106,
};



const std::string MUSIC_PATH {"./assetManager/mp3/"};

class StartDialogEvent;
class StartTransitionEvent;
class StartPlayEvent;


typedef std::deque<std::variant<
        StartDialogEvent,StartTransitionEvent,StartPlayEvent>>
        EventVariant;
