#include "bibliotecas/renderer.h"
#include "bibliotecas/collision.h"
#include "bibliotecas/ai_boids.h"
#include "bibliotecas/ai_fsm.h"
#include "bibliotecas/particles.h"
#include "bibliotecas/audio.h"
#include <vector>
#include <iostream>
#include <vector>
#include <functional>

// Instanciação dos Módulos Principais
Renderer *renderer = nullptr;
BoidsEngine *boidsEngine = nullptr;
AudioManager *audioManager = nullptr;
SpatialHashGrid *grid = nullptr;
bool teclaEspacoPressionada = false;
bool teclas[256] = {false};
bool teclasEspeciais[200] = {false};

struct Button
{
    int xMin, xMax, yMin, yMax;
    void (*action)(); // Ponteiro de função para a ação
};

struct MenuButton
{
    int xMin, xMax, yMin, yMax;
    std::string label;
    std::function<void()> onClick;
};

enum TelaEstado
{
    TELA_MENU = 0,
    FASE_1 = 1,
    FASE_2 = 2,
    FASE_3 = 3,
    FASE_4 = 4,
    FASE_5 = 5,
    TELA_VITORIA = 6,
    TELA_DERROTA = 7,
    TELA_CREDITOS = 8,
    TELA_FIM = 9,
    TELA_INSTRUCOES = 10,
    TELA_FUNDO = 11
};

TelaEstado telaAtual = TELA_MENU; 

void StartGame()
{
    std::cout << "Iniciando o jogo..." << std::endl;
    audioManager->TocarSurge();
    EmitirExplosao(0.0f, -4.0f, 1.0f, 1.0f, 1.0f);
    telaAtual = FASE_1;
}

// Estado Sólido das Entidades e Variáveis de Gameplay
float playerX = 0.0f, playerY = -4.0f;
float barraSurge = 0.0f;
bool surgeAtivo = false;
int score = 0, combo = 1, dnaColetado = 0;
float hsp = 100.0f;

std::vector<Boid> listaVirus;

void DisplayLoop();
void ReshapeCallback(int w, int h);
void KeyboardDown(unsigned char key, int x, int y);
void ControleTeclasAcao(unsigned char key);
void SairDoJogo();
void TentarAtivarSurge();
void AtivarTiro();
void KeyboardUp(unsigned char key, int x, int y);
void MouseClick(int button, int state, int mouseX, int mouseY);
void TimerPhysics(int value);
void PassouFase(bool todosVirusMortos);
void ColisaoPlayerVirus(const SphereHitbox &hitboxJogador, const SphereHitbox &hitboxVirus, Boid &v);
void ColisaoTiroVirus(bool espacoPressionado, const SphereHitbox &hitboxVirus, Boid &v);
void ContencaoVirus();
void ControleSurge();
void PassarFase(bool todosVirusMortos);
void LoopColisao(ColisaoObjeto &objPlayer);
bool IsTelaJogavel();
void VerificaEstadosFinal();
void SpecialKeys(int key, int x, int y);
void RenderizarHUDTexto(int score, float hsp, float barraSurge, int dnaColetado, const char* nomeFase);
void DesenhaPlayer(float x, float y);
void DesenhaVirus();
void DefineCorVirus();
void InicializaTexturasMenu(Renderer *renderer);
void ExameBoidsInicial();
void RenderizarTelas();
void SpecialUpKeys(int key, int x, int y);
void AtualizarInputLogico();
void HandleMenuClick(int mouseX, int realMouseY);

const char *ObterNomeFase(void);

/**
 * @brief Função principal do jogo, responsável por inicializar o ambiente gráfico, 
 * módulos de áudio, partículas e iniciar o loop principal do GLUT.
 */
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutCreateWindow("Imunidade: A Guerra Celular");

    renderer = new Renderer();
    renderer->InicializarGL();
    renderer->CompilarModelos();
    renderer->ConfigurarCamera(1024, 768);

    grid = new SpatialHashGrid(1.0f); 
    boidsEngine = new BoidsEngine(0.06f, 0.05f);

    InicializarSistemaParticulas();
    audioManager = new AudioManager();
    audioManager->InicializarAudio();

    InicializaTexturasMenu(renderer);

    // População inicial do enxame de boids (inimigos)
    ExameBoidsInicial();

    // Registro das Callbacks de Hardware no freeGLUT
    glutDisplayFunc(DisplayLoop);
    glutReshapeFunc(ReshapeCallback);
    glutKeyboardFunc(KeyboardDown);
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(SpecialKeys);
    glutSpecialUpFunc(SpecialUpKeys);
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

/**
 * @brief Inicializa o enxame de boids (inimigos) com posições e velocidades iniciais.
 */
void ExameBoidsInicial()
{
    for (int i = 0; i < 20; ++i)
    {
        listaVirus.push_back({static_cast<float>(i - 10) * 0.6f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, i, true});
    }
}

/**
 * @brief Inicializa as texturas para os diferentes estados do menu.
 * @param renderer Ponteiro para o objeto Renderer.
 */
void InicializaTexturasMenu(Renderer* renderer)
{
    renderer->InicializarTexturaEstado(0, "assets/textures/background_inicial.png");
    renderer->InicializarTexturaEstado(1, "assets/textures/fim.png");
    renderer->InicializarTexturaEstado(2, "assets/textures/vitoria.png");
    renderer->InicializarTexturaEstado(3, "assets/textures/derrota.png");
    renderer->InicializarTexturaEstado(4, "assets/textures/creditos.png");
    renderer->InicializarTexturaEstado(5, "assets/textures/instrucoes.png");
    renderer->InicializarTexturaEstado(6, "assets/textures/fundo.png");
}

/**
 * @brief Obtém o nome da fase atual com base no estado do jogo.
 * @return Nome da fase como uma string constante.
 */
const char* ObterNomeFase()
{
    switch (telaAtual)
    {
    case TELA_MENU:           return "MENU PRINCIPAL";
    case FASE_1:              return "FASE 1";
    case FASE_2:              return "FASE 2";
    case FASE_3:              return "FASE 3";
    case FASE_4:              return "FASE 4";
    case FASE_5:              return "FASE 5";
    case TELA_VITORIA:        return "PACIENTE SALVO! VITORIA!";
    case TELA_DERROTA:        return "GAME OVER";
    case TELA_CREDITOS:       return "TELA DE CREDITOS";
    case TELA_INSTRUCOES:     return "COMO JOGAR - INSTRUCOES";
    case TELA_FIM:            return "OBRIGADO POR JOGAR!";
    default:                  return "";
    }
}

/**
 * @brief Trata o clique do mouse em um botão do menu.
 * @param mouseX Coordenada X do mouse.
 * @param realMouseY Coordenada Y do mouse (invertida).
 */
void HandleMenuClick(int mouseX, int realMouseY)
{
    const std::vector<MenuButton> menuButtons = {
        {360, 660, 445, 525, "Start", []()
         {
             std::cout << "Iniciando o jogo..." << std::endl;
             audioManager->TocarSurge();
             EmitirExplosao(0.0f, -4.0f, 1.0f, 1.0f, 1.0f);
             telaAtual = FASE_1;
         }},
        {360, 660, 335, 415, "Creditos", []()
         {
             std::cout << "Abrindo creditos..." << std::endl;
             audioManager->TocarLaser();
             telaAtual = TELA_CREDITOS;
         }},
        {360, 660, 225, 305, "Sair", []()
         {
             std::cout << "Saindo do jogo..." << std::endl;
             audioManager->LimparAudio();
             exit(0);
         }},
        {360, 660, 115, 195, "Instrucoes", []()
         {
             std::cout << "Abrindo instrucoes..." << std::endl;
             audioManager->TocarLaser();
             telaAtual = TELA_INSTRUCOES;
         }}};

    for (const auto &btn : menuButtons)
        if (mouseX >= btn.xMin && mouseX <= btn.xMax &&
            realMouseY >= btn.yMin && realMouseY <= btn.yMax)
        {

            btn.onClick(); 
            return;       
        }
    
}

/**
 * @brief Callback para o clique do mouse, responsável por detectar cliques em botões do menu.
 * @param button Botão do mouse (GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, etc.).
 * @param state Estado do botão (GLUT_DOWN ou GLUT_UP).
 * @param mouseX Coordenada X do mouse.
 * @param mouseY Coordenada Y do mouse.
 */
void MouseClick(int button, int state, int mouseX, int mouseY)
{
    if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN){ return; }

    int realMouseY = 768 - mouseY;
    switch (telaAtual)
    {
    case TELA_MENU:
        HandleMenuClick(mouseX, realMouseY);
        break;

    case TELA_CREDITOS:
    case TELA_FIM:
    case TELA_INSTRUCOES:
        telaAtual = TELA_MENU;
        break;

    default:
        break;
    }
}

/**
 * @brief Callback para o timer de física, responsável por atualizar a lógica do jogo em intervalos regulares.
 * @param value Valor do timer.
 */
void TimerPhysics(int value)
{
    if(!IsTelaJogavel()){
        glutPostRedisplay();
        glutTimerFunc(16, TimerPhysics, 0);
        return;
    }

    boidsEngine->ProcessarEnxame(listaVirus, playerX, playerY, surgeAtivo);

    ContencaoVirus();

    if (hsp <= 0.0f) telaAtual = TELA_DERROTA;

    SphereHitbox hitboxJogador = {playerX, playerY, 0.4f};
    bool todosVirusMortos = true;
    bool espacoPressionado = (GetAsyncKeyState(VK_SPACE) & 0x8000) != 0;

    for (auto &v : listaVirus)
    {
        if (!v.ativo)
            continue;

        todosVirusMortos = false;
        SphereHitbox hitboxVirus = {v.x, v.y, 0.3f};

        ColisaoPlayerVirus(hitboxJogador, hitboxVirus, v);
        ColisaoTiroVirus(espacoPressionado, hitboxVirus, v);
    }

    PassouFase(todosVirusMortos);
    ControleSurge();
    glutPostRedisplay();
    glutTimerFunc(16, TimerPhysics, 0);
}

/**
 * @brief Verifica se todos os vírus foram eliminados e avança para a próxima fase ou tela de vitória.
 * @param todosVirusMortos Indica se todos os vírus foram eliminados.
 */
void PassouFase(bool todosVirusMortos)
{
    if (!todosVirusMortos)
        return;

    if (telaAtual == FASE_5)
        telaAtual = TELA_VITORIA;
    else
    {
        telaAtual = static_cast<TelaEstado>(static_cast<int>(telaAtual) + 1);
        EmitirExplosao(0.0f, 0.0f, 0.0f, 0.7f, 1.0f);

        for (auto &v : listaVirus)
        {
            v.ativo = true;
            v.y = 4.0f;
            v.vx *= 1.1f;
        }
    }
}

/**
 * @brief Trata a colisão entre o jogador e um vírus.
 * @param hitboxJogador Hitbox do jogador.
 * @param hitboxVirus Hitbox do vírus.
 * @param v Referência para o vírus.
 */
void ColisaoPlayerVirus(const SphereHitbox &hitboxJogador, const SphereHitbox &hitboxVirus, Boid &v)
{
    if (CollisionEngine::ChecarEsferaParaEsfera(hitboxJogador, hitboxVirus))
    {
        if (!surgeAtivo)
        {
            hsp = std::max(0.0f, hsp - 1.0f);
            v.y += 1.5f;
            EmitirExplosao(playerX, playerY, 1.0f, 0.0f, 0.0f);
        }
    }
}

/**
 * @brief Trata a colisão entre um tiro e um vírus.
 * @param espacoPressionado Indica se o espaço foi pressionado.
 * @param hitboxVirus Hitbox do vírus.
 * @param v Referência para o vírus.
 */
void ColisaoTiroVirus(bool espacoPressionado, const SphereHitbox &hitboxVirus, Boid &v)
{
    if (espacoPressionado)
    {
        SphereHitbox hitboxAtaque = {playerX, playerY + 0.6f, 0.5f};
        if (CollisionEngine::ChecarEsferaParaEsfera(hitboxAtaque, hitboxVirus))
        {
            v.ativo = false;
            score += 100;
            dnaColetado += 1;
            EmitirExplosao(v.x, v.y, 0.1f, 0.9f, 0.2f);
        }
    }
}

/**
 * @brief Mantém os vírus dentro dos limites da tela, invertendo sua direção ao atingir as bordas.
 */
void ContencaoVirus()
{
    for (size_t i = 0; i < listaVirus.size(); ++i)
    {
        if (!listaVirus[i].ativo)
            continue;

        if (listaVirus[i].x < -6.0f)
        {
            listaVirus[i].x = -6.0f;
            listaVirus[i].vx *= -1.0f;
        }
        else if (listaVirus[i].x > 6.0f)
        {
            listaVirus[i].x = 6.0f;
            listaVirus[i].vx *= -1.0f;
        }

        if (listaVirus[i].y < -5.5f)
        {
            listaVirus[i].y = 5.0f;
            listaVirus[i].x = static_cast<float>(i - 10) * 0.6f;
        }
    }
}

/**
 * @brief Controla a barra de Surge, diminuindo-a quando ativa e recarregando-a quando inativa.
 */
void ControleSurge()
{
    if (surgeAtivo)
    {
        barraSurge = std::max(0.0f, barraSurge - 0.8f);
        if (barraSurge == 0)
            surgeAtivo = false;
    }
    else
    {
        barraSurge = std::min(100.0f, barraSurge + 0.1f);
    }
}

/**
 * @brief Avança para a próxima fase ou tela de vitória se todos os vírus foram eliminados.
 * @param todosVirusMortos Indica se todos os vírus foram eliminados.
 */
void PassarFase(bool todosVirusMortos)
{
    if (todosVirusMortos)
    {
        switch (telaAtual)
        {
            case FASE_1:
                telaAtual = FASE_2;
                break;
            case FASE_2:
                telaAtual = FASE_3;
                break;
            case FASE_3:
                telaAtual = FASE_4;
                break;
            case FASE_4:
                telaAtual = FASE_5;
                break;
            case FASE_5:
                telaAtual = TELA_VITORIA;
                break;
            default:
                break;
        }

        if (telaAtual != TELA_VITORIA)
        {
            for (auto &v : listaVirus)
            {
                v.ativo = true;
                v.y = 4.0f;
                v.vx *= 1.05f;
            }
        }
    }
}

/**
 * @brief Loop de detecção de colisões entre o jogador e outros objetos.
 * @param objPlayer Referência para o objeto do jogador.
 */
void LoopColisao(ColisaoObjeto &objPlayer)
{
    std::vector<ColisaoObjeto> vizinhosPlayer = grid->ObterVizinhosProximos(objPlayer);

    for (const auto &vizinho : vizinhosPlayer)
    {
        if (std::abs(objPlayer.corpoEsfera.x - vizinho.corpoEsfera.x) > 2.0f)
            continue;

        if (vizinho.tipo == 1)
        { 
            if (CollisionEngine::ChecarEsferaParaEsfera(objPlayer.corpoEsfera, vizinho.corpoEsfera))
            {
                if (surgeAtivo)
                {
                    for (auto &v : listaVirus)
                    {
                        if (v.id == vizinho.id)
                        {   
                            v.ativo = false;
                            EmitirExplosao(v.x, v.y, 0.5f, 0.1f, 0.8f); 
                            score += 100;
                            break;
                        }
                    }
                }
                else
                {
                    EmitirExplosao(playerX, playerY, 1.0f, 0.0f, 0.0f); 
                    telaAtual = TELA_DERROTA;
                }
            }
        }
    }
}

/**
 * @brief Verifica se a tela atual é jogável.
 * @return true se a tela for jogável, false caso contrário.
 */
bool IsTelaJogavel()
{
    if (telaAtual != TELA_MENU && telaAtual != TELA_VITORIA && telaAtual != TELA_DERROTA &&
        telaAtual != TELA_CREDITOS && telaAtual != TELA_FIM && telaAtual != TELA_INSTRUCOES)
    {
        return true;
    }
    return false;
}

/**
 * @brief Verifica os estados finais do jogo e realiza ações apropriadas.
 */
void DisplayLoop()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 15, 0, 0, 0, 0, 1, 0); 
    renderer->AtualizarIluminacaoDinamica(playerX, playerY);
    RenderizarTelas();
    DesenhaPlayer(playerX, playerY);
    DesenhaVirus();
    AtualizarERenderizarParticulas();
    RenderizarHUDTexto(score, hsp, barraSurge, dnaColetado, ObterNomeFase());

    glutSwapBuffers();
}

/**
 * @brief Renderiza a tela correspondente ao estado atual do jogo.
 */
void RenderizarTelas()
{
    switch (telaAtual)
    {
        case TELA_MENU:
            renderer->RenderizarTelaEstado(0);
            return;
        case TELA_FIM:
            renderer->RenderizarTelaEstado(1);
            return;
        case TELA_VITORIA:
            renderer->RenderizarTelaEstado(2);
            return;
        case TELA_DERROTA:
            renderer->RenderizarTelaEstado(3);
            return;
        case TELA_CREDITOS:
            renderer->RenderizarTelaEstado(4);
            return;
        case TELA_INSTRUCOES:
            renderer->RenderizarTelaEstado(5);
            return;
        case FASE_1:
        case FASE_2:
        case FASE_3:
        case FASE_4:
        case FASE_5:
            glDisable(GL_DEPTH_TEST);
            renderer->RenderizarTelaEstado(6);
            glEnable(GL_DEPTH_TEST);
            return;
        default:
            break;
    }
}

/**
 * @brief Renderiza o texto do HUD (Heads-Up Display) com informações do jogo.
 * @param score Pontuação atual do jogador.
 * @param hsp Vida atual do jogador.
 * @param barraSurge Valor da barra de Surge.
 * @param dnaColetado Quantidade de DNA coletado.
 * @param nomeFase Nome da fase atual.
 */
void RenderizarHUDTexto(int score, float hsp, float barraSurge, int dnaColetado, const char* nomeFase)
{
    char bufferHUD[256];

    snprintf(bufferHUD, sizeof(bufferHUD), "Fase: %s | Score: %d | Vida: %.0f | DNA: %d | Surge: %.0f%%",
             nomeFase, score, hsp, dnaColetado, barraSurge);

    renderer->RenderizarTextoHUD(15.0f, 570.0f, bufferHUD, GLUT_BITMAP_HELVETICA_18);
}

/**
 * @brief Desenha o jogador na tela com base em suas coordenadas.
 * @param x Coordenada X do jogador.
 * @param y Coordenada Y do jogador.
 */
void DesenhaPlayer(float x, float y)
{
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glDisable(GL_COLOR_MATERIAL); 
    float matAmbDifPlayer[] = { 1.0f, 0.753f, 0.796f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, matAmbDifPlayer);
    glCallList(renderer->displayLists[0]);  
    glEnable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

/**
 * @brief Desenha os vírus na tela com base em suas coordenadas e estado de atividade.
 */
void DesenhaVirus()
{
    for (const auto &v : listaVirus)
    {
        if (v.ativo)
        {
            glPushMatrix();
            glTranslatef(v.x, v.y, 0.0f);

            DefineCorVirus();

            int idx = 1;
            if (telaAtual == FASE_2)
                idx = 2;
            else if (telaAtual == FASE_3)
                idx = 3;
            else if (telaAtual == FASE_4)
                idx = 4;

            glCallList(renderer->displayLists[idx]);
            glPopMatrix();
        }
    }
}

/**
 * @brief Define a cor do vírus com base na fase atual do jogo.
 */
void DefineCorVirus()
{
    float matVirus[4];
    if (telaAtual == FASE_1)
    {
        matVirus[0] = 0.541f;
        matVirus[1] = 0.169f;
        matVirus[2] = 0.886f;
        matVirus[3] = 1.0f;
    }
    else if (telaAtual == FASE_2)
    {
        matVirus[0] = 1.0f;
        matVirus[1] = 0.271f;
        matVirus[2] = 0.0f;
        matVirus[3] = 1.0f;
    }
    else if (telaAtual == FASE_3)
    {
        matVirus[0] = 1.0f;
        matVirus[1] = 0.843f;
        matVirus[2] = 0.0f;
        matVirus[3] = 1.0f;
    }
    else
    {
        matVirus[0] = 0.0f;
        matVirus[1] = 0.1f;
        matVirus[2] = 0.0f;
        matVirus[3] = 1.0f;
    }
    glColor4f(matVirus[0], matVirus[1], matVirus[2], matVirus[3]);
}

/**
 * @brief Callback para redimensionamento da janela, ajustando a câmera e o viewport.
 * @param w Nova largura da janela.
 * @param h Nova altura da janela.
 */
void ReshapeCallback(int w, int h)
{
    renderer->ConfigurarCamera(w, h);
    glViewport(0, 0, w, h);
}

/**
 * @brief Callback para teclas pressionadas, atualizando o estado do jogo.
 * @param key Tecla pressionada.
 * @param x Coordenada X do mouse.
 * @param y Coordenada Y do mouse.
 */
void KeyboardDown(unsigned char key, int x, int y)
{
    VerificaEstadosFinal();
    teclas[key] = true;
    AtualizarInputLogico();
    ControleTeclasAcao(key);

    switch (telaAtual)
    {
        case TELA_MENU:
            if (key == 27)
                exit(0);
            break;  

        case TELA_CREDITOS:
        case TELA_INSTRUCOES:
        case TELA_VITORIA:
        case TELA_DERROTA:
        case TELA_FIM:
            if (key == 27) exit(0);
            else telaAtual = TELA_MENU;
            break;

        default:
            break;
    }
    
}

/**
 * @brief Controla as ações associadas às teclas pressionadas.
 * @param key Tecla pressionada.
 */
void ControleTeclasAcao(unsigned char key)
{
    AtualizarInputLogico();

    switch (key)
    {
        case 32:
            AtivarTiro();
            break;
        case 'q':
        case 'Q':
            TentarAtivarSurge();
            break;
        case 27:
            SairDoJogo();
            break;
        }
}

/**
 * @brief Encerra o jogo, limpando os recursos de áudio e saindo do programa.
 */
void SairDoJogo()
{
    audioManager->LimparAudio();
    exit(0);
}

/**
 * @brief Tenta ativar o Surge, se a barra estiver cheia.
 */
void TentarAtivarSurge()
{
    if (barraSurge >= 100.0f)
    {
        surgeAtivo = true;
        audioManager->TocarSurge();
        EmitirExplosao(playerX, playerY, 1.0f, 0.7f, 0.0f);
    }
}

/**
 * @brief Ativa o tiro do jogador, emitindo uma explosão e tocando o som do laser.
 */
void AtivarTiro()
{
    teclaEspacoPressionada = true;
    audioManager->TocarLaser();
    EmitirExplosao(playerX, playerY + 0.5f, 1.0f, 1.0f, 1.0f);
}

/**
 * @brief Verifica os estados finais do jogo e reinicia o jogo se necessário.
 */
void VerificaEstadosFinal()
{
    if (telaAtual == TELA_VITORIA || telaAtual == TELA_DERROTA || telaAtual == TELA_FIM) {
        telaAtual = TELA_MENU;
        hsp = 100.0f; score = 0; dnaColetado = 0;
        for (auto& v : listaVirus) { v.ativo = true; v.y = 4.0f; }
        return;
    }
}

/**
 * @brief Callback para teclas soltas, atualizando o estado do jogo.
 * @param key Tecla solta.
 * @param x Coordenada X do mouse.
 * @param y Coordenada Y do mouse.
 */
void KeyboardUp(unsigned char key, int x, int y)
{
    teclas[key] = false; // Tecla solta
    if (key == 32)
        teclaEspacoPressionada = false;
}

/**
 * @brief Callback para teclas especiais pressionadas, atualizando o estado do jogo.
 * @param key Tecla especial pressionada.
 * @param x Coordenada X do mouse.
 * @param y Coordenada Y do mouse.
 */
void SpecialKeys(int key, int x, int y)
{
    teclasEspeciais[key] = true; 
}

/**
 * @brief Callback para teclas especiais soltas, atualizando o estado do jogo.
 * @param key Tecla especial solta.
 * @param x Coordenada X do mouse.
 * @param y Coordenada Y do mouse.
 */
void SpecialUpKeys(int key, int x, int y)
{
    teclasEspeciais[key] = false; 
}

/**
 * @brief Atualiza a posição do jogador com base nas teclas pressionadas.
 */
void AtualizarInputLogico()
{
    float passoMovimento = 0.05f;

    if (teclas['w'] || teclas['W'] || teclasEspeciais[GLUT_KEY_UP])
        playerY += passoMovimento;
    
    if (teclas['s'] || teclas['S'] || teclasEspeciais[GLUT_KEY_DOWN])
        playerY -= passoMovimento;
    
    if (teclas['a'] || teclas['A'] || teclasEspeciais[GLUT_KEY_LEFT])
        playerX -= passoMovimento;
    
    if (teclas['d'] || teclas['D'] || teclasEspeciais[GLUT_KEY_RIGHT])
        playerX += passoMovimento;
}