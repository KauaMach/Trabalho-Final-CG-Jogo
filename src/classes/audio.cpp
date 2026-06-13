#include "../bibliotecas/audio.h"
#include <iostream>

AudioManager::AudioManager() : musicaFundo(nullptr), somLaser(nullptr), somSurge(nullptr) {}

AudioManager::~AudioManager() {
    LimparAudio();
}

void AudioManager::InicializarAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Falha ao inicializar subsistema de Áudio SDL!" << std::endl;
        return;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Erro de configuração no SDL_mixer!" << std::endl;
        return;
    }

    musicaFundo = Mix_LoadMUS("assets/audio/background_music.ogg");
    somLaser    = Mix_LoadWAV("assets/audio/laser_shoot.wav");
    somSurge    = Mix_LoadWAV("assets/audio/surge_activation.wav");
    
    if (!musicaFundo || !somLaser || !somSurge) {
        std::cerr << "Aviso: Um ou mais arquivos de áudio não foram encontrados em assets/audio/" << std::endl;
    }

    Mix_PlayMusic(musicaFundo, -1); // Toca a música de fundo em loop infinito
}

void AudioManager::TocarLaser() {
    if (somLaser) Mix_PlayChannel(-1, somLaser, 0);
}

void AudioManager::TocarSurge() {
    if (somSurge) Mix_PlayChannel(-1, somSurge, 0);
}

void AudioManager::LimparAudio() {
    if (somLaser) { Mix_FreeChunk(somLaser); somLaser = nullptr; }
    if (somSurge) { Mix_FreeChunk(somSurge); somSurge = nullptr; }
    if (musicaFundo) { Mix_FreeMusic(musicaFundo); musicaFundo = nullptr; }
    Mix_CloseAudio();
    SDL_Quit();
}