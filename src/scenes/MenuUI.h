#pragma once
#include <vector>
#include <string>
#include <functional>
#include "core/Renderer.h"

enum GameState {
    STATE_MENU,
    STATE_CREDITOS,
    STATE_CONTROLES,
    STATE_CONFIG,
    STATE_SELECAO_FASE,
    STATE_PLAYING,
    STATE_PAUSE,
    STATE_VICTORY,
    STATE_GAMEOVER
};

struct MenuButton {
    float xMin, xMax, yMin, yMax;
    std::string label;
    std::function<void()> onClick;
};

class MenuUI {
private:
    GLuint bgTexture;
    GLuint creditosTexture;
    GLuint controlesTexture;
    GLuint configTexture;
    GLuint fasesTexture;
    GLuint cadeadoTexture;
    GLuint pauseTexture;
    GLuint vitoriaTexture;
    GLuint derrotaTexture;
    std::vector<MenuButton> buttons;
    
    struct Slider {
        float xMin, xMax, yMin, yMax;
        float value; // 0.0 a 1.0
    };
    Slider musicSlider;
    Slider sfxSlider;
    MenuButton btnVoltar;
    
    int draggingSlider = 0; // 0=nenhum, 1=musica, 2=sfx

public:
    void Init();
    void HandleClick(int mouseX, int realMouseY, GameState& currentState);
    void HandleHover(int mouseX, int realMouseY);
    void HandleDrag(int mouseX, int realMouseY, GameState& currentState);
    void StopDrag();
    void Render(GameState currentState);

    int hoverX = -1;
    int hoverY = -1;
};
