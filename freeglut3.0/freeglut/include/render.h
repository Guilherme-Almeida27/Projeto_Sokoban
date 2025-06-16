#ifndef RENDER_H
#define RENDER_H
#include <GL/freeglut.h>

void initRender(int width, int height); //Inicializa parâmetros de Renderização (GL) e viewport 2D 
void desenhaCena(); // Callback do GLUT para desenhar cada frame
void redimensionaJanela(int width, int height); // Callback do GLUT para redimensionar a janela
void desenhaCelula(int x, int y, int tipo); // Desenha uma célula do mapa
void desenhaPlayer(int x, int y); // Desenha o jogador
void desenhaTexto(const char* texto, int x, int y); // Desenha um texto na tela
void desenhaBlocoEntregue(int x, int y, int tipo); // Desenha um bloco que foi entregue na lixeira correta
void carregaTexturaParede(); // Carrega a textura para as paredes
void carregaTexturasPlayer(); // Carrega as texturas do jogador
//... Outras funções de renderização podem ser adicionadas aqui

#endif // RENDER_H