#include "ParticleSystem.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdlib>
#include <algorithm>

ParticleSystem::ParticleSystem() {}

void ParticleSystem::Update(float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
        p.life -= dt;
    }

    // Remover particulas mortas
    particles.erase(std::remove_if(particles.begin(), particles.end(), 
        [](const Particle& p) { return p.life <= 0.0f; }), particles.end());
}

void ParticleSystem::Draw() {
    if (particles.empty()) return;

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING); // Particulas emitem luz, nao recebem luz
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Nao escreve no z-buffer para evitar recortes feios entre particulas

    glBegin(GL_QUADS);
    for (const auto& p : particles) {
        // Vida vai de maxLife ate 0, entao alpha vai de 1.0 ate 0.0
        float alpha = p.life / p.maxLife;
        if (alpha < 0.0f) alpha = 0.0f;

        if (p.polarity == 0) { // Azul / Ciano
            glColor4f(0.0f, 0.8f, 1.0f, alpha);
        } else { // Vermelho
            glColor4f(1.0f, 0.2f, 0.2f, alpha);
        }

        float size = 2.0f; // Tamanho do fragmento

        // Para performance, desenhamos um quadradinho virado pra frente 
        // (Billboarding muito basico)
        glVertex3f(p.x - size, p.y - size, p.z);
        glVertex3f(p.x + size, p.y - size, p.z);
        glVertex3f(p.x + size, p.y + size, p.z);
        glVertex3f(p.x - size, p.y + size, p.z);
    }
    glEnd();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void ParticleSystem::SpawnExplosion(float x, float y, float z, int polarity, int count) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.x = x;
        p.y = y;
        p.z = z;
        
        // Velocidades aleatorias esfericas
        // Rand entre -150 e 150 para x e y
        p.vx = ((rand() % 300) - 150.0f);
        p.vy = ((rand() % 300) - 150.0f);
        // Z pode ir pra frente ou pra tras (-150 a 150)
        p.vz = ((rand() % 300) - 150.0f);
        
        p.maxLife = 0.5f + ((rand() % 50) / 100.0f); // Entre 0.5 e 1.0 segundos
        p.life = p.maxLife;
        p.polarity = polarity;
        
        particles.push_back(p);
    }
}
