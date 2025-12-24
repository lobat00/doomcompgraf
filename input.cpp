#include <GL/glut.h>
#include <math.h>
#include "scene.h"
#include "utils/levelmetrics.h"

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        keyW = true;
        break;

    case 's':
    case 'S':
        keyS = true;
        break;

    case 'a':
    case 'A':
        keyA = true;
        break;

    case 'd':
    case 'D':
        keyD = true;
        break;

    case 27: // ESC
        std::exit(0);
        break;
    }
}

void keyboardUp(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
    case 'W':
        keyW = false;
        break;

    case 's':
    case 'S':
        keyS = false;
        break;

    case 'a':
    case 'A':
        keyA = false;
        break;

    case 'd':
    case 'D':
        keyD = false;
        break;
    }

    if ((key == 13 || key == '\r') && (glutGetModifiers() & GLUT_ACTIVE_ALT))
    {
        altFullScreen();
    }
}

bool podeAndar(float x, float z) {
    // 1. Criamos a mesma métrica usada no desenho
    float TILE = 4.0f;
    LevelMetrics m = LevelMetrics::fromMap(gMap, TILE);

    // 2. Convertemos a posição do mundo (x, z) para coordenadas de grid (col, row)
    // A LevelMetrics tem funções para isso. Se não tiver, usamos a lógica reversa do tileCenter.
    int gridX, gridZ;
    
    // O LevelMetrics geralmente centraliza o mapa subtraindo metade do tamanho total.
    gridX = (int)floor((x / TILE) + (gMap.getWidth() / 2.0f));
    gridZ = (int)floor((z / TILE) + (gMap.getHeight() / 2.0f));

    // 3. Verificamos as fronteiras
    if (gridZ < 0 || gridZ >= (int)gMap.getHeight() || gridX < 0) return false;

    const auto &data = gMap.data();
    if (gridX >= (int)data[gridZ].size()) return false;

    // 4. Retorna true se NÃO for parede ('1')
    return (data[gridZ][gridX] != '1');
}
// input.cpp

void atualizaMovimento() {
    float passo = 0.15f; 
    float radYaw = yaw * M_PI / 180.0f;
    float dirX = std::sin(radYaw);
    float dirZ = -std::cos(radYaw);
    float strafeX = dirZ;
    float strafeZ = -dirX;

    float dx = 0, dz = 0;
    if (keyW) { dx += dirX; dz += dirZ; }
    if (keyS) { dx -= dirX; dz -= dirZ; }
    if (keyA) { dx += strafeX; dz += strafeZ; }
    if (keyD) { dx -= strafeX; dz -= strafeZ; }

    float tentX = dx * passo;
    float tentZ = dz * passo;

    // "Gordura" do jogador (metade do tamanho do corpo)
    // Se a margem for 1.0, o jogador tem 2.0 de largura total.
    float LARGURA = 1.0f; 

    // --- EIXO X (Sliding) ---
    // Calculamos onde o jogador estaria no eixo X
    float proximoX = camX + tentX;
    bool colidiuX = false;

    // VERIFICAÇÃO DE QUINA (4 PONTOS):
    // Testamos os 4 cantos do quadrado do jogador na nova posição X, 
    // mas mantendo o Z atual.
    if (!podeAndar(proximoX + LARGURA, camZ + LARGURA)) colidiuX = true; // Frente-Direita
    if (!podeAndar(proximoX + LARGURA, camZ - LARGURA)) colidiuX = true; // Trás-Direita
    if (!podeAndar(proximoX - LARGURA, camZ + LARGURA)) colidiuX = true; // Frente-Esquerda
    if (!podeAndar(proximoX - LARGURA, camZ - LARGURA)) colidiuX = true; // Trás-Esquerda

    // Só anda no X se NENHUM dos 4 cantos bater
    if (!colidiuX) {
        camX = proximoX;
    }

    // --- EIXO Z (Sliding) ---
    // Calculamos onde o jogador estaria no eixo Z
    float proximoZ = camZ + tentZ;
    bool colidiuZ = false;

    // Testamos os 4 cantos na nova posição Z, usando o X (que já pode ter mudado ou não)
    if (!podeAndar(camX + LARGURA, proximoZ + LARGURA)) colidiuZ = true;
    if (!podeAndar(camX + LARGURA, proximoZ - LARGURA)) colidiuZ = true;
    if (!podeAndar(camX - LARGURA, proximoZ + LARGURA)) colidiuZ = true;
    if (!podeAndar(camX - LARGURA, proximoZ - LARGURA)) colidiuZ = true;

    // Só anda no Z se NENHUM dos 4 cantos bater
    if (!colidiuZ) {
        camZ = proximoZ;
    }
}

void mouseMotion(int x, int y)
{
    // se o evento foi gerado pelo glutWarpPointer, ignorar
    if (ignoreWarp)
    {
        ignoreWarp = false;
        return;
    }

    // PRIMEIRA VEZ: só centraliza, sem aplicar rotação
    if (firstMouse)
    {
        firstMouse = false;
        ignoreWarp = true;
        glutWarpPointer(centerX, centerY);
        return;
    }

    int dx = x - centerX;
    int dy = y - centerY;

    float sens = 0.1f; // sensibilidade do mouse

    yaw += dx * sens;   // horizontal
    pitch -= dy * sens; // vertical (invertido pra sensação de FPS)

    // limita o pitch pra não virar o pescoço ao contrário
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // volta o mouse pro centro – isso vai gerar outro evento,
    // por isso marcamos ignoreWarp = true.
    ignoreWarp = true;
    glutWarpPointer(centerX, centerY);

    glutPostRedisplay();
}
