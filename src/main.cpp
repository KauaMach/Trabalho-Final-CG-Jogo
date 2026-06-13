#include "bibliotecas/renderer.h"
#include "bibliotecas/collision.h"
#include "bibliotecas/ai_boids.h"
#include "bibliotecas/ai_fsm.h"
#include "bibliotecas/particles.h"
#include "bibliotecas/audio.h"
#include <vector>
#include <iostream>

// Instanciação dos Módulos Principais
Renderer *renderer = nullptr;
BoidsEngine *boidsEngine = nullptr;
BossFSM *bossFSM = nullptr;
AudioManager *audioManager = nullptr;

// Definição do Enumerador de Fases com a Tela Inicial
enum Fase
{
    TELA_INICIAL,
    FASE_1_SANGUE,
    FASE_2_PULMAO,
    FASE_3_LINFATICO,
    FASE_4_NERVOSO,
    FASE_5_NUCLEO_VIRAL,
    VITORIA_TOTAL
};
Fase faseAtual = TELA_INICIAL; // O jogo começa estritamente na Tela Inicial

// IDs de Textura do OpenGL
unsigned int texturaFundoInicioID = 0;

// Estado Sólido das Entidades e Variáveis de Gameplay
float playerX = 0.0f, playerY = -4.0f;
Polaridade playerPolaridade = AZUL;
float barraSurge = 0.0f;
bool surgeAtivo = false;
int score = 0, combo = 1, dnaColetado = 0;
float hsp = 100.0f;
float bossHP = 150.0f;

std::vector<Boid> listaViruses;

// Protótipos das Funções de Callback do freeGLUT
void DisplayLoop();
void ReshapeCallback(int w, int h);
void KeyboardDown(unsigned char key, int x, int y);
void SpecialKeys(int key, int x, int y);
void MouseClick(int button, int state, int mouseX, int mouseY);
void TimerPhysics(int value);
const char *ObterNomeFaseEBoss(const char *&nomeBoss);
void RenderizarTelaMenu();

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Imunidade: A Guerra Celular");

    renderer = new Renderer();
    renderer->InicializarGL();

    boidsEngine = new BoidsEngine(0.06f, 0.005f);
    bossFSM = new BossFSM(500.0f);

    InicializarSistemaParticulas();
    audioManager = new AudioManager();
    audioManager->InicializarAudio();

    // Carregamento da sua imagem incrível de abertura
    texturaFundoInicioID = renderer->CarregarTextura("assets/textures/background_inicial.png");

    // Inicia a população de boids (inimigos)
    for (int i = 0; i < 20; ++i)
    {
        listaViruses.push_back({static_cast<float>(i - 10) * 0.6f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, i, true});
    }

    // Registro das Callbacks de Hardware no freeGLUT
    glutDisplayFunc(DisplayLoop);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardDown);
    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(MouseClick); // Monitor do clique no botão Start
    glutTimerFunc(16, TimerPhysics, 0);

    std::cout << "Engine Inicializada. Aguardando comando START no Menu Principal..." << std::endl;
    glutMainLoop();

    delete renderer;
    delete boidsEngine;
    delete bossFSM;
    delete audioManager;
    return 0;
}

// Retorna o nome da fase e do boss correspondente para atualização do HUD
const char *ObterNomeFaseEBoss(const char *&nomeBoss)
{
    switch (faseAtual)
    {
    case TELA_INICIAL:
        nomeBoss = "Nenhum";
        return "MENU PRINCIPAL";
    case FASE_1_SANGUE:
        nomeBoss = "Leukocyte Corrupto";
        return "FASE 1 - CORRENTE SANGUINEA";
    case FASE_2_PULMAO:
        nomeBoss = "Pneumococo Gigante";
        return "FASE 2 - BARREIRA PULMONAR";
    case FASE_3_LINFATICO:
        nomeBoss = "Nexus-7 Beta";
        return "FASE 3 - NODULO LINFATICO";
    case FASE_4_NERVOSO:
        nomeBoss = "Ganglion Prime";
        return "FASE 4 - SISTEMA NERVOSO";
    case FASE_5_NUCLEO_VIRAL:
        nomeBoss = "Nexus-7 Omega";
        return "FASE 5 - NUCLEO VIRAL (FINAL)";
    case VITORIA_TOTAL:
        nomeBoss = "Nenhum";
        return "PACIENTE SALVO! VITORIA!";
    }
    return "";
}

// Renderiza a sua imagem de abertura em tela cheia usando projeção ortogonal 2D
void RenderizarTelaMenu()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1024, 0, 768);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaFundoInicioID);

    // Renderiza a textura esticada perfeitamente na janela de 1024x768
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1024.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1024.0f, 768.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 768.0f);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Trata o clique do mouse em cima do botão START da sua imagem
void MouseClick(int button, int state, int mouseX, int mouseY)
{
    if (faseAtual == TELA_INICIAL && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Correção de orientação do eixo Y do freeGLUT (0 passa a ser a base do monitor)
        int realMouseY = 768 - mouseY;

        // Coordenadas calibradas milimetricamente para o botão da sua imagem
        if (mouseX >= 365 && mouseX <= 660 && realMouseY >= 115 && realMouseY <= 215)
        {
            std::cout << "Botao START detectado! Iniciando Jogo..." << std::endl;

            audioManager->TocarSurge();
            EmitirExplosao(0.0f, -4.0f, 1.0f, 1.0f, 1.0f);

            // Transiciona o estado do jogo para a Fase 1
            faseAtual = FASE_1_SANGUE;
        }
    }
}

void TimerPhysics(int value)
{
    // Se o jogo estiver nas telas de menu ou de fim de jogo, pausa as atualizações físicas
    if (faseAtual == TELA_INICIAL || faseAtual == VITORIA_TOTAL)
    {
        glutPostRedisplay();
        glutTimerFunc(16, TimerPhysics, 0);
        return;
    }

    // 1. Atualiza FSM do Boss da fase atual
    bossFSM->AtualizarEstado(bossHP);

    // 2. Atualiza os Boids inimigos
    boidsEngine->ProcessarEnxame(listaViruses, playerX, playerY, surgeAtivo);

    // 3. Monitoramento e Progressão de Fases
    if (bossHP <= 0.0f)
    {
        int proximaFase = static_cast<int>(faseAtual) + 1;
        faseAtual = static_cast<Fase>(proximaFase);

        if (faseAtual != VITORIA_TOTAL)
        {
            std::cout << "Zona purificada! Avancando de fase..." << std::endl;
            EmitirExplosao(0.0f, 0.0f, 0.0f, 0.7f, 1.0f);

            // Aumenta o HP do próximo Boss baseado na fase
            bossHP = 150.0f + (proximaFase * 50.0f);

            // Reorganiza as células virais no topo da tela para a nova fase
            for (size_t i = 0; i < listaViruses.size(); ++i)
            {
                listaViruses[i].ativo = true;
                listaViruses[i].y = 4.0f;
                listaViruses[i].x = static_cast<float>(i - 10) * 0.6f;
            }
        }
        else
        {
            std::cout << "Parabens! O corpo do paciente esta 100% imunizado!" << std::endl;
        }
    }

    // 4. Mecânica Completa de Colisões (Narrow Phase)
    SphereHitbox hitboxJogador = {playerX, playerY, 0.4f};

    for (size_t i = 0; i < listaViruses.size(); ++i)
    {
        if (!listaViruses[i].ativo)
            continue;

        SphereHitbox hitboxVirus = {listaViruses[i].x, listaViruses[i].y, 0.3f};

        // Colisão: Inimigo causa dano no Jogador
        if (CollisionEngine::ChecarEsferaParaEsfera(hitboxJogador, hitboxVirus))
        {
            if (!surgeAtivo)
            {
                hsp -= 2.0f;
                if (hsp < 0.0f)
                    hsp = 0.0f;
                listaViruses[i].y += 1.5f;                          // Repele o vírus para trás
                EmitirExplosao(playerX, playerY, 1.0f, 0.0f, 0.0f); // Sangue (Vermelho)
            }
        }

        // Colisão: Ataque do Jogador destrói o Inimigo e fere o Boss
        SphereHitbox hitboxAtaque = {playerX, playerY + 0.6f, 0.5f};
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            if (CollisionEngine::ChecarEsferaParaEsfera(hitboxAtaque, hitboxVirus))
            {
                listaViruses[i].ativo = false;
                score += 100;
                dnaColetado += 1;
                EmitirExplosao(listaViruses[i].x, listaViruses[i].y, 0.1f, 0.9f, 0.2f); // Explosão celular (Verde)

                bossHP -= 10.0f; // Dano colateral causado no Boss central
            }
        }
    }

    // Gerenciador de esvaziamento/recarga da barra de SURGE (Modo Fúria)
    if (surgeAtivo)
    {
        barraSurge -= 0.8f;
        if (barraSurge <= 0.0f)
        {
            barraSurge = 0.0f;
            surgeAtivo = false;
        }
    }
    else
    {
        if (barraSurge < 100.0f)
            barraSurge += 0.1f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, TimerPhysics, 0);
}

void DisplayLoop()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Se estiver no Menu Inicial, renderiza apenas a imagem de abertura e sai
    if (faseAtual == TELA_INICIAL)
    {
        RenderizarTelaMenu();
        glutSwapBuffers();
        return;
    }

    // --- RENDERIZAÇÃO DO SCENARIO DO JOGO 3D ---
    renderer->ConfigurarCamera(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    renderer->AtualizarIluminacaoDinamica(playerPolaridade, playerX, playerY);

    // Desenho tridimensional da Nave Nanocell (Jogador)
    glPushMatrix();
    glTranslatef(playerX, playerY, 0.0f);
    if (playerPolaridade == AZUL)
        glColor3f(0.1f, 0.4f, 1.0f);
    else
        glColor3f(1.0f, 0.1f, 0.1f);
    renderer->DesenharCuboGouraud(0.8f);
    glPopMatrix();

    // Desenho tridimensional dos Vírus (Inimigos normais)
    for (const auto &v : listaViruses)
    {
        if (v.ativo)
        {
            glPushMatrix();
            glTranslatef(v.x, v.y, 0.0f);
            glColor3f(0.1f, 0.9f, 0.2f);
            renderer->DesenharEsferaGouraud(0.3f, 12);
            glPopMatrix();
        }
    }

    // Desenho tridimensional do Boss da Fase
    if (faseAtual != VITORIA_TOTAL)
    {
        glPushMatrix();
        glTranslatef(0.0f, 3.5f, 0.0f); // Posicionado no topo central

        // A cor do modelo do Boss responde em tempo real ao estado da FSM
        if (bossFSM->ObterEstadoAtual() == PATRULHA)
            glColor3f(0.5f, 0.2f, 0.8f); // Roxo
        else if (bossFSM->ObterEstadoAtual() == AGRESSIVO)
            glColor3f(0.8f, 0.5f, 0.0f); // Laranja
        else
            glColor3f(1.0f, 0.0f, 0.3f); // Vermelho Raivoso

        renderer->DesenharEsferaGouraud(0.7f, 16);
        glPopMatrix();
    }

    // Atualiza o motor dinâmico de partículas na tela
    AtualizarERenderizarParticulas();

    // Renderização Dinâmica do HUD de Texto por cima da tela
    const char *nomeBossCurrent = nullptr;
    const char *nomeFaseCurrent = ObterNomeFaseEBoss(nomeBossCurrent);

    char bufferHUD[128];
    sprintf(bufferHUD, "%s  |  BOSS: %s (HP: %.0f)", nomeFaseCurrent, nomeBossCurrent, (bossHP > 0 ? bossHP : 0));
    renderer->RenderizarTextoHUD(15.0f, 570.0f, bufferHUD, GLUT_BITMAP_HELVETICA_18);

    sprintf(bufferHUD, "PACIENTE HSP: %.1f%%  |  SCORE: %d  |  SURGE: %.0f%%  |  DNA: %d", hsp, score, barraSurge, dnaColetado);
    renderer->RenderizarTextoHUD(15.0f, 545.0f, bufferHUD, GLUT_BITMAP_HELVETICA_12);

    glutSwapBuffers();
}

void ReshapeCallback(int w, int h)
{
    renderer->ConfigurarCamera(w, h);
}

void KeyboardDown(unsigned char key, int x, int y)
{
    // Ignora inputs de movimentação se estiver na Tela Inicial do Menu
    if (faseAtual == TELA_INICIAL)
    {
        if (key == 27)
            exit(0);
        return;
    }

    float passoMovimento = 0.25f;
    switch (key)
    {
    case 'w':
    case 'W':
        playerY += passoMovimento;
        break;
    case 's':
    case 'S':
        playerY -= passoMovimento;
        break;
    case 'a':
    case 'A':
        playerX -= passoMovimento;
        break;
    case 'd':
    case 'D':
        playerX += passoMovimento;
        break;
    case 32: // Espaço (Atirar)
        audioManager->TocarLaser();
        EmitirExplosao(playerX, playerY + 0.5f, 1.0f, 1.0f, 1.0f); // Flash do tiro
        break;
    case 'q':
    case 'Q': // Ativar o Modo SURGE (Especial)
        if (barraSurge >= 100.0f)
        {
            surgeAtivo = true;
            audioManager->TocarSurge();
            EmitirExplosao(playerX, playerY, 1.0f, 0.7f, 0.0f); // Onda de choque dourada
            if (faseAtual != VITORIA_TOTAL)
                bossHP -= 50.0f; // Dano massivo automático no Boss
        }
        break;
    case 27: // ESC para sair do jogo com segurança
        audioManager->LimparAudio();
        exit(0);
        break;
    }
}

void SpecialKeys(int key, int x, int y)
{
    if (faseAtual == TELA_INICIAL)
        return;

    // Altera a polaridade celular (cor do cubo) ao apertar SHIFT
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
    {
        playerPolaridade = (playerPolaridade == AZUL) ? VERMELHA : AZUL;
    }
}