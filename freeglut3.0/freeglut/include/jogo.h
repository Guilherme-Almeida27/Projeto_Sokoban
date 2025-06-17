#ifndef JOGO_H
#define JOGO_H

#include <GL/freeglut.h>

#define MAP_WIDTH 20 // Largura do mapa
#define MAP_HEIGHT 20 // Altura do mapa

// Definições de tipos de blocos
#define PISO 0
#define PAREDE 1
#define BLOCO_PAPEL 2
#define BLOCO_PLASTICO 3
#define LIXEIRA_PAPEL 4
#define LIXEIRA_PLASTICO 5

#define JOGADOR 9

//Novos tipos simplificados para blocos e lixeiras
#define PAPEL 2
#define PLASTICO 3

extern int nivelAtual; // Nível atual do jogo

typedef struct { 
    int x, y; // Posição do jogador no mapa
} Player;

typedef struct{
    int x, y; // Posição do bloco no mapa
    int tipo; // Tipo do bloco (papel, plástico, vidro, metal)
} Block;

typedef struct {
    int x, y; // Posição da lixeira no mapa
    int tipo; // Tipo da lixeira (papel, plástico, vidro, metal)
} Lixeira;
 
// Estrutura para armazenar o estado atual do nivel
typedef struct {
    int mapaAtual[MAP_WIDTH][MAP_HEIGHT]; // Mapa do nível
    int mapaInicial[MAP_WIDTH][MAP_HEIGHT]; // Mapa inicial do nível
    Player jogador;
    Block blocos[20];
    Lixeira lixeiras[20];
    int numBlocos; // Número de blocos no nível
    int numLixeiras; // Número de lixeiras no nível
} Level;

extern Level level; // Variável global para armazenar o level


void initLevel(); // Inicializa o nível
void reiniciaLevel(); // Reinicia o nível
void movePlayer(int dx, int dy); // Move o jogador na direção especificada
int checkVitoria(); // Verifica se o jogador venceu o nível
void MapaInicial(int indeceNivel); // Cria o mapa inicial do nível
int indiceBloco(int x, int y); // Retorna o índice do bloco na lista de blocos
int indiceLixeira(int x, int y); // Retorna o índice da lixeira na lista de lixeiras
int celulaLivre(int x, int y); // Verifica se a célula (x, y) está livre
int moveBloco(int b, int dx, int dy); // Move o bloco b na direção especificada
//... Outras funções relacionadas ao jogo podem ser adicionadas aqui (eu num sei)(;-;)

#endif // JOGO_H

