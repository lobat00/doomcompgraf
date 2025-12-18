#include <GL/glut.h>
#include "core/window.h"

// Tamanho inicial
int janelaW = 1920;
int janelaH = 1080;

// Centro (atualizado em reshape)
int centerX = janelaW / 2;
int centerY = janelaH / 2;

// Estado do fullscreen
bool fullScreen = false;

void atualizaCentroJanela(int w, int h)
{
    centerX = w / 2;
    centerY = h / 2;
}

void altFullScreen()
{
    if (!fullScreen)
    {
        glutFullScreen();
        fullScreen = true;
    }
    else
    {
        glutReshapeWindow(janelaW, janelaH);
        fullScreen = false;
    }
}
