#define _USE_MATH_DEFINES
#include "../bibliotecas/renderer.h"
#include <cmath>
#include <iostream>

// --- ESTA LINHA É A DEFINIÇÃO DA BIBLIOTECA ---
// Ela avisa ao compilador para transformar o .h abaixo em código real compilável!
#define STB_IMAGE_IMPLEMENTATION
#include "../bibliotecas/stb_image.h"

Renderer::Renderer() : cameraX(0.0f), cameraY(0.0f), cameraZ(15.0f), targetX(0.0f), targetY(0.0f), targetZ(0.0f) {}
Renderer::~Renderer() {}

void Renderer::InicializarGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void Renderer::ConfigurarCamera(int width, int height)
{
    if (height == 0)
        height = 1;
    float aspecto = (float)width / (float)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0f, aspecto, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0.0f, 1.0f, 0.0f);
}

void Renderer::AtualizarIluminacaoDinamica(Polaridade polaridade, float playerX, float playerY)
{
    GLfloat luzAmbiente[] = {0.15f, 0.15f, 0.15f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbiente);

    GLfloat posicaoLuz[] = {playerX, playerY, 1.5f, 1.0f};
    GLfloat corLuz[4];

    if (polaridade == AZUL)
    {
        corLuz[0] = 0.0f;
        corLuz[1] = 0.5f;
        corLuz[2] = 1.0f;
        corLuz[3] = 1.0f;
    }
    else
    {
        corLuz[0] = 1.0f;
        corLuz[1] = 0.1f;
        corLuz[2] = 0.1f;
        corLuz[3] = 1.0f;
    }

    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, corLuz);
    glLightfv(GL_LIGHT0, GL_SPECULAR, corLuz);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.02f);
}

unsigned int Renderer::CarregarTextura(const char *caminhoArquivo)
{
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char *data = stbi_load(caminhoArquivo, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum formato = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        // Envia os dados da imagem para a GPU
        glTexImage2D(GL_TEXTURE_2D, 0, formato, width, height, 0, formato, GL_UNSIGNED_BYTE, data);

        // --- ALTERAÇÃO AQUI: Mudamos de GL_LINEAR_MIPMAP_LINEAR para GL_LINEAR ---
        // Isso remove a necessidade do glGenerateMipmap e usa filtragem bilinear linear clássica,
        // que funciona nativamente em qualquer versão do OpenGL/Windows sem erros!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Também configuramos o modo de repetição padrão para evitar artefatos nas bordas
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        std::cout << "Falha Critica: Textura nao pode ser carregada em: " << caminhoArquivo << std::endl;
    }
    stbi_image_free(data);
    return texID;
}

void Renderer::DesenharCuboGouraud(float tamanho)
{
    float h = tamanho / 2.0f;
    glBegin(GL_TRIANGLES);

    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-h, -h, h);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(h, -h, h);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(h, h, h);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-h, -h, h);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(h, h, h);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-h, h, h);

    // Lados esquerdos e traseiros simplificados para garantir estabilidade da malha
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-h, -h, -h);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-h, h, -h);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(h, h, -h);

    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-h, -h, -h);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(h, h, -h);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(h, -h, -h);
    glEnd();
}

void Renderer::DesenharEsferaGouraud(float raio, int subdivisoes)
{
    for (int i = 0; i < subdivisoes; ++i)
    {
        float lat0 = M_PI * (-0.5f + (float)(i) / subdivisoes);
        float z0 = raio * sin(lat0);
        float zr0 = raio * cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)(i + 1) / subdivisoes);
        float z1 = raio * sin(lat1);
        float zr1 = raio * cos(lat1);

        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= subdivisoes; ++j)
        {
            float lng = 2.0f * M_PI * (float)(j - 1) / subdivisoes;
            float x = cos(lng);
            float y = sin(lng);

            glNormal3f(x * cos(lat1), y * cos(lat1), sin(lat1));
            glVertex3f(x * zr1, y * zr1, z1);

            glNormal3f(x * cos(lat0), y * cos(lat0), sin(lat0));
            glVertex3f(x * zr0, y * zr0, z0);
        }
        glEnd();
    }
}

void Renderer::RenderizarTextoHUD(float x, float y, const char *texto, void *fonte)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    while (*texto)
    {
        glutBitmapCharacter(fonte, *texto);
        texto++;
    }
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}