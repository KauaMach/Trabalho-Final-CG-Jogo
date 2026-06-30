#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <string>

class AudioManager {
private:
    Mix_Music* bgm;
    Mix_Chunk* clickSound;
    Mix_Chunk* laserBlueSound;
    Mix_Chunk* laserRedSound;
    Mix_Chunk* surgeBlueSound;
public:
    AudioManager();
    ~AudioManager();
    
    bool Init();
    void PlayMusic(const std::string& path, int loops = -1);
    void StopMusic();
    void PlayClickSound();
    void PlayLaserBlue(int loops = 0);
    void PlayLaserRed(int loops = 0);
    void PlaySurgeBlue(int durationMs);
    void PlaySurgeRed(int durationMs);
    void SetMusicVolume(float volume); // 0.0 a 1.0
    void SetSFXVolume(float volume);   // 0.0 a 1.0
};
