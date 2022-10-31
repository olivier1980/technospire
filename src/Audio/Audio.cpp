#include "Audio.h"

void Audio::Init() {

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        Logger::Err("Failed to init SDL");
        exit(1);
    }

    if (flags != (result = Mix_Init(flags))) {
        Logger::Err("Could not initialize mixer, result: " + std::to_string(result));
        // Logger::Err("Mix_Init: " + std::to_string(Mix_GetError()));
        exit(1);
    }

    //Initialize SDL_mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
        exit(-1);
    }

    //128
    Mix_VolumeMusic(40);
    Mix_Volume(-1, 100);

//        while (!SDL_QuitRequested()) {
//            SDL_Delay(250);
//        }
}

void Audio::FadeIn(int s)
{
    //Mix_FadeInMusic(music, 0, s);
}

void Audio::FadeOut(int s)
{
    Mix_FadeOutMusic(s);
}

void Audio::Play(const std::string& name) {
    //Logger::Log("Audio: play music "+ name);
    Mix_PlayMusic(bg[name], -1);
}


void Audio::PlayMp3(const std::string& name)
{
    FadeOut(250);
    //Logger::Log("Audio: play music "+ name);
    Mix_PlayMusic(bg[name], -1);
}

void Audio::PlayWav(const std::string &name, int channel)
{
    //Logger::Log(Mix_Playing(1));

    if (channel != -1 and Mix_Playing(channel) != 0) {
        return;
    }
    //Logger::Log("Audio: play sfx "+ name);
    Mix_PlayChannel( channel, effects[name], 0 );
}

void Audio::Pause()
{
    paused = !paused;
    if (paused) {
        Mix_PauseMusic();
    } else {
        Mix_ResumeMusic();
    }
}

void Audio::Free()
{
    for (const auto& sfx : bg) {
        Mix_FreeMusic(sfx.second);
    }
    for (const auto& sfx : effects) {
        Mix_FreeChunk(sfx.second);
    }

    Mix_Quit();
}