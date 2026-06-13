#ifndef AUDIO_H
#define AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class AudioManager {
private:
    Mix_Music* musicaFundo;
    Mix_Chunk* somLaser;
    Mix_Chunk* somSurge;

public:
    AudioManager();
    ~AudioManager();

    void InicializarAudio();
    void TocarLaser();
    void TocarSurge();
    void LimparAudio();
};

#endif // AUDIO_H