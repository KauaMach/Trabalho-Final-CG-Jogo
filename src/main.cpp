#include <iostream>
#include <cmath>
#include "core/Renderer.h"
#include "scenes/MenuUI.h"
#include "core/AudioManager.h"
#include "entities/Player.h"
#include "entities/inimigo.h"
#include "core/colisao.h"
#include "core/ParticleSystem.h"
#include <cstdlib> // Para rand()

// Variavel de Debug (Controla a exibicao dos fios de colisao)
bool showCollisionBoxes = false;

GameState currentState = STATE_MENU;
MenuUI menuUI;
AudioManager audioManager;
Player player;
bool keyState[256] = {false};
int cameraMode = 3; // 3 Top-Down (Padrao)

// Sistema de Gerenciamento da Horda (Boids e Inimigos)
std::vector<Inimigo *> listaInimigos;
int spawnTimer = 220;     // Alfa nasce 20 frames (aprox 0.3s) apos o jogo comecar
int spawnCocoTimer = 360; // Coco nasce 2 segundos apos o jogo comecar

float phase1Time = 0.0f;
bool bossSpawned = false;
float gameTimer = 0.0f;
int enemiesKilled = 0;

ParticleSystem globalParticles;
std::vector<EnemyProjectile> enemyLasers;
std::vector<GLuint> bgFramesFase1;

void ResetGame()
{
    for (Inimigo *ini : listaInimigos)
    {
        delete ini;
    }
    listaInimigos.clear();
    enemyLasers.clear();

    gameTimer = 0.0f;
    enemiesKilled = 0;
    bossSpawned = false;
    spawnTimer = 220;
    spawnCocoTimer = 360;
    phase1Time = 0.0f;

    player.Reset();
}

void Init()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);

    menuUI.Init();
    player.Init();
    VirusAlfa::InicializarModelo(); // Inicializa o Shared Mesh dos Inimigos
    BacteriaCoco::InicializarModelo();
    LeukocyteCorrupto::InicializarModelo();

    // Carrega frames do cenário DEPOIS das UIs essenciais (pra não estourar VRAM)
    std::cout << "Carregando 120 frames de cenário... Aguarde!" << std::endl;
    for (int i = 1; i <= 120; i++)
    {
        char buf[256];
        sprintf(buf, "assets/textures/fundo-fase1_frames/frame_%03d.jpg", i);
        bgFramesFase1.push_back(Renderer::LoadTexture(buf));
    }
    std::cout << "Frames carregados com sucesso!" << std::endl;

    glEnable(GL_TEXTURE_2D);

    // Matriz de Projecao (Ortografica para 2D inicial)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1024.0, 0.0, 768.0);
    glMatrixMode(GL_MODELVIEW);

    if (audioManager.Init())
    {
        audioManager.SetMusicVolume(0.5f);
        audioManager.SetSFXVolume(0.5f);
        audioManager.PlayMusic("assets/audio/musica-tema.wav");
    }
}

void Display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (currentState == STATE_PLAYING || currentState == STATE_PAUSE)
    {
        // === RENDERIZA FUNDO ANIMADO (2D) ===
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0.0, 1024.0, 768.0, 0.0);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        if (!bgFramesFase1.empty())
        {
            int currentFrame = (int)(gameTimer * 8.0f) % 120; // 8 FPS super arrastado/lento
            Renderer::DrawTexture(bgFramesFase1[currentFrame], 0, 0, 1024, 768);

            // Película escura (Overlay) de 60% preto para não ofuscar o jogo 3D
            Renderer::DrawSemiTransparentRect(0, 1024, 0, 768, 0.0f, 0.0f, 0.0f, 0.6f);
        }

        glEnable(GL_DEPTH_TEST);
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        // === RENDERIZA MUNDO (3D) ===
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // Dinamica de Lentes: A Câmera 3 (Ikaruga) usa uma Lente Telescópica (FOV 15.0) para esmagar a perspectiva.
        // As demais câmeras usam a Lente de Ação Padrão (FOV 45.0) com imersão 3D pesada.
        if (cameraMode == 3)
        {
            // Viewport restrito para colocar barras pretas e esconder o wrap-around
            int viewWidth = 724; // Largura ajustada para coincidir com os limites de -250 e 250
            int viewX = (1024 - viewWidth) / 2;
            glViewport(viewX, 0, viewWidth, 768);
            gluPerspective(15.0, (GLdouble)viewWidth / 768.0, 1.0, 8000.0);
        }
        else
        {
            glViewport(0, 0, 1024, 768); // Restaura viewport padrão
            gluPerspective(45.0, 1024.0 / 768.0, 1.0, 8000.0);
        }

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // Sistema Multi-Câmeras
        switch (cameraMode)
        {
        case 1:
            // Camera inclinada a direita (Visão Cinematica Fixa no X=0)
            gluLookAt(80.0, player.GetY() + 150.0, player.GetZ() + 450.0,
                      0.0, player.GetY() + 150.0, player.GetZ(), 0.0, 1.0, 0.0);
            break;
        case 2:
            // Camera inclinada a esquerda (Visão Cinematica Fixa no X=0)
            gluLookAt(-80.0, player.GetY() + 150.0, player.GetZ() + 450.0,
                      0.0, player.GetY() + 150.0, player.GetZ(), 0.0, 1.0, 0.0);
            break;
        case 3:
            // Camera Lente Telescopio super alta (Y=1500), cravada no centro
            // O recuo extra no eixo Z (+550 no olho, -50 no alvo) serve para subir a nave na tela
            gluLookAt(0.0, player.GetY() + 1900.0, player.GetZ() + 450.0,
                      0.0, player.GetY(), player.GetZ() - 220.0, 0.0, 1.0, 0.0);
            break;
        case 4:
        {
            // Primeira Pessoa (FPP)
            // Em vez de usar gluLookAt (que tem bug de matriz no driver Mesa Linux ao rolar),
            // aplicamos a matriz de transformacao manualmente e invertida.
            float roll = player.GetRollAngle();

            // 1. Rolar a camera inversamente ao roll do player
            glRotatef(-roll, 0.0f, 0.0f, 1.0f);

            // 2. Transladar o mundo na direcao contraria ao player
            // Movemos a visao para frente do player e um pouco acima
            glTranslatef(-player.GetX(), -(player.GetY() + 20.0f), -(player.GetZ() - 35.0f));
            break;
        }
        }

        // Ativando teste de profundidade para o 3D funcionar perfeitamente
        glEnable(GL_DEPTH_TEST);

        // Desenha um grid verde estilo Tron no chao (Y = -20) para dar nocao de velocidade e profundidade
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glBegin(GL_LINES);
        glColor3f(0.0f, 0.5f, 0.0f);
        // Linhas correndo junto com a nave no Z
        for (int i = -1000; i <= 1000; i += 50)
        {
            glVertex3f(i, -20.0f, player.GetZ() - 1500.0f);
            glVertex3f(i, -20.0f, player.GetZ() + 1000.0f);

            float fixedZ = (int)(player.GetZ() / 50) * 50.0f; // Fixa as linhas horizontais na grade
            glVertex3f(-1000.0f, -20.0f, fixedZ + i);
            glVertex3f(1000.0f, -20.0f, fixedZ + i);
        }
        glEnd();

        player.Draw();

        // Desenha todos os inimigos vivos na arena
        for (Inimigo *ini : listaInimigos)
        {
            ini->Desenhar();
        }

        // Desenha tiros inimigos
        for (auto &laser : enemyLasers)
        {
            laser.Draw();
        }

        globalParticles.Draw();

        // Desativa depth test para desenhar o HUD Holografico por cima
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, 1024, 768); // Restaura Viewport antes do HUD
        player.DrawHUD();

        // --- HUD DO BOSS ---
        for (Inimigo *ini : listaInimigos)
        {
            if (ini->IsBoss())
            {
                LeukocyteCorrupto *boss = (LeukocyteCorrupto *)ini;
                float hpPercent = boss->GetCurrentHealth() / boss->GetMaxHealth();

                glMatrixMode(GL_PROJECTION);
                glPushMatrix();
                glLoadIdentity();
                gluOrtho2D(0, 1024, 1000, 0); // Mesmo setup da Player HUD (Y invertido, 0 topo)
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glLoadIdentity();

                // Texto do Boss
                glColor3f(1.0f, 0.2f, 0.2f);
                glRasterPos2f(512.0f - 80.0f, 150.0f);
                std::string bossName = "LEUKOCYTE CORRUPTO";
                for (char c : bossName)
                    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

                // Barra de Vida Fundo
                glColor3f(0.3f, 0.0f, 0.0f);
                glBegin(GL_QUADS);
                glVertex2f(200.0f, 160.0f);
                glVertex2f(824.0f, 160.0f);
                glVertex2f(824.0f, 180.0f);
                glVertex2f(200.0f, 180.0f);
                glEnd();

                // Barra de Vida Frente
                if (boss->GetPolarity() == 0)
                    glColor3f(0.0f, 0.5f, 1.0f); // Azul se estiver absorvendo azul
                else
                    glColor3f(1.0f, 0.1f, 0.1f); // Vermelho

                glBegin(GL_QUADS);
                glVertex2f(200.0f, 160.0f);
                glVertex2f(200.0f + (624.0f * hpPercent), 160.0f);
                glVertex2f(200.0f + (624.0f * hpPercent), 180.0f);
                glVertex2f(200.0f, 180.0f);
                glEnd();

                glMatrixMode(GL_PROJECTION);
                glPopMatrix();
                glMatrixMode(GL_MODELVIEW);
                glPopMatrix();

                break; // Só desenha 1 boss
            }
        }
    }

    // Renderiza as telas de Menu ou a janela de Pause sobreposta ao jogo
    if (currentState != STATE_PLAYING)
    {
        glViewport(0, 0, 1024, 768); // Restaura Viewport para as telas de menu
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, 1024.0, 0.0, 768.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        menuUI.Render(currentState);
    }

    glutSwapBuffers();
}

void MouseClick(int button, int state, int mouseX, int mouseY)
{
    int realMouseY = 768 - mouseY;
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            menuUI.HandleClick(mouseX, realMouseY, currentState);
        }
        else if (state == GLUT_UP)
        {
            menuUI.StopDrag();
        }
        glutPostRedisplay();
    }
}

void MouseDrag(int mouseX, int mouseY)
{
    int realMouseY = 768 - mouseY;
    menuUI.HandleDrag(mouseX, realMouseY, currentState);
    glutPostRedisplay();
}

void MouseMotion(int mouseX, int mouseY)
{
    int realMouseY = 768 - mouseY;
    if (currentState == STATE_MENU || currentState == STATE_SELECAO_FASE || currentState == STATE_RELATORIO || currentState == STATE_PAUSE || currentState == STATE_VICTORY || currentState == STATE_GAMEOVER)
    {
        menuUI.HandleHover(mouseX, realMouseY);
        glutPostRedisplay();
    }
}

void KeyboardDown(unsigned char key, int x, int y)
{
    if (key < 256)
        keyState[key] = true;

    // Sair do jogo com ESC
    if (key == 27)
    { // ESC
        if (currentState == STATE_PLAYING)
        {
            currentState = STATE_PAUSE;
        }
        else if (currentState == STATE_PAUSE)
        {
            currentState = STATE_PLAYING;
        }
    }

    // Troca de câmeras em tempo real
    if (key == '1')
        cameraMode = 1;
    if (key == '2')
        cameraMode = 2;
    if (key == '3')
        cameraMode = 3;
    if (key == '4')
        cameraMode = 4;

    // Troca de Polaridade da Nave
    if (key == 9 || key == 'c' || key == 'C')
        player.TogglePolarity(); // 9 = TAB

    // Disparo Duplo
    if (key == ' ')
        player.Shoot();

    // SURGE
    if (key == 'q' || key == 'Q')
        player.ActivateSurge();
}

void KeyboardUp(unsigned char key, int x, int y)
{
    keyState[key] = false;
}

void KeyboardSpecialDown(int key, int x, int y)
{
    // Retirada a intercepcao de SHIFT daqui pois o FreeGLUT ignora teclas modificadoras no Linux
}

void Timer(int value)
{
    if (currentState == STATE_PLAYING)
    {
        player.Update(keyState);

        phase1Time += 0.016f; // Atualiza o relógio da fase (60 fps = 0.016s por frame)
        gameTimer += 0.016f;

        // --- GATILHO DO BOSS ---
        // Nasce aos 300s (5 minutos) ou IMEDIATAMENTE se o jogador estiver muito bem de vida (HSP >= 90.0f)
        if (!bossSpawned && (phase1Time >= 300.0f || player.GetPatientHealth() >= 90.0f))
        {
            bossSpawned = true;
            listaInimigos.push_back(new LeukocyteCorrupto(0.0f, player.GetY(), player.GetZ() - 350.0f));

            // Destroi todos os outros inimigos menores para focar na Boss Fight
            for (Inimigo *ini : listaInimigos)
            {
                if (!ini->IsBoss())
                    ini->Destruir();
            }
        }

        // --- GERADOR DE VÍRUS ALFA (Enxame em Formação V) ---
        // Só continua nascendo se o Boss não tiver spawnado
        if (!bossSpawned)
        {
            // Intervalo para 240 frames (4 segundos) pois nascem 5 por vez
            spawnTimer++;
            if (spawnTimer >= 240)
            {
                spawnTimer = 0;
                // Sorteia o líder um pouco mais centralizado para os alas não saírem muito da arena
                float randomX = (rand() % 340) - 170.0f;
                float spawnZ = player.GetZ() - 800.0f;

                // Lider (Ponta do V)
                listaInimigos.push_back(new VirusAlfa(randomX, player.GetY(), spawnZ));
                // Alas Internos
                listaInimigos.push_back(new VirusAlfa(randomX - 45.0f, player.GetY(), spawnZ + 50.0f));
                listaInimigos.push_back(new VirusAlfa(randomX + 45.0f, player.GetY(), spawnZ + 50.0f));
                // Alas Externos
                listaInimigos.push_back(new VirusAlfa(randomX - 90.0f, player.GetY(), spawnZ + 100.0f));
                listaInimigos.push_back(new VirusAlfa(randomX + 90.0f, player.GetY(), spawnZ + 100.0f));
            }
        }

        // --- GERADOR DE BACTÉRIA COCO (Tanque Vermelho) ---
        if (!bossSpawned)
        {
            // Nasce menos frequentemente que o Alfa (a cada ~8 segundos)
            spawnCocoTimer++;
            if (spawnCocoTimer >= 480)
            {
                spawnCocoTimer = 0;
                float randomX = (rand() % 340) - 170.0f;
                float spawnZ = player.GetZ() - 800.0f;
                listaInimigos.push_back(new BacteriaCoco(randomX, player.GetY(), spawnZ));
            }
        }

        // --- ATUALIZADOR FÍSICO DO ENXAME E COLETA DE LIXO ---
        for (auto it = listaInimigos.begin(); it != listaInimigos.end();)
        {
            Inimigo *ini = *it;
            ini->Atualizar(0.016f, player);

            // Regra da Fuga: Se o inimigo passou nas costas do jogador
            if (ini->GetZ() > player.GetZ() + 100.0f)
            {
                player.DamagePatient(3.0f); // Punição!
                ini->Destruir();
            }

            // Remove da memória se estiver morto
            if (!ini->IsAtivo())
            {
                if (ini->IsBoss())
                {
                    currentState = STATE_VICTORY;
                }
                delete ini;
                it = listaInimigos.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Atualiza tiros inimigos
        for (auto it = enemyLasers.begin(); it != enemyLasers.end();)
        {
            it->Update();
            if (!it->active)
            {
                it = enemyLasers.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // --- MOTOR DE COLISÕES ---
        Colisao::ChecarColisaoPlayer(player, listaInimigos);
        Colisao::ChecarLaserInimigo(player, listaInimigos);
        Colisao::ChecarLaserInimigoNoPlayer(player, enemyLasers);
        Colisao::ChecarSurgeInimigo(player, listaInimigos, 0.016f);

        globalParticles.Update(0.016f);

        // --- CHECAGEM DE DERROTA ---
        if (player.GetCurrentHealth() <= 0.0f || player.GetPatientHealth() <= 0.0f)
        {
            currentState = STATE_GAMEOVER;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Imunidade: A Guerra Celular");

    Init();

    glutDisplayFunc(Display);
    glutMouseFunc(MouseClick);
    glutMotionFunc(MouseDrag);
    glutPassiveMotionFunc(MouseMotion);
    glutKeyboardFunc(KeyboardDown);
    glutKeyboardUpFunc(KeyboardUp);
    glutSpecialFunc(KeyboardSpecialDown);
    glutTimerFunc(16, Timer, 0);

    glutMainLoop();
    return 0;
}
