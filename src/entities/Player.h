#pragma once
#include "core/Renderer.h"
#include <string>
#include <vector>
#include "Projectile.h"

struct Vertex {
    float x, y, z;
    float nx, ny, nz;
    float u, v;
};

class Player {
private:
    float posX, posY, posZ;
    float speed;
    GLuint textureID;
    
    // Mecanica de Evasao (Dash)
    bool isDashing;
    float dashTimer;
    float dashCooldown;
    float rollAngle;
    
    // Mecanica de Barrel Roll Puramente Estetica
    bool isRolling;
    float rollTimer;
    
    // Mecanica de SURGE
    bool isSurgeActive;
    float surgeTimer;
    int surgePolarity; 
    float surgeRadius; 
    
    // HUD Passivo e Status
    float health;
    float maxHealth;
    float surgeEnergy;
    float maxSurge;
    
    // Sistema de Polaridade Cromatica
    int polarity; // 0 = Azul, 1 = Vermelho
    
    std::vector<Projectile> lasers;
    
    std::vector<Vertex> vertices;
    
    bool LoadModel(const std::string& objPath);

public:
    Player();
    void Init();
    void Update(bool* keys);
    void Draw();
    void DrawHUD();
    void Shoot();
    void ActivateSurge();
    
    void TogglePolarity() { polarity = 1 - polarity; }
    
    float GetX() const { return posX; }
    float GetY() const { return posY; }
    float GetZ() const { return posZ; }
    float GetRollAngle() const { return rollAngle; }
};
