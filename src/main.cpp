#include "bibliotecas/renderer.h"
#include "bibliotecas/collision.h"
#include "bibliotecas/ai_boids.h"
#include "bibliotecas/ai_fsm.h"
#include "bibliotecas/particles.h"
#include "bibliotecas/audio.h"
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

// Instanciação dos Módulos Principais
Renderer *renderer = nullptr;
BoidsEngine *boidsEngine = nullptr;
BossFSM *bossFSM = nullptr;
AudioManager *audioManager = nullptr;

// Definição do Enumerador de Fases (Alinhado com os índices de estado do renderer)
enum TelaEstado
{
    TELA_MENU = 0,
    FASE_1_SANGUE = 1,
    FASE_2_PULMAO = 2,
    FASE_3_LINFATICO = 3,
    FASE_4_NERVOSO = 4,
    FASE_5_NUCLEO_VIRAL = 5,
    TELA_VITORIA = 6,
    TELA_DERROTA = 7,
    TELA_CREDITOS = 8,
    TELA_FIM = 9
};

TelaEstado telaAtual = TELA_MENU; // O jogo começa estritamente na Tela Inicial

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

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Imunidade: A Guerra Celular [cite: 612]");

    renderer = new Renderer();
    renderer->InicializarGL();

    boidsEngine = new BoidsEngine(0.06f, 0.005f);
    bossFSM = new BossFSM(500.0f);

    InicializarSistemaParticulas();
    audioManager = new AudioManager();
    audioManager->InicializarAudio();

    // ====================================================
    // --- INICIALIZAÇÃO ABSTRAÍDA DAS TEXTURAS DE MENU ---
    // ====================================================
    // Mapeamento explícito casando com o array texturasID[5] da classe Renderer
    renderer->InicializarTexturaEstado(0, "assets/textures/background_inicial.png"); // Menu Inicial com 3 botões
    renderer->InicializarTexturaEstado(1, "assets/textures/fim.png");                // Tela de Fim de Jogo opcional
    renderer->InicializarTexturaEstado(2, "assets/textures/vitoria.png");            // Vitória
    renderer->InicializarTexturaEstado(3, "assets/textures/derrota.png");            // Derrota
    renderer->InicializarTexturaEstado(4, "assets/textures/creditos.png");           // Créditos

    // População inicial do enxame de boids (inimigos)
    for (int i = 0; i < 20; ++i)
    {
        listaViruses.push_back({static_cast<float>(i - 10) * 0.6f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, i, true});
    }

    // Registro das Callbacks de Hardware no freeGLUT
    glutDisplayFunc(DisplayLoop);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardDown);
    glutSpecialFunc(SpecialKeys);
    glutMouseFunc(MouseClick);
    glutTimerFunc(16, TimerPhysics, 0);

    std::cout << "Engine Inicializada. Aguardando comando START no Menu Principal..." << std::endl;
    glutMainLoop();

    // Desalocação segura de memória
    delete renderer;
    delete boidsEngine;
    delete bossFSM;
    delete audioManager;
    return 0;
}

// Retorna o nome da fase e do boss correspondente para atualização do HUD
const char *ObterNomeFaseEBoss(const char *&nomeBoss)
{
    switch (telaAtual)
    {
    case TELA_MENU:
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
    case TELA_VITORIA:
        nomeBoss = "Nenhum";
        return "PACIENTE SALVO! VITORIA!";
    case TELA_DERROTA:
        nomeBoss = "Nenhum";
        return "GAME OVER";
    default:
        nomeBoss = "Nenhum";
        return "";
    }
}

// Trata o clique do mouse em cima do menu de 3 botões empilhados verticalmente
void MouseClick(int button, int state, int mouseX, int mouseY)
{
    if (telaAtual == TELA_MENU && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        int realMouseY = 768 - mouseY; // Inversão do Y para o padrão matemático gráfico [cite: 95]

        // Todos os botões estão alinhados horizontalmente entre X (360 e 660)
        if (mouseX >= 360 && mouseX <= 660)
        {
            // --- BOTÃO 1: START ---
            if (realMouseY >= 400 && realMouseY <= 490)
            {
                std::cout << "Iniciando o jogo..." << std::endl;
                audioManager->TocarSurge();
                EmitirExplosao(0.0f, -4.0f, 1.0f, 1.0f, 1.0f);
                telaAtual = FASE_1_SANGUE;
            }

            // --- BOTÃO 2: CRÉDITOS ---
            else if (realMouseY >= 250 && realMouseY <= 340)
            {
                std::cout << "Abrindo creditos..." << std::endl;
                audioManager->TocarLaser();
                telaAtual = TELA_CREDITOS;
            }

            // --- BOTÃO 3: SAIR ---
            else if (realMouseY >= 100 && realMouseY <= 190)
            {
                std::cout << "Saindo do jogo..." << std::endl;
                audioManager->LimparAudio();
                exit(0);
            }
        }
    }
    else if ((telaAtual == TELA_CREDITOS || telaAtual == TELA_FIM) && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        // Cliques em telas estáticas de repouso fazem o jogo voltar de forma segura para o Menu
        telaAtual = TELA_MENU;
    }
}

void TimerPhysics(int value)
{
    // Congela processamento se o jogo estiver em qualquer tela de repouso ou menus
    if (telaAtual == TELA_MENU || telaAtual == TELA_VITORIA ||
        telaAtual == TELA_DERROTA || telaAtual == TELA_CREDITOS || telaAtual == TELA_FIM)
    {
        glutPostRedisplay();
        glutTimerFunc(16, TimerPhysics, 0);
        return;
    }

    // 1. Atualiza FSM do Boss da fase atual
    bossFSM->AtualizarEstado(bossHP);

    // 2. Atualiza os Boids inimigos
    boidsEngine->ProcessarEnxame(listaViruses, playerX, playerY, surgeAtivo);

    // 3. Monitoramento de Derrota (HP do Paciente zerou)
    if (hsp <= 0.0f)
    {
        telaAtual = TELA_DERROTA;
    }

    // 4. Progressão de Fases controlada ao derrotar o Boss
    if (bossHP <= 0.0f)
    {
        if (telaAtual == FASE_5_NUCLEO_VIRAL)
        {
            std::cout << "Parabens! O corpo do paciente esta 100% imunizado!" << std::endl;
            telaAtual = TELA_VITORIA;
        }
        else
        {
            int proximaFase = static_cast<int>(telaAtual) + 1;
            telaAtual = static_cast<TelaEstado>(proximaFase);

            std::cout << "Zona purificada! Avancando de fase..." << std::endl;
            EmitirExplosao(0.0f, 0.0f, 0.0f, 0.7f, 1.0f);

            bossHP = 150.0f + (proximaFase * 50.0f);

            // Reorganiza as células virais no topo da tela para a nova fase
            for (size_t i = 0; i < listaViruses.size(); ++i)
            {
                listaViruses[i].ativo = true;
                listaViruses[i].y = 4.0f;
                listaViruses[i].x = static_cast<float>(i - 10) * 0.6f;
            }
        }
    }

    // 5. Mecânica Completa de Colisões (Narrow Phase)
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
                listaViruses[i].y += 1.5f;
                EmitirExplosao(playerX, playerY, 1.0f, 0.0f, 0.0f);
            }
        }

        // Colisão: Ataque do Jogador destrói o Inimigo e fere o Boss
        SphereHitbox hitboxAtaque = {playerX, playerY + 0.6f, 0.5f};
#ifdef _WIN32
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            if (CollisionEngine::ChecarEsferaParaEsfera(hitboxAtaque, hitboxVirus))
            {
                listaViruses[i].ativo = false;
                score += 100;
                dnaColetado += 1;
                EmitirExplosao(listaViruses[i].x, listaViruses[i].y, 0.1f, 0.9f, 0.2f);
                bossHP -= 10.0f;
            }
        }
#endif
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

    // ====================================================
    // --- RENDERS DE TELA INTEIRA USANDO A ABSTRAÇÃO ---
    // ====================================================
    switch (telaAtual)
    {
    case TELA_MENU:
        renderer->RenderizarTelaEstado(0); // Imagem 0: Menu Principal (3 Botões)
        glutSwapBuffers();
        return;
    case TELA_FIM:
        renderer->RenderizarTelaEstado(1); // Imagem 1: fim.png
        glutSwapBuffers();
        return;
    case TELA_VITORIA:
        renderer->RenderizarTelaEstado(2); // Imagem 2: vitoria.png
        glutSwapBuffers();
        return;
    case TELA_DERROTA:
        renderer->RenderizarTelaEstado(3); // Imagem 3: derrota.png
        glutSwapBuffers();
        return;
    case TELA_CREDITOS:
        renderer->RenderizarTelaEstado(4); // Imagem 4: creditos.png
        glutSwapBuffers();
        return;
    default:
        break;
    }

    // --- RENDERIZAÇÃO DO SCENARIO DO JOGO 3D COM OS NOVOS ARQUIVOS ---
    renderer->ConfigurarCamera(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    renderer->AtualizarIluminacaoDinamica(playerPolaridade, playerX, playerY);

    // 1. Desenho do Jogador (Utilizando o seu Leucócito .obj convertido estaticamente)
    glPushMatrix();
    glTranslatef(playerX, playerY, 0.0f);
    if (playerPolaridade == AZUL)
        glColor3f(0.1f, 0.4f, 1.0f);
    else
        glColor3f(1.0f, 0.1f, 0.1f);

    renderer->DesenharLeukocito(0.6f);
    glPopMatrix();

    // 2. Desenho do Enxame de Inimigos (Alternando dinamicamente os 4 renders de vírus por fase)
    for (const auto &v : listaViruses)
    {
        if (v.ativo)
        {
            glPushMatrix();
            glTranslatef(v.x, v.y, 0.0f);
            glColor3f(0.1f, 0.9f, 0.2f);

            if (telaAtual == FASE_1_SANGUE)
                renderer->DesenharVirus1(0.3f);
            else if (telaAtual == FASE_2_PULMAO)
                renderer->DesenharVirus2(0.3f);
            else if (telaAtual == FASE_3_LINFATICO)
                renderer->DesenharVirus3(0.3f);
            else
                renderer->DesenharVirus4(0.3f);

            glPopMatrix();
        }
    }

    // 3. Desenho do Boss da Fase (Mantido como Esfera Gouraud estável de CG) [cite: 619]
    if (telaAtual != TELA_VITORIA && telaAtual != TELA_DERROTA)
    {
        glPushMatrix();
        glTranslatef(0.0f, 3.5f, 0.0f);

        if (bossFSM->ObterEstadoAtual() == PATRULHA)
            glColor3f(0.5f, 0.2f, 0.8f);
        else if (bossFSM->ObterEstadoAtual() == AGRESSIVO)
            glColor3f(0.8f, 0.5f, 0.0f);
        else
            glColor3f(1.0f, 0.0f, 0.3f);

        renderer->DesenharEsferaGouraud(0.7f, 16);
        glPopMatrix();
    }

    // Atualiza o motor dinâmico de partículas na tela
    AtualizarERenderizarParticulas();

    // Renderização Dinâmica do HUD de Texto
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
    if (telaAtual == TELA_MENU || telaAtual == TELA_VITORIA || telaAtual == TELA_DERROTA || telaAtual == TELA_CREDITOS || telaAtual == TELA_FIM)
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
        EmitirExplosao(playerX, playerY + 0.5f, 1.0f, 1.0f, 1.0f);
        break;
    case 'q':
    case 'Q': // Ativar o Modo SURGE
        if (barraSurge >= 100.0f)
        {
            surgeAtivo = true;
            audioManager->TocarSurge();
            EmitirExplosao(playerX, playerY, 1.0f, 0.7f, 0.0f);
            if (telaAtual != TELA_VITORIA)
                bossHP -= 50.0f;
        }
        break;
    case 27:
        audioManager->LimparAudio();
        exit(0);
        break;
    }
}

void SpecialKeys(int key, int x, int y)
{
    if (telaAtual == TELA_MENU || telaAtual == TELA_VITORIA || telaAtual == TELA_DERROTA || telaAtual == TELA_CREDITOS || telaAtual == TELA_FIM)
        return;

    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
    {
        playerPolaridade = (playerPolaridade == AZUL) ? VERMELHA : AZUL;
    }
}