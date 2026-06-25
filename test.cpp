#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int main() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) return 1;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return 1;
    Mix_Chunk* clickSound = Mix_LoadWAV("assets/audio/click-botoes.wav");
    if (!clickSound) return 1;
    Mix_PlayChannel(-1, clickSound, 0);
    SDL_Delay(500);
    Mix_FreeChunk(clickSound);
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    std::cout << "OK" << std::endl;
    return 0;
}
