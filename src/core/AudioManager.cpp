#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() : bgm(nullptr), clickSound(nullptr), laserBlueSound(nullptr), laserRedSound(nullptr), surgeBlueSound(nullptr) {}

AudioManager::~AudioManager() {
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    if (bgm) {
        Mix_FreeMusic(bgm);
    }
    if (clickSound) {
        Mix_FreeChunk(clickSound);
    }
    if (laserBlueSound) {
        Mix_FreeChunk(laserBlueSound);
    }
    if (laserRedSound) {
        Mix_FreeChunk(laserRedSound);
    }
    if (surgeBlueSound) {
        Mix_FreeChunk(surgeBlueSound);
    }
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool AudioManager::Init() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Falha ao inicializar SDL Audio! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Falha ao inicializar SDL_mixer! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }
    
    clickSound = Mix_LoadWAV("assets/audio/click-botoes.wav");
    if (clickSound == nullptr) {
        std::cerr << "Aviso: Falha ao carregar efeito de clique! Mix_Error: " << Mix_GetError() << std::endl;
    }
    
    laserBlueSound = Mix_LoadWAV("assets/audio/laser-azul.wav");
    if (laserBlueSound == nullptr) {
        std::cerr << "Aviso: Falha ao carregar laser azul! Mix_Error: " << Mix_GetError() << std::endl;
    }
    
    laserRedSound = Mix_LoadWAV("assets/audio/laser-vermelho.wav");
    if (laserRedSound == nullptr) {
        std::cerr << "Aviso: Falha ao carregar laser vermelho! Mix_Error: " << Mix_GetError() << std::endl;
    }
    
    surgeBlueSound = Mix_LoadWAV("assets/audio/surge-azul.wav");
    if (surgeBlueSound == nullptr) {
        std::cerr << "Aviso: Falha ao carregar surge azul! Mix_Error: " << Mix_GetError() << std::endl;
    }
    
    return true;
}

void AudioManager::PlayMusic(const std::string& path, int loops) {
    if (bgm) {
        Mix_FreeMusic(bgm);
    }
    bgm = Mix_LoadMUS(path.c_str());
    if (bgm == nullptr) {
        std::cerr << "Falha ao carregar musica " << path << "! Mix_Error: " << Mix_GetError() << std::endl;
    } else {
        Mix_PlayMusic(bgm, loops);
    }
}

void AudioManager::StopMusic() {
    Mix_HaltMusic();
}

void AudioManager::PlayClickSound() {
    if (clickSound) {
        Mix_PlayChannel(-1, clickSound, 0);
    }
}

void AudioManager::PlayLaserBlue(int loops) {
    if (laserBlueSound) {
        Mix_PlayChannel(-1, laserBlueSound, loops); // -1 aloca no primeiro canal livre
    }
}

void AudioManager::PlayLaserRed(int loops) {
    if (laserRedSound) {
        Mix_PlayChannel(-1, laserRedSound, loops);
    }
}

void AudioManager::PlaySurgeBlue(int durationMs) {
    if (surgeBlueSound) {
        // Toca em loop infinito (-1) ate ser cortado automaticamente apos durationMs
        Mix_PlayChannelTimed(-1, surgeBlueSound, -1, durationMs);
    }
}

void AudioManager::PlaySurgeRed(int durationMs) {
    if (laserRedSound) {
        // Usa o laser vermelho repetido sem parar, mas corta exato aos durationMs
        Mix_PlayChannelTimed(-1, laserRedSound, -1, durationMs);
    }
}

void AudioManager::SetMusicVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    Mix_VolumeMusic(static_cast<int>(volume * 128));
}

void AudioManager::SetSFXVolume(float volume) {
    if (volume < 0.0f) volume = 0.0f;
    if (volume > 1.0f) volume = 1.0f;
    Mix_Volume(-1, static_cast<int>(volume * 128));
}
