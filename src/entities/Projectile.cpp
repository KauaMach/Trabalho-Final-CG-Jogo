#include "Projectile.h"

Projectile::Projectile(float startX, float startY, float startZ, int pol) {
    x = startX;
    y = startY;
    z = startZ;
    polarity = pol;
    speed = 25.0f; // Laser ultra rapido
    active = true;
}

void Projectile::Update() {
    if (!active) return;
    z -= speed; // Viaja pelo eixo Z rumo ao fundo da tela
    if (z < -3000.0f) active = false; // Desativa para economizar memoria
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
