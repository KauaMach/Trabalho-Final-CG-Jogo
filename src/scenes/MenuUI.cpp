#include "MenuUI.h"
#include "core/AudioManager.h"
#include "entities/Player.h"
#include <iostream>

extern GameState currentState;    // Definido na main.cpp
extern AudioManager audioManager; // Definido na main.cpp
extern Player player;    // Definido na main.cpp

void MenuUI::Init()
{
    bgTexture = Renderer::LoadTexture("assets/textures/bg_menu.png");
    creditosTexture = Renderer::LoadTexture("assets/textures/bg-creditos.png");
    controlesTexture = Renderer::LoadTexture("assets/textures/bg-controles.png");
    configTexture = Renderer::LoadTexture("assets/textures/bg-configuracoes.png");
    fasesTexture = Renderer::LoadTexture("assets/textures/bg-fases.png");
    cadeadoTexture = Renderer::LoadTexture("assets/textures/bg-cadeado-fase.png");
    relatorioTexture = Renderer::LoadTexture("assets/textures/bg-relatorio.png");
    pauseTexture = Renderer::LoadTexture("assets/textures/bg-pause.png");

    // Coordenadas estimadas para sliders de audio
    musicSlider = {85.0f, 770.0f, 305.0f, 333.0f, 1.0f}; // Default 100%
    sfxSlider = {85.0f, 770.0f, 195.0f, 223.0f, 1.0f};   // Default 100%

    // Reajuste baseado no feedback visual do usuario:
    // Retangulos menores (Width 250 em vez de 300)
    float startX = 55.0f;
    float endX = 305.0f;

    // Y movido para baixo para os de cima, e para cima para o 'Sair'.
    // Altura reduzida para 50px por botao, espacamento de 20px.
    buttons = {
        {startX, endX, 382.0f, 418.0f, "Iniciar Missao", []()
         {
             std::cout << "INICIANDO JOGO..." << std::endl;
             currentState = STATE_SELECAO_FASE; // Agora vai pra tela de fases
         }},
        {startX, endX, 325.0f, 359.0f, "Configuracoes", []()
         {
             std::cout << "ABRINDO CONFIGURACOES..." << std::endl;
             currentState = STATE_CONFIG;
         }},
        {startX, endX, 267.0f, 302.0f, "Controles", []()
         {
             std::cout << "ABRINDO CONTROLES..." << std::endl;
             currentState = STATE_CONTROLES;
         }},
        {startX, endX, 210.0f, 246.0f, "Creditos", []()
         {
             std::cout << "ABRINDO CREDITOS..." << std::endl;
             currentState = STATE_CREDITOS;
         }},
        {startX, endX, 153.0f, 188.0f, "Sair", []()
         {
             std::cout << "SAINDO..." << std::endl;
             exit(0);
         }}};
}

void MenuUI::HandleHover(int mouseX, int realMouseY)
{
    hoverX = mouseX;
    hoverY = realMouseY;
}

void MenuUI::HandleClick(int mouseX, int realMouseY, GameState &state)
{
    if (state == STATE_MENU)
    {
        for (const auto &btn : buttons)
        {
            if (mouseX >= btn.xMin && mouseX <= btn.xMax &&
                realMouseY >= btn.yMin && realMouseY <= btn.yMax)
            {
                audioManager.PlayClickSound(); // <- Toca o efeito sonoro!
                btn.onClick();
                return;
            }
        }
    }
    else if (state == STATE_CREDITOS || state == STATE_CONTROLES)
    {
        // Clique em qualquer lugar para voltar ao menu
        audioManager.PlayClickSound(); // <- Toca o som tbm ao voltar
        state = STATE_MENU;
    }
    else if (state == STATE_CONFIG)
    {
        // Iniciar arraste do slider de musica
        if (mouseX >= musicSlider.xMin && mouseX <= musicSlider.xMax &&
            realMouseY >= musicSlider.yMin && realMouseY <= musicSlider.yMax)
        {
            draggingSlider = 1;
            HandleDrag(mouseX, realMouseY, state);
        }
        // Iniciar arraste do slider de sfx
        else if (mouseX >= sfxSlider.xMin && mouseX <= sfxSlider.xMax &&
                 realMouseY >= sfxSlider.yMin && realMouseY <= sfxSlider.yMax)
        {
            draggingSlider = 2;
            HandleDrag(mouseX, realMouseY, state);
        }
        // Se nao clicou em nenhum slider, volta pro menu!
        else
        {
            audioManager.PlayClickSound();
            state = STATE_MENU;
        }
    }
    else if (state == STATE_SELECAO_FASE)
    {
        // Hitboxes das fases (estimativas baseadas no bg-fases.png)
        // Fase 1
        if (mouseX >= 120.0f && mouseX <= 900.0f && realMouseY >= 450.0f && realMouseY <= 560.0f)
        {
            audioManager.PlayClickSound();
            state = STATE_PLAYING; // Fase 1 ativa
            return;
        }
        // Fase 2 (Bloqueada)
        if (mouseX >= 120.0f && mouseX <= 900.0f && realMouseY >= 310.0f && realMouseY <= 460.0f)
        {
            audioManager.PlayClickSound();
            std::cout << "FASE 2 BLOQUEADA!" << std::endl;
            return;
        }
        // Fase 3 (Bloqueada)
        if (mouseX >= 120.0f && mouseX <= 900.0f && realMouseY >= 150.0f && realMouseY <= 300.0f)
        {
            audioManager.PlayClickSound();
            std::cout << "FASE 3 BLOQUEADA!" << std::endl;
            return;
        }
        // Botao Relatorio
        if (mouseX >= 730.0f && mouseX <= 950.0f && realMouseY >= 40.0f && realMouseY <= 120.0f)
        {
            audioManager.PlayClickSound();
            state = STATE_RELATORIO;
            return;
        }
        // Botao Voltar
        if (mouseX >= 30.0f && mouseX <= 200.0f && realMouseY >= 30.0f && realMouseY <= 100.0f) {
            audioManager.PlayClickSound();
            state = STATE_MENU;
        }
    }
    else if (state == STATE_PAUSE)
    {
        // Continuar Jogo (Botoes em 3 andares - estimativa no centro da lousa)
        if (mouseX >= 380.0f && mouseX <= 644.0f && realMouseY >= 460.0f && realMouseY <= 530.0f) {
            audioManager.PlayClickSound();
            state = STATE_PLAYING;
            return;
        }
        // Reiniciar Jogo (Meio)
        if (mouseX >= 380.0f && mouseX <= 644.0f && realMouseY >= 360.0f && realMouseY <= 430.0f) {
            audioManager.PlayClickSound();
            player.Init(); // Reposiciona a nave, zera a vida e o surge!
            state = STATE_PLAYING;
            return;
        }
        // Desistir / Voltar as Fases (Inferior)
        if (mouseX >= 380.0f && mouseX <= 644.0f && realMouseY >= 260.0f && realMouseY <= 330.0f) {
            audioManager.PlayClickSound();
            state = STATE_SELECAO_FASE;
            return;
        }
    }
    else if (state == STATE_RELATORIO)
    {
        // Clique em qualquer lugar volta para a selecao de fase
        audioManager.PlayClickSound();
        state = STATE_SELECAO_FASE;
    }
}

void MenuUI::StopDrag()
{
    draggingSlider = 0;
}

void MenuUI::HandleDrag(int mouseX, int realMouseY, GameState &state)
{
    if (state != STATE_CONFIG || draggingSlider == 0)
        return;

    Slider *activeSlider = (draggingSlider == 1) ? &musicSlider : &sfxSlider;

    float pos = (float)(mouseX - activeSlider->xMin) / (activeSlider->xMax - activeSlider->xMin);
    if (pos < 0.0f)
        pos = 0.0f;
    if (pos > 1.0f)
        pos = 1.0f;

    activeSlider->value = pos;

    // Atualizar no motor de audio
    if (draggingSlider == 1)
    {
        audioManager.SetMusicVolume(pos);
    }
    else if (draggingSlider == 2)
    {
        audioManager.SetSFXVolume(pos);
    }
}

void MenuUI::Render(GameState state)
{
    if (state == STATE_MENU)
    {
        Renderer::DrawTexture(bgTexture, 0, 0, 1024, 768);

        // Desenhar feedback de Hover
        for (const auto &btn : buttons)
        {
            if (hoverX >= btn.xMin && hoverX <= btn.xMax &&
                hoverY >= btn.yMin && hoverY <= btn.yMax)
            {
                // Filtro verde neon (combina com os paineis da imagem) bem suave (15% de opacidade)
                Renderer::DrawSemiTransparentRect(btn.xMin, btn.xMax, btn.yMin, btn.yMax, 0.0f, 1.0f, 0.3f, 0.5f);
            }
        }
    }
    else if (state == STATE_CREDITOS)
    {
        Renderer::DrawTexture(creditosTexture, 0, 0, 1024, 768);
    }
    else if (state == STATE_CONTROLES)
    {
        Renderer::DrawTexture(controlesTexture, 0, 0, 1024, 768);
    }
    else if (state == STATE_CONFIG)
    {
        Renderer::DrawTexture(configTexture, 0, 0, 1024, 768);

        // Desenhar a "caixinha" de música
        float mKnobX = musicSlider.xMin + (musicSlider.value * (musicSlider.xMax - musicSlider.xMin));
        Renderer::DrawSemiTransparentRect(mKnobX - 10, mKnobX + 10, musicSlider.yMin - 5, musicSlider.yMax + 5, 0.0f, 1.0f, 0.3f, 1.0f);

        // Desenhar a "caixinha" de sfx
        float sKnobX = sfxSlider.xMin + (sfxSlider.value * (sfxSlider.xMax - sfxSlider.xMin));
        Renderer::DrawSemiTransparentRect(sKnobX - 10, sKnobX + 10, sfxSlider.yMin - 5, sfxSlider.yMax + 5, 0.0f, 1.0f, 0.3f, 1.0f);
    }
    else if (state == STATE_SELECAO_FASE)
    {
        Renderer::DrawTexture(fasesTexture, 0, 0, 1024, 768);

        // Desenha o cadeado em cima da Fase 2
        // Calculado no centro da hitbox da Fase 2 (X=490, Y=385)
        Renderer::DrawTexture(cadeadoTexture, 450, 330, 80, 80);

        // Desenha o cadeado em cima da Fase 3
        Renderer::DrawTexture(cadeadoTexture, 450, 165, 80, 80);

        // Efeito Hover FASE 1
        if (hoverX >= 120.0f && hoverX <= 900.0f && hoverY >= 450.0f && hoverY <= 560.0f)
        {
            Renderer::DrawSemiTransparentRect(120, 900, 450, 560, 0.0f, 1.0f, 0.3f, 0.15f);
        }
        // Efeito Hover Voltar
        if (hoverX >= 30.0f && hoverX <= 200.0f && hoverY >= 30.0f && hoverY <= 100.0f)
        {
            Renderer::DrawSemiTransparentRect(30, 200, 30, 100, 0.0f, 1.0f, 0.3f, 0.15f);
        }
    }
    else if (state == STATE_RELATORIO)
    {
        Renderer::DrawTexture(relatorioTexture, 0, 0, 1024, 768);
        
        // --- TEXTOS FALSOS PARA ESTATISTICAS (Temporario) ---
        Renderer::DrawText("ESTATISTICAS DO JOGADOR", 180, 560, 0.0f, 1.0f, 0.3f);
        Renderer::DrawText("STATUS DAS FASES", 550, 560, 0.0f, 1.0f, 0.3f);
        
        Renderer::DrawText("Pontos Acumulados: 12.500", 180, 480, 1.0f, 1.0f, 1.0f);
        Renderer::DrawText("Inimigos Destruidos: 342", 180, 430, 1.0f, 1.0f, 1.0f);
        Renderer::DrawText("Precisao de Tiros: 87%", 180, 380, 1.0f, 1.0f, 1.0f);
        Renderer::DrawText("Mortes: 3", 180, 330, 1.0f, 1.0f, 1.0f);
        
        Renderer::DrawText("Corrente Sanguinea: RANK S (Concluido)", 550, 480, 1.0f, 1.0f, 1.0f);
        Renderer::DrawText("Pulmoes: BLOQUEADO", 550, 430, 0.5f, 0.5f, 0.5f);
        Renderer::DrawText("Sistema Nervoso: BLOQUEADO", 550, 380, 0.5f, 0.5f, 0.5f);
        
        // Botao voltar
        if (hoverX >= 30.0f && hoverX <= 200.0f && hoverY >= 30.0f && hoverY <= 100.0f) {
            Renderer::DrawSemiTransparentRect(30, 200, 30, 100, 0.0f, 1.0f, 0.3f, 0.15f);
        }
    }
    else if (state == STATE_PAUSE)
    {
        // 1. Escurece o jogo congelado no fundo (Overlay transparente preto mais claro)
        Renderer::DrawSemiTransparentRect(0, 1024, 0, 768, 0.0f, 0.0f, 0.0f, 0.35f);
        
        // 2. Desenha a textura do Pause de volta no tamanho cheio 1024x768 (O centro e transparente)
        Renderer::DrawTexture(pauseTexture, 0, 0, 1024, 768);
        
        // Efeito Hover Continuar
        if (hoverX >= 380.0f && hoverX <= 644.0f && hoverY >= 460.0f && hoverY <= 530.0f) {
            Renderer::DrawSemiTransparentRect(380, 644, 460, 530, 0.0f, 1.0f, 0.3f, 0.15f);
        }
        // Efeito Hover Reiniciar
        if (hoverX >= 380.0f && hoverX <= 644.0f && hoverY >= 360.0f && hoverY <= 430.0f) {
            Renderer::DrawSemiTransparentRect(380, 644, 360, 430, 0.0f, 1.0f, 1.0f, 0.15f); // Ciano para diferir
        }
        // Efeito Hover Desistir
        if (hoverX >= 380.0f && hoverX <= 644.0f && hoverY >= 260.0f && hoverY <= 330.0f) {
            Renderer::DrawSemiTransparentRect(380, 644, 260, 330, 1.0f, 0.0f, 0.0f, 0.15f);
        }
    }
    else if (state == STATE_PLAYING)
    {
        // Placeholder render para tela de jogo
        Renderer::DrawSemiTransparentRect(0, 1024, 0, 768, 0.1f, 0.1f, 0.1f, 1.0f);
    }
}
