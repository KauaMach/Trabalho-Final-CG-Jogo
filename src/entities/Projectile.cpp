#include "Projectile.h"
#include <iostream>
#include <cmath>

Projectile::Projectile(float startX, float startY, float startZ, int pol) {
    x = startX;
    y = startY;
    z = startZ;
    polarity = pol;
    speed = 25.0f; // Laser ultra rapido
    active = true;
    distanceTraveled = 0.0f;
}

void Projectile::Update() {
    if (!active) return;
    z -= speed; // Viaja pelo eixo Z rumo ao fundo da tela
    distanceTraveled += speed;
    
    // Agora o laser morre apenas se voar 3500 metros (vai ate o fim da tela visivel)
    if (distanceTraveled > 3500.0f) active = false; 
}

void Projectile::Draw() {
    if (!active) return;
    
    glPushMatrix();
    glTranslatef(x, y, z);
    
    // Forma do Laser: Esmagado nos lados e esticado na ponta
    glScalef(0.5f, 0.5f, 6.0f);
    
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    
    // Efeito de energia Neon (Transparente e Brilhante)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    if (polarity == 0) { // Azul / Ciano
        glColor4f(0.0f, 0.8f, 1.0f, 0.8f);
    } else { // Vermelho Rubi
        glColor4f(1.0f, 0.2f, 0.2f, 0.8f);
    }
    
    glutSolidSphere(2.0, 12, 12); // Bolinha que sera esticada
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glPopMatrix();
}

EnemyProjectile::EnemyProjectile(float startX, float startY, float startZ, int pol) {
    x = startX;
    y = startY;
    z = startZ;
    speedZ = 120.0f; // velocidade fixa original
    speedX = 0.0f;
    polarity = pol;
    active = true;
    distanceTraveled = 0.0f;
}

EnemyProjectile::EnemyProjectile(float startX, float startY, float startZ, float dirX, float dirY, float dirZ, float spd, int pol) {
    x = startX;
    y = startY;
    z = startZ;
    speedZ = dirZ * spd;
    speedX = dirX * spd;
    polarity = pol;
    active = true;
    distanceTraveled = 0.0f;
}

void EnemyProjectile::Update() {
    float moveZ = speedZ * 0.016f;
    float moveX = speedX * 0.016f;
    z += moveZ; 
    x += moveX;
    
    distanceTraveled += std::sqrt(moveZ * moveZ + moveX * moveX);
    if (distanceTraveled > 2500.0f) active = false; // Destroy if out of range
}

void EnemyProjectile::Draw() {
    if (!active) return;
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(1.5f, 1.5f, 1.5f); // Tiro redondo ou esférico
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING); // <-- Desliga a sombra pra cor brilhar pura
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    
    if (polarity == 0) { // Azul Neon
        glColor4f(0.0f, 0.8f, 1.0f, 1.0f);
    } else { // Vermelho Sangue Vívido
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    }
    
    glutSolidSphere(2.0, 8, 8);
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING); // <-- Restaura a luz do mundo
    
    glPopMatrix();
}
