#include <GL/glew.h>
#include <GL/glut.h>
#include <math.h>
#include <cstdio>
#include "engine/scene.h"
#include "engine/input.h"
#include "engine/texture.h"
#include "engine/shader.h"
#include "game/maploader.h"
#include "game/drawlevel.h"
#include "game/levelmetrics.h"

static MapLoader gMap;

float tempo = 0.0f;

int fps = 0;
int frameCount = 0;
int previousTime = 0;

GLuint texChao;
GLuint texParede;
GLuint texSangue;
GLuint texLava;
GLuint texChaoInterno;
GLuint texParedeInterna;
GLuint texTeto;
GLuint progSangue;
GLuint progLava;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float radYaw = yaw * M_PI / 180.0f;
    float radPitch = pitch * M_PI / 180.0f;

    float dirX = cosf(radPitch) * sinf(radYaw);
    float dirY = sinf(radPitch);
    float dirZ = -cosf(radPitch) * cosf(radYaw);

    gluLookAt(
        camX, camY, camZ,
        camX + dirX, camY + dirY, camZ + dirZ,
        0.0f, 1.0f, 0.0f);

    drawLevel(gMap);

    glutSwapBuffers();

    frameCount++;
    int currentTime = glutGet(GLUT_ELAPSED_TIME);

    if (currentTime - previousTime > 1000) // passou 1 segundo
    {
        fps = frameCount;
        frameCount = 0;
        previousTime = currentTime;

        char titulo[64];
        sprintf(titulo, "Um dia vai ser DOOM (%d FPS)", fps);
        glutSetWindowTitle(titulo);
    }
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    float a = (float)w / (float)h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, a, 1.0f, 100.0f);

    glMatrixMode(GL_MODELVIEW);

    // informa ao módulo de input onde é o centro da janela
    atualizaCentroJanela(w, h);
}

void timer(int v)
{
    tempo += 0.016f;

    atualizaMovimento();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); // ~60 FPS
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    glutInitWindowSize(janelaW, janelaH);
    glutCreateWindow("Um dia vai ser DOOM");

    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        printf("Erro GLEW: %s\n", glewGetErrorString(err));
        return 1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // carregando texturas
    texChao = carregaTextura("assets/181.png");
    texParede = carregaTextura("assets/091.png");
    texSangue = carregaTextura("assets/016.png");
    texLava = carregaTextura("assets/179.png");
    texChaoInterno = carregaTextura("assets/100.png");
    texParedeInterna = carregaTextura("assets/060.png");
    texTeto = carregaTextura("assets/081.png");
    

    // cria o shader
    progSangue = criaShader("shaders/blood.vert", "shaders/blood.frag");
    progLava = criaShader("shaders/lava.vert", "shaders/lava.frag");

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);

    glutSetCursor(GLUT_CURSOR_NONE); // esconde o cursor

    glutTimerFunc(0, timer, 0);

    gMap.load("maps/map1.txt");
    LevelMetrics m = LevelMetrics::fromMap(gMap, 4.0f); 
    m.spawnPos(gMap, camX, camZ);
    camY = 1.5f;

    glutMainLoop();
    return 0;
}
