#pragma once
#include <vector>
#include "../entities/Player.h"
#include "../entities/inimigo.h"
#include "../entities/Projectile.h"

class Player;
class Inimigo;

class Colisao {
public:
    // Checa colisao de todos os lasers (projeteis) do jogador com os inimigos vivos
    static void ChecarLaserInimigo(Player& player, std::vector<Inimigo*>& inimigos);
    
    // Checa colisao do ataque especial (SURGE) com os inimigos (Dano continuo em area)
    static void ChecarSurgeInimigo(Player& player, std::vector<Inimigo*>& inimigos, float dt);
    
    // Laser do inimigo no player
    static void ChecarLaserInimigoNoPlayer(Player& player, std::vector<EnemyProjectile>& lasersInimigos);

    // Checa colisao fisica direta entre os inimigos e a nave do jogador
    static void ChecarColisaoPlayer(Player& player, std::vector<Inimigo*>& inimigos);
};
