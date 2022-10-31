#pragma once

#include <SDL.h>
#include <functional>
#include <utility>
#include "../Logger/Logger.h"
#include <queue>
#include <memory>
#include <variant>
#include <math.h>
#include "../Headers.h"

extern float DTR;// = M_PI/180.0f;
extern float RTD;// = 180.0f/M_PI;

struct Mode7Param {
    float fFoVDivider{3.6f};
    float sampleHeight{1000.0f};
    float sampleWidth{978.0f};
    float horizon = 38.0f; //5

    int winHeight = WINDOW_HEIGHT-546; //animate this
    int winYPos = 546;
};

class Camera {
public:
    Camera() = default;
    Mode7Param mode7Params;
    float x{};
    float y{};
    int w{};
    int h{-16};

    float angle{0*DTR};
    int height = 7;

    float dt;

    void Update();
    void rotate(float degrees);
};

