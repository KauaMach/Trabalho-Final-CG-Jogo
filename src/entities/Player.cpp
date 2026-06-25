#include "Player.h"
#include "core/Renderer.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <algorithm>
#include "core/AudioManager.h"

extern AudioManager audioManager;

Player::Player() : posX(0.0f), posY(0.0f), posZ(0.0f), speed(3.0f), textureID(0),
                   isDashing(false), dashTimer(0.0f), dashCooldown(0.0f), rollAngle(0.0f), polarity(0),
                   isRolling(false), rollTimer(0.0f),
                   isSurgeActive(false), surgeTimer(0.0f), surgePolarity(0), surgeRadius(0.0f),
                   health(100.0f), maxHealth(100.0f), surgeEnergy(0.0f), maxSurge(100.0f) {}

void Player::Init()
{
    textureID = Renderer::LoadTexture("assets/textures/player_tex.jpg");
    if (!LoadModel("assets/models/player.obj"))
    {
        std::cerr << "Failed to load player model!" << std::endl;
    }
}

bool Player::LoadModel(const std::string &objPath)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str(), "assets/models/");
    if (!warn.empty())
        std::cout << "WARN: " << warn << std::endl;
    if (!err.empty())
        std::cerr << "ERR: " << err << std::endl;
    if (!ret)
        return false;

    // Converte os vertices lidos no padrao interleave do tinyobjloader
    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            int fv = shapes[s].mesh.num_face_vertices[f];
            for (size_t v = 0; v < fv; v++)
            {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Vertex vertex;
                vertex.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.z = attrib.vertices[3 * idx.vertex_index + 2];

                if (idx.normal_index >= 0)
                {
                    vertex.nx = attrib.normals[3 * idx.normal_index + 0];
                    vertex.ny = attrib.normals[3 * idx.normal_index + 1];
                    vertex.nz = attrib.normals[3 * idx.normal_index + 2];
                }
                else
                {
                    vertex.nx = vertex.ny = vertex.nz = 0.0f;
                }

                if (idx.texcoord_index >= 0)
                {
                    vertex.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.v = attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else
                {
                    vertex.u = vertex.v = 0.0f;
                }
                vertices.push_back(vertex);
            }
            index_offset += fv;
        }
    }

    // Calcula Bounding Box para escalar dinamicamente (para nao ficar gigante/invisivel)
    float minX = 9999, maxX = -9999;
    for (auto &v : vertices)
    {
        if (v.x < minX)
            minX = v.x;
        if (v.x > maxX)
            maxX = v.x;
    }
    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0)
    {
        scale = 20.0f / width; // Limita o tamanho horizontal da nave
    }

    for (auto &v : vertices)
    {
        v.x *= scale;
        v.y *= scale;
        v.z *= scale;
    }

    return true;
}

void Player::Update(bool *keys)
{
    float dt = 0.01666f; // Simulando delta time de 60 FPS (1/60)

    if (dashCooldown > 0.0f)
    {
        dashCooldown -= dt;
    }

    // Iniciar o Dash
    if ((keys['e'] || keys['E']) && dashCooldown <= 0.0f && !isDashing)
    {
        isDashing = true;
        dashTimer = 0.8f;    // Dura 800ms
        dashCooldown = 2.0f; // 2 segundos sem poder usar de novo
        rollAngle = 0.0f;
    }

    float currentSpeed = speed;

    // Se estivermos em Dash, rodamos a logica frenetica
    if (isDashing)
    {
        currentSpeed = speed * 0.5f; // Permite andar prós lados, mas um pouco mais devagar que antes
        dashTimer -= dt;

        // Para dar uma volta inteira (360) em 0.8 seg = 450 graus por segundo
        rollAngle += 450.0f * dt;

        // Recuo automatico (Drift evasivo para tras)
        // Isso empurra a nave para tras independentemente do que o jogador apertar
        posZ += (speed * 0.5f);

        if (dashTimer <= 0.0f)
        {
            isDashing = false;
            rollAngle = 0.0f; // Crava no eixo certo ao terminar
        }
    }
    
    // Barrel Roll cosmetico (giro sem recuo)
    if ((keys['r'] || keys['R']) && !isRolling && !isDashing)
    {
        isRolling = true;
        rollTimer = 0.8f; // Dura 800ms
        rollAngle = 0.0f;
    }

    if (isRolling)
    {
        rollTimer -= dt;
        rollAngle += 450.0f * dt; // Mesma velocidade de giro do Back-dash
        
        if (rollTimer <= 0.0f)
        {
            isRolling = false;
            rollAngle = 0.0f; // Crava no eixo certo ao terminar
        }
    }
    
    if (isSurgeActive) {
        surgeTimer -= dt;
        if (surgePolarity == 1) { // Cresce a esfera do Inferno vermelho
            surgeRadius += 600.0f * dt; 
        }
        if (surgeTimer <= 0.0f) {
            isSurgeActive = false;
            // Retorna o audio ao normal
            audioManager.SetMusicVolume(0.5f);
            audioManager.SetSFXVolume(0.5f);
        }
    } else {
        // Carregador automatico de testes (10% de SURGE por segundo)
        if (surgeEnergy < maxSurge) {
            surgeEnergy += 10.0f * dt;
            if (surgeEnergy > maxSurge) surgeEnergy = maxSurge;
        }
    }

    if (keys['w'] || keys['W'])
        posZ -= currentSpeed; // Move para 'frente' (fundo da tela)
    if (keys['s'] || keys['S'])
        posZ += currentSpeed; // Move para 'tras' (frente da tela)
    if (keys['a'] || keys['A'])
        posX -= currentSpeed; // Esquerda
    if (keys['d'] || keys['D'])
        posX += currentSpeed; // Direita
        
    // Atualiza fisicas dos projeteis
    for (auto& laser : lasers) {
        laser.Update();
    }
    // Remove lasers inativos da memoria (que sumiram da tela)
    lasers.erase(std::remove_if(lasers.begin(), lasers.end(), [](const Projectile& p) { return !p.active; }), lasers.end());
}

void Player::Draw()
{
    // Desenha todos os lasers no espaco do mundo (antes da matriz da nave ser criada)
    for (auto& laser : lasers) {
        laser.Draw();
    }

    glPushMatrix();
    glTranslatef(posX, posY, posZ); // Posicao no espaco 3D

    // Barrel Roll Evasivo (Eixo Z é a "frente" da nave)
    if (rollAngle > 0.0f)
    {
        glRotatef(rollAngle, 0.0f, 0.0f, 1.0f);
    }

    // A nave original já está voltada para a frente (-Z), então não precisamos virá-la 180 graus.
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Configura material padrao branco para não escurecer a textura
    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    GLfloat lightPos[] = {0.0f, 200.0f, 200.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glBegin(GL_TRIANGLES);
    for (const auto &v : vertices)
    {
        glNormal3f(v.nx, v.ny, v.nz);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();

    // Desenhando a Aura de Polaridade
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE); // Nao bloqueia renderizacao atras do escudo
    
    glEnable(GL_COLOR_MATERIAL);
    if (polarity == 0) {
        glColor4f(0.0f, 0.5f, 1.0f, 0.3f); // Aura Azul (30% opacidade)
    } else {
        glColor4f(1.0f, 0.1f, 0.1f, 0.3f); // Aura Vermelha (30% opacidade)
    }
    glutSolidSphere(28.0, 32, 32); // Esfera aumentada englobando toda a nave
    
    // Desenho do SURGE Visual
    if (isSurgeActive) {
        if (surgePolarity == 0) {
            // SURGE AZUL: Feixe continuo (Blizzard)
            glColor4f(0.0f, 0.8f, 1.0f, 0.9f);
            glPushMatrix();
            glTranslatef(0.0f, 0.0f, -400.0f); // Projeta o raio mil unidades pra frente
            glScalef(8.0f, 8.0f, 800.0f); // Estica absurdamente como um mega laser
            glutSolidCube(1.0); 
            glPopMatrix();
        } else {
            // SURGE VERMELHO: Explosao radial crescendo (Inferno)
            float alpha = 0.8f - (surgeRadius/1200.0f);
            if (alpha < 0.0f) alpha = 0.0f;
            glColor4f(1.0f, 0.1f, 0.1f, alpha); // Fica transparente ao crescer
            glutSolidSphere(surgeRadius, 40, 40);
        }
    }
    
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);

    glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();
}

void Player::Shoot() {
    // Dispara dois lasers simetricos (um de cada asa da nave) copiando a cor/polaridade atual
    lasers.push_back(Projectile(posX - 8.0f, posY, posZ - 15.0f, polarity));
    lasers.push_back(Projectile(posX + 8.0f, posY, posZ - 15.0f, polarity));
    
    // Toca o som adequado de acordo com a polaridade
    if (polarity == 0) {
        audioManager.PlayLaserBlue();
    } else {
        audioManager.PlayLaserRed();
    }
}

void Player::DrawHUD() {
    // Salvar estado da projecao 3D para entrar no modo Holografico 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1024, 768, 0); // Tela 1024x768 com (0,0) no topo-esquerdo
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Desenha Barra de Vida (Top-Left)
    // Fundo cinza escuro/chassis
    glColor4f(0.1f, 0.1f, 0.1f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(20, 20); glVertex2f(220, 20);
    glVertex2f(220, 35); glVertex2f(20, 35);
    glEnd();
    
    // Preenchimento de Vida (Verde Biologico)
    float healthPercent = health / maxHealth;
    glColor4f(0.1f, 0.9f, 0.2f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(20, 20); glVertex2f(20 + 200 * healthPercent, 20);
    glVertex2f(20 + 200 * healthPercent, 35); glVertex2f(20, 35);
    glEnd();
    
    // Desenha Barra de SURGE (Top-Left, logo abaixo da vida)
    // Fundo cinza escuro
    glColor4f(0.1f, 0.1f, 0.1f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(20, 40); glVertex2f(220, 40);
    glVertex2f(220, 50); glVertex2f(20, 50);
    glEnd();
    
    // Preenchimento SURGE (Reativo a Polaridade da Nave)
    float surgePercent = surgeEnergy / maxSurge;
    if (polarity == 0) { // Azul/Ciano
        glColor4f(0.0f, 0.8f, 1.0f, 0.8f);
    } else { // Vermelho/Rubi
        glColor4f(1.0f, 0.1f, 0.1f, 0.8f);
    }
    glBegin(GL_QUADS);
    glVertex2f(20, 40); glVertex2f(20 + 200 * surgePercent, 40);
    glVertex2f(20 + 200 * surgePercent, 50); glVertex2f(20, 50);
    glEnd();
    
    // Borda/Aro extra para o SURGE indicando 100%
    if (surgeEnergy >= maxSurge) {
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(20, 40); glVertex2f(220, 40);
        glVertex2f(220, 50); glVertex2f(20, 50);
        glEnd();
    }
    
    // Restaurar matrizes
    glDisable(GL_BLEND);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Player::ActivateSurge() {
    // So funciona se estiver em 100% e nao estiver em uso
    if (isSurgeActive || surgeEnergy < maxSurge) return; 
    
    isSurgeActive = true;
    surgeEnergy = 0.0f; // Drena a barra
    surgeTimer = 2.0f; // Dura 2 segundos cravados
    surgePolarity = polarity;
    surgeRadius = 30.0f; // Tamanho inicial da explosao
    
    // Impacto Sonoro (Muta a musica, explode os SFX)
    audioManager.SetMusicVolume(0.1f);
    audioManager.SetSFXVolume(1.0f);
    
    if (surgePolarity == 0) {
        audioManager.PlaySurgeBlue(2000); // 2000 ms (2 segundos cravados para bater com a animacao)
    } else {
        audioManager.PlaySurgeRed(2000); 
    }
}
