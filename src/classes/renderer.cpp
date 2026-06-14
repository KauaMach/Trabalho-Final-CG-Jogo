#define _USE_MATH_DEFINES
#include "../bibliotecas/renderer.h"
#include <cmath>
#include <iostream>

// Ativação única da biblioteca stb_image para carregar as matrizes de pixels
#define STB_IMAGE_IMPLEMENTATION
#include "../bibliotecas/stb_image.h"

Renderer::Renderer() : cameraX(0.0f), cameraY(0.0f), cameraZ(15.0f), targetX(0.0f), targetY(0.0f), targetZ(0.0f) {
    // Inicializa o array de texturas com zero para segurança
    for (int i = 0; i < 4; i++) {
        texturasID[i] = 0;
    }
}

Renderer::~Renderer() {}

void Renderer::InicializarGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fundo preto celular
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // ====================================================
    // --- CONFIGURAÇÃO OTIMIZADA DE ILUMINAÇÃO (3D) ---
    // ====================================================
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

void Renderer::AtualizarIluminacaoDinamica(Polaridade polaridade, float playerX, float playerY) {
    // Certifique-se de que a sua luz está configurada com valores altos (1.0f) na difusa e especular:
    float luzAmbiente[] = {0.2f, 0.2f, 0.2f, 1.0f};
    float luzDifusa[] = {1.0f, 1.0f, 1.0f, 1.0f};    // Branco total
    float luzEspecular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // Branco total para criar o ponto de brilho

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular); // Brilho nos reflexos das curvas

    // 2. Posicionamento da Luz: Ligeiramente acima e à frente do ecrã (Efeito Holofote)
    // O quarto valor sendo 1.0f transforma a luz numa fonte posicional (Direct / Point Light)
    float posicaoLuz[] = {0.0f, 5.0f, 4.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);

    // 3. Opcional: Configura o "brilho" do material plástico/celular dos vírus
    float especularMaterial[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, especularMaterial);
    glMateriali(GL_FRONT, GL_SHININESS, 64);

    // 4. Opcional: Configura o "brilho" do material plástico/celular do jogador
    float especularMaterialPlayer[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, especularMaterialPlayer);
    glMateriali(GL_FRONT, GL_SHININESS, 64);
}

unsigned int Renderer::CarregarTextura(const char* caminhoArquivo) {
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Correção de orientação vertical de cabeça para baixo
    
    unsigned char *data = stbi_load(caminhoArquivo, &width, &height, &nrChannels, 0);
    
    if (data) {
        GLenum formato = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, formato, width, height, 0, formato, GL_UNSIGNED_BYTE, data);
        
        // Filtros bilineares clássicos recomendados para compatibilidade e performance
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    } else {
        std::cout << "Erro: Arquivo de imagem nao encontrado em: " << caminhoArquivo << std::endl;
    }
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