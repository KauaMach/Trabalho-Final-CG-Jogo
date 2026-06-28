#include "colisao.h"
#include "../entities/Player.h"
#include "../entities/inimigo.h"
#include "../entities/Projectile.h"
#include "../core/ParticleSystem.h"
#include <cmath>
#include <iostream>

extern ParticleSystem globalParticles;
extern int enemiesKilled;


static float Distancia3D(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
}

void Colisao::ChecarLaserInimigo(Player &player, std::vector<Inimigo *> &inimigos)
{
    auto &lasers = player.GetLasers();

    for (auto &laser : lasers)
    {
        if (!laser.active)
            continue;

        for (auto *inimigo : inimigos)
        {
            if (!inimigo || !inimigo->IsAtivo())
                continue;

            // Assume que o laser tem um raio pequeno de colisão (ex: 5.0f)
            float distXY = std::sqrt(std::pow(laser.x - inimigo->GetX(), 2) + std::pow(laser.y - inimigo->GetY(), 2));
            float distZ = std::abs(laser.z - inimigo->GetZ());

            if (distZ < (5.0f + inimigo->GetComprimentoZ()) && distXY < (5.0f + inimigo->GetRaio()))
            {
                // HOUVE COLISÃO DO LASER COM O INIMIGO!
                laser.active = false; // Laser some na hora do impacto

                // NOVA REGRA (Pedido pelo Jogador): Cor igual = Dano Completo. Cor diferente = Dano Mínimo.
                float danoCompleto = 7.5f; // Mata Alfa (15 HP) em 2 tiros

                if (laser.polarity == inimigo->GetPolarity())
                {
                    inimigo->TomarDano(danoCompleto); // Dano Absoluto 100%
                    player.AddSurgeEnergy(5.0f); // 5% de Surge ganho por tiro CERTO
                }
                else
                {
                    inimigo->TomarDano(1.0f); // Mata Alfa em 15 tiros
                    player.AddSurgeEnergy(1.0f); // 1% de Surge ganho por tiro ERRADO
                }

                if (inimigo->IsBoss()) {
                    player.HealNanocell(0.5f); // 0.5 de vida (meio porcento) curado a cada tiro que acerta o boss
                }

                // Se esse tiro matou o inimigo: Recompensa o Hospedeiro
                if (!inimigo->IsAtivo())
                {
                    enemiesKilled++;
                    player.AddPatientHealth(2.0f); // Paciente melhora em +2% HSP
                    globalParticles.SpawnExplosion(inimigo->GetX(), inimigo->GetY(), inimigo->GetZ(), inimigo->GetPolarity(), 25);
                }

                break; // Sai do for interno, vai pro próximo laser (ja que este quebrou)
            }
        }
    }
}

void Colisao::ChecarSurgeInimigo(Player &player, std::vector<Inimigo *> &inimigos, float dt)
{
    if (!player.IsSurgeActive())
        return;

    // Especial SURGE causa Dano Contínuo ABSURDO, garantindo obliteração instantânea
    float danoPorSegundo = 50.0f;
    float danoDoFrame = danoPorSegundo * dt;

    for (auto *inimigo : inimigos)
    {
        if (!inimigo->IsAtivo())
            continue;

        bool atingido = false;

        if (player.GetSurgePolarity() == 0)
        {
            // SURGE AZUL (Blizzard): Feixe contínuo à frente do jogador no eixo Z
            float distX = std::abs(inimigo->GetX() - player.GetX());
            // Se o inimigo esta no "túnel" da largura do raio E está na frente da nave
            if (distX < 40.0f && inimigo->GetZ() < player.GetZ())
            {
                atingido = true;
            }
        }
        else
        {
            // SURGE VERMELHO (Inferno): Explosão radial que cresce ao redor do jogador
            float dist = Distancia3D(player.GetX(), player.GetY(), player.GetZ(),
                                     inimigo->GetX(), inimigo->GetY(), inimigo->GetZ());
            if (dist < player.GetSurgeRadius())
            {
                atingido = true;
            }
        }

        if (atingido)
        {
            inimigo->TomarDano(danoDoFrame);

            // Se o SURGE obliterou o inimigo
            if (!inimigo->IsAtivo())
            {
                enemiesKilled++;
                player.AddPatientHealth(2.0f);
                globalParticles.SpawnExplosion(inimigo->GetX(), inimigo->GetY(), inimigo->GetZ(), inimigo->GetPolarity(), 30);
            }
        }
    }
}

void Colisao::ChecarColisaoPlayer(Player &player, std::vector<Inimigo *> &inimigos)
{
    // Hitbox Cilíndrica da Nanocel
    float raioNave = 12.0f;        // Espessura da nave no plano XY
    float comprimentoNave = 35.0f; // Distancia do centro para frente/tras no eixo Z

    for (auto *inimigo : inimigos)
    {
        if (!inimigo->IsAtivo())
            continue;

        float raioInimigo = inimigo->GetRaio();
        float compInimigo = inimigo->GetComprimentoZ();

        // Distancia longitudinal (Z)
        float distZ = std::abs(inimigo->GetZ() - player.GetZ());

        // Rebaixando a colisao da nave em 5.0f para centralizar na fuselagem
        float offsetY = 4.0f;

        // Distancia transversal (plano X, Y usando pitágoras)
        float distXY = std::sqrt(std::pow(inimigo->GetX() - player.GetX(), 2) +
                                 std::pow(inimigo->GetY() - (player.GetY() + offsetY), 2));

        // Para colidir no cilindro, o inimigo tem que invadir a grossura do tubo E o comprimento dele ao mesmo tempo
        if (distZ < (comprimentoNave + compInimigo) && distXY < (raioNave + raioInimigo))
        {
            // COLISAO DETECTADA ENTRE NAVE E INIMIGO!
            player.DamageNanocell(inimigo->GetCollisionDamageNanocell());
            float dmgPatient = inimigo->GetCollisionDamagePatient();
            if (dmgPatient > 0.0f) {
                player.DamagePatient(dmgPatient);
            }
            globalParticles.SpawnExplosion(inimigo->GetX(), inimigo->GetY(), inimigo->GetZ(), inimigo->GetPolarity(), 40);
            inimigo->Destruir();
            enemiesKilled++;
        }
    }
}

void Colisao::ChecarLaserInimigoNoPlayer(Player &player, std::vector<EnemyProjectile> &lasersInimigos)
{
    float raioNave = 12.0f; // Espessura aproximada
    float offsetY = 4.0f; // Centro de massa
    
    for (auto &laser : lasersInimigos) {
        if (!laser.active) continue;
        
        float distZ = std::abs(laser.z - player.GetZ());
        float distXY = std::sqrt(std::pow(laser.x - player.GetX(), 2) +
                                 std::pow(laser.y - (player.GetY() + offsetY), 2));
                                 
        if (distZ < 35.0f && distXY < (raioNave + 5.0f)) { // 5.0f = raio do laser
            // HIT NO JOGADOR!
            laser.active = false;
            player.DamageNanocell(5.0f);
            globalParticles.SpawnExplosion(player.GetX(), player.GetY(), player.GetZ(), laser.polarity, 15);
        }
    }
}
