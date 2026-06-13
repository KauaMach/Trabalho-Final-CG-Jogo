#include "../bibliotecas/ai_fsm.h"

BossFSM::BossFSM(float HPMaximo) : estadoAtual(PATRULHA), saudeMax(HPMaximo) {}

void BossFSM::AtualizarEstado(float saudeAtual)
{
    float porcentagemVida = saudeAtual / saudeMax;

    // Transições de estado baseadas na curva de saúde do Boss
    if (porcentagemVida > 0.75f)
    {
        estadoAtual = PATRULHA;
    }
    else if (porcentagemVida > 0.40f && porcentagemVida <= 0.75f)
    {
        estadoAtual = AGRESSIVO;
    }
    else if (porcentagemVida > 0.15f && porcentagemVida <= 0.40f)
    {
        estadoAtual = FURIA;
    }
    else
    {
        estadoAtual = ADAPTACAO; // Último estágio defensivo: comportamento errático
    }
}

float BossFSM::ObterModificadorVelocidade() const
{
    switch (estadoAtual)
    {
    case PATRULHA:
        return 1.0f;
    case AGRESSIVO:
        return 1.4f;
    case FURIA:
        return 1.9f;
    case ADAPTACAO:
        return 2.2f;
    }
    return 1.0f;
}

float BossFSM::ObterFrequenciaTiro() const
{
    switch (estadoAtual)
    {
    case PATRULHA:
        return 1.5f; // Segundos por disparo
    case AGRESSIVO:
        return 0.9f;
    case FURIA:
        return 0.4f;
    case ADAPTACAO:
        return 0.2f; // Cadência insana (Bullet Hell)
    }
    return 1.0f;
}

int BossFSM::ObterPadraoAtaque() const
{
    switch (estadoAtual)
    {
    case PATRULHA:
        return 1; // Tiro Retilíneo
    case AGRESSIVO:
        return 2; // Tiro Triplo em Leque
    case FURIA:
        return 3; // Barragem Radial Espiral ($360^{\circ}$)
    case ADAPTACAO:
        return 4; // Disparos de Projéteis com Perseguição Preditiva
    }
    return 1;
}