#ifndef COLLISION_H
#define COLLISION_H

#include <vector>
#include <unordered_map>

struct BoxHitbox
{
    float minX, minY, maxX, maxY;
};
struct SphereHitbox
{
    float x, y, raio;
};

// Entidade simplificada registrada no Grid para evitar acoplamento
struct ColisaoObjeto
{
    int id;
    int tipo; // 0: Player, 1: Inimigo, 2: Projetil, 3: PowerUp
    SphereHitbox corpoEsfera;
    BoxHitbox corpoCaixa;
};

class SpatialHashGrid
{
private:
    float tamanhoCelula;
    std::unordered_map<int, std::vector<ColisaoObjeto>> grid;
    int CalcularChaveHash(float x, float y);

public:
    SpatialHashGrid(float cellIdSize);
    void LimparGrid();
    void InserirObjeto(ColisaoObjeto obj);
    std::vector<ColisaoObjeto> ObterVizinhosProximos(ColisaoObjeto obj);
};

class CollisionEngine
{
public:
    static bool ChecarAABB(BoxHitbox a, BoxHitbox b);
    static bool ChecarEsferaParaEsfera(SphereHitbox a, SphereHitbox b);
    static bool ChecarMistas(SphereHitbox esfe, BoxHitbox caixa);
};

#endif