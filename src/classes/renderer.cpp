#define _USE_MATH_DEFINES
#include "../bibliotecas/renderer.h"
#include <cmath>
#include <iostream>

// Ativação única da biblioteca stb_image para carregar as matrizes de pixels
#define STB_IMAGE_IMPLEMENTATION
#include "../bibliotecas/stb_image.h"

Renderer::Renderer() : cameraX(0.0f), cameraY(0.0f), cameraZ(15.0f), targetX(0.0f), targetY(0.0f), targetZ(0.0f) {
    // Inicializa o array de texturas com zero para segurança
    for (int i = 0; i < 6; i++) {
        texturasID[i] = 0;
    }
}

Renderer::~Renderer() {}

void Renderer::InicializarGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fundo preto celular
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHTING); // Ativa o motor de iluminação
    glEnable(GL_LIGHT0);   // Ativa a primeira fonte de luz

    // Força o OpenGL a corrigir as normais após aplicar a escala (Evita que os modelos fiquem escuros)
    glEnable(GL_NORMALIZE);

    // Ativa o rastreio de cor para que o glColor3f funcione perfeitamente com a luz
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    // Configura uma luz ambiente global suave (para não termos sombras pretas absolutas)
    float luzAmbienteGlobal[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, luzAmbienteGlobal); // Permite que glColor3f funcione com luz

    float luzBranca[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzBranca); // Garante que a luz pode criar reflexos brancos
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void Renderer::ConfigurarCamera(int width, int height) {
    if (height == 0) height = 1;
    float aspecto = (float)width / (float)height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    gluPerspective(45.0f, aspecto, 0.1f, 100.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, targetX, targetY, targetZ, 0.0f, 1.0f, 0.0f);
}

void Renderer::AtualizarIluminacaoDinamica(float playerX, float playerY)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Luz branca neutra (biológica/limpa)
    float luzDifusa[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float luzEspecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float luzAmbiente[] = {0.3f, 0.3f, 0.3f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);

    // A luz segue a posição do player (efeito holofote)
    float posicaoLuz[] = {playerX, playerY, 2.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

unsigned int Renderer::CarregarTextura(const char* caminhoArquivo) {
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Correção de orientação vertical de cabeça para baixo
    
    unsigned char *data = stbi_load(caminhoArquivo, &width, &height, &nrChannels, 0);
    if (!data) {
        std::cout << "Erro: Arquivo de imagem nao encontrado em: " << caminhoArquivo << std::endl;
        return 0;
    }

    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    GLenum formato = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, formato, width, height, 0, formato, GL_UNSIGNED_BYTE, data);
    
    // Filtros bilineares clássicos recomendados para compatibilidade e performance
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(data);
    return texID;
}

void Renderer::RenderizarTextoHUD(float x, float y, const char* texto, void* fonte) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING);
    glRasterPos2f(x, y);
    while (*texto) {
        glutBitmapCharacter(fonte, *texto);
        texto++;
    }
    glEnable(GL_LIGHTING);
    glPopMatrix(); glMatrixMode(GL_PROJECTION);
    glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

void Renderer::CompilarModelos()
{
    for (int i = 0; i < 5; i++)
    {
        displayLists[i] = glGenLists(1);
        glNewList(displayLists[i], GL_COMPILE);

        switch (i)
        {
        case 0:
            DesenharLeukocito(4.0f); 
            break;
        case 1:
            DesenharVirus1(3.5f);
            break;
        case 2: 
            DesenharVirus2(3.5f);
            break;
        case 3:
            DesenharVirus3(3.5f);
            break;
        case 4:
            DesenharVirus4(3.5f);
            break;
        }

        glEndList();
    }
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