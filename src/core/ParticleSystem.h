#pragma once
#include <vector>

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;     // 1.0 down to 0.0
    float maxLife;
    int polarity;   // 0 = Azul, 1 = Vermelho
};

class ParticleSystem {
private:
    std::vector<Particle> particles;

public:
    ParticleSystem();
    void Update(float dt);
    void Draw();
    
    // Gerador de explosao ao redor de um ponto
    void SpawnExplosion(float x, float y, float z, int polarity, int count = 20);
};
