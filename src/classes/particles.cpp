#define _USE_MATH_DEFINES
#include "../bibliotecas/particles.h"
#include <cstdlib>
#include <cmath>

// Alocação estática do pool na memória para evitar fragmentação e gargalos de malloc/new em tempo de execução
Particula poolParticulas[MAX_PARTICULAS];

void InicializarSistemaParticulas()
{
    for (int i = 0; i < MAX_PARTICULAS; ++i)
    {
        poolParticulas[i].ativa = false;
    }
}

void EmitirExplosao(float px, float py, float r, float g, float b)
{
    int emitidas = 0;
    // Limita a emissão a um bloco de 60 partículas por explosão para balanceamento de performance
    for (int i = 0; i < MAX_PARTICULAS && emitidas < 60; ++i)
    {
        if (!poolParticulas[i].ativa)
        {
            poolParticulas[i].ativa = true;
            poolParticulas[i].x = px;
            poolParticulas[i].y = py;
            poolParticulas[i].z = 0.0f;

            // Distribuição vetorial trigonométrica para espalhar as partículas em um raio 3D projetado
            float angulo = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
            float vel = static_cast<float>(rand()) / RAND_MAX * 0.15f + 0.05f;

            poolParticulas[i].vx = cos(angulo) * vel;
            poolParticulas[i].vy = sin(angulo) * vel;
            poolParticulas[i].vz = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 0.05f;

            // Atribuição de cores e controle de tempo de vida (fade-out)
            poolParticulas[i].r = r;
            poolParticulas[i].g = g;
            poolParticulas[i].b = b;
            poolParticulas[i].alpha = 1.0f;
            poolParticulas[i].vida = 1.0f;
            poolParticulas[i].decaimento = static_cast<float>(rand()) / RAND_MAX * 0.04f + 0.01f;

            emitidas++;
        }
    }
}

void AtualizarERenderizarParticulas()
{
    // Desabilita a iluminação para que as partículas tenham "luz própria" (efeito de brilho/glow)
    glDisable(GL_LIGHTING);

    glBegin(GL_POINTS);
    for (int i = 0; i < MAX_PARTICULAS; ++i)
    {
        if (poolParticulas[i].ativa)
        {
            // Integração de Euler simples para atualizar posições baseadas na velocidade
            poolParticulas[i].x += poolParticulas[i].vx;
            poolParticulas[i].y += poolParticulas[i].vy;
            poolParticulas[i].z += poolParticulas[i].vz;

            // Consome a vida da partícula e aplica transparência proporcional
            poolParticulas[i].vida -= poolParticulas[i].decaimento;
            poolParticulas[i].alpha = poolParticulas[i].vida;

            // Se a vida acabar, libera o espaço no pool para reuso imediato
            if (poolParticulas[i].vida <= 0.0f)
            {
                poolParticulas[i].ativa = false;
                continue;
            }

            // Envia o vértice processado com canal alpha ativo para o pipeline do OpenGL
            glColor4f(poolParticulas[i].r, poolParticulas[i].g, poolParticulas[i].b, poolParticulas[i].alpha);
            glVertex3f(poolParticulas[i].x, poolParticulas[i].y, poolParticulas[i].z);
        }
    }
    glEnd();

    // Reativa a iluminação padrão do cenário para as demais entidades do jogo
    glEnable(GL_LIGHTING);
}