#pragma once

#include <SDL.h>
#include <SDL_mixer.h>
#include "../Logger/Logger.h"
#include <string>
#include <unordered_map>

class Audio {
public:
    std::unordered_map<std::string, Mix_Chunk*> effects;
    std::unordered_map<std::string, Mix_Music *> bg;

    Audio(const Audio&) = delete;
    Audio(Audio&&) = delete;
    Audio& operator=(const Audio&) = delete;
    Audio& operator=(Audio&&) = delete;

    static Audio &GetInstance() {
        static Audio INSTANCE;
        return INSTANCE;
    }

    void Init();
    void FadeIn(int s);
    void FadeOut(int s);
    void Play(const std::string& name);
    void PlayMp3(const std::string& name);
    void PlayWav(const std::string &name, int channel = -1);
    void Pause();
    void Free();

private:

    Audio()= default;
    bool paused{};
    int result = 0;
    int flags = MIX_INIT_MP3;
};

