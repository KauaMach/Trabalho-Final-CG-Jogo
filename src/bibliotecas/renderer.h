#ifndef RENDERER_H
#define RENDERER_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// Definição de vetores simples para evitar dependências pesadas externas na matemática básica
struct Vector3
{
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

enum Polaridade
{
    AZUL,
    VERMELHA
};

class Renderer
{
    private:
        unsigned int texturasID[6]; 
        float cameraX, cameraY, cameraZ;
        float targetX, targetY, targetZ;

    public:
        Renderer();
        ~Renderer();

        void InicializarGL();
        void ConfigurarCamera(int width, int height);
        void AtualizarIluminacaoDinamica(Polaridade polaridade, float playerX, float playerY);
        unsigned int CarregarTextura(const char *caminhoArquivo);

        void RenderizarTextoHUD(float x, float y, const char *texto, void *fonte);
        void DesenharEsferaGouraud(float raio, int subdivisoes);
        void InicializarTexturaEstado(int indiceEstado, const char *caminhoImagem);

        void RenderizarTelaEstado(int indiceEstado);

        void DesenharVirus1(float tamanho);
        void DesenharVirus2(float tamanho);
        void DesenharVirus3(float tamanho);
        void DesenharVirus4(float tamanho);
        void DesenharLeukocito(float tamanho);
};

#endif