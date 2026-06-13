#ifndef PARTICLES_H
#define PARTICLES_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

// Estrutura que define as propriedades físicas e visuais de cada partícula
struct Particula
{
    float x, y, z;
    float vx, vy, vz;
    float r, g, b, alpha;
    float vida, decaimento;
    bool ativa;
};

// Definição do tamanho máximo do pool em memória
const int MAX_PARTICULAS = 1000;

// Assinaturas das funções globais do sistema de partículas
void InicializarSistemaParticulas();
void EmitirExplosao(float px, float py, float r, float g, float b);
void AtualizarERenderizarParticulas();

#endif // PARTICLES_H