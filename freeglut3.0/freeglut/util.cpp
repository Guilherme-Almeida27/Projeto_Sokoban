#include <GL/freeglut.h>
#include "jogo.h"

//Desenha texto bitmap em coordenadas de tela (pixels)
void BitmapText(const char* text, int x, int y, void* font) {
    glRasterPos2i(x, y); // Define a posição do raster
    while (*text) {
        glutBitmapCharacter(font, *text); // Desenha cada caractere
        text++;
    }
}