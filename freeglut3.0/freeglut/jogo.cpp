#include <string.h>
#include <stdio.h>
#include <GL/freeglut.h>
#include "jogo.h"
#include "util.h"

Level nivel; // Variável global para armazenar o nível atual

extern int direcaoPlayer; // 0=cima, 1=baixo, 2=esquerda, 3=direita
extern int framePlayer; // 0=andando_01, 1=parado, 2=andando_02
extern int contadorAnim; // Contador de animação para o jogador

// Variáveis globais para controle de níveis
int nivelAtual = 1;
const int totalNiveis = 4; 

// ---------------------------Funções de configuração inicial---------------------------


// Inicializa o nível com o mapa inicial
/* 0 = PISO , 1 = PAREDE, 2 = BLOCO_PAPEL, 3 = BLOCO_PLASTICO 4 = LIXEIRA_PAPEL, 5 = LIXEIRA_PLASTICO */
void MapaInicial(int indiceNivel) {    
    if(indiceNivel == 1){ // Nível 1
        int tmp[MAP_WIDTH][MAP_HEIGHT] = {
            {1,1,1,1,1,1,0},
            {1,9,0,0,0,1,0},
            {1,0,0,3,2,1,1},
            {1,0,0,0,0,0,1},
            {1,0,0,4,5,0,1},
            {1,1,0,0,0,0,1},
            {0,1,0,0,0,0,1},
            {0,1,1,1,1,1,1}
        };
        memcpy(nivel.mapaInicial, tmp, sizeof(tmp));
    }
    else if(indiceNivel == 2){ // Nível 2
        int tmp[MAP_WIDTH][MAP_HEIGHT] = {
            {1,1,1,1,1,1,1,1,0},
            {1,9,0,0,0,0,0,1,0},
            {1,0,0,2,0,0,0,1,1},
            {1,0,0,3,0,0,5,4,1},
            {1,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1}
        };

        memcpy(nivel.mapaInicial, tmp, sizeof(tmp));
    }
    else if(indiceNivel == 3){ // Nível 3
        int tmp[MAP_WIDTH][MAP_HEIGHT] = {
            {0,1,1,1,1,0},
            {1,1,0,0,1,0},
            {1,0,3,0,1,0},
            {1,5,1,4,1,0},
            {1,0,2,0,1,1},
            {1,0,0,0,0,1},
            {1,1,0,0,9,1},
            {0,1,1,1,1,1}
        };
        memcpy(nivel.mapaInicial, tmp, sizeof(tmp));
    }
    else if(indiceNivel == 4){ // Nível 4 
        int tmp[MAP_WIDTH][MAP_HEIGHT] = {
            {0,0,1,1,1,1,0},
            {1,1,9,0,0,1,1},
            {1,0,0,5,2,0,1},
            {1,0,3,4,0,0,1},
            {1,1,1,0,0,1,1},
            {0,0,1,1,1,1,0}
        };
        memcpy(nivel.mapaInicial, tmp, sizeof(tmp));
    }
}

// Inicializa ou reseta o nível: mapaAtual = mapaInicial, posiciona player, blocos e lixeiras 
void initLevel(){
    memcpy(nivel.mapaAtual, nivel.mapaInicial, sizeof(nivel.mapaInicial));
    nivel.numBlocos = 0;
    nivel.numLixeiras = 0;

    //  Varre o mapa e armazena blocos e lixeiras numa lista, e remove-os do mapaAtual
    for(int y = 0; y < MAP_HEIGHT; y++) {
        for(int x = 0; x < MAP_WIDTH; x++) {
            int cell = nivel.mapaAtual[x][y];
            if(cell == BLOCO_PAPEL) {
                nivel.blocos[nivel.numBlocos].x = x;
                nivel.blocos[nivel.numBlocos].y = y;
                nivel.blocos[nivel.numBlocos].tipo = PAPEL;
                nivel.numBlocos++;
                nivel.mapaAtual[x][y] = PISO; // Remove o bloco do mapa
            }
            else if(cell == BLOCO_PLASTICO) {
                nivel.blocos[nivel.numBlocos].x = x;
                nivel.blocos[nivel.numBlocos].y = y;
                nivel.blocos[nivel.numBlocos].tipo = PLASTICO;
                nivel.numBlocos++;
                nivel.mapaAtual[x][y] = PISO; // Remove o bloco do mapa
            }
            else if(cell == LIXEIRA_PAPEL) {
                nivel.lixeiras[nivel.numLixeiras].x = x;
                nivel.lixeiras[nivel.numLixeiras].y = y;
                nivel.lixeiras[nivel.numLixeiras].tipo = PAPEL;
                nivel.numLixeiras++;
                nivel.mapaAtual[x][y] = PISO; // Remove a lixeira do mapa
            }
            else if(cell == LIXEIRA_PLASTICO) {
                nivel.lixeiras[nivel.numLixeiras].x = x;
                nivel.lixeiras[nivel.numLixeiras].y = y;
                nivel.lixeiras[nivel.numLixeiras].tipo = PLASTICO;
                nivel.numLixeiras++;
                nivel.mapaAtual[x][y] = PISO; // Remove a lixeira do mapa
            }
            else if(cell == JOGADOR) {
                // Posiciona o jogador no mapa
                nivel.jogador.x = x;
                nivel.jogador.y = y;
                // Remove o jogador do mapaAtual
                nivel.mapaAtual[x][y] = PISO; 
            }
            else if(cell == PAREDE || cell == PISO) {
                // Apenas paredes e pisos permanecem no mapaAtual
                continue;
            }  
            else {
                // Se for um bloco ou lixeira desconhecido, trata como piso
                nivel.mapaAtual[x][y] = PISO;
            }
        }
    }
}

// Reinicia o nível por completo
void reiniciaLevel(){
    initLevel();
    glutPostRedisplay(); // Redesenha a cena após reiniciar o nível
}

// -----------------------------Funções de logica do jogo-----------------------------

// Função para verificar se é possível mover o jogador para a posição (x, y)
int celulaLivre(int x, int y){
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if(y < 0 || y >= MAP_WIDTH || x < 0 || x >= MAP_HEIGHT) {
        return 0; // Fora dos limites
    }
    // Verifica se a célula é piso ou lixeira
    if(nivel.mapaAtual[x][y] == PISO || indiceLixeira(x, y) != -1) {
        return 1; // Célula livre
    }
    return 0; // Célula ocupada
}

// Função para retornar o índice do bloco em nivel.blocos[] 
int indiceBloco(int x, int y) {
    for(int i = 0; i < nivel.numBlocos; i++) {
        if(nivel.blocos[i].x == x && nivel.blocos[i].y == y) {
            return i; // Retorna o índice do bloco encontrado
        }
    }
    return -1; // Retorna -1 se não encontrar o bloco
}

// Função para retornar o índice da lixeira em nivel.lixeiras[]
int indiceLixeira(int x, int y) {
    for(int i = 0; i < nivel.numLixeiras; i++) {
        if(nivel.lixeiras[i].x == x && nivel.lixeiras[i].y == y) {
            return i; // Retorna o índice da lixeira encontrada
        }
    }
    return -1; // Retorna -1 se não encontrar a lixeira
}

// Move o bloco b em (dx, dy) se possível
int moveBloco(int b, int dx, int dy) {
    int novox = nivel.blocos[b].x + dx;
    int novoy = nivel.blocos[b].y + dy;

    // Verifica limites e se não é parede
    if(novox < 0 || novox >= MAP_WIDTH || novoy < 0 || novoy >= MAP_HEIGHT) {
        return 0; // Fora dos limites do mapa
    }
    if(nivel.mapaAtual[novox][novoy] == PAREDE) {
        return 0; // Não pode mover para uma parede
    }

    // Verifica se existe um bloco na nova posição
    if(indiceBloco(novox, novoy) != -1) {
        return 0; // Não pode mover para uma posição já ocupada por outro bloco
    }

     // Remove o bloco da posição antiga no mapaAtual
    nivel.mapaAtual[nivel.blocos[b].x][nivel.blocos[b].y] = PISO; // Marca a posição antiga como piso
    // Move o bloco para a nova posição
    nivel.blocos[b].x = novox;
    nivel.blocos[b].y = novoy;
    return 1; // Movimento bem-sucedido
}


// Função para mover o jogador em (dx, dy). Se houver bloco, tenta empurrar
void movePlayer(int dx, int dy) {
    int novoX = nivel.jogador.x + dx;
    int novoY = nivel.jogador.y + dy;

    // Verifica se a celula esta ocupada
    if(!celulaLivre(novoX, novoY)) {
        return; // Não pode mover o jogador
    }
    // Verifica se há um bloco na nova posição do jogador
    int blocoIndex = indiceBloco(novoX, novoY);
    if(blocoIndex >= 0){
        // Tenta empurrar: ver a celula adiante
        int frenteX = novoX + dx;
        int frenteY = novoY + dy;
        if(!celulaLivre(frenteX, frenteY)) return; // Não pode mover o jogador se não houver espaço na frente 

        // Certifica-se de que não há outro bloco na célula de frente
        if(indiceBloco(frenteX, frenteY) != -1) return;
        

        // Empurra o bloco
        moveBloco(blocoIndex, dx, dy);
        // Atualiza a posição do jogador
        nivel.jogador.x = novoX;
        nivel.jogador.y = novoY;
    }
    else {
        // Se não há bloco, apenas move o jogador
        nivel.jogador.x = novoX;
        nivel.jogador.y = novoY;
    }

    // Atualiza direção
    if(dx == 0 && dy == -1) direcaoPlayer = 0; // cima
    else if(dx == 0 && dy == 1) direcaoPlayer = 1; // baixo
    else if(dx == -1 && dy == 0) direcaoPlayer = 2; // esquerda
    else if(dx == 1 && dy == 0) direcaoPlayer = 3; // direita

    // Avança frame de animação
    contadorAnim++;
    if(contadorAnim >= 2) { // Troca de frame a cada 2 movimentos (ajuste se quiser mais rápido/lento)
        framePlayer = (framePlayer + 1) % 4;
        // Ciclo: 0=andando_01, 1=parado, 2=andando_02, 3=parado
        if(framePlayer == 3) framePlayer = 1; // Sempre volta para parado após andando_02
        contadorAnim = 0;
    }
    glutPostRedisplay(); // Redesenha a cena após mover o jogador
}


// Funcção para verificar se o jogador venceu o nível(a vitoria é quando todos os blocos no mapa estão nas suas respectivas lixeiras)
int checkVitoria() {
    // Verifica se todos os blocos estão em lixeiras
    for(int i = 0; i < nivel.numBlocos; i++) {
        int blocoX = nivel.blocos[i].x;
        int blocoY = nivel.blocos[i].y;
        int lixeiraIndex = indiceLixeira(blocoX, blocoY);
        
        // Se o bloco não está em uma lixeira ou o tipo não bate, não venceu
        if(lixeiraIndex == -1 || nivel.blocos[i].tipo != nivel.lixeiras[lixeiraIndex].tipo) {
            return 0; // Não venceu
        }
    }
    // Se venceu, prepara próximo nível
    nivelAtual++;
    // Verifica se ainda há níveis disponíveis
    if(nivelAtual <= totalNiveis) {
        MapaInicial(nivelAtual); // Carrega o mapa inicial do próximo nível
        initLevel(); // Reinicializa o nível com o novo mapa
    }
    // Não há mais níveis disponíveis
    else {
        return 2; // Indica que venceu o jogo
    }
    return 1; // Venceu o nível
}