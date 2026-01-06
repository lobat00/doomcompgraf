#include <GL/glut.h>
#include <math.h>
#include "input/keystate.h"
#include "core/camera.h"
#include "core/window.h"
#include "core/game.h"
#include "core/config.h"
#include "level/levelmetrics.h"
#include <cmath>

static bool isWallTile(int tx, int tz)
{
    const auto &data = gLevel.map.data();
    if (tz < 0 || tx < 0 || tz >= gLevel.map.getHeight())
        return false;
    if (tx >= (int)data[tz].size())
        return false;
    char c = data[tz][tx];
    return (c == '1' || c == '2');
}

static bool pointIntersectsTile(float px, float pz, int tx, int tz, const LevelMetrics &m, float radius)
{
    float wx, wz;
    m.tileCenter(tx, tz, wx, wz);
    float half = m.tile * 0.5f;

    // AABB do tile
    float minX = wx - half;
    float maxX = wx + half;
    float minZ = wz - half;
    float maxZ = wz + half;

    // ponto mais próximo dentro do AABB
    float closestX = std::fmax(minX, std::fmin(px, maxX));
    float closestZ = std::fmax(minZ, std::fmin(pz, maxZ));

    float dx = px - closestX;
    float dz = pz - closestZ;
    return (dx * dx + dz * dz) < (radius * radius);
}

static bool canMoveTo(float nx, float nz)
{
    // se não tiver mapa carregado, permitir
    if (gLevel.map.getHeight() == 0)
        return true;

    const LevelMetrics &m = gLevel.metrics;
    float tile = m.tile;
    float radius = tile * GameConfig::PLAYER_RADIUS_FACTOR;

    // converte posição pra índice aproximado de tile
    float localX = nx - m.offsetX;
    float localZ = nz - m.offsetZ;
    int tx = (int)std::floor(localX / tile);
    int tz = (int)std::floor(localZ / tile);

    // checa vizinhança 3x3
    for (int dz = -1; dz <= 1; ++dz)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            int ntx = tx + dx;
            int ntz = tz + dz;
            if (!isWallTile(ntx, ntz))
                continue;
            if (pointIntersectsTile(nx, nz, ntx, ntz, m, radius))
                return false;
        }
    }
    return true;
}

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

void atualizaMovimento()
{
    float passo = GameConfig::PLAYER_STEPS; // pode ajustar a velocidade aqui

    float radYaw = yaw * M_PI / 180.0f;
    float dirX = std::sin(radYaw);
    float dirZ = -std::cos(radYaw);

    // vetor perpendicular pra strafe
    float strafeX = dirZ;
    float strafeZ = -dirX;

    // acumula deltas pro movimento desejado
    float dx = 0.0f;
    float dz = 0.0f;

    if (keyW)
    { // frente
        dx += dirX * passo;
        dz += dirZ * passo;
    }
    if (keyS)
    { // trás
        dx -= dirX * passo;
        dz -= dirZ * passo;
    }
    if (keyA)
    { // strafe esquerda
        dx += strafeX * passo;
        dz += strafeZ * passo;
    }
    if (keyD)
    { // strafe direita
        dx -= strafeX * passo;
        dz -= strafeZ * passo;
    }

    if (dx == 0.0f && dz == 0.0f)
        return;

    // evita movimento mais rápido na diagonal e reduz travadas
    float len = std::sqrt(dx * dx + dz * dz);
    if (len > 0.0f)
    {
        dx = (dx / len) * passo;
        dz = (dz / len) * passo;
    }

    // SLIDE por eixo sempre 
    float nx = camX + dx;
    if (canMoveTo(nx, camZ))
        camX = nx;

    float nz = camZ + dz;
    if (canMoveTo(camX, nz))
        camZ = nz;
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
