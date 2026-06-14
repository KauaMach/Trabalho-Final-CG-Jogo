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
    TELA_FIM = 9,
    TELA_INSTRUCOES = 10
};

TelaEstado telaAtual = TELA_MENU; // O jogo começa estritamente na Tela Inicial

// Estado Sólido das Entidades e Variáveis de Gameplay
float playerX = 0.0f, playerY = -4.0f;
Polaridade playerPolaridade = AZUL;
float barraSurge = 0.0f;
bool surgeAtivo = false;
int score = 0, combo = 1, dnaColetado = 0;
float hsp = 100.0f;

std::vector<Boid> listaViruses;

// Protótipos das Funções de Callback do freeGLUT
void DisplayLoop();
void ReshapeCallback(int w, int h);
void KeyboardDown(unsigned char key, int x, int y);
void SpecialKeys(int key, int x, int y);
void MouseClick(int button, int state, int mouseX, int mouseY);
void TimerPhysics(int value);
const char *ObterNomeFase(void);

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Imunidade: A Guerra Celular");

    renderer = new Renderer();
    renderer->InicializarGL();

    boidsEngine = new BoidsEngine(0.06f, 0.005f);

    InicializarSistemaParticulas();
    audioManager = new AudioManager();
    audioManager->InicializarAudio();

    // ====================================================
    // --- INICIALIZAÇÃO ABSTRAÍDA DAS TEXTURAS DE MENU ---
    // ====================================================
    renderer->InicializarTexturaEstado(0, "assets/textures/background_inicial.png"); // Menu Inicial com 4 botões
    renderer->InicializarTexturaEstado(1, "assets/textures/fim.png");                // Tela de Fim de Jogo opcional
    renderer->InicializarTexturaEstado(2, "assets/textures/vitoria.png");            // Vitória
    renderer->InicializarTexturaEstado(3, "assets/textures/derrota.png");            // Derrota
    renderer->InicializarTexturaEstado(4, "assets/textures/creditos.png");           // Créditos
    renderer->InicializarTexturaEstado(5, "assets/textures/instrucoes.png");         // Instruções

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
    delete audioManager;
    return 0;
}

// Retorna o título amigável da zona anatómica atual para exibição no HUD
const char* ObterNomeFase()
{
    switch (telaAtual)
    {
    case TELA_MENU:           return "MENU PRINCIPAL";
    case FASE_1_SANGUE:       return "FASE 1 - CORRENTE SANGUINEA";
    case FASE_2_PULMAO:       return "FASE 2 - BARREIRA PULMONAR";
    case FASE_3_LINFATICO:    return "FASE 3 - NODULO LINFATICO";
    case FASE_4_NERVOSO:      return "FASE 4 - SISTEMA NERVOSO";
    case FASE_5_NUCLEO_VIRAL: return "FASE 5 - NUCLEO VIRAL (FINAL)";
    case TELA_VITORIA:        return "PACIENTE SALVO! VITORIA!";
    case TELA_DERROTA:        return "GAME OVER";
    case TELA_CREDITOS:       return "TELA DE CREDITOS";
    case TELA_INSTRUCOES:     return "COMO JOGAR - INSTRUCOES";
    case TELA_FIM:            return "OBRIGADO POR JOGAR!";
    default:                  return "";
    }
}

// Trata o clique do mouse considerando a imagem com os 4 botões verticais
void MouseClick(int button, int state, int mouseX, int mouseY)
{
    if (telaAtual == TELA_MENU && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        int realMouseY = 768 - mouseY; // Inversão para o padrão ortogonal 2D

        if (mouseX >= 360 && mouseX <= 660)
        {
            // --- BOTÃO 1: START ---
            if (realMouseY >= 445 && realMouseY <= 525)
            {
                std::cout << "Iniciando o jogo..." << std::endl;
                audioManager->TocarSurge();
                EmitirExplosao(0.0f, -4.0f, 1.0f, 1.0f, 1.0f);
                telaAtual = FASE_1_SANGUE;
            }
            // --- BOTÃO 2: CRÉDITOS ---
            else if (realMouseY >= 335 && realMouseY <= 415)
            {
                std::cout << "Abrindo creditos..." << std::endl;
                audioManager->TocarLaser();
                telaAtual = TELA_CREDITOS;
            }
            // --- BOTÃO 3: SAIR ---
            else if (realMouseY >= 225 && realMouseY <= 305)
            {
                std::cout << "Saindo do jogo..." << std::endl;
                audioManager->LimparAudio();
                exit(0);
            }
            // --- BOTÃO 4: INSTRUÇÕES ---
            else if (realMouseY >= 115 && realMouseY <= 195)
            {
                std::cout << "Abrindo instrucoes..." << std::endl;
                audioManager->TocarLaser();
                telaAtual = TELA_INSTRUCOES;
            }
        }
    }
    else if ((telaAtual == TELA_CREDITOS || telaAtual == TELA_FIM || telaAtual == TELA_INSTRUCOES) && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        telaAtual = TELA_MENU;
    }
}

void TimerPhysics(int value)
{
    // Congela a física se estivermos em telas de menu ou estados finais
    if (telaAtual == TELA_MENU || telaAtual == TELA_VITORIA ||
        telaAtual == TELA_DERROTA || telaAtual == TELA_CREDITOS ||
        telaAtual == TELA_FIM || telaAtual == TELA_INSTRUCOES)
    {
        glutPostRedisplay();
        glutTimerFunc(16, TimerPhysics, 0);
        return;
    }

    // 1. Atualiza a IA dos vírus (Boids)
    boidsEngine->ProcessarEnxame(listaViruses, playerX, playerY, surgeAtivo);

    // --- LÓGICA DE CONTENÇÃO: Mantém os vírus dentro do campo de visão ---
    for (size_t i = 0; i < listaViruses.size(); ++i)
    {
        if (!listaViruses[i].ativo)
            continue;

        // Rebater nas paredes laterais
        if (listaViruses[i].x < -6.0f)
        {
            listaViruses[i].x = -6.0f;
            listaViruses[i].vx *= -1.0f;
        }
        else if (listaViruses[i].x > 6.0f)
        {
            listaViruses[i].x = 6.0f;
            listaViruses[i].vx *= -1.0f;
        }

        // Se passar da base, volta ao topo (efeito loop)
        if (listaViruses[i].y < -5.5f)
        {
            listaViruses[i].y = 5.0f;
            listaViruses[i].x = static_cast<float>(i - 10) * 0.6f;
        }
    }

    // 2. Monitoramento de Derrota (HSP do Paciente zerou)
    if (hsp <= 0.0f)
    {
        telaAtual = TELA_DERROTA;
    }

    // 3. Mecânica de Colisões e Verificação de Onda
    SphereHitbox hitboxJogador = {playerX, playerY, 0.4f};
    bool todosVirusMortos = true;

    for (size_t i = 0; i < listaViruses.size(); ++i)
    {
        if (!listaViruses[i].ativo)
            continue;

        todosVirusMortos = false; // Existe pelo menos um inimigo vivo
        SphereHitbox hitboxVirus = {listaViruses[i].x, listaViruses[i].y, 0.3f};

        // Dano ao jogador
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

        // Ataque do jogador
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
            }
        }
#endif
    }

    // 4. Progressão de Fase Automática
    if (todosVirusMortos)
    {
        if (telaAtual == FASE_5_NUCLEO_VIRAL)
        {
            telaAtual = TELA_VITORIA;
        }
        else
        {
            int proximaFase = static_cast<int>(telaAtual) + 1;
            telaAtual = static_cast<TelaEstado>(proximaFase);

            EmitirExplosao(0.0f, 0.0f, 0.0f, 0.7f, 1.0f);

            // Reseta enxame
            for (size_t i = 0; i < listaViruses.size(); ++i)
            {
                listaViruses[i].ativo = true;
                listaViruses[i].y = 4.0f;
                listaViruses[i].x = static_cast<float>(i - 10) * 0.6f;
                listaViruses[i].vx = 0.0f;
                listaViruses[i].vy = 0.0f;
            }
        }
    }

    // 5. Gerenciador de SURGE
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
            renderer->RenderizarTelaEstado(0); 
            glutSwapBuffers();
            return;
        case TELA_FIM:
            renderer->RenderizarTelaEstado(1); 
            glutSwapBuffers();
            return;
        case TELA_VITORIA:
            renderer->RenderizarTelaEstado(2); 
            glutSwapBuffers();
            return;
        case TELA_DERROTA:
            renderer->RenderizarTelaEstado(3); 
            glutSwapBuffers();
            return;
        case TELA_CREDITOS:
            renderer->RenderizarTelaEstado(4); 
            glutSwapBuffers();
            return;
        case TELA_INSTRUCOES:
            renderer->RenderizarTelaEstado(5); 
            glutSwapBuffers();
            return;
        default:
        break;
    }

    // --- RENDERIZAÇÃO DO CENÁRIO DO JOGO 3D ---
    renderer->ConfigurarCamera(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
    renderer->AtualizarIluminacaoDinamica(playerPolaridade, playerX, playerY);

    // ====================================================
    // 1. DESENHO DO JOGADOR (LEUCÓCITO 3D METÁLICO/BRILHANTE)
    // ====================================================
    glPushMatrix();
        glTranslatef(playerX, playerY, 0.0f);

        // Define as cores do material do Player dependendo da Polaridade
        if (playerPolaridade == AZUL)
        {
            float matAmbDif[] = {0.1f, 0.4f, 1.0f, 1.0f}; // Azul Biológico
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDif);
        }
        else
        {
            float matAmbDif[] = {1.0f, 0.1f, 0.1f, 1.0f}; // Vermelho de Alerta
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDif);
        }

        // Configura o brilho especular (Efeito de plástico reflexivo/metalizado)
        float matEspecularPlayer[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Brilho branco puro
        glMaterialfv(GL_FRONT, GL_SPECULAR, matEspecularPlayer);
        glMateriali(GL_FRONT, GL_SHININESS, 128); // Brilho máximo e concentrado (0-128)

        renderer->DesenharLeukocito(4.0f);
    glPopMatrix();

    // ====================================================
    // 2. DESENHO DOS VÍRUS (VERDE BIOQUÍMICO COM REFLEXO ESFERICO)
    // ====================================================
    // Configura o material padrão para os vírus (Verde Celular Dinâmico)
    float matAmbDifVirus[] = {0.1f, 0.8f, 0.2f, 1.0f};    // Verde limão
    float matEspecularVirus[] = {0.6f, 1.0f, 0.6f, 1.0f}; // Brilho esverdeado nas bochechas do modelo

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDifVirus);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matEspecularVirus);
    glMateriali(GL_FRONT, GL_SHININESS, 64); // Brilho ligeiramente espalhado para parecer orgânico

    for (const auto &v : listaViruses)
    {
        if (v.ativo)
        {
            glPushMatrix();
                glTranslatef(v.x, v.y, 0.0f);

                if (telaAtual == FASE_1_SANGUE)
                    renderer->DesenharVirus1(3.5f);
                else if (telaAtual == FASE_2_PULMAO)
                    renderer->DesenharVirus2(3.5f);
                else if (telaAtual == FASE_3_LINFATICO)
                    renderer->DesenharVirus3(3.5f);
                else
                    renderer->DesenharVirus4(3.5f);

            glPopMatrix();
        }
    }

    // Atualiza o motor dinâmico de partículas na tela
    AtualizarERenderizarParticulas();

    // Renderização Dinâmica do HUD de Texto
    const char* nomeFaseCurrent = ObterNomeFase();

    char bufferHUD[128];
    sprintf(bufferHUD, "ANATOMIA: %s  |  INIMIGOS ATIVOS: %d", nomeFaseCurrent, (int)listaViruses.size());
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
    // Verifica se estamos em um dos estados finais
    if (telaAtual == TELA_VITORIA || telaAtual == TELA_DERROTA || telaAtual == TELA_FIM)
    {
        // Qualquer tecla pressionada (incluindo ESC) retorna ao menu
        telaAtual = TELA_MENU;

        // Opcional: Resetar variáveis de jogo para um novo início limpo
        hsp = 100.0f;
        score = 0;
        dnaColetado = 0;
        // Reinicializa a posição dos vírus se necessário
        for (size_t i = 0; i < listaViruses.size(); ++i)
        {
            listaViruses[i].ativo = true;
            listaViruses[i].y = 4.0f;
        }
        return;
    }

    // Comportamento normal para o Menu (apenas ESC sai)
    if (telaAtual == TELA_MENU || telaAtual == TELA_CREDITOS || telaAtual == TELA_INSTRUCOES)
    {
        if (key == 27)
            exit(0);
        return;
    }

    // Movimentação e ações de jogo (apenas enquanto estiver jogando)
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
    case 32: // Espaço
        audioManager->TocarLaser();
        EmitirExplosao(playerX, playerY + 0.5f, 1.0f, 1.0f, 1.0f);
        break;
    case 'q':
    case 'Q': // Modo SURGE
        if (barraSurge >= 100.0f)
        {
            surgeAtivo = true;
            audioManager->TocarSurge();
            EmitirExplosao(playerX, playerY, 1.0f, 0.7f, 0.0f);
        }
        break;
    case 27: // ESC
        audioManager->LimparAudio();
        exit(0);
        break;
    }
}

void SpecialKeys(int key, int x, int y)
{
    if (telaAtual == TELA_MENU || telaAtual == TELA_VITORIA || telaAtual == TELA_DERROTA || telaAtual == TELA_CREDITOS || telaAtual == TELA_FIM || telaAtual == TELA_INSTRUCOES)
        return;

    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
    {
        playerPolaridade = (playerPolaridade == AZUL) ? VERMELHA : AZUL;
    }
}