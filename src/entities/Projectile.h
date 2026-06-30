#pragma once
#include <GL/freeglut.h>

class Projectile {
public:
    float x, y, z;
    float speed;
    int polarity; // 0 = Azul, 1 = Vermelho
    bool active;
    float distanceTraveled;

    Projectile(float startX, float startY, float startZ, int pol);
    void Update();
    void Draw();
};

class EnemyProjectile {
public:
    float x, y, z;
    float speedZ;
    float speedX;
    int polarity; // 1 = Vermelho para Bacteria Coco
    bool active;
    float distanceTraveled;
    
    EnemyProjectile(float startX, float startY, float startZ, int pol);
    // Construtor estendido para tiros em múltiplas direções
    EnemyProjectile(float startX, float startY, float startZ, float dirX, float dirY, float dirZ, float spd, int pol);
    
    void Update();
    void Draw();
};
