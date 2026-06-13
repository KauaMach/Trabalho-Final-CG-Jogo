#define _USE_MATH_DEFINES
#include "../bibliotecas/ai_boids.h"
#include <cmath>

BoidsEngine::BoidsEngine(float maxVel, float maxFrc) : maxVelocidade(maxVel), maxForca(maxFrc) {}

void BoidsEngine::AplicarForcaProcura(Boid &boid, float tx, float ty, float peso)
{
    float desejadoX = tx - boid.x;
    float desejadoY = ty - boid.y;
    float d = std::sqrt(desejadoX * desejadoX + desejadoY * desejadoY);

    if (d > 0.001f)
    {
        desejadoX = (desejadoX / d) * maxVelocidade;
        desejadoY = (desejadoY / d) * maxVelocidade;

        float steerX = desejadoX - boid.vx;
        float steerY = desejadoY - boid.vy;

        // Limita a força máxima de manobra (Steering Force limit)
        float fs = std::sqrt(steerX * steerX + steerY * steerY);
        if (fs > maxForca)
        {
            steerX = (steerX / fs) * maxForca;
            steerY = (steerY / fs) * maxForca;
        }

        boid.ax += steerX * peso;
        boid.ay += steerY * peso;
    }
}

void BoidsEngine::AplicarForcaFuga(Boid &boid, float tx, float ty, float peso)
{
    // Comportamento inverso da procura: corre para a direção oposta ao perigo do SURGE
    float desejadoX = boid.x - tx;
    float desejadoY = boid.y - ty;
    float d = std::sqrt(desejadoX * desejadoX + desejadoY * desejadoY);

    if (d > 0.001f && d < 6.0f)
    { // Raio de pânico do enxame
        desejadoX = (desejadoX / d) * (maxVelocidade * 1.5f);
        desejadoY = (desejadoY / d) * (maxVelocidade * 1.5f);

        boid.ax += desejadoX * peso;
        boid.ay += desejadoY * peso;
    }
}

void BoidsEngine::ProcessarEnxame(std::vector<Boid> &enxame, float alvoX, float alvoY, bool jogadorEmSurge)
{
    float raioVizinhanca = 2.5f;

    for (size_t i = 0; i < enxame.size(); ++i)
    {
        if (!enxame[i].ativo)
            continue;

        float sepX = 0, sepY = 0;
        float aliX = 0, aliY = 0;
        float coeX = 0, coeY = 0;
        int totalVizinhos = 0;

        enxame[i].ax = 0;
        enxame[i].ay = 0;

        for (size_t j = 0; j < enxame.size(); ++j)
        {
            if (i == j || !enxame[j].ativo)
                continue;

            float dx = enxame[i].x - enxame[j].x;
            float dy = enxame[i].y - enxame[j].y;
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist > 0 && dist < raioVizinhanca)
            {
                // Força de Separação (Evita sobreposição mecânica)
                sepX += dx / (dist * dist);
                sepY += dy / (dist * dist);

                // Força de Alinhamento (Imita a velocidade e rumo do grupo)
                aliX += enxame[j].vx;
                aliY += enxame[j].vy;

                // Força de Coesão (Caminha em direção ao centro de massa local)
                coeX += enxame[j].x;
                coeY += enxame[j].y;

                totalVizinhos++;
            }
        }

        if (totalVizinhos > 0)
        {
            sepX /= totalVizinhos;
            sepY /= totalVizinhos;
            aliX /= totalVizinhos;
            aliY /= totalVizinhos;
            coeX /= totalVizinhos;
            coeY /= totalVizinhos;

            // Ajusta vetores finais para as regras clássicas de Reynolds
            AplicarForcaProcura(enxame[i], enxame[i].x + sepX, enxame[i].y + sepY, pesoSeparacao);
            AplicarForcaProcura(enxame[i], enxame[i].x + aliX, enxame[i].y + aliY, pesoAlinhamento);
            AplicarForcaProcura(enxame[i], coeX, coeY, pesoCoesao);
        }

        // Regra de Cerco x Fuga Dinâmica baseada no estado do Jogador
        if (!jogadorEmSurge)
        {
            AplicarForcaProcura(enxame[i], alvoX, alvoY, 0.8f); // Cerco predatório ao Jogador
        }
        else
        {
            AplicarForcaFuga(enxame[i], alvoX, alvoY, 2.5f); // Fuga imediata do perigo
        }

        // Integração de Euler Clássica para atualização física discreta
        enxame[i].vx += enxame[i].ax;
        enxame[i].vy += enxame[i].ay;

        float vel = std::sqrt(enxame[i].vx * enxame[i].vx + enxame[i].vy * enxame[i].vy);
        if (vel > maxVelocidade)
        {
            enxame[i].vx = (enxame[i].vx / vel) * maxVelocidade;
            enxame[i].vy = (enxame[i].vy / vel) * maxVelocidade;
        }

        enxame[i].x += enxame[i].vx;
        enxame[i].y += enxame[i].vy;
    }
}