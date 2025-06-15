#include <stdio.h>
#include "render.h"
#include "jogo.h"
#include "util.h"

// Dimensões da janela em pixels
static int windowWidth = 800; 
static int windowHeight = 600; 

// Tamanho da celula em unidades OpenGL
static float cellWidth = 1.0f, cellHeight = 1.0f;

extern Level nivel; // Variável global para armazenar o nível atual

// Função para inicializar viewport e projeção ortográfica 2D
void initRender(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, MAP_WIDTH, MAP_HEIGHT, 0.0); // Projeção de (0,0) no canto inferior esquerdo até (MAP_WIDTH, MAP_HEIGHT) no canto superior direito  
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, windowWidth, windowHeight); // Define a viewport para toda a janela

}

// Função de callback do GLUT para redimensionar a janela
void redimensionaJanela(int width, int height) {
    initRender(width, height);
    glutPostRedisplay(); // Solicita a atualização da tela
}

// Função para desenhar uma célula do mapa, com cor correspondente ao tipo(Depois sera modificado para aplicar texturas)
void desenhaCelula(int x, int y, int tipo){
    switch(tipo){
        case PISO:
            glColor3f(0.8f, 0.8f, 0.8f); // Cor cinza para piso
            break;
        case PAREDE:
            glColor3f(0.5f, 0.5f, 0.5f); // Cor cinza escuro para parede
            break;
        case BLOCO_PAPEL:
            glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para bloco de papel
            break;
        case BLOCO_PLASTICO:
            glColor3f(1.0f, 0.0f, 1.0f); // Cor magenta para bloco de plástico
            break;
        case LIXEIRA_PAPEL:
            glColor3f(0.0f, 0.0f, 1.0f); // Cor azul para lixeira de papel
            break;
        case LIXEIRA_PLASTICO:
            glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha para lixeira de plástico
            break;
        default:
            glColor3f(0.0f, 0.0f, 0.0f); // Cor preta para tipos desconhecidos
            break;
    }
    // Desenha um retangulo representando a célula
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + cellWidth, y);
        glVertex2f(x + cellWidth, y + cellHeight);
        glVertex2f(x, y + cellHeight);
    glEnd();
}

// Função para desenhar o jogador(apenas um retangulo por enquanto)
void desenhaPlayer(int x, int y) {
    glColor3f(1.0f, 0.5f, 0.0f); // Cor laranja para o jogador
    glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + cellWidth, y);
        glVertex2f(x + cellWidth, y + cellHeight);
        glVertex2f(x, y + cellHeight);
    glEnd();
}

// Função para desenhar o texto usando uma função utilitária
void desenhaTexto(const char* texto, int x, int y) {
    glColor3f(0.0f, 0.0f, 0.0f); // Cor preta para o texto
    BitmapText(texto, x, y, GLUT_BITMAP_HELVETICA_18); // Desenha o texto na posição especificada
}


// Adicione esta função auxiliar para desenhar um bloco "entregue"
void desenhaBlocoEntregue(int x, int y, int tipo) {
    // Exemplo: desenha um bloco menor e com cor diferente
    float margem = 0.2f; // Margem para deixar o bloco menor
    if(tipo == BLOCO_PAPEL)
        glColor3f(0.7f, 0.7f, 1.0f); // Azul claro para papel entregue
    else if(tipo == BLOCO_PLASTICO)
        glColor3f(1.0f, 0.7f, 0.7f); // Vermelho claro para plástico entregue
    else
        glColor3f(0.7f, 0.7f, 0.7f); // Cinza claro para outros

    glBegin(GL_QUADS);
        glVertex2f(x + margem, y + margem);
        glVertex2f(x + 1 - margem, y + margem);
        glVertex2f(x + 1 - margem, y + 1 - margem);
        glVertex2f(x + margem, y + 1 - margem);
    glEnd();
}


// Função para desenhar a cena(mapa inteiro + blocos + lixeiras + jogador)
void desenhaCena() {
    glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor

    //  Desenha todas as paredes, pisos e lixeiras a partir do mapaInicial
    for(int i = 0; i < MAP_WIDTH; i++){
        for(int j = 0; j < MAP_HEIGHT; j++){
            int cell = nivel.mapaInicial[i][j];
            if(cell == PAREDE){
                desenhaCelula(i, j, PAREDE); // Desenha paredes
            }
            else if(cell == PISO){
                desenhaCelula(i, j, PISO); // Desenha pisos
            }
            else if(cell == LIXEIRA_PAPEL){
                desenhaCelula(i, j, LIXEIRA_PAPEL); // Desenha lixeiras de papel
            }
            else if(cell == LIXEIRA_PLASTICO){
                desenhaCelula(i, j, LIXEIRA_PLASTICO); // Desenha lixeiras de plástico
            }
            else {
                desenhaCelula(i, j, PISO); // Desenha piso como padrão
            }
        }
    }

    // Desenha blocos, erificando se estão sobre a lixeira correta
    for(int i = 0; i < nivel.numBlocos; i++) {
        int x = nivel.blocos[i].x;
        int y = nivel.blocos[i].y;
        int tipo = nivel.blocos[i].tipo;
        int lixeiraIndex = indiceLixeira(x, y);
        if(lixeiraIndex != -1 && 
            nivel.lixeiras[lixeiraIndex].tipo == (tipo == BLOCO_PAPEL ? LIXEIRA_PAPEL : LIXEIRA_PLASTICO)) {
            // Se o bloco está na lixeira correta, desenha como entregue
            desenhaBlocoEntregue(x, y, tipo);
        } else {
            // Caso contrário, desenha o bloco normalmente
            desenhaCelula(x, y, tipo);
            }
            
    }

    // Desenha o jogador
    desenhaPlayer(nivel.jogador.x, nivel.jogador.y);
}

