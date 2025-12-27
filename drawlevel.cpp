#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include "drawlevel.h"
#include "scene.h"
#include "utils/levelmetrics.h"
#include <cstdio>

extern GLuint texParede; // pode usar como textura de parede (ou crie texParede)
extern GLuint texLava;
extern GLuint texSangue; // pode usar como textura base do sangue (ou crie texBlood)
extern GLuint texChao;
extern GLuint texParedeMetal;
extern GLuint texPisoAlt;

extern GLuint progLava;
extern GLuint progSangue;

// Controle de tempo
extern float tempoEsfera;

// Config do grid
static const float TILE = 4.0f;    // tamanho do tile no mundo (ajuste)
static const float WALL_H = 4.0f;  // altura da parede
static const float EPS_Y = 0.001f; // evita z-fighting

static void bindTexture0(GLuint tex)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
}

static void desenhaTeto(float x, float z) {
    glUseProgram(0);
    glColor3f(0.7f, 0.7f, 0.7f); // Um pouco mais escuro para dar profundidade
    bindTexture0(texChao); // Pode usar a mesma do chão ou uma específica
    
    float half = (TILE + 0.01f) * 0.5f;
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x - half, WALL_H, z + half);
        glTexCoord2f(2.0f, 0.0f); glVertex3f(x + half, WALL_H, z + half);
        glTexCoord2f(2.0f, 2.0f); glVertex3f(x + half, WALL_H, z - half);
        glTexCoord2f(0.0f, 2.0f); glVertex3f(x - half, WALL_H, z - half);
    glEnd();
}

static void desenhaQuadChao(float x, float z, float tile, float tilesUV)
{
    float half = (tile + 0.01f) * 0.5f; //teste

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - half, EPS_Y, z + half);
    glTexCoord2f(tilesUV, 0.0f);
    glVertex3f(x + half, EPS_Y, z + half);
    glTexCoord2f(tilesUV, tilesUV);
    glVertex3f(x + half, EPS_Y, z - half);
    glTexCoord2f(0.0f, tilesUV);
    glVertex3f(x - half, EPS_Y, z - half);
    glEnd();
}

static void desenhaTileChao(float x, float z)
{
    glUseProgram(0); // sem shader
    glColor3f(1, 1, 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texChao);

    // repete a textura no tile
    desenhaQuadChao(x, z, TILE, 2.0f);
}

static void desenhaParede(float x, float z)
{
    float half = TILE * 0.5f;

    glColor3f(1, 1, 1);
    

    // textura repetida ao longo da parede
    float tilesX = 1.0f;
    float tilesY = 2.0f;

    glBegin(GL_QUADS);

    // Frente (z+)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - half, 0.0f, z + half);
    glTexCoord2f(tilesX, 0.0f);
    glVertex3f(x + half, 0.0f, z + half);
    glTexCoord2f(tilesX, tilesY);
    glVertex3f(x + half, WALL_H, z + half);
    glTexCoord2f(0.0f, tilesY);
    glVertex3f(x - half, WALL_H, z + half);

    // Trás (z-)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x + half, 0.0f, z - half);
    glTexCoord2f(tilesX, 0.0f);
    glVertex3f(x - half, 0.0f, z - half);
    glTexCoord2f(tilesX, tilesY);
    glVertex3f(x - half, WALL_H, z - half);
    glTexCoord2f(0.0f, tilesY);
    glVertex3f(x + half, WALL_H, z - half);

    // Direita (x+)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x + half, 0.0f, z + half);
    glTexCoord2f(tilesX, 0.0f);
    glVertex3f(x + half, 0.0f, z - half);
    glTexCoord2f(tilesX, tilesY);
    glVertex3f(x + half, WALL_H, z - half);
    glTexCoord2f(0.0f, tilesY);
    glVertex3f(x + half, WALL_H, z + half);

    // Esquerda (x-)
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - half, 0.0f, z - half);
    glTexCoord2f(tilesX, 0.0f);
    glVertex3f(x - half, 0.0f, z + half);
    glTexCoord2f(tilesX, tilesY);
    glVertex3f(x - half, WALL_H, z + half);
    glTexCoord2f(0.0f, tilesY);
    glVertex3f(x - half, WALL_H, z - half);

    // Topo
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x - half, WALL_H, z + half);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + half, WALL_H, z + half);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + half, WALL_H, z - half);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x - half, WALL_H, z - half);

    glEnd();
}

static void desenhaTileLava(float x, float z)
{
    glUseProgram(progLava);

    GLint locTime = glGetUniformLocation(progLava, "uTime");
    GLint locStr = glGetUniformLocation(progLava, "uStrength");
    GLint locScr = glGetUniformLocation(progLava, "uScroll");
    GLint locHeat = glGetUniformLocation(progLava, "uHeat");
    GLint locTex = glGetUniformLocation(progLava, "uTexture");

    glUniform1f(locTime, tempoEsfera);
    glUniform1f(locStr, 1.0f);
    glUniform2f(locScr, 0.1f, 0.0f);
    glUniform1f(locHeat, 0.6f);

    bindTexture0(texLava);
    glUniform1i(locTex, 0);

    glColor3f(1, 1, 1);
    desenhaQuadChao(x, z, TILE, 2.0f);

    glUseProgram(0);
}

static void desenhaTileSangue(float x, float z)
{
    glUseProgram(progSangue); // seu shader blood

    GLint locTime = glGetUniformLocation(progSangue, "uTime");
    GLint locStr = glGetUniformLocation(progSangue, "uStrength");
    GLint locSpd = glGetUniformLocation(progSangue, "uSpeed");
    GLint locTex = glGetUniformLocation(progSangue, "uTexture");

    glUniform1f(locTime, tempoEsfera);
    glUniform1f(locStr, 1.0f);
    glUniform2f(locSpd, 2.0f, 1.3f);

    bindTexture0(texSangue); // textura base do “sangue”
    glUniform1i(locTex, 0);

    glColor3f(1, 1, 1);
    desenhaQuadChao(x, z, TILE, 2.0f);

    glUseProgram(0);
}

void drawLevel(const MapLoader &map) {
    const auto &data = map.data();
    int H = map.getHeight();
    LevelMetrics m = LevelMetrics::fromMap(map, TILE);

    for (int z = 0; z < H; z++) {
        for (int x = 0; x < (int)data[z].size(); x++) {
            float wx, wz;
            m.tileCenter(x, z, wx, wz);
            char c = data[z][x];

            // LOGICA DE PISO
            if (c != 'L' && c != 'B') { 
                glUseProgram(0);
                glBindTexture(GL_TEXTURE_2D, (c == 'A' || c == 'C') ? texPisoAlt : texChao);
                desenhaQuadChao(wx, wz, TILE, 2.0f);
            }

            // LOGICA DE PAREDE
            if (c == '1' || c == '2') {
                glBindTexture(GL_TEXTURE_2D, (c == '1') ? texParede : texParedeMetal);
                desenhaParede(wx, wz);
            }
            

            // LOGICA DE TETO (Opcional por Tile)
            if (c == 'T' || c == 'C') {
                desenhaTeto(wx, wz);
            }
            
            // Outros tipos (Lava, Sangue...)
            else if (c == 'L') desenhaTileLava(wx, wz);
        }
    }
}