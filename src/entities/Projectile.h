#pragma once
#include <GL/freeglut.h>

class Projectile {
public:
    float x, y, z;
    float speed;
    int polarity; // 0 = Azul, 1 = Vermelho
    bool active;

    Projectile(float startX, float startY, float startZ, int pol);
    void Update();
    void Draw();
};
