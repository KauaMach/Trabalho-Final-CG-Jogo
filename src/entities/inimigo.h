#pragma once
#include <cmath>
#include "Player.h" // Traz a struct Vertex e a definicao do Player

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
    
    void TomarDano(float dano) { 
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
