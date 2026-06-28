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
    GLuint displayListID;
    
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
    float patientHealth;
    float maxPatientHealth;
    bool bossSpawned;
    
    // Sistema de Polaridade Cromatica
    int polarity; // 0 = Azul, 1 = Vermelho
    
    std::vector<Projectile> lasers;
    
    std::vector<Vertex> vertices;
    
    bool LoadModel(const std::string& objPath);

    int score;
    int combo;

public:
    Player();
    void Init();
    void Reset();
    void Update(bool* keys);
    void Draw();
    void DrawHUD();
    void Shoot();
    void ActivateSurge();
    
    void AddScore(int pts) { score += (pts * combo); combo++; }
    void ResetCombo() { combo = 1; }
    
    int GetScore() const { return score; }
    int GetCombo() const { return combo; }
    
    void TogglePolarity() { polarity = 1 - polarity; }
    
    float GetX() const { return posX; }
    float GetY() const { return posY; }
    float GetZ() const { return posZ; }
    float GetRollAngle() const { return rollAngle; }
    
    // Getters para a engine de colisao
    std::vector<Projectile>& GetLasers() { return lasers; }
    bool IsSurgeActive() const { return isSurgeActive; }
    int GetSurgePolarity() const { return surgePolarity; }
    float GetSurgeRadius() const { return surgeRadius; }
    
    // Novas funcoes de dano e HSP
    float GetPatientHealth() const { return patientHealth; }
    float GetCurrentHealth() const { return health; }
    void AddSurgeEnergy(float v) { surgeEnergy += v; if(surgeEnergy > maxSurge) surgeEnergy = maxSurge; }
    void AddPatientHealth(float v) { patientHealth += v; if(patientHealth > maxPatientHealth) patientHealth = maxPatientHealth; }
    void DamagePatient(float v) { patientHealth -= v; if(patientHealth < 0) patientHealth = 0; }
    void DamageNanocell(float v) { health -= v; if(health < 0) health = 0; }
    void HealNanocell(float v) { health += v; if(health > maxHealth) health = maxHealth; }
    bool IsDead() const { return health <= 0.0f; }
    bool IsBossReady() const { return (patientHealth >= 90.0f && !bossSpawned); }
    void SpawnBoss() { bossSpawned = true; }
};
