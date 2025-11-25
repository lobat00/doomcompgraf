#include <GL/glut.h>
#include <math.h>
#include "scene.h"

#define NUM_TORRES 5
#define RAIO 10.0f // raio das torres ao redor do centro

extern GLuint texChao;
extern GLuint texTorre;
extern GLuint texDegrau;

static void desenhaLosango(float altura)
{
    float h = altura / 2.0f;
    float s = altura / 3.0f;

    float claro[3] = {0.3f, 1.0f, 0.3f};
    float escuro[3] = {0.0f, 0.6f, 0.0f};

    glBegin(GL_TRIANGLES);
    // metade de cima
    glColor3fv(claro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(-s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, s);

    glColor3fv(escuro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(0.0f, 0.0f, s);
    glVertex3f(s, 0.0f, 0.0f);

    glColor3fv(claro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);

    glColor3fv(escuro);
    glVertex3f(0.0f, h, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);
    glVertex3f(-s, 0.0f, 0.0f);

    // metade de baixo
    glColor3fv(claro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(0.0f, 0.0f, s);
    glVertex3f(-s, 0.0f, 0.0f);

    glColor3fv(escuro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, s);

    glColor3fv(claro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);
    glVertex3f(s, 0.0f, 0.0f);

    glColor3fv(escuro);
    glVertex3f(0.0f, -h, 0.0f);
    glVertex3f(-s, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, -s);
    glEnd();
}

void desenhaChao()
{
    glBindTexture(GL_TEXTURE_2D, texChao);
    glColor3f(1, 1, 1);

    float tiles = 75.0f;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-80, 0, -80);
    glTexCoord2f(tiles, 0);
    glVertex3f(80, 0, -80);
    glTexCoord2f(tiles, tiles);
    glVertex3f(80, 0, 80);
    glTexCoord2f(0, tiles);
    glVertex3f(-80, 0, 80);
    glEnd();
}

void desenhaTorresELosangos()
{
    float alturaTorre = 2.5f;
    float w = 0.7f;

    float ang0 = -M_PI / 2.0f;
    float passo = 2.0f * M_PI / NUM_TORRES;

    for (int i = 0; i < NUM_TORRES; i++)
    {
        float ang = ang0 + passo * i;
        float x = RAIO * cosf(ang);
        float z = RAIO * sinf(ang);

        glPushMatrix();
        glTranslatef(x, 0.0f, z);

        // Torre roxa
        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, texTorre);
        glColor3f(1.0f, 1.0f, 1.0f); // não “tingir” a textura
        glTranslatef(0.0f, alturaTorre / 2.0f, 0.0f);
        glScalef(w, alturaTorre, w);

        float half = 0.5f;   // cubo unitário de -0.5 a 0.5
        float tilesX = 1.0f; // repete 1x na horizontal
        float tilesY = 2.0f; // repete 2x na vertical (ajuste se quiser)

        glBegin(GL_QUADS);
        // Frente (z positivo)
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-half, -half, half);
        glTexCoord2f(tilesX, 0.0f);
        glVertex3f(half, -half, half);
        glTexCoord2f(tilesX, tilesY);
        glVertex3f(half, half, half);
        glTexCoord2f(0.0f, tilesY);
        glVertex3f(-half, half, half);

        // Trás (z negativo)
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(half, -half, -half);
        glTexCoord2f(tilesX, 0.0f);
        glVertex3f(-half, -half, -half);
        glTexCoord2f(tilesX, tilesY);
        glVertex3f(-half, half, -half);
        glTexCoord2f(0.0f, tilesY);
        glVertex3f(half, half, -half);

        // Direita (x positivo)
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(half, -half, half);
        glTexCoord2f(tilesX, 0.0f);
        glVertex3f(half, -half, -half);
        glTexCoord2f(tilesX, tilesY);
        glVertex3f(half, half, -half);
        glTexCoord2f(0.0f, tilesY);
        glVertex3f(half, half, half);

        // Esquerda (x negativo)
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-half, -half, -half);
        glTexCoord2f(tilesX, 0.0f);
        glVertex3f(-half, -half, half);
        glTexCoord2f(tilesX, tilesY);
        glVertex3f(-half, half, half);
        glTexCoord2f(0.0f, tilesY);
        glVertex3f(-half, half, -half);

        // Topo
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-half, half, half);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(half, half, half);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(half, half, -half);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-half, half, -half);

        glEnd();
        glPopMatrix();

        // Losango verde girando em cima
        glPushMatrix();
        glTranslatef(0.0f, alturaTorre + 1.2f, 0.0f);
        glRotatef(anguloPiramide, 0.0f, 1.0f, 0.0f);
        desenhaLosango(1.5f);
        glPopMatrix();

        glPopMatrix();
    }
}

void desenhaPiramideDegraus()
{
    float alturaDegrau = 0.5f;
    float tamanhoBase = 6.0f;
    float reducao = 0.65f;

    glPushMatrix();

    float roxo1[3] = {0.55f, 0.00f, 0.75f};
    float roxo2[3] = {0.65f, 0.10f, 0.85f};
    float roxo3[3] = {0.75f, 0.20f, 0.95f};

    // Degrau 1
    glColor3f(roxo1[0], roxo1[1], roxo1[2]);
    glPushMatrix();
    glTranslatef(0.0f, alturaDegrau / 2.0f, 0.0f);
    glScalef(tamanhoBase, alturaDegrau, tamanhoBase);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Degrau 2
    glColor3f(roxo2[0], roxo2[1], roxo2[2]);
    glPushMatrix();
    glTranslatef(0.0f, alturaDegrau + alturaDegrau / 2.0f, 0.0f);
    glScalef(tamanhoBase * reducao, alturaDegrau, tamanhoBase * reducao);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Degrau 3
    glColor3f(roxo3[0], roxo3[1], roxo3[2]);
    glPushMatrix();
    glTranslatef(0.0f, 2 * alturaDegrau + alturaDegrau / 2.0f, 0.0f);
    glScalef(tamanhoBase * reducao * reducao,
             alturaDegrau,
             tamanhoBase * reducao * reducao);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Esfera flutuando acima
    float topoDegrausY = 5.0f * alturaDegrau;
    float raioEsfera = 3.0f;

    glPushMatrix();
    glTranslatef(0.0f, topoDegrausY + raioEsfera + 0.2f, 0.0f);

    glRotatef(anguloEsfera, 1.0f, 1.5f, 0.0f);

    double eq[4];

    // metade de cima
    glColor3f(0.40f, 0.00f, 0.00f);
    eq[0] = 0;
    eq[1] = -1;
    eq[2] = 0;
    eq[3] = 0;
    glClipPlane(GL_CLIP_PLANE0, eq);
    glEnable(GL_CLIP_PLANE0);
    glutSolidSphere(raioEsfera, 40, 40);
    glDisable(GL_CLIP_PLANE0);

    // metade de baixo
    glColor3f(0.80f, 0.00f, 0.00f);
    eq[0] = 0;
    eq[1] = 1;
    eq[2] = 0;
    eq[3] = 0;
    glClipPlane(GL_CLIP_PLANE1, eq);
    glEnable(GL_CLIP_PLANE1);
    glutSolidSphere(raioEsfera, 40, 40);
    glDisable(GL_CLIP_PLANE1);

    glPopMatrix();

    glPopMatrix();
}
