#include <iostream>
#include <cmath>
#include "core/Renderer.h"
#include "scenes/MenuUI.h"
#include "core/AudioManager.h"
#include "entities/Player.h"

GameState currentState = STATE_MENU;
MenuUI menuUI;
AudioManager audioManager;
Player player;
bool keyState[256] = {false};
int cameraMode = 1; // 1 = Dir, 2 = Esq, 3 = Top, 4 = FPP

void Init() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    
    // Matriz de Projecao (Ortografica para 2D)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 1024.0, 0.0, 768.0);
    glMatrixMode(GL_MODELVIEW);

    if (audioManager.Init()) {
        audioManager.SetMusicVolume(0.5f);
        audioManager.SetSFXVolume(0.5f);
        audioManager.PlayMusic("assets/audio/musica-tema.wav");
    }

    menuUI.Init();
    player.Init();
}

void Display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (currentState == STATE_PLAYING || currentState == STATE_PAUSE) {
        // Configuracao da camera 2.5D (Perspectiva com angulo inclinado)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 1024.0 / 768.0, 1.0, 2000.0);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        // Sistema Multi-Câmeras
        switch(cameraMode) {
            case 1:
                // Camera deslocada para a direita (X = 80) e apontada para o centro, revelando a lateral esquerda da nave
                gluLookAt(80.0, player.GetY() + 150.0, player.GetZ() + 250.0,
                          0.0, player.GetY(), player.GetZ(), 0.0, 1.0, 0.0);
                break;
            case 2:
                // Camera deslocada para a esquerda (X = -80)
                gluLookAt(-80.0, player.GetY() + 150.0, player.GetZ() + 250.0,
                          0.0, player.GetY(), player.GetZ(), 0.0, 1.0, 0.0);
                break;
            case 3:
                // Top-Down (visão superior clássica)
                gluLookAt(0.0, player.GetY() + 400.0, player.GetZ() + 50.0,
                          0.0, player.GetY(), player.GetZ() - 100.0, 0.0, 1.0, 0.0);
                break;
            case 4: {
                // Primeira Pessoa (FPP) - Camera projetada à frente do bico rolando junto com a nave
                float theta = player.GetRollAngle() * 3.14159f / 180.0f;
                float upX = -sin(theta);
                float upY = cos(theta);
                gluLookAt(player.GetX(), player.GetY() + 20.0, player.GetZ() - 35.0,
                          player.GetX(), player.GetY() + 0.0, player.GetZ() - 200.0, 
                          upX, upY, 0.0);
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
        for (int i = -1000; i <= 1000; i += 50) {
            glVertex3f(i, -20.0f, player.GetZ() - 1500.0f);
            glVertex3f(i, -20.0f, player.GetZ() + 1000.0f);
            
            float fixedZ = (int)(player.GetZ() / 50) * 50.0f; // Fixa as linhas horizontais na grade
            glVertex3f(-1000.0f, -20.0f, fixedZ + i);
            glVertex3f(1000.0f, -20.0f, fixedZ + i);
        }
        glEnd();
        
        player.Draw();
        
        // Desativa depth test para desenhar o HUD Holografico por cima
        glDisable(GL_DEPTH_TEST);
        player.DrawHUD();
    } 
    
    // Renderiza as telas de Menu ou a janela de Pause sobreposta ao jogo
    if (currentState != STATE_PLAYING) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluOrtho2D(0.0, 1024.0, 0.0, 768.0);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        menuUI.Render(currentState);
    }

    glutSwapBuffers();
}

void MouseClick(int button, int state, int mouseX, int mouseY) {
    int realMouseY = 768 - mouseY;
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            menuUI.HandleClick(mouseX, realMouseY, currentState);
        } else if (state == GLUT_UP) {
            menuUI.StopDrag();
        }
        glutPostRedisplay();
    }
}

void MouseDrag(int mouseX, int mouseY) {
    int realMouseY = 768 - mouseY;
    menuUI.HandleDrag(mouseX, realMouseY, currentState);
    glutPostRedisplay();
}

void MouseMotion(int mouseX, int mouseY) {
    int realMouseY = 768 - mouseY;
    if (currentState == STATE_MENU || currentState == STATE_SELECAO_FASE || currentState == STATE_RELATORIO || currentState == STATE_PAUSE) {
        menuUI.HandleHover(mouseX, realMouseY);
        glutPostRedisplay();
    }
}

void KeyboardDown(unsigned char key, int x, int y) {
    if (key < 256) keyState[key] = true;
    
    // Sair do jogo com ESC
    if (key == 27) { // ESC
        if (currentState == STATE_PLAYING) {
            currentState = STATE_PAUSE;
        } else if (currentState == STATE_PAUSE) {
            currentState = STATE_PLAYING;
        }
    }
    
    // Troca de câmeras em tempo real
    if (key == '1') cameraMode = 1;
    if (key == '2') cameraMode = 2;
    if (key == '3') cameraMode = 3;
    if (key == '4') cameraMode = 4;
    
    // Troca de Polaridade da Nave
    if (key == 9 || key == 'c' || key == 'C') player.TogglePolarity(); // 9 = TAB
    
    // Disparo Duplo
    if (key == ' ') player.Shoot();
    
    // SURGE
    if (key == 'q' || key == 'Q') player.ActivateSurge();
}

void KeyboardUp(unsigned char key, int x, int y) {
    keyState[key] = false;
}

void KeyboardSpecialDown(int key, int x, int y) {
    // Retirada a intercepcao de SHIFT daqui pois o FreeGLUT ignora teclas modificadoras no Linux
}

void Timer(int value) {
    if (currentState == STATE_PLAYING) {
        player.Update(keyState);
    }
    glutPostRedisplay();
    glutTimerFunc(16, Timer, 0);
}

int main(int argc, char** argv) {
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
