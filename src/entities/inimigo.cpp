#include "inimigo.h"
#include "Player.h"
#include "core/Renderer.h"
#include <iostream>
#include "tiny_obj_loader.h"

void Inimigo::Atualizar(float dt, const Player &player)
{
    // Movimento padrao (se move pelo eixo Z na direcao do jogador)
    posZ += velZ * dt;
    posX += velX * dt;

    // --- PAREDES INVISÍVEIS PARA INIMIGOS ---
    float limiteDireita = 250.0f;
    float limiteEsquerda = -250.0f;

    if (posX > limiteDireita)
    {
        posX = limiteDireita;
        velX = -velX; // Bate e volta (Efeito rebote)
    }
    else if (posX < limiteEsquerda)
    {
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

void VirusAlfa::Atualizar(float dt, const Player &player)
{
    // Chama a fisica basica das paredes
    Inimigo::Atualizar(dt, player);

    // Regra do Alfa: Mergulho Kamikaze (Boids Cerco)
    // Se ele chegar a uma certa distancia Z do jogador, ele acelera e vai pra cima
    float distZ = player.GetZ() - posZ;

    if (!emMergulho && distZ > 0 && distZ < 400.0f)
    {
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

void VirusAlfa::InicializarModelo()
{
    if (alfaCarregado)
        return;

    alfaTextureID = Renderer::LoadTexture("assets/textures/virus_alfa.png");

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/virus_alfa.obj", "assets/models/");
    // if (!warn.empty()) std::cout << "WARN Alfa: " << warn << std::endl; // Desativado pois o mtl nao e necessario
    if (!err.empty())
        std::cerr << "ERR Alfa: " << err << std::endl;
    if (!ret)
    {
        std::cerr << "Falha ao carregar modelo virus_alfa.obj!" << std::endl;
        return;
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t startIndex = alfaVertices.size(); // Salva onde o triangulo comecou

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
                alfaVertices.push_back(vertex);
            }
            index_offset += fv;

            // --- GERADOR DINAMICO DE NORMAIS (CROSS PRODUCT) ---
            // Se o modelo veio sem normais e lemos 3 vertices (um triangulo)
            if (fv == 3)
            {
                Vertex &v1 = alfaVertices[startIndex];
                Vertex &v2 = alfaVertices[startIndex + 1];
                Vertex &v3 = alfaVertices[startIndex + 2];

                if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f)
                {
                    float ux = v2.x - v1.x;
                    float uy = v2.y - v1.y;
                    float uz = v2.z - v1.z;
                    float vx = v3.x - v1.x;
                    float vy = v3.y - v1.y;
                    float vz = v3.z - v1.z;

                    // Produto Vetorial (Nx, Ny, Nz) perpendicular a face
                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;

                    // Normalizar o vetor
                    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
                    if (len > 0)
                    {
                        nx /= len;
                        ny /= len;
                        nz /= len;
                    }

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
    for (auto &v : alfaVertices)
    {
        if (v.x < minX)
            minX = v.x;
        if (v.x > maxX)
            maxX = v.x;
        if (v.y < minY)
            minY = v.y;
        if (v.y > maxY)
            maxY = v.y;
        if (v.z < minZ)
            minZ = v.z;
        if (v.z > maxZ)
            maxZ = v.z;
    }

    // Descobre o centro geometrico
    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;

    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0)
    {
        scale = 30.0f / width;
    }

    // Move os vertices para o centro e aplica a escala
    for (auto &v : alfaVertices)
    {
        v.x = (v.x - centroX) * scale;
        v.y = (v.y - centroY) * scale;
        v.z = (v.z - centroZ) * scale;
    }

    // --- COMPILACAO DA DISPLAY LIST (OTIMIZACAO DE PERFORMANCE) ---
    alfaDisplayListID = glGenLists(1);
    glNewList(alfaDisplayListID, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (const auto &v : alfaVertices)
    {
        glNormal3f(v.nx, v.ny, v.nz);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
    glEndList();

    alfaCarregado = true;
}

void VirusAlfa::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (alfaCarregado)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, alfaTextureID);

        // Configurando a reacao da textura a luz
        GLfloat mat_ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};  // Luz ambiente base alta para manter visibilidade
        GLfloat mat_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};  // Luz direta
        GLfloat mat_specular[] = {0.4f, 0.4f, 0.4f, 1.0f}; // Reflexo nas curvas
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialf(GL_FRONT, GL_SHININESS, 15.0f);

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        // Em vez de recalcular tudo vertice a vertice, usamos a memoria da GPU direto
        glCallList(alfaDisplayListID);
    }
    else
    {
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

BacteriaCoco::BacteriaCoco(float x, float y, float z) : Inimigo(x, y, z, 1)
{                                                    // 1 = Vermelho
    health = 40.0f;                                  // Tanque!
    velZ = 60.0f;                                    // Desce devagar
    velX = 0.0f;                                     // Linear
    raioColisao = 15.0f;                             // Espessura menor
    comprimentoZ = 35.0f;                            // Hitbox alongada como cilindro
    cooldownTiro = 1.0f + ((rand() % 100) / 100.0f); // Tiro entre 1 e 2 segs iniciais
    anguloRotacao = 0.0f;
    velocidadeRotacao = 60.0f; // Roda 60 graus por segundo (lenta e constante)
}

#include "Projectile.h"
extern std::vector<EnemyProjectile> enemyLasers; // Vector global para lidar com os tiros

void BacteriaCoco::Atualizar(float dt, const Player &player)
{
    Inimigo::Atualizar(dt, player);

    // Gira a bactéria
    anguloRotacao += velocidadeRotacao * dt;
    if (anguloRotacao >= 360.0f)
        anguloRotacao -= 360.0f;

    cooldownTiro -= dt;

    // Nao persegue em X, vai reto como um tanque.
    // Atira em direcao ao player se estiver na frente do player e perto o suficiente
    float distZ = player.GetZ() - posZ;
    if (distZ > 0 && distZ < 700.0f)
    { // Visível na tela
        if (cooldownTiro <= 0.0f)
        {
            cooldownTiro = 2.0f; // A cada 2 segundos, um tiro duplo

            // Atira dois esporos vermelhos
            enemyLasers.push_back(EnemyProjectile(posX - 10.0f, posY, posZ + 20.0f, 1));
            enemyLasers.push_back(EnemyProjectile(posX + 10.0f, posY, posZ + 20.0f, 1));
        }
    }
}

void BacteriaCoco::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (cocoCarregado)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, cocoTextureID);

        glEnable(GL_LIGHTING); // Volta a luz pra dar volume e reflexo
        glEnable(GL_COLOR_MATERIAL);

        // Iluminação fosca limpa copiada diretamente do Leukocyte Corrupto
        GLfloat mat_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
        GLfloat mat_emission[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Brilha um pouco
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

        // Cor branco puro para exibir a textura original da Bacteria Coco
        glColor3f(1.0f, 1.0f, 1.0f);

        glPushMatrix();
        glRotatef(anguloRotacao, 0.0f, 0.0f, 1.0f); // Rotação no eixo Z (Barrel Roll da Fisica do Jogo)
        glRotatef(-90.0f, 0.0f, 1.0f, 0.0f);        // Conserto invertido para botar caudas pro lado certo
        glCallList(cocoDisplayListID);
        glPopMatrix();

        // --- RESET MATERIAL PARA NÃO VAZAR PARA A NAVE ---
        GLfloat reset_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
        GLfloat reset_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
        GLfloat reset_specular[] = {0.0f, 0.0f, 0.0f, 1.0f};
        GLfloat reset_emission[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT, reset_ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, reset_diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, reset_specular);
        glMaterialfv(GL_FRONT, GL_EMISSION, reset_emission);
        glMaterialf(GL_FRONT, GL_SHININESS, 0.0f);
    }
    else
    {
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

void BacteriaCoco::InicializarModelo()
{
    cocoTextureID = Renderer::LoadTexture("assets/textures/bacteria_coco.png");
    if (cocoTextureID == 0)
    {
        std::cerr << "Falha ao carregar textura bacteria_coco.png!" << std::endl;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/bacteria_coco.obj", "assets/models/");
    if (!err.empty())
        std::cerr << "ERR Coco: " << err << std::endl;
    if (!ret)
    {
        std::cerr << "Falha ao carregar assets/models/bacteria_coco.obj!" << std::endl;
        return;
    }

    float minX = 9999, maxX = -9999;
    float minY = 9999, maxY = -9999;
    float minZ = 9999, maxZ = -9999;

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t startIndex = cocoVertices.size();
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
                cocoVertices.push_back(vertex);
            }
            index_offset += fv;

            // --- GERADOR DINAMICO DE NORMAIS (CROSS PRODUCT) ---
            if (fv == 3)
            {
                Vertex &v1 = cocoVertices[startIndex];
                Vertex &v2 = cocoVertices[startIndex + 1];
                Vertex &v3 = cocoVertices[startIndex + 2];

                if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f)
                {
                    float ux = v2.x - v1.x;
                    float uy = v2.y - v1.y;
                    float uz = v2.z - v1.z;
                    float vx = v3.x - v1.x;
                    float vy = v3.y - v1.y;
                    float vz = v3.z - v1.z;

                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;

                    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
                    if (len > 0)
                    {
                        nx /= len;
                        ny /= len;
                        nz /= len;
                    }

                    v1.nx = v2.nx = v3.nx = nx;
                    v1.ny = v2.ny = v3.ny = ny;
                    v1.nz = v2.nz = v3.nz = nz;
                }
            }
        }
    }

    for (auto &v : cocoVertices)
    {
        if (v.x < minX)
            minX = v.x;
        if (v.x > maxX)
            maxX = v.x;
        if (v.y < minY)
            minY = v.y;
        if (v.y > maxY)
            maxY = v.y;
        if (v.z < minZ)
            minZ = v.z;
        if (v.z > maxZ)
            maxZ = v.z;
    }

    // Restaurando Régua Geométrica (Bounding Box) pura
    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;

    // Compensação: Puxar a malha vermelha para trás na caixa, arrastando 10% do peso da cauda pro núcleo
    centroX += (maxX - minX) * 0.10f;

    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0)
    {
        scale = 130.0f / width; // Escala segura que não ultrapassa a hitbox
    }

    for (auto &v : cocoVertices)
    {
        v.x = (v.x - centroX) * scale;
        v.y = (v.y - centroY) * scale;
        v.z = (v.z - centroZ) * scale; // Zero offsets artificiais!
    }

    // OTIMIZACAO DISPLAY LIST
    cocoDisplayListID = glGenLists(1);
    glNewList(cocoDisplayListID, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (const auto &v : cocoVertices)
    {
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

LeukocyteCorrupto::LeukocyteCorrupto(float x, float y, float z) : Inimigo(x, y, z, 1)
{
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

void LeukocyteCorrupto::Atualizar(float dt, const Player &player)
{
    // Não chama o Atualizar do pai para não descer linearmente no eixo Z

    stateTimer += dt;
    attackCooldown -= dt;

    // Atualiza polaridade e mecânicas baseadas na Vida!
    float percentLife = health / maxHealth;
    if (percentLife > 0.6f)
    {
        bossState = 0; // Patrulha
        polarity = 1;  // Vermelho constante
    }
    else if (percentLife > 0.25f)
    {
        bossState = 1;                    // Espiral no centro
        polarity = (int)(stateTimer) % 2; // Pisca azul/vermelho por segundo
    }
    else
    {
        bossState = 2; // FÚRIA
        polarity = 1;  // Vermelho frenético
    }

    // FASE 0: PATRULHA SENOIDAL (0.6 - 1.0 hp)
    if (bossState == 0)
    {
        float baseZ = 350.0f;
        if (cameraMode != 3)
            baseZ *= 2.0f; // Afasta mais se não for top-down
        posZ = player.GetZ() - baseZ;

        posX = std::sin(stateTimer * 2.0f) * 200.0f; // Vai e vem suave
        posY = 0.0f;                                 // Fixo na mesma altura da nave

        if (attackCooldown <= 0.0f)
        {
            attackCooldown = 1.0f;
            // Atira mirando no jogador
            float dx = player.GetX() - posX;
            float dz = player.GetZ() - posZ;
            float mag = std::sqrt(dx * dx + dz * dz);
            if (mag > 0)
            {
                dx /= mag;
                dz /= mag;
            }
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, dx, 0.0f, dz, 200.0f, polarity));
        }
    }
    // FASE 1: ATAQUE EM ESPIRAL (0.25 - 0.6 hp)
    else if (bossState == 1)
    {
        float baseZ = 250.0f;
        if (cameraMode != 3)
            baseZ *= 2.0f;
        posZ = player.GetZ() - baseZ; // Se aproxima

        // Padrão infinito (Oito/Figure-8) varrendo a tela
        posX = std::sin(stateTimer * 2.5f) * 250.0f;
        posY = std::sin(stateTimer * 5.0f) * 15.0f; // Oscila de -15 até 15

        anguloEspiral += 120.0f * dt; // Gira rápido
        if (anguloEspiral > 360.0f)
            anguloEspiral -= 360.0f;

        if (attackCooldown <= 0.0f)
        {
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
    else if (bossState == 2)
    {
        float baseZ = 150.0f;
        if (cameraMode != 3)
            baseZ *= 2.0f;
        posZ = player.GetZ() - baseZ; // Fica cara a cara

        // Varredura lateral frenética + Tremor errático (Shake)
        float posicaoBaseX = std::sin(stateTimer * 6.0f) * 320.0f;
        posX = posicaoBaseX + ((rand() % 40) - 20) * 1.0f;
        posY = ((rand() % 20) - 10) * 1.0f; // Treme sempre pertinho do 0

        anguloEspiral += 50.0f * dt;

        if (attackCooldown <= 0.0f)
        {
            attackCooldown = 0.4f; // Explosão em 8 direções frequente
            for (int i = 0; i < 8; i++)
            {
                float rad = (anguloEspiral + (i * 45.0f)) * 3.14159f / 180.0f;
                // Os tiros alternam de polaridade para forçar o jogador a desviar
                int shotPol = i % 2;
                enemyLasers.push_back(EnemyProjectile(posX, 0.0f, posZ, std::sin(rad), 0.0f, std::cos(rad), 250.0f, shotPol));
            }
        }
    }
}

void LeukocyteCorrupto::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Cor pulsante dependendo da polaridade e fase
    if (polarity == 0)
    {
        glColor3f(0.0f, 0.5f, 1.0f); // Azul
    }
    else
    {
        glColor3f(1.0f, 0.1f, 0.1f); // Vermelho
    }

    GLfloat mat_ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_emission[] = {0.2f, 0.2f, 0.2f, 1.0f}; // Brilha um pouco
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

    // Gira no eixo dependendo da fase
    if (bossState == 1 || bossState == 2)
    {
        glRotatef(anguloEspiral, 0.0f, 1.0f, 0.0f); // Gira como um liquidificador
    }
    else
    {
        glRotatef(stateTimer * 40.0f, 1.0f, 1.0f, 0.0f); // Rola suavemente
    }

    // Aplica o material e textura do modelo OBJ
    if (bossCarregado)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, bossTextureID);
        glCallList(bossDisplayListID);
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
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

void LeukocyteCorrupto::Destruir()
{
    ativo = false;
    // Tocar Som de Morte Gigante
}

// Variáveis estáticas do Boss
std::vector<Vertex> LeukocyteCorrupto::bossVertices;
GLuint LeukocyteCorrupto::bossTextureID = 0;

void LeukocyteCorrupto::InicializarModelo()
{
    bossTextureID = Renderer::LoadTexture("assets/textures/leukocyte_corrupto_tex.png");
    if (bossTextureID == 0)
    {
        std::cerr << "Falha ao carregar textura leukocyte_corrupto_tex.png!" << std::endl;
    }

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/leukocyte_corrupto.obj", "assets/models/");
    if (!err.empty())
        std::cerr << "ERR Boss: " << err << std::endl;
    if (!ret)
    {
        std::cerr << "Falha ao carregar modelo leukocyte_corrupto.obj!" << std::endl;
        return;
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t startIndex = bossVertices.size();

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
                    // Inverte o V pois OpenGL (imagem) vs Blender (obj) geralmente difere
                    vertex.v = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else
                {
                    vertex.u = vertex.v = 0.0f;
                }
                bossVertices.push_back(vertex);
            }
            index_offset += fv;

            // --- GERADOR DINAMICO DE NORMAIS (CROSS PRODUCT) ---
            if (fv == 3)
            {
                Vertex &v1 = bossVertices[startIndex];
                Vertex &v2 = bossVertices[startIndex + 1];
                Vertex &v3 = bossVertices[startIndex + 2];

                if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f)
                {
                    float ux = v2.x - v1.x;
                    float uy = v2.y - v1.y;
                    float uz = v2.z - v1.z;
                    float vx = v3.x - v1.x;
                    float vy = v3.y - v1.y;
                    float vz = v3.z - v1.z;

                    float nx = uy * vz - uz * vy;
                    float ny = uz * vx - ux * vz;
                    float nz = ux * vy - uy * vx;

                    float len = std::sqrt(nx * nx + ny * ny + nz * nz);
                    if (len > 0)
                    {
                        nx /= len;
                        ny /= len;
                        nz /= len;
                    }

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
    for (auto &v : bossVertices)
    {
        if (v.x < minX)
            minX = v.x;
        if (v.x > maxX)
            maxX = v.x;
        if (v.y < minY)
            minY = v.y;
        if (v.y > maxY)
            maxY = v.y;
        if (v.z < minZ)
            minZ = v.z;
        if (v.z > maxZ)
            maxZ = v.z;
    }

    float centroX = (minX + maxX) / 2.0f;
    float centroY = (minY + maxY) / 2.0f;
    float centroZ = (minZ + maxZ) / 2.0f;

    float width = maxX - minX;
    float scale = 1.0f;
    if (width > 0)
    {
        scale = 100.0f / width; // Tamanho do Boss = 100 (bem maior que os virus comuns de 30)
    }

    for (auto &v : bossVertices)
    {
        v.x = (v.x - centroX) * scale;
        v.y = (v.y - centroY) * scale;
        v.z = (v.z - centroZ) * scale;
    }

    bossDisplayListID = glGenLists(1);
    glNewList(bossDisplayListID, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    for (const auto &v : bossVertices)
    {
        glNormal3f(v.nx, v.ny, v.nz);
        glTexCoord2f(v.u, v.v);
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();
    glEndList();

    bossCarregado = true;
}

// ==============================================================================
// VÍRUS GAMA (BIPOLAR)
// ==============================================================================
std::vector<Vertex> VirusGama::gamaVertices;
GLuint VirusGama::gamaTextureID = 0;
GLuint VirusGama::gamaDisplayListID = 0;
bool VirusGama::gamaCarregado = false;

VirusGama::VirusGama(float x, float y, float z)
    : Inimigo(x, y, z, 0)
{ // Nasce Azul
    health = 15.0f;
    raioColisao = 15.0f; // Reduzido (era 25.0f)
    timerBipolar = 3.0f;
    fireTimer = 1.0f + (rand() % 100) / 50.0f;
}

void VirusGama::Atualizar(float dt, const Player &player)
{
    posZ += velZ * dt;

    // Troca polaridade a cada 3 segundos
    timerBipolar -= dt;
    if (timerBipolar <= 0.0f)
    {
        polarity = 1 - polarity;
        timerBipolar = 3.0f;
    }

    // Atirar projéteis da cor atual
    fireTimer -= dt;
    if (fireTimer <= 0.0f && posZ > player.GetZ() - 600.0f)
    {
        float dx = player.GetX() - posX;
        float dz = player.GetZ() - posZ;
        float dist = std::sqrt(dx * dx + dz * dz);
        if (dist > 0)
        {
            dx /= dist;
            dz /= dist;
        }
        else
        {
            dx = 0;
            dz = 1;
        }

        // Dispara tiro rápido mirado no player usando a cor atual
        extern std::vector<EnemyProjectile> enemyLasers;
        enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, dx, 0.0f, dz, 250.0f, polarity));

        fireTimer = 1.5f + (rand() % 100) / 100.0f; // Atira a cada ~1.5 a 2.5s
    }
}

void VirusGama::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    if (gamaCarregado)
    {
        // Desligamos a textura para evitar multiplicação por preto e renderizar a geometria pura!
        glDisable(GL_TEXTURE_2D);

        // Desliga a iluminação completamente para garantir o Azul Brilhante e Vermelho Vívido puros,
        // sem que as sombras do jogo afetem a cor.
        glDisable(GL_LIGHTING);

        if (polarity == 0)
        {
            glColor3f(0.0f, 0.0f, 1.0f); // Azul Escuro Puro (igual ao Esporo Fúngico)
        }
        else
        {
            glColor3f(1.0f, 0.1f, 0.1f); // Vermelho Vívido e Puro
        }

        glCallList(gamaDisplayListID);

        // Religa a luz e a textura para manter os outros objetos perfeitos
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glutSolidCube(30.0);
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
        glEnable(GL_LIGHTING);
    }

    glPopMatrix();
}

void VirusGama::InicializarModelo()
{
    gamaTextureID = Renderer::LoadTexture("assets/textures/virus_gama_tex.png");
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/virus_gama.obj", "assets/models/"))
    {
        float minX = 9999, maxX = -9999;
        float minY = 9999, maxY = -9999;
        float minZ = 9999, maxZ = -9999;
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                int fv = shapes[s].mesh.num_face_vertices[f];
                for (size_t v = 0; v < (size_t)fv; v++)
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
                    if (idx.texcoord_index >= 0)
                    {
                        vertex.u = attrib.texcoords[2 * idx.texcoord_index + 0];
                        vertex.v = attrib.texcoords[2 * idx.texcoord_index + 1];
                    }
                    gamaVertices.push_back(vertex);
                    if (vertex.x < minX)
                        minX = vertex.x;
                    if (vertex.x > maxX)
                        maxX = vertex.x;
                    if (vertex.y < minY)
                        minY = vertex.y;
                    if (vertex.y > maxY)
                        maxY = vertex.y;
                    if (vertex.z < minZ)
                        minZ = vertex.z;
                    if (vertex.z > maxZ)
                        maxZ = vertex.z;
                }
                index_offset += fv;
            }
        }
        float scale = 30.0f / (maxX - minX + 0.1f);
        float centroX = (maxX + minX) / 2.0f;
        float centroY = (maxY + minY) / 2.0f;
        float centroZ = (maxZ + minZ) / 2.0f;

        gamaDisplayListID = glGenLists(1);
        glNewList(gamaDisplayListID, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (auto &v : gamaVertices)
        {
            v.x = (v.x - centroX) * scale;
            v.y = (v.y - centroY) * scale;
            v.z = (v.z - centroZ) * scale;
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
        glEndList();
        gamaCarregado = true;
    }
}

// ==============================================================================
// ESPORO FÚNGICO (KAMIKAZE)
// ==============================================================================
std::vector<Vertex> EsporoFungico::esporoVertices;
GLuint EsporoFungico::esporoTextureID = 0;
GLuint EsporoFungico::esporoDisplayListID = 0;
bool EsporoFungico::esporoCarregado = false;

EsporoFungico::EsporoFungico(float x, float y, float z) : Inimigo(x, y, z, 0)
{
    health = 10.0f;      // Fraco, mas rapido
    velZ = 380.0f;       // Kamikaze ULTRA RAPIDO
    raioColisao = 10.0f; // Reduzido (era 15.0f)
    anguloGiro = 0.0f;
}

void EsporoFungico::Atualizar(float dt, const Player &player)
{
    posZ += velZ * dt;
    anguloGiro += 700.0f * dt; // Rotação louca

    // IA melhorada: Persegue ativamente o jogador em X e Y
    float dirX = player.GetX() - posX;
    float dirY = player.GetY() - posY;
    float dist = std::sqrt(dirX * dirX + dirY * dirY);

    if (dist > 0.1f) {
        posX += (dirX / dist) * 150.0f * dt;
        posY += (dirY / dist) * 150.0f * dt;
    }
}

void EsporoFungico::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    glRotatef(anguloGiro, 0.0f, 0.0f, 1.0f); // Gira rápido

    if (esporoCarregado)
    {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, esporoTextureID);
        glColor3f(1.0f, 1.0f, 1.0f);

        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);

        // Emissão leve (esporo toxico)
        GLfloat mat_emission[] = {0.2f, 0.5f, 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

        glCallList(esporoDisplayListID);

        GLfloat zero_em[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, zero_em);
    }
    else
    {
        glColor3f(0.0f, 0.0f, 1.0f);
        glPushMatrix();
        glScalef(15.0f, 15.0f, 15.0f);
        glutSolidIcosahedron();
        glPopMatrix();
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
        glEnable(GL_LIGHTING);
    }
    glPopMatrix();
}
void EsporoFungico::Destruir() { ativo = false; }

void EsporoFungico::InicializarModelo()
{
    esporoTextureID = Renderer::LoadTexture("assets/textures/esporo_fungico_tex.png");

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/esporo_fungico.obj", "assets/models/"))
    {
        float minX = 9999, maxX = -9999;
        float minY = 9999, maxY = -9999;
        float minZ = 9999, maxZ = -9999;
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                int fv = shapes[s].mesh.num_face_vertices[f];
                for (size_t v = 0; v < (size_t)fv; v++)
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
                    esporoVertices.push_back(vertex);
                    if (vertex.x < minX)
                        minX = vertex.x;
                    if (vertex.x > maxX)
                        maxX = vertex.x;
                    if (vertex.y < minY)
                        minY = vertex.y;
                    if (vertex.y > maxY)
                        maxY = vertex.y;
                    if (vertex.z < minZ)
                        minZ = vertex.z;
                    if (vertex.z > maxZ)
                        maxZ = vertex.z;
                }
                index_offset += fv;
            }
        }
        float scale = 15.0f / (maxX - minX + 0.1f);
        float centroX = (maxX + minX) / 2.0f;
        float centroY = (maxY + minY) / 2.0f;
        float centroZ = (maxZ + minZ) / 2.0f;

        esporoDisplayListID = glGenLists(1);
        glNewList(esporoDisplayListID, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (auto &v : esporoVertices)
        {
            v.x = (v.x - centroX) * scale;
            v.y = (v.y - centroY) * scale;
            v.z = (v.z - centroZ) * scale;
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
        glEndList();
        esporoCarregado = true;
    }
    else
    {
        std::cout << "Aviso: Falha ao carregar esporo_fungico.obj" << std::endl;
        esporoCarregado = false;
    }
}

// ==============================================================================
// BOSS 2: PNEUMOCOCO GIGANTE
// ==============================================================================
std::vector<Vertex> PneumococoGigante::pneuVertices;
GLuint PneumococoGigante::pneuTextureID = 0;
GLuint PneumococoGigante::pneuDisplayListID = 0;
bool PneumococoGigante::pneuCarregado = false;

PneumococoGigante::PneumococoGigante(float x, float y, float z) : Inimigo(x, y, z, 1)
{
    maxHealth = 600.0f;
    health = maxHealth;
    raioColisao = 45.0f;
    cicloInspiracao = 4.0f; 
    isInspirando = true;    
    attackTimer = 0.0f;
    attackAngle = 0.0f;
}

void PneumococoGigante::Atualizar(float dt, const Player &player)
{
    // Verifica estagio atual baseado no HP (maxHealth é 600)
    int estagio = 1;
    if (health <= 400.0f && health > 200.0f) estagio = 2;
    else if (health <= 200.0f) estagio = 3;

    float baseCicloTempo = 4.0f;
    float amplitudeX = 120.0f;

    if (estagio == 2) {
        baseCicloTempo = 3.0f;
        amplitudeX = 160.0f;
    } else if (estagio == 3) {
        baseCicloTempo = 2.0f;
        amplitudeX = 200.0f;
    }

    // Boss acompanha a nave no eixo Z, mas bem mais afastado para dar tempo de reação
    extern int cameraMode;
    float baseZ = 350.0f; // Aumentado de 200 para 350
    if (cameraMode != 3) baseZ *= 2.0f;
    posZ = player.GetZ() - baseZ;

    // Patrulha em X e Oscilação em Y
    posX = std::sin(cicloInspiracao * 3.0f) * amplitudeX;
    posY = 0.0f + std::sin(cicloInspiracao * 2.0f) * 20.0f;

    cicloInspiracao -= dt;
    if (cicloInspiracao <= 0.0f)
    {
        isInspirando = !isInspirando;
        cicloInspiracao = baseCicloTempo;
    }

    if (isInspirando)
        raioColisao = 45.0f; // Hitbox reduzida blindada
    else
        raioColisao = 30.0f; // Hitbox reduzida vulneravel

    // Lógica Bullet-Hell (3 Fases)
    attackTimer -= dt;
    attackAngle += 90.0f * dt;

    if (attackTimer <= 0.0f) {
        extern std::vector<EnemyProjectile> enemyLasers;
        
        if (estagio == 1) {
            attackTimer = 0.8f; // Muito tempo de reação
            // Dois lasers retos
            enemyLasers.push_back(EnemyProjectile(posX - 15.0f, posY, posZ, 0.0f, 0.0f, 1.0f, 200.0f, 0)); // Azul
            enemyLasers.push_back(EnemyProjectile(posX + 15.0f, posY, posZ, 0.0f, 0.0f, 1.0f, 200.0f, 1)); // Vermelho
        }
        else if (estagio == 2) {
            attackTimer = 0.4f; // Aumenta a cadência
            // Leque de 3 lasers
            float radC = 0.0f;
            float radL = -15.0f * 3.14159f / 180.0f; // Esquerda
            float radR = 15.0f * 3.14159f / 180.0f;  // Direita
            
            int polPrincipal = isInspirando ? 1 : 0;
            
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(radL), 0.0f, std::cos(radL), 220.0f, polPrincipal));
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(radC), 0.0f, std::cos(radC), 220.0f, 1 - polPrincipal));
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(radR), 0.0f, std::cos(radR), 220.0f, polPrincipal));
        }
        else if (estagio == 3) {
            attackTimer = 0.2f; // Frenético!
            // Espiral rotativa Bullet-Hell
            float rad1 = attackAngle * 3.14159f / 180.0f;
            float rad2 = (attackAngle + 180.0f) * 3.14159f / 180.0f;
            
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(rad1), 0.0f, std::cos(rad1), 250.0f, 0));
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(rad2), 0.0f, std::cos(rad2), 250.0f, 1));
            
            if (!isInspirando) {
                // Dobra a espiral se vulnerável
                float rad3 = (attackAngle + 90.0f) * 3.14159f / 180.0f;
                float rad4 = (attackAngle + 270.0f) * 3.14159f / 180.0f;
                enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(rad3), 0.0f, std::cos(rad3), 250.0f, 1));
                enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(rad4), 0.0f, std::cos(rad4), 250.0f, 0));
            }
        }
    }
}
void PneumococoGigante::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);

    float t = (4.0f - cicloInspiracao) / 4.0f; // 0.0 a 1.0
    float oscilacao = std::sin(t * 3.14159f); 
    
    // Animação de pulsação orgânica! Infla brutalmente se blindado, diminui se fraco
    float scaleAnim = isInspirando ? (1.6f + oscilacao * 0.15f) : (1.0f + oscilacao * 0.1f);
    
    glPushMatrix(); // Isola a escala do modelo
    glScalef(scaleAnim, scaleAnim, scaleAnim);

    if (pneuCarregado)
    {
        // Desligamos texturas e iluminação para que o verde neon domine
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        
        if (isInspirando) {
            glColor3f(0.0f, 0.4f, 0.0f); // Verde Escuro Forte
        } else {
            glColor3f(0.2f, 0.9f, 0.1f); // Verde Claro/Limão
        }

        glCallList(pneuDisplayListID);
        
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidSphere(50.0f, 20, 20); 
    }
    glPopMatrix(); // Fim da escala do modelo

    extern bool showCollisionBoxes;
    if (showCollisionBoxes)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
        glLineWidth(2.0f);
        glutWireSphere(raioColisao, 16, 16); 
        glLineWidth(1.0f);
    }
    glPopMatrix(); // Fim do Translado
}
void PneumococoGigante::Destruir() { ativo = false; }
void PneumococoGigante::TomarDano(float dano)
{
    if (!isInspirando)
    {
        health -= dano;
        if (health <= 0.0f)
            ativo = false;
    }
}
void PneumococoGigante::InicializarModelo()
{
    pneuTextureID = Renderer::LoadTexture("assets/textures/pneumococo_gigante_tex.png");
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/pneumococo_gigante.obj", "assets/models/"))
    {
        float minX = 9999, maxX = -9999;
        float minY = 9999, maxY = -9999;
        float minZ = 9999, maxZ = -9999;
        for (size_t s = 0; s < shapes.size(); s++)
        {
            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
            {
                int fv = shapes[s].mesh.num_face_vertices[f];
                size_t startIndex = pneuVertices.size();
                for (size_t v = 0; v < (size_t)fv; v++)
                {
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
                    pneuVertices.push_back(vertex);
                    if (vertex.x < minX) minX = vertex.x;
                    if (vertex.x > maxX) maxX = vertex.x;
                    if (vertex.y < minY) minY = vertex.y;
                    if (vertex.y > maxY) maxY = vertex.y;
                    if (vertex.z < minZ) minZ = vertex.z;
                    if (vertex.z > maxZ) maxZ = vertex.z;
                }
                index_offset += fv;
                
                // Cross product normal fallback
                if (fv == 3)
                {
                    Vertex &v1 = pneuVertices[startIndex];
                    Vertex &v2 = pneuVertices[startIndex + 1];
                    Vertex &v3 = pneuVertices[startIndex + 2];
                    if (v1.nx == 0.0f && v1.ny == 0.0f && v1.nz == 0.0f)
                    {
                        float ux = v2.x - v1.x; float uy = v2.y - v1.y; float uz = v2.z - v1.z;
                        float vx = v3.x - v1.x; float vy = v3.y - v1.y; float vz = v3.z - v1.z;
                        float nx = uy*vz - uz*vy; float ny = uz*vx - ux*vz; float nz = ux*vy - uy*vx;
                        float len = std::sqrt(nx*nx + ny*ny + nz*nz);
                        if (len > 0) { nx/=len; ny/=len; nz/=len; }
                        v1.nx = v2.nx = v3.nx = nx;
                        v1.ny = v2.ny = v3.ny = ny;
                        v1.nz = v2.nz = v3.nz = nz;
                    }
                }
            }
        }
        
        float scale = 50.0f / (maxX - minX + 0.1f); // Raio 50 natural
        float centroX = (maxX + minX) / 2.0f;
        float centroY = (maxY + minY) / 2.0f;
        float centroZ = (maxZ + minZ) / 2.0f;

        pneuDisplayListID = glGenLists(1);
        glNewList(pneuDisplayListID, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (auto &v : pneuVertices)
        {
            v.x = (v.x - centroX) * scale;
            v.y = (v.y - centroY) * scale;
            v.z = (v.z - centroZ) * scale;
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
        glEndList();
        pneuCarregado = true;
    }
    else
    {
        std::cout << "Aviso: Falha ao carregar pneumococo_gigante.obj" << std::endl;
        pneuCarregado = false;
    }
}

// --------------------------------------------------------------------------------------
// FASE 3: PRÍON MIMÉTICO
// --------------------------------------------------------------------------------------
std::vector<Vertex> PrionMimetico::prionVertices;
GLuint PrionMimetico::prionTextureID = 0;
GLuint PrionMimetico::prionDisplayListID = 0;
bool PrionMimetico::prionCarregado = false;

PrionMimetico::PrionMimetico(float startX, float startY, float startZ)
    : Inimigo(startX, startY, startZ, 0)
{
    health = 30.0f;
    raioColisao = 20.0f;
    comprimentoZ = 20.0f;
    maxBufferSize = 90; // 1.5 seconds at 60 FPS
    attackTimer = 0.0f;
    anguloGiro = 0.0f;
}

void PrionMimetico::Atualizar(float dt, const Player& player)
{
    anguloGiro += 60.0f * dt; // Rotação lenta
    
    // Grava a posicao atual do jogador
    posHistory.push_back({player.GetX(), player.GetZ()});
    
    // Diminui o delay se a vida cair
    if (health < 15.0f) {
        maxBufferSize = 48; // 0.8 seconds at 60 FPS
    }
    
    // Trava o Z permanentemente na frente da camera, mesmo antes do buffer encher
    posZ = player.GetZ() - 400.0f; 

    // Puxa a posicao atrasada
    if (posHistory.size() > (size_t)maxBufferSize) {
        auto oldPos = posHistory.front();
        posHistory.pop_front();
        
        // Copia a posicao X do passado
        posX = oldPos.first;
        
        // posY copia do player
        posY = player.GetY();
    }
    
    // Polaridade oposta
    polarity = 1 - player.GetPolarity();
    
    // Ataque
    attackTimer += dt;
    if (attackTimer >= 1.0f) {
        attackTimer = 0.0f;
        extern std::vector<EnemyProjectile> enemyLasers;
        // Atira em direcao ao jogador
        float dx = player.GetX() - posX;
        float dz = player.GetZ() - posZ;
        float mag = std::sqrt(dx*dx + dz*dz);
        if (mag > 0.1f) {
            dx /= mag;
            dz /= mag;
        }
        float speed = 150.0f;
        enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, dx, 0.0f, dz, speed, polarity));
    }
}

void PrionMimetico::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    
    glPushMatrix(); // Matrix para o modelo (isolando rotação)
    // Vira pra frente do player (na verdade, vira no sentido -Z)
    glRotatef(180.0f, 0, 1, 0);
    // Rolagem longitudinal lenta sobre o eixo X (pião horizontal)
    glRotatef(anguloGiro, 1.0f, 0.0f, 0.0f);

    if (prionCarregado) {
        // Desligamos a luz e a textura para forçar geometria pura de Neon
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        
        // Aplica tom de cor puro baseado na polaridade (Cores Escuras/Sombrias)
        if (polarity == 0) glColor3f(0.0f, 0.2f, 0.6f); // Azul-Marinho Escuro
        else glColor3f(0.6f, 0.0f, 0.0f); // Vermelho Sangue
        
        glCallList(prionDisplayListID);
        
        // Restaura
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    } else {
        if (polarity == 0) glColor3f(0.0f, 0.0f, 1.0f);
        else glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidSphere(20.0f, 16, 16);
    }
    glPopMatrix(); // Fim da matrix do modelo
    
    extern bool showCollisionBoxes;
    if (showCollisionBoxes)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
        glLineWidth(2.0f);
        glutWireSphere(raioColisao, 16, 16); 
        glLineWidth(1.0f);
    }
    glPopMatrix(); // Fim da matrix de translação
}

void PrionMimetico::Destruir() { ativo = false; }

void PrionMimetico::InicializarModelo()
{
    if (prionCarregado) return;
    
    prionTextureID = Renderer::LoadTexture("assets/textures/prion.png");
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/prion.obj")) {
        float minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9, minZ = 1e9, maxZ = -1e9;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex v;
                v.x = attrib.vertices[3 * index.vertex_index + 0];
                v.y = attrib.vertices[3 * index.vertex_index + 1];
                v.z = attrib.vertices[3 * index.vertex_index + 2];
                if (index.normal_index >= 0) {
                    v.nx = attrib.normals[3 * index.normal_index + 0];
                    v.ny = attrib.normals[3 * index.normal_index + 1];
                    v.nz = attrib.normals[3 * index.normal_index + 2];
                }
                if (index.texcoord_index >= 0) {
                    v.u = attrib.texcoords[2 * index.texcoord_index + 0];
                    v.v = attrib.texcoords[2 * index.texcoord_index + 1];
                }
                prionVertices.push_back(v);
                
                if (v.x < minX) minX = v.x;
                if (v.x > maxX) maxX = v.x;
                if (v.y < minY) minY = v.y;
                if (v.y > maxY) maxY = v.y;
                if (v.z < minZ) minZ = v.z;
                if (v.z > maxZ) maxZ = v.z;
            }
        }
        float scale = 40.0f / (maxX - minX + 0.1f);
        float cx = (maxX + minX) / 2.0f;
        float cy = (maxY + minY) / 2.0f;
        float cz = (maxZ + minZ) / 2.0f;
        
        prionDisplayListID = glGenLists(1);
        glNewList(prionDisplayListID, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (auto& v : prionVertices) {
            v.x = (v.x - cx) * scale;
            v.y = (v.y - cy) * scale;
            v.z = (v.z - cz) * scale;
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
        glEndList();
        prionCarregado = true;
    }
}

// ==============================================================================
// VÍRUS DELTA (Hacker Bipolar - Espelho)
// ==============================================================================
std::vector<Vertex> VirusDelta::deltaVertices;
GLuint VirusDelta::deltaTextureID = 0;
GLuint VirusDelta::deltaDisplayListID = 0;
bool VirusDelta::deltaCarregado = false;

VirusDelta::VirusDelta(float startY, float startZ, int pol) : Inimigo(0.0f, startY, startZ, pol)
{
    health = 20.0f;
    raioColisao = 25.0f;
    velZ = 100.0f;
    velX = 0.0f;
    comprimentoZ = 20.0f;
    attackTimer = 0.0f;
    anguloGiro = 0.0f;
}

void VirusDelta::Atualizar(float dt, const Player& player)
{
    // Espelha o X do jogador (invertido)
    posX = -player.GetX();
    
    // Y do jogador
    posY = player.GetY();
    
    // Z se aproxima normalmente
    posZ += velZ * dt;
    
    // Giro agressivo no eixo Z
    anguloGiro += 300.0f * dt;
    
    // Ataque
    attackTimer += dt;
    if (attackTimer >= 1.5f) {
        attackTimer = 0.0f;
        extern std::vector<EnemyProjectile> enemyLasers;
        
        // Atira em cruz ou spread
        float speed = 180.0f;
        // Tiro central em direção à tela
        enemyLasers.push_back(EnemyProjectile(posX, posY, posZ + 20.0f, 0.0f, 0.0f, 1.0f, speed, polarity));
        // Tiros diagonais
        enemyLasers.push_back(EnemyProjectile(posX, posY, posZ + 20.0f, 0.5f, 0.0f, 0.866f, speed, polarity));
        enemyLasers.push_back(EnemyProjectile(posX, posY, posZ + 20.0f, -0.5f, 0.0f, 0.866f, speed, polarity));
    }
}

void VirusDelta::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    
    glPushMatrix(); // Matrix para o modelo
    // Gira no eixo Z (como um shuriken/helicóptero)
    glRotatef(anguloGiro, 0.0f, 0.0f, 1.0f);

    if (deltaCarregado) {
        // Neon sombrio idêntico ao Príon Mimético
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        
        if (polarity == 0) glColor3f(0.0f, 0.2f, 0.6f); // Azul-Marinho
        else glColor3f(0.6f, 0.0f, 0.0f); // Vermelho Sangue
        
        glCallList(deltaDisplayListID);
        
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
    } else {
        if (polarity == 0) glColor3f(0.0f, 0.0f, 1.0f);
        else glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(20.0f);
    }
    glPopMatrix(); // Fim da matrix do modelo
    
    extern bool showCollisionBoxes;
    if (showCollisionBoxes)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
        glLineWidth(2.0f);
        glutWireSphere(raioColisao, 16, 16); 
        glLineWidth(1.0f);
    }
    glPopMatrix(); // Fim da matrix de translação
}

void VirusDelta::Destruir() { ativo = false; }

void VirusDelta::InicializarModelo()
{
    if (deltaCarregado) return;
    
    deltaTextureID = Renderer::LoadTexture("assets/textures/delta.png");
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/delta.obj")) {
        float minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9, minZ = 1e9, maxZ = -1e9;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex v;
                v.x = attrib.vertices[3 * index.vertex_index + 0];
                v.y = attrib.vertices[3 * index.vertex_index + 1];
                v.z = attrib.vertices[3 * index.vertex_index + 2];
                if (index.normal_index >= 0) {
                    v.nx = attrib.normals[3 * index.normal_index + 0];
                    v.ny = attrib.normals[3 * index.normal_index + 1];
                    v.nz = attrib.normals[3 * index.normal_index + 2];
                } else {
                    v.nx = 0.0f; v.ny = 1.0f; v.nz = 0.0f;
                }
                if (index.texcoord_index >= 0) {
                    v.u = attrib.texcoords[2 * index.texcoord_index + 0];
                    v.v = attrib.texcoords[2 * index.texcoord_index + 1];
                } else {
                    v.u = 0.0f; v.v = 0.0f;
                }
                deltaVertices.push_back(v);
                
                if (v.x < minX) minX = v.x;
                if (v.x > maxX) maxX = v.x;
                if (v.y < minY) minY = v.y;
                if (v.y > maxY) maxY = v.y;
                if (v.z < minZ) minZ = v.z;
                if (v.z > maxZ) maxZ = v.z;
            }
        }
        float scale = 40.0f / (maxX - minX + 0.1f);
        float cx = (maxX + minX) / 2.0f;
        float cy = (maxY + minY) / 2.0f;
        float cz = (maxZ + minZ) / 2.0f;
        
        deltaDisplayListID = glGenLists(1);
        glNewList(deltaDisplayListID, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (auto& v : deltaVertices) {
            v.x = (v.x - cx) * scale;
            v.y = (v.y - cy) * scale;
            v.z = (v.z - cz) * scale;
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
        glEndList();
        deltaCarregado = true;
    }
}

// ==============================================================================
// BOSS 3: NEXUS OMEGA
// ==============================================================================
std::vector<Vertex> NexusOmega::nexusVertices;
GLuint NexusOmega::nexusTextureID = 0;
GLuint NexusOmega::nexusDisplayListID = 0;
bool NexusOmega::nexusCarregado = false;

NexusOmega::NexusOmega(float x, float y, float z) : Inimigo(x, y, z, 1)
{
    maxHealth = 6000.0f; // Buff massivo: de 1200 para 6000!
    health = maxHealth;
    raioColisao = 40.0f; // Hitbox reduzida, exigindo precisão no núcleo
    velZ = 0.0f;
    velX = 0.0f;
    comprimentoZ = 80.0f;
    attackTimer = 0.0f;
    attackAngle = 0.0f;
    moveAngle = 0.0f;
}

void NexusOmega::Atualizar(float dt, const Player& player)
{
    // Movimentação em Oito (Lissajous curve) majestosa e fluida
    moveAngle += 40.0f * dt;
    float rad = moveAngle * 3.14159f / 180.0f;
    
    // Determina o estágio atual
    int estagio = 1;
    if (health <= 4000.0f && health > 1500.0f) estagio = 2;
    else if (health <= 1500.0f) estagio = 3;

    // Lógica de Movimentação baseada no Estágio
    if (estagio == 1) {
        posX = std::sin(rad) * 150.0f;
    } else if (estagio == 2) {
        // Oito mais rápido e começa a seguir levemente o player
        moveAngle += 20.0f * dt;
        posX = std::sin(rad) * 200.0f;
        posX += (player.GetX() - posX) * 0.5f * dt; // Segue lentamente
    } else {
        // Estágio 3: Fúria! Segue agressivamente o player no eixo X
        moveAngle += 40.0f * dt;
        posX += (player.GetX() - posX) * 1.5f * dt;
    }
    
    extern int cameraMode;
    float baseZ = 400.0f;
    if (cameraMode != 3) baseZ *= 2.0f;
    
    // Z oscila levemente, mas fica mais próximo no estágio 3
    float zOscillation = std::cos(rad * 0.5f) * (estagio == 3 ? 100.0f : 50.0f);
    posZ = player.GetZ() - baseZ + zOscillation;

    // Bullet-Hell Extremo (Fase Final)
    attackTimer -= dt;
    attackAngle += (60.0f + estagio * 40.0f) * dt; // Rotação dos lasers fica insana

    if (attackTimer <= 0.0f) {
        extern std::vector<EnemyProjectile> enemyLasers;
        
        if (estagio == 1) {
            attackTimer = 0.5f; // Cadência aumentada
            // Estrela de 5 pontas dupla rápida
            for (int i = 0; i < 5; i++) {
                float a1 = (attackAngle + i * 72.0f) * 3.14159f / 180.0f;
                float a2 = (attackAngle + i * 72.0f + 36.0f) * 3.14159f / 180.0f;
                enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(a1), 0.0f, std::cos(a1), 300.0f, 0)); // Azul
                enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(a2), 0.0f, std::cos(a2), 300.0f, 1)); // Vermelho
            }
        } 
        else if (estagio == 2) {
            attackTimer = 0.8f; 
            // Paredes Intercaladas de Lasers
            int pol = (int(attackAngle) / 90) % 2 == 0 ? 0 : 1; // Alterna a polaridade
            for (int i = -4; i <= 4; i++) {
                // Dispara uma linha reta formando uma parede
                float dx = i * 0.2f;
                enemyLasers.push_back(EnemyProjectile(posX + i * 20.0f, posY, posZ + 20.0f, dx, 0.0f, 1.0f, 350.0f, pol));
            }
        } 
        else if (estagio == 3) {
            attackTimer = 0.1f; // Metralhadora absurda!
            
            // Dupla Espiral
            float a3 = attackAngle * 2.0f * 3.14159f / 180.0f;
            float a4 = (attackAngle * 2.0f + 180.0f) * 3.14159f / 180.0f;
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(a3), 0.0f, std::cos(a3), 400.0f, 1));
            enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, std::sin(a4), 0.0f, std::cos(a4), 400.0f, 0));
            
            // Ocasionalmente solta tiros focados rápidos
            if (rand() % 10 == 0) {
                float px = player.GetX() - posX;
                float pz = player.GetZ() - posZ;
                float mag = std::sqrt(px*px + pz*pz);
                if (mag > 0.1f) { px /= mag; pz /= mag; }
                enemyLasers.push_back(EnemyProjectile(posX, posY, posZ, px, 0.0f, pz, 600.0f, rand()%2));
            }
        }
    }
}

void NexusOmega::Desenhar()
{
    glPushMatrix();
    glTranslatef(posX, posY, posZ);
    
    glPushMatrix(); // Matrix para o modelo
    // Gira lentamente em torno de si mesmo
    glRotatef(moveAngle * 0.5f, 0, 1, 0);
    glRotatef(std::sin(moveAngle * 3.14159f / 180.0f) * 15.0f, 1, 0, 0); // Inclinação

    if (nexusCarregado) {
        glEnable(GL_LIGHTING);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nexusTextureID);
        
        // Material Metálico Especular!
        GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat mat_shininess[] = { 100.0f }; // Alto brilho metálico
        GLfloat mat_ambient_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_diffuse);
        
        // Assegura que color tracking está desligado para usar o Material
        glDisable(GL_COLOR_MATERIAL);
        
        glCallList(nexusDisplayListID);
        
        // Restaura
        GLfloat zero_spec[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        GLfloat zero_shine[] = { 0.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, zero_spec);
        glMaterialfv(GL_FRONT, GL_SHININESS, zero_shine);
        glEnable(GL_COLOR_MATERIAL);
    } else {
        glColor3f(0.5f, 0.5f, 0.5f);
        glutSolidCube(40.0f);
    }
    glPopMatrix(); // fim model matrix
    
    extern bool showCollisionBoxes;
    if (showCollisionBoxes)
    {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glColor3f(1.0f, 1.0f, 0.0f); // Amarelo
        glLineWidth(2.0f);
        glutWireSphere(raioColisao, 16, 16); 
        glLineWidth(1.0f);
    }
    glPopMatrix(); // fim translation matrix
    
    // Barra de Vida do Boss
    extern bool showCollisionBoxes;
    if (!showCollisionBoxes) {
        glPushMatrix();
        glTranslatef(posX, posY + 60.0f, posZ); // Acima do Boss
        
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        
        // Fundo vermelho escuro
        glColor3f(0.3f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex3f(-50.0f, -5.0f, 0.0f);
        glVertex3f(50.0f, -5.0f, 0.0f);
        glVertex3f(50.0f, 5.0f, 0.0f);
        glVertex3f(-50.0f, 5.0f, 0.0f);
        glEnd();
        
        // Barra vermelha clara proporcional à vida
        float width = (health / maxHealth) * 100.0f; // total width is 100
        glColor3f(1.0f, 0.2f, 0.2f);
        glBegin(GL_QUADS);
        glVertex3f(-50.0f, -5.0f, 0.1f);
        glVertex3f(-50.0f + width, -5.0f, 0.1f);
        glVertex3f(-50.0f + width, 5.0f, 0.1f);
        glVertex3f(-50.0f, 5.0f, 0.1f);
        glEnd();
        
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glPopMatrix();
    }
}

void NexusOmega::Destruir() { ativo = false; }

void NexusOmega::TomarDano(float dano)
{
    health -= dano;
    if (health <= 0)
    {
        health = 0;
        Destruir();
    }
}

void NexusOmega::InicializarModelo()
{
    if (nexusCarregado) return;
    
    nexusTextureID = Renderer::LoadTexture("assets/textures/nexus.png");
    
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;
    
    if (tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "assets/models/nexus.obj")) {
        float minX = 1e9, maxX = -1e9, minY = 1e9, maxY = -1e9, minZ = 1e9, maxZ = -1e9;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex v;
                v.x = attrib.vertices[3 * index.vertex_index + 0];
                v.y = attrib.vertices[3 * index.vertex_index + 1];
                v.z = attrib.vertices[3 * index.vertex_index + 2];
                if (index.normal_index >= 0) {
                    v.nx = attrib.normals[3 * index.normal_index + 0];
                    v.ny = attrib.normals[3 * index.normal_index + 1];
                    v.nz = attrib.normals[3 * index.normal_index + 2];
                } else {
                    v.nx = 0.0f; v.ny = 1.0f; v.nz = 0.0f;
                }
                if (index.texcoord_index >= 0) {
                    v.u = attrib.texcoords[2 * index.texcoord_index + 0];
                    v.v = attrib.texcoords[2 * index.texcoord_index + 1];
                } else {
                    v.u = 0.0f; v.v = 0.0f;
                }
                nexusVertices.push_back(v);
                
                if (v.x < minX) minX = v.x;
                if (v.x > maxX) maxX = v.x;
                if (v.y < minY) minY = v.y;
                if (v.y > maxY) maxY = v.y;
                if (v.z < minZ) minZ = v.z;
                if (v.z > maxZ) maxZ = v.z;
            }
        }
        float scale = 60.0f / (maxX - minX + 0.1f);
        float cx = (maxX + minX) / 2.0f;
        float cy = (maxY + minY) / 2.0f;
        float cz = (maxZ + minZ) / 2.0f;
        
        nexusDisplayListID = glGenLists(1);
        glNewList(nexusDisplayListID, GL_COMPILE);
        glBegin(GL_TRIANGLES);
        for (auto& v : nexusVertices) {
            v.x = (v.x - cx) * scale;
            v.y = (v.y - cy) * scale;
            v.z = (v.z - cz) * scale;
            glNormal3f(v.nx, v.ny, v.nz);
            glTexCoord2f(v.u, v.v);
            glVertex3f(v.x, v.y, v.z);
        }
        glEnd();
        glEndList();
        nexusCarregado = true;
    }
}
