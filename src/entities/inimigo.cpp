#include "inimigo.h"
#include "Player.h"
#include "core/Renderer.h"
#include <iostream>
#include "tiny_obj_loader.h"

void Inimigo::Atualizar(float dt, const Player& player) {
    // Movimento padrao (se move pelo eixo Z na direcao do jogador)
    posZ += velZ * dt;
    posX += velX * dt;

    // --- PAREDES INVISÍVEIS PARA INIMIGOS ---
    float limiteDireita = 250.0f;
    float limiteEsquerda = -250.0f;

    if (posX > limiteDireita) {
        posX = limiteDireita;
        velX = -velX; // Bate e volta (Efeito rebote)
    } else if (posX < limiteEsquerda) {
        posX = limiteEsquerda;
        velX = -velX;
    }
}

// === VÍRUS ALFA ===
VirusAlfa::VirusAlfa(float x, float y, float z) 
    : Inimigo(x, y, z, 0) // 0 = Polaridade Azul
{
    health = 15.0f; 
    raioColisao = 18.0f;
    velZ = 80.0f; // Velocidade base
    emMergulho = false;
}

void VirusAlfa::Atualizar(float dt, const Player& player) {
    // Chama a fisica basica das paredes
    Inimigo::Atualizar(dt, player);

    // Regra do Alfa: Mergulho Kamikaze (Boids Cerco)
    // Se ele chegar a uma certa distancia Z do jogador, ele acelera e vai pra cima
    float distZ = player.GetZ() - posZ;
    
    if (!emMergulho && distZ > 0 && distZ < 400.0f) {
        emMergulho = true;
        velZ = 250.0f; // Acelera absurdamente no mergulho
        
        // Mira perfeitamente no eixo X do jogador
        float tempoParaAlcancar = distZ / velZ;
        velX = (player.GetX() - posX) / tempoParaAlcancar;
    }
}

// Variáveis Estáticas do Vírus Alfa
std::vector<Vertex> VirusAlfa::alfaVertices;
GLuint VirusAlfa::alfaTextureID = 0;
GLuint VirusAlfa::alfaDisplayListID = 0;
bool VirusAlfa::alfaCarregado = false;

void VirusAlfa::InicializarModelo() {
    if (alfaCarregado) return;

    alfaTextureID = Renderer::LoadTexture("assets/textures/virus_alfa.png");
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/virus_alfa.obj", "assets/models/");
    // if (!warn.empty()) std::cout << "WARN Alfa: " << warn << std::endl; // Desativado pois o mtl nao e necessario
    if (!err.empty()) std::cerr << "ERR Alfa: " << err << std::endl;
    if (!ret) {
        std::cerr << "Falha ao carregar modelo virus_alfa.obj!" << std::endl;
        return;
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t startIndex = alfaVertices.size(); // Salva onde o triangulo comecou
            
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Vertex vertex;
                vertex.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.z = attrib.vertices[3 * idx.vertex_index + 2];

                if (idx.normal_index >= 0) {
                    vertex.nx = attrib.normals[3 * idx.normal_index + 0];
                    vertex.ny = attrib.normals[3 * idx.normal_index + 1];
                    vertex.nz = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    vertex.nx = vertex.ny = vertex.nz = 0.0f;
                }

                if (idx.texcoord_index >= 0) {
                    vertex.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.v = attrib.texcoords[2 * idx.texcoord_index + 1];
                } else {
                    vertex.u = vertex.v = 0.0f;
                }
                alfaVertices.push_back(vertex);
            }
            index_offset += fv;
            
            // --- GERADOR DINAMICO DE NORMAIS (CROSS PRODUCT) ---
            // Se o modelo veio sem normais e lemos 3 vertices (um triangulo)
            if (fv == 3) {
                Vertex& v1 = alfaVertices[startIndex];
                Vertex& v2 = alfaVertices[startIndex + 1];
                Vertex& v3 = alfaVertices[startIndex + 2];
                
                if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f) {
                    float ux = v2.x - v1.x; float uy = v2.y - v1.y; float uz = v2.z - v1.z;
                    float vx = v3.x - v1.x; float vy = v3.y - v1.y; float vz = v3.z - v1.z;
                    
                    // Produto Vetorial (Nx, Ny, Nz) perpendicular a face
                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;
                    
                    // Normalizar o vetor
                    float len = std::sqrt(nx*nx + ny*ny + nz*nz);
                    if (len > 0) { nx /= len; ny /= len; nz /= len; }
                    
                    v1.nx = v2.nx = v3.nx = nx;
                    v1.ny = v2.ny = v3.ny = ny;
                    v1.nz = v2.nz = v3.nz = nz;
                }
            }
        }
    }

    // Calcula Bounding Box para escalar e centralizar o virus
    float minX = 9999, maxX = -9999;
    float minY = 9999, maxY = -9999;
    float minZ = 9999, maxZ = -9999;
    for (auto &v : alfaVertices) {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
        if (v.z < minZ) minZ = v.z;
        if (v.z > maxZ) maxZ = v.z;
    }
    
    // Descobre o centro geometrico
    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;
    
    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0) {
        scale = 30.0f / width; 
    }

    // Move os vertices para o centro e aplica a escala
    for (auto &v : alfaVertices) {
        v.x = (v.x - centroX) * scale;
        v.y = (v.y - centroY) * scale;
        v.z = (v.z - centroZ) * scale;
    }
    
    // --- COMPILACAO DA DISPLAY LIST (OTIMIZACAO DE PERFORMANCE) ---
    alfaDisplayListID = glGenLists(1);
    glNewList(alfaDisplayListID, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (const auto &v : alfaVertices) {
        glNormal3f(v.nx, v.ny, v.nz);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
    glEndList();
    
    alfaCarregado = true;
}

void VirusAlfa::Desenhar() {
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (alfaCarregado) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, alfaTextureID);
        
        // Configurando a reacao da textura a luz
        GLfloat mat_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f }; // Luz ambiente base alta para manter visibilidade
        GLfloat mat_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Luz direta
        GLfloat mat_specular[] = { 0.4f, 0.4f, 0.4f, 1.0f };// Reflexo nas curvas
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, 15.0f);
        
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        
        // Em vez de recalcular tudo vertice a vertice, usamos a memoria da GPU direto
        glCallList(alfaDisplayListID);
        
    } else {
        // Fallback Dodecaedro
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(0.0f, 0.4f, 1.0f);
        glutSolidDodecahedron();
        glDisable(GL_COLOR_MATERIAL);
    }
    
    extern bool showCollisionBoxes;
    if (showCollisionBoxes) 
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
        glLineWidth(2.0f);
        glutWireSphere(raioColisao, 16, 16);
        glLineWidth(1.0f);
    }

    glPopMatrix();
}

// ==============================================================================
// BACTÉRIA COCO
// ==============================================================================

std::vector<Vertex> BacteriaCoco::cocoVertices;
GLuint BacteriaCoco::cocoTextureID = 0;
GLuint BacteriaCoco::cocoDisplayListID = 0;
bool BacteriaCoco::cocoCarregado = false;

BacteriaCoco::BacteriaCoco(float x, float y, float z) : Inimigo(x, y, z, 1) { // 1 = Vermelho
    health = 40.0f; // Tanque!
    velZ = 60.0f; // Desce devagar
    velX = 0.0f; // Linear
    raioColisao = 15.0f; // Espessura menor
    comprimentoZ = 35.0f; // Hitbox alongada como cilindro
    cooldownTiro = 1.0f + ((rand() % 100) / 100.0f); // Tiro entre 1 e 2 segs iniciais
    anguloRotacao = 0.0f;
    velocidadeRotacao = 60.0f; // Roda 60 graus por segundo (lenta e constante)
}

#include "Projectile.h"
extern std::vector<EnemyProjectile> enemyLasers; // Vector global para lidar com os tiros

void BacteriaCoco::Atualizar(float dt, const Player& player) {
    Inimigo::Atualizar(dt, player);

    // Gira a bactéria
    anguloRotacao += velocidadeRotacao * dt;
    if (anguloRotacao >= 360.0f) anguloRotacao -= 360.0f;

    cooldownTiro -= dt;
    
    // Nao persegue em X, vai reto como um tanque.
    // Atira em direcao ao player se estiver na frente do player e perto o suficiente
    float distZ = player.GetZ() - posZ;
    if (distZ > 0 && distZ < 700.0f) { // Visível na tela
        if (cooldownTiro <= 0.0f) {
            cooldownTiro = 2.0f; // A cada 2 segundos, um tiro duplo
            
            // Atira dois esporos vermelhos
            enemyLasers.push_back(EnemyProjectile(posX - 10.0f, posY, posZ + 20.0f, 1));
            enemyLasers.push_back(EnemyProjectile(posX + 10.0f, posY, posZ + 20.0f, 1));
        }
    }
}

void BacteriaCoco::Desenhar() {
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (cocoCarregado) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cocoTextureID);
        
        glEnable(GL_LIGHTING); // Volta a luz pra dar volume e reflexo
        glEnable(GL_COLOR_MATERIAL);
        
        // Cores base explodidas (Volume e Brilho "Molhado")
        GLfloat mat_ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // Fundo sempre vermelho
        GLfloat mat_diffuse[] = { 1.0f, 0.2f, 0.2f, 1.0f }; // Difuso avermelhado
        GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };// Reflexos brancos super intensos
        GLfloat mat_emission[] = { 0.6f, 0.0f, 0.0f, 1.0f };// Emite brilho base 60%

        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, 64.0f); // Brilho afiado (Plástico molhado)
        
        glColor3f(1.0f, 1.0f, 1.0f);
        
        glPushMatrix();
        glRotatef(anguloRotacao, 0.0f, 0.0f, 1.0f); // Rotação no eixo Z (Barrel Roll)
        glCallList(cocoDisplayListID);
        glPopMatrix();
        
        // --- RESET MATERIAL PARA NÃO VAZAR PARA A NAVE ---
        GLfloat reset_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat reset_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        GLfloat reset_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat reset_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, reset_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, reset_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, reset_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, reset_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
        
    } else {
        // Fallback Sphere
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_COLOR_MATERIAL);
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidSphere(25.0, 16, 16);
        glDisable(GL_COLOR_MATERIAL);
    }
    
    extern bool showCollisionBoxes;
    if (showCollisionBoxes) 
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 1.0f); // Branco
        glLineWidth(2.0f);
        
        glPushMatrix();
        // Escala a esfera no eixo Z para formar uma pílula alongada
        glScalef(1.0f, 1.0f, comprimentoZ / raioColisao);
        glutWireSphere(raioColisao, 16, 16);
        glPopMatrix();
        
        glLineWidth(1.0f);
    }

    glPopMatrix();
}

void BacteriaCoco::InicializarModelo() {
    cocoTextureID = Renderer::LoadTexture("assets/textures/bacteria_coco.png");
    if (cocoTextureID == 0) {
        std::cerr << "[AVISO] Nao foi possivel carregar a textura da Bacteria Coco." << std::endl;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/bacteria_coco.obj", "assets/models/");

    if (!warn.empty()) std::cout << "WARN: " << warn << std::endl;
    if (!err.empty()) std::cerr << "ERR: " << err << std::endl;

    if (!ret) {
        std::cerr << "Falha ao carregar assets/models/bacteria_coco.obj!" << std::endl;
        return;
    }

    float minX = 9999, maxX = -9999;
    float minY = 9999, maxY = -9999;
    float minZ = 9999, maxZ = -9999;

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t startIndex = cocoVertices.size();
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Vertex vertex;

                vertex.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.z = attrib.vertices[3 * idx.vertex_index + 2];

                if (idx.normal_index >= 0) {
                    vertex.nx = attrib.normals[3 * idx.normal_index + 0];
                    vertex.ny = attrib.normals[3 * idx.normal_index + 1];
                    vertex.nz = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    vertex.nx = vertex.ny = vertex.nz = 0.0f;
                }

                if (idx.texcoord_index >= 0) {
                    vertex.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.v = attrib.texcoords[2 * idx.texcoord_index + 1];
                } else {
                    vertex.u = vertex.v = 0.0f;
                }
                cocoVertices.push_back(vertex);
            }
            index_offset += fv;
            
            // --- GERADOR DINAMICO DE NORMAIS (CROSS PRODUCT) ---
            if (fv == 3) {
                Vertex& v1 = cocoVertices[startIndex];
                Vertex& v2 = cocoVertices[startIndex + 1];
                Vertex& v3 = cocoVertices[startIndex + 2];
                
                if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f) {
                    float ux = v2.x - v1.x; float uy = v2.y - v1.y; float uz = v2.z - v1.z;
                    float vx = v3.x - v1.x; float vy = v3.y - v1.y; float vz = v3.z - v1.z;
                    
                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;
                    
                    float len = std::sqrt(nx*nx + ny*ny + nz*nz);
                    if (len > 0) { nx /= len; ny /= len; nz /= len; }
                    
                    v1.nx = v2.nx = v3.nx = nx;
                    v1.ny = v2.ny = v3.ny = ny;
                    v1.nz = v2.nz = v3.nz = nz;
                }
            }
        }
    }

    for (auto &v : cocoVertices) {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
        if (v.z < minZ) minZ = v.z;
        if (v.z > maxZ) maxZ = v.z;
    }
    
    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;
    
    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0) {
        scale = 50.0f / width; // Bactéria bem maior e imponente
    }

    // Offset em Z: O modelo estava muito para frente em relação à hitbox.
    // Vamos recuar o modelo em Z para que ele fique perfeitamente alinhado com a caixa de colisão.
    float offsetZ = -15.0f; 

    for (auto &v : cocoVertices) {
        v.x = (v.x - centroX) * scale;
        v.y = (v.y - centroY) * scale;
        v.z = ((v.z - centroZ) * scale) + offsetZ;
    }
    
    // OTIMIZACAO DISPLAY LIST
    cocoDisplayListID = glGenLists(1);
    glNewList(cocoDisplayListID, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (const auto &v : cocoVertices) {
        glNormal3f(v.nx, v.ny, v.nz);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
    glEndList();
    
    cocoCarregado = true;
}

// ==============================================================================
// BOSS 1: LEUKOCYTE CORRUPTO
// ==============================================================================

GLuint LeukocyteCorrupto::bossDisplayListID = 0;
bool LeukocyteCorrupto::bossCarregado = false;

LeukocyteCorrupto::LeukocyteCorrupto(float x, float y, float z) : Inimigo(x, y, z, 1) {
    maxHealth = 1500.0f; // Boss Tank
    health = maxHealth;
    velZ = 0.0f;
    velX = 0.0f;
    raioColisao = 35.0f; // Boss Ajustado
    comprimentoZ = 35.0f; 
    
    bossState = 0; 
    stateTimer = 0.0f;
    attackCooldown = 2.0f;
    anguloEspiral = 0.0f;
}

extern int cameraMode;

void LeukocyteCorrupto::Atualizar(float dt, const Player& player) {
    // Não chama o Atualizar do pai para não descer linearmente no eixo Z
    
    stateTimer += dt;
    attackCooldown -= dt;
    
    // Atualiza polaridade e mecânicas baseadas na Vida!
    float percentLife = health / maxHealth;
    if (percentLife > 0.6f) {
        bossState = 0; // Patrulha
        polarity = 1; // Vermelho constante
    } else if (percentLife > 0.25f) {
        bossState = 1; // Espiral no centro
        polarity = (int)(stateTimer) % 2; // Pisca azul/vermelho por segundo
    } else {
        bossState = 2; // FÚRIA
        polarity = 1; // Vermelho frenético
    }

    // FASE 0: PATRULHA SENOIDAL (0.6 - 1.0 hp)
    if (bossState == 0) {
        float baseZ = 350.0f;
        if (cameraMode != 3) baseZ *= 2.0f; // Afasta mais se não for top-down
        posZ = player.GetZ() - baseZ;
        
        posX = std::sin(stateTimer * 2.0f) * 200.0f; // Vai e vem suave
        posY = 0.0f; // Fixo na mesma altura da nave
        
        if (attackCooldown <= 0.0f) {
            attackCooldown = 1.0f;
            // Atira mirando no jogador
            float dx = player.GetX() - posX;
            float dz = player.GetZ() - posZ;
            float mag = std::sqrt(dx*dx + dz*dz);
            if (mag > 0) { dx /= mag; dz /= mag; }
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, dx, 0.0f, dz, 200.0f, polarity));
        }
    } 
    // FASE 1: ATAQUE EM ESPIRAL (0.25 - 0.6 hp)
    else if (bossState == 1) {
        float baseZ = 250.0f;
        if (cameraMode != 3) baseZ *= 2.0f;
        posZ = player.GetZ() - baseZ; // Se aproxima
        
        // Padrão infinito (Oito/Figure-8) varrendo a tela
        posX = std::sin(stateTimer * 2.5f) * 250.0f;
        posY = std::sin(stateTimer * 5.0f) * 15.0f; // Oscila de -15 até 15
        
        anguloEspiral += 120.0f * dt; // Gira rápido
        if (anguloEspiral > 360.0f) anguloEspiral -= 360.0f;
        
        if (attackCooldown <= 0.0f) {
            attackCooldown = 0.15f; // Metralhadora circular
            float rad = anguloEspiral * 3.14159f / 180.0f;
            float dx = std::sin(rad);
            float dz = std::cos(rad);
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, dx, 0.0f, dz, 180.0f, polarity));
            
            // Tiro oposto simultâneo
            float opRad = (anguloEspiral + 180.0f) * 3.14159f / 180.0f;
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(opRad), 0.0f, std::cos(opRad), 180.0f, polarity));
        }
    }
    // FASE 2: FÚRIA BULLET-HELL (0.0 - 0.25 hp)
    else if (bossState == 2) {
        float baseZ = 150.0f;
        if (cameraMode != 3) baseZ *= 2.0f;
        posZ = player.GetZ() - baseZ; // Fica cara a cara
        
        // Varredura lateral frenética + Tremor errático (Shake)
        float posicaoBaseX = std::sin(stateTimer * 6.0f) * 320.0f;
        posX = posicaoBaseX + ((rand() % 40) - 20) * 1.0f;
        posY = ((rand() % 20) - 10) * 1.0f; // Treme sempre pertinho do 0
        
        anguloEspiral += 50.0f * dt;
        
        if (attackCooldown <= 0.0f) {
            attackCooldown = 0.4f; // Explosão em 8 direções frequente
            for (int i = 0; i < 8; i++) {
                float rad = (anguloEspiral + (i * 45.0f)) * 3.14159f / 180.0f;
                // Os tiros alternam de polaridade para forçar o jogador a desviar
                int shotPol = i % 2; 
                enemyLasers.push_back(EnemyProjectile(posX, 0.0f, posZ, std::sin(rad), 0.0f, std::cos(rad), 250.0f, shotPol));
            }
        }
    }
}

void LeukocyteCorrupto::Desenhar() {
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    
    // Cor pulsante dependendo da polaridade e fase
    if (polarity == 0) {
        glColor3f(0.0f, 0.5f, 1.0f); // Azul
    } else {
        glColor3f(1.0f, 0.1f, 0.1f); // Vermelho
    }
    
    GLfloat mat_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f }; 
    GLfloat mat_emission[] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Brilha um pouco
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    // Gira no eixo dependendo da fase
    if (bossState == 1 || bossState == 2) {
        glRotatef(anguloEspiral, 0.0f, 1.0f, 0.0f); // Gira como um liquidificador
    } else {
        glRotatef(stateTimer * 40.0f, 1.0f, 1.0f, 0.0f); // Rola suavemente
    }

    // Aplica o material e textura do modelo OBJ
    if (bossCarregado) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bossTextureID);
        glCallList(bossDisplayListID);
        glDisable(GL_TEXTURE_2D);
    } else {
        glutSolidOctahedron(); // Fallback se der erro
    }
    
    extern bool showCollisionBoxes;
    if (showCollisionBoxes) 
    {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo = Boss Hitbox
        glLineWidth(2.0f);
        glutWireSphere(raioColisao, 16, 16);
        glLineWidth(1.0f);
    }

    glPopMatrix();
}

void LeukocyteCorrupto::Destruir() {
    ativo = false;
    // Tocar Som de Morte Gigante
}



// Variáveis estáticas do Boss
std::vector<Vertex> LeukocyteCorrupto::bossVertices;
GLuint LeukocyteCorrupto::bossTextureID = 0;


void LeukocyteCorrupto::InicializarModelo() {
    if (bossCarregado) return;

    bossTextureID = Renderer::LoadTexture("assets/models/boss/diffuse.png");
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/boss/cyberVir.obj", "assets/models/boss/");
    if (!err.empty()) std::cerr << "ERR Boss: " << err << std::endl;
    if (!ret) {
        std::cerr << "Falha ao carregar modelo cyberVir.obj!" << std::endl;
        return;
    }

    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t startIndex = bossVertices.size();
            
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Vertex vertex;
                vertex.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.z = attrib.vertices[3 * idx.vertex_index + 2];

                if (idx.normal_index >= 0) {
                    vertex.nx = attrib.normals[3 * idx.normal_index + 0];
                    vertex.ny = attrib.normals[3 * idx.normal_index + 1];
                    vertex.nz = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    vertex.nx = vertex.ny = vertex.nz = 0.0f;
                }

                if (idx.texcoord_index >= 0) {
                    vertex.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                    // Inverte o V pois OpenGL (imagem) vs Blender (obj) geralmente difere
                    vertex.v = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
                } else {
                    vertex.u = vertex.v = 0.0f;
                }
                bossVertices.push_back(vertex);
            }
            index_offset += fv;
            
            // --- GERADOR DINAMICO DE NORMAIS (CROSS PRODUCT) ---
            if (fv == 3) {
                Vertex& v1 = bossVertices[startIndex];
                Vertex& v2 = bossVertices[startIndex + 1];
                Vertex& v3 = bossVertices[startIndex + 2];
                
                if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f) {
                    float ux = v2.x - v1.x; float uy = v2.y - v1.y; float uz = v2.z - v1.z;
                    float vx = v3.x - v1.x; float vy = v3.y - v1.y; float vz = v3.z - v1.z;
                    
                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;
                    
                    float len = std::sqrt(nx*nx + ny*ny + nz*nz);
                    if (len > 0) { nx /= len; ny /= len; nz /= len; }
                    
                    v1.nx = v2.nx = v3.nx = nx;
                    v1.ny = v2.ny = v3.ny = ny;
                    v1.nz = v2.nz = v3.nz = nz;
                }
            }
        }
    }

    float minX = 9999, maxX = -9999;
    float minY = 9999, maxY = -9999;
    float minZ = 9999, maxZ = -9999;
    for (auto &v : bossVertices) {
        if (v.x < minX) minX = v.x;
        if (v.x > maxX) maxX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.y > maxY) maxY = v.y;
        if (v.z < minZ) minZ = v.z;
        if (v.z > maxZ) maxZ = v.z;
    }
    
    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;
    
    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0) {
        scale = 100.0f / width; // Tamanho do Boss = 100 (bem maior que os virus comuns de 30)
    }

    for (auto &v : bossVertices) {
        v.x = (v.x - centroX) * scale;
        v.y = (v.y - centroY) * scale;
        v.z = (v.z - centroZ) * scale;
    }

    bossDisplayListID = glGenLists(1);
    glNewList(bossDisplayListID, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (const auto &v : bossVertices) {
        glNormal3f(v.nx, v.ny, v.nz);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
    glEndList();

    bossCarregado = true;
}
