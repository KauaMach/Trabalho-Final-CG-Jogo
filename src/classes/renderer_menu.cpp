#include "../bibliotecas/renderer.h"

/**
 * @brief Inicializa uma textura para um estado específico
 * @param indiceEstado Índice do estado para o qual inicializar a textura
 * @param caminhoImagem Caminho para o arquivo de imagem da textura
 */
void Renderer::InicializarTexturaEstado(int indiceEstado, const char *caminhoImagem)
{
    if (indiceEstado >= 0 && indiceEstado < 7)
        texturasID[indiceEstado] = CarregarTextura(caminhoImagem);
    
}

/**
 * @brief Renderiza a tela correspondente ao estado atual
 * @param indiceEstado Índice do estado a ser renderizado
 */
void Renderer::RenderizarTelaEstado(int indiceEstado)
{
    if (indiceEstado < 0 || indiceEstado >= 7 || texturasID[indiceEstado] == 0)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1024, 0, 768); 

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturasID[indiceEstado]);

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