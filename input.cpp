#include <GL/glut.h>
#include <math.h>
#include "scene.h"
#include "utils/levelmetrics.h" // para LevelMetrics

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
    // 1. Obtemos as métricas do mapa
    float TILE = 4.0f;
    LevelMetrics m = LevelMetrics::fromMap(gMap, TILE);
    int gridX, gridZ;
    
    gridX = (int)floor((x / TILE) + (gMap.getWidth() / 2.0f));
    gridZ = (int)floor((z / TILE) + (gMap.getHeight() / 2.0f));

    if (gridZ < 0 || gridZ >= (int)gMap.getHeight() || gridX < 0) return false;

    const auto &data = gMap.data();
    if (gridX >= (int)data[gridZ].size()) return false;


    char celula = data[gridZ][gridX];

    // Se a célula for '1' (Pedra) OU '2' (Metal), o jogador NÃO pode andar
    if (celula == '1' || celula == '2') {
        return false;
    }

    // Para qualquer outro caractere (0, T, A, C, L, B), o caminho está livre
    return true; 
}
// Atualiza a posição da câmera com base nas teclas pressionadas
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

    // Slide 4: Margem de segurança (corpo do jogador)
    float margem = 1.5f;
    float cX = (tentX > 0) ? tentX + margem : tentX - margem;
    float cZ = (tentZ > 0) ? tentZ + margem : tentZ - margem;

    // Tenta mover no X
    if (podeAndar(camX + cX, camZ)) {
        camX += tentX;
    }

    // Tenta mover no Z
    if (podeAndar(camX, camZ + cZ)) {
        camZ += tentZ;
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
