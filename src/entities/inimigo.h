#pragma once
#include <cmath>
#include "Player.h" // Traz a struct Vertex e a definicao do Player
#include <deque>

class Inimigo {
protected:
    float posX, posY, posZ;
    float velX, velZ;
    float health;
    int polarity; // 0 = Azul, 1 = Vermelho
    float raioColisao;
    float comprimentoZ; // Adicionado para hitbox cilindrica
    bool ativo;

public:
    Inimigo(float x, float y, float z, int pol) 
        : posX(x), posY(y), posZ(z), velX(0.0f), velZ(50.0f), 
          health(10.0f), polarity(pol), raioColisao(15.0f), comprimentoZ(15.0f), ativo(true) {}
          
    virtual ~Inimigo() {}

    virtual void Atualizar(float dt, const Player& player);
    virtual void Desenhar() = 0;
    virtual void Destruir() {
        ativo = false;
        // Adicionar som de explosao e aumento de pontos no futuro
    }
    
    virtual bool IsBoss() const { return false; }

    // Getters para colisao e gerenciamento
    float GetX() const { return posX; }
    float GetY() const { return posY; }
    float GetZ() const { return posZ; }
    float GetRaio() const { return raioColisao; }
    float GetComprimentoZ() const { return comprimentoZ; }
    int GetPolarity() const { return polarity; }
    bool IsAtivo() const { return ativo; }
    
    virtual float GetCollisionDamageNanocell() const { return 10.0f; }
    virtual float GetCollisionDamagePatient() const { return 0.0f; }
    
    virtual void TomarDano(float dano) { 
        health -= dano; 
        if(health <= 0.0f) ativo = false; 
    }
};

// Vírus Alfa (Polaridade Azul)
// Regra: Fica longe e de repente mergulha em direcao ao jogador
class VirusAlfa : public Inimigo {
private:
    bool emMergulho;
    
    // Variaveis Estáticas para o Modelo 3D Otimizado (Shared Mesh)
    static std::vector<Vertex> alfaVertices;
    static GLuint alfaTextureID;
    static GLuint alfaDisplayListID;
    static bool alfaCarregado;

public:
    VirusAlfa(float x, float y, float z);
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    
    // Inicializador global chamado 1 vez no main.cpp
    static void InicializarModelo();
};

class BacteriaCoco : public Inimigo {
private:
    float cooldownTiro;

    float anguloRotacao;
    float velocidadeRotacao;
    static std::vector<Vertex> cocoVertices;
    static GLuint cocoTextureID;
    static GLuint cocoDisplayListID;
    static bool cocoCarregado;

public:
    BacteriaCoco(float x, float y, float z);
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    
    // Inicializador global chamado 1 vez no main.cpp
    static void InicializarModelo();
};

// ==============================================================================
// BOSS 1: LEUKOCYTE CORRUPTO
// ==============================================================================
class LeukocyteCorrupto : public Inimigo {
public:
    LeukocyteCorrupto(float x, float y, float z);
    
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    void Destruir() override;
    bool IsBoss() const override { return true; }
    
    float GetMaxHealth() const { return maxHealth; }
    float GetCurrentHealth() const { return health; }
    
    static void InicializarModelo();

private:
    float maxHealth;
    int bossState; // 0 = Patrulha, 1 = Espiral, 2 = Fúria
    float stateTimer;
    float attackCooldown;
    
    float anguloEspiral;
    
    static std::vector<Vertex> bossVertices;
    static GLuint bossTextureID;
    static GLuint bossDisplayListID;
    static bool bossCarregado;
};

// ==============================================================================
// FASE 2: INIMIGOS
// ==============================================================================

// Vírus Gama (Alterna polaridade)
class VirusGama : public Inimigo {
private:
    float timerBipolar;
    float fireTimer;

    static std::vector<Vertex> gamaVertices;
    static GLuint gamaTextureID;
    static GLuint gamaDisplayListID;
    static bool gamaCarregado;

public:
    VirusGama(float x, float y, float z);
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    
    static void InicializarModelo();
};

// Esporo Fúngico (Kamikaze)
class EsporoFungico : public Inimigo {
private:
    float anguloGiro;
    
    static std::vector<Vertex> esporoVertices;
    static GLuint esporoTextureID;
    static GLuint esporoDisplayListID;
    static bool esporoCarregado;

public:
    EsporoFungico(float x, float y, float z);
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    void Destruir() override;

    float GetCollisionDamageNanocell() const override { return 20.0f; }
    float GetCollisionDamagePatient() const override { return 5.0f; }

    static void InicializarModelo();
};

// BOSS 2: PNEUMOCOCO GIGANTE
class PneumococoGigante : public Inimigo {
public:
    PneumococoGigante(float x, float y, float z);
    
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    void Destruir() override;
    void TomarDano(float dano) override;
    bool IsBoss() const override { return true; }
    
    float GetMaxHealth() const { return maxHealth; }
    float GetCurrentHealth() const { return health; }
    
    static void InicializarModelo();

private:
    float maxHealth;
    float cicloInspiracao;
    bool isInspirando; 
    
    float attackTimer;
    float attackAngle;
    
    static std::vector<Vertex> pneuVertices;
    static GLuint pneuTextureID;
    static GLuint pneuDisplayListID;
    static bool pneuCarregado;
};

// FASE 3: PRÍON MIMÉTICO (Buffer Circular)
class PrionMimetico : public Inimigo {
public:
    PrionMimetico(float startX, float startY, float startZ);
    
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    void Destruir() override;

    float GetCollisionDamageNanocell() const override { return 15.0f; }
    float GetCollisionDamagePatient() const override { return 0.0f; }

    static void InicializarModelo();

private:
    std::deque<std::pair<float, float>> posHistory; // x, z
    int maxBufferSize;
    
    float attackTimer;
    float anguloGiro;
    
    static std::vector<Vertex> prionVertices;
    static GLuint prionTextureID;
    static GLuint prionDisplayListID;
    static bool prionCarregado;
};

// FASE 3: VÍRUS DELTA (Hacker Bipolar)
class VirusDelta : public Inimigo {
public:
    VirusDelta(float startY, float startZ, int pol);
    
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    void Destruir() override;

    float GetCollisionDamageNanocell() const override { return 15.0f; }
    float GetCollisionDamagePatient() const override { return 0.0f; }

    static void InicializarModelo();

private:
    float attackTimer;
    float anguloGiro;
    
    static std::vector<Vertex> deltaVertices;
    static GLuint deltaTextureID;
    static GLuint deltaDisplayListID;
    static bool deltaCarregado;
};

// BOSS 3: NEXUS OMEGA
class NexusOmega : public Inimigo {
public:
    NexusOmega(float x, float y, float z);
    
    void Atualizar(float dt, const Player& player) override;
    void Desenhar() override;
    void Destruir() override;
    void TomarDano(float dano) override;
    bool IsBoss() const override { return true; }
    
    float GetMaxHealth() const { return maxHealth; }
    float GetCurrentHealth() const { return health; }

    static void InicializarModelo();

private:
    float maxHealth;
    float attackTimer;
    float attackAngle;
    float moveAngle;
    
    static std::vector<Vertex> nexusVertices;
    static GLuint nexusTextureID;
    static GLuint nexusDisplayListID;
    static bool nexusCarregado;
};



