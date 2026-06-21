#ifndef AI_BOIDS_H
#define AI_BOIDS_H

#include <vector>

struct Boid
{
    float x, y;
    float vx, vy;
    float ax, ay;
    int id;
    bool ativo;
};

class BoidsEngine
{
private:
    float maxVelocidade;
    float maxForca;

    float pesoSeparacao = 2.5f;
    float pesoAlinhamento = 1.0f;
    float pesoCoesao = 1.0f;

public:
    BoidsEngine(float maxVel, float maxFrc);
    void ProcessarEnxame(std::vector<Boid> &enxame, float alvoX, float alvoY, bool jogadorEmSurge);

private:
    void AplicarForcaProcura(Boid &boid, float tx, float ty, float peso);
    void AplicarForcaFuga(Boid &boid, float tx, float ty, float peso);
};

#endif