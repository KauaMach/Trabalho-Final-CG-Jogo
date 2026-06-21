#include "../bibliotecas/collision.h"
#include <cmath>
#include <algorithm>

SpatialHashGrid::SpatialHashGrid(float cellIdSize) : tamanhoCelula(cellIdSize) {}

int SpatialHashGrid::CalcularChaveHash(float x, float y)
{
    int ix = static_cast<int>(std::floor(x / tamanhoCelula));
    int iy = static_cast<int>(std::floor(y / tamanhoCelula));
    // Função polinomial de espalhamento hash estável para chaves bidimensionais primárias
    return ix * 73856093 ^ iy * 19349663;
}

void SpatialHashGrid::LimparGrid()
{
    grid.clear();
}

void SpatialHashGrid::InserirObjeto(ColisaoObjeto obj)
{
    int chave = CalcularChaveHash(obj.corpoEsfera.x, obj.corpoEsfera.y);
    grid[chave].push_back(obj);
}

std::vector<ColisaoObjeto> SpatialHashGrid::ObterVizinhosProximos(ColisaoObjeto obj)
{
    std::vector<ColisaoObjeto> vizinhos;
    float x = obj.corpoEsfera.x;
    float y = obj.corpoEsfera.y;

    // Varre as 9 células adjacentes no espaço euclidiano do hash
    for (int dx = -1; dx <= 1; ++dx)
        for (int dy = -1; dy <= 1; ++dy)
        {
            int chave = CalcularChaveHash(x + dx * tamanhoCelula, y + dy * tamanhoCelula);
            if (grid.find(chave) != grid.end())
                vizinhos.insert(vizinhos.end(), grid[chave].begin(), grid[chave].end());
        }
    return vizinhos;
}

bool CollisionEngine::ChecarAABB(BoxHitbox a, BoxHitbox b)
{
    return (a.minX <= b.maxX && a.maxX >= b.minX &&
            a.minY <= b.maxY && a.maxY >= b.minY);
}

bool CollisionEngine::ChecarEsferaParaEsfera(SphereHitbox a, SphereHitbox b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    float distanciaAoQuadrado = dx * dx + dy * dy;
    float somaRaios = a.raio + b.raio;
    return distanciaAoQuadrado <= (somaRaios * somaRaios);
}

bool CollisionEngine::ChecarMistas(SphereHitbox esfe, BoxHitbox caixa)
{
    float maisProximoX = std::max(caixa.minX, std::min(esfe.x, caixa.maxX));
    float maisProximoY = std::max(caixa.minY, std::min(esfe.y, caixa.maxY));

    float dx = esfe.x - maisProximoX;
    float dy = esfe.y - maisProximoY;

    return (dx * dx + dy * dy) <= (esfe.raio * esfe.raio);
}