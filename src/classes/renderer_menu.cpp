#include "../bibliotecas/renderer.h"

void Renderer::InicializarTexturaEstado(int indiceEstado, const char *caminhoImagem)
{
    if (indiceEstado >= 0 && indiceEstado < 5)
    {
        texturasID[indiceEstado] = CarregarTextura(caminhoImagem); // Mapeia na memória [cite: 672, 673]
    }
}

// 2. Renderiza qualquer tela usando uma única lógica reaproveitável
void Renderer::RenderizarTelaEstado(int indiceEstado)
{
    if (indiceEstado < 0 || indiceEstado >= 5 || texturasID[indiceEstado] == 0)
        return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1024, 0, 768); // Projeção ortogonal estável para o plano 2D

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturasID[indiceEstado]); // Ativa a textura do estado [cite: 674, 690]

    // Aplica a textura no objeto 2D conforme o modelo de aula [cite: 677, 679]
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