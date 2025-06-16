#include <stdio.h>
#include "render.h"
#include "jogo.h"
#include "util.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
GLuint texturaParede = 0; // Textura para as paredes
GLuint texturaPiso = 0; // Textura para o piso 

// Texturas do jogador: [direção][frame]: 0=andando_01, 1=parado, 2=andando_02
GLuint texturaPlayer[4][3] = {0}; // [direção][frame]: 0=andando_01, 1=parado, 2=andando_02
int direcaoPlayer = 2; // 0=cima, 1=baixo, 2=esquerda, 3=direita
int framePlayer = 1;   // 0=andando_01, 1=parado, 2=andando_02
int contadorAnim = 0;  // Para controlar a troca de frame

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

// Função para carregar a textura da parede
void carregaTexturaParede() {
    int largura, altura, canais;
    unsigned char* dados = stbi_load("img/Arvore_Grande_01.png", &largura, &altura, &canais, 0);
    if(dados){
        glGenTextures(1, &texturaParede);
        glBindTexture(GL_TEXTURE_2D, texturaParede);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largura, altura, 0, canais == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, dados);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(dados);
    } else {
        printf("Falha ao carregar textura da parede!\n");
    }
}

void carregaTexturaPiso() {
    int largura, altura, canais;
    unsigned char* dados = stbi_load("img/Grama_Simples_01.png", &largura, &altura, &canais, 0);
    if (dados) {
        glGenTextures(1, &texturaPiso);
        glBindTexture(GL_TEXTURE_2D, texturaPiso);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, dados);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(dados);
    } else {
        printf("Falha ao carregar textura do piso!\n");
    }
}

// Função de callback do GLUT para redimensionar a janela
void redimensionaJanela(int width, int height) {
    initRender(width, height);
    glutPostRedisplay(); // Solicita a atualização da tela
}

// Função para desenhar uma célula do mapa, com cor correspondente ao tipo(Depois sera modificado para aplicar texturas)
void desenhaCelula(int x, int y, int tipo){

    if(tipo == PAREDE && texturaParede) {// Se for parede e textura carregada, desenha com textura
        glEnable(GL_BLEND); // Habilita o blending para transparência
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Configura o modo de blending
        glEnable(GL_TEXTURE_2D); // Habilita o uso de texturas
        glBindTexture(GL_TEXTURE_2D, texturaParede); // Vincula a textura da parede
        glColor4f(1,1,1,1); // Garante cor branca para não alterar a textura
        // Desenha um quadrado com a textura da parede
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x + 1, y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x + 1, y + 1);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + 1);
        glEnd();
        glDisable(GL_TEXTURE_2D); // Desabilita o uso de texturas
        glDisable(GL_BLEND); // Desabilita o blending
        return;
    } else if (tipo == PISO && texturaPiso) { // Se for piso e textura carregada
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texturaPiso);
        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para não tingir a textura
        glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(x + cellWidth, y);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(x + cellWidth, y + cellHeight);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + cellHeight);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        return;
    }
    switch(tipo){
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
    int frame = framePlayer;
    glEnable(GL_BLEND); // Habilita o blending para transparência
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Configura o modo de blending
    if(frame > 2) frame = 1; // Garante que só usa 0, 1, 2.
    GLuint tex = texturaPlayer[direcaoPlayer][frame]; // Seleciona a textura do jogador com base na direção e frame
    glEnable(GL_TEXTURE_2D); // Habilita o uso de texturas
    glBindTexture(GL_TEXTURE_2D, tex); // Vincula a textura do jogador
    glColor4f(1,1,1,1);
    // Desenha um quadrado representando o jogador
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + 1, y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + 1, y + 1);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + 1);
    glEnd();
    glDisable(GL_TEXTURE_2D); // Desabilita o uso de texturas
    glDisable(GL_BLEND); // Desabilita o blending
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
    if(tipo == PAPEL)
        glColor3f(0.7f, 0.7f, 1.0f); // Azul claro para papel entregue
    else if(tipo == PLASTICO)
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

    // Desenha o piso base em todas as células primeiro
    for(int i = 0; i < MAP_WIDTH; i++){
        for(int j = 0; j < MAP_HEIGHT; j++){
            desenhaCelula(i, j, PISO); // Desenha a textura do piso em todas as células
        }
    }

    //  Desenha todas as paredes, pisos e lixeiras a partir do mapaInicial
    for(int i = 0; i < MAP_WIDTH; i++){
        for(int j = 0; j < MAP_HEIGHT; j++){
            int cell = nivel.mapaInicial[i][j];
            if(cell == PAREDE){
                desenhaCelula(i, j, PAREDE); // Desenha paredes
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
    // Desenha blocos, verificando se estão sobre a lixeira correta
    for(int i = 0; i < nivel.numBlocos; i++) {
        int x = nivel.blocos[i].x;
        int y = nivel.blocos[i].y;
        int tipo = nivel.blocos[i].tipo;
        int lixeiraIndex = indiceLixeira(x, y);
       if(lixeiraIndex != -1 && nivel.lixeiras[lixeiraIndex].tipo == tipo) {
        desenhaBlocoEntregue(x, y, tipo);
        } else {
            // Desenha o bloco normalmente 
            if(tipo == PAPEL)
                desenhaCelula(x, y, BLOCO_PAPEL);
            else if(tipo == PLASTICO)
                desenhaCelula(x, y, BLOCO_PLASTICO);
        }
    }
    // Desenha o jogador
    desenhaPlayer(nivel.jogador.x, nivel.jogador.y);
}

// Função para carregar as texturas do jogador
void carregaTexturaPlayer() {
    const char* nomes[4][3] = {
        {"img/Sprites_Player/Andando_Atras_01.png", "img/Sprites_Player/Parado_Atras.png", "img/Sprites_Player/Andando_Atras_02.png"},
        {"img/Sprites_Player/Andando_Frente_01.png", "img/Sprites_Player/Parado_Frente.png", "img/Sprites_Player/Andando_Frente_02.png"},
        {"img/Sprites_Player/Andando_Esquerda_01.png", "img/Sprites_Player/Parado_Esquerda.png", "img/Sprites_Player/Andando_Esquerda_02.png"},
        {"img/Sprites_Player/Andando_Direita_01.png", "img/Sprites_Player/Parado_Direita.png", "img/Sprites_Player/Andando_Direita_02.png"}
    };
    for(int dir=0; dir<4; dir++) {
        for(int f=0; f<3; f++) {
            int w, h, c;
            unsigned char* dados = stbi_load(nomes[dir][f], &w, &h, &c, 4);
            if(dados) {
                glGenTextures(1, &texturaPlayer[dir][f]);
                glBindTexture(GL_TEXTURE_2D, texturaPlayer[dir][f]);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, dados);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                stbi_image_free(dados);
            }
        }
    }
}