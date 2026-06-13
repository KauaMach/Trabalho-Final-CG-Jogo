#ifndef AI_FSM_H
#define AI_FSM_H

enum EstadoBoss
{
    PATRULHA,
    AGRESSIVO,
    FURIA,
    ADAPTACAO
};

class BossFSM
{
private:
    EstadoBoss estadoAtual;
    float saudeMax;

public:
    BossFSM(float HPMaximo);
    void AtualizarEstado(float saudeAtual);

    EstadoBoss ObterEstadoAtual() const { return estadoAtual; }
    float ObterModificadorVelocidade() const;
    float ObterFrequenciaTiro() const;
    int ObterPadraoAtaque() const;
};

#endif