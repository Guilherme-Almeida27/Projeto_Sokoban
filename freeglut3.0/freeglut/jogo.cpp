#include <string.h>
#include <stdio.h>
#include <GL/freeglut.h>
#include "jogo.h"
#include "util.h"

Level nivel; // Variável global para armazenar o nível atual

// ---------------------------Funções de configuração inicial---------------------------


// Inicializa o nível com o mapa inicial
/* 0 = PISO , 1 = PAREDE, 2 = BLOCO_PAPEL, 3 = BLOCO_PLASTICO 4 = LIXEIRA_PAPEL, 5 = LIXEIRA_PLASTICO */
void MapaInicial() {    
 
    int tmp[MAP_WIDTH][MAP_HEIGHT] = {
        {1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,2,0,0,0,3,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,4,0,0,0,0,1},
        {1,0,0,0,5,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1}
    };
    memcpy(nivel.mapaInicial, tmp, sizeof(tmp));
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
            if(cell == BLOCO_PAPEL || cell == BLOCO_PLASTICO) {
                nivel.blocos[nivel.numBlocos].x = x;
                nivel.blocos[nivel.numBlocos].y = y;
                nivel.blocos[nivel.numBlocos].tipo = cell;
                nivel.numBlocos++;
                nivel.mapaAtual[x][y] = PISO; // Remove o bloco do mapa
            }
            else if(cell == LIXEIRA_PAPEL || cell == LIXEIRA_PLASTICO) {
                nivel.lixeiras[nivel.numLixeiras].x = x;
                nivel.lixeiras[nivel.numLixeiras].y = y;
                nivel.lixeiras[nivel.numLixeiras].tipo = cell;
                nivel.numLixeiras++;
                nivel.mapaAtual[x][y] = PISO; // Remove a lixeira do mapa
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
    // Posiciona o jogador no promepio espaço vazio encontrado(pode se ajustado, no caso aqui está em (1,1) se for PISO)
    if(nivel.mapaAtual[1][1] == PISO){
        nivel.jogador.x = 1;
        nivel.jogador.y = 1;
    } else {
        // Se (1,1) não for piso, procura o proximo espaço vazio
        nivel.jogador.x = -1; // Marca como não encontrado
        nivel.jogador.y = -1; // Marca como não encontrado
        for(int y = 0; y < MAP_HEIGHT; y++) {
            for(int x = 0; x < MAP_WIDTH; x++) {
                if(nivel.mapaAtual[x][y] == PISO) {
                    nivel.jogador.x = x;
                    nivel.jogador.y = y;
                    goto Encontrado; // Sai do loop quando encontra o primeiro piso livre
                }
            }
        }
        Encontrado:
        // Se não encontrou nenhum piso, deixa o jogador em (-1,-1) indicando que não foi posicionado
        if(nivel.jogador.x == -1 && nivel.jogador.y == -1) {
            printf("Erro: Nenhum espaço vazio encontrado para posicionar o jogador.\n");
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
static int celulaLivre(int x, int y){
    // Verifica se as coordenadas estão dentro dos limites do mapa
    if(y < 0 || y >= MAP_WIDTH || x < 0 || x >= MAP_HEIGHT) {
        return 0; // Fora dos limites
    }
    // Verifica se a célula é piso ou lixeira
    if(nivel.mapaAtual[x][y] == PISO ||
       nivel.mapaAtual[x][y] == LIXEIRA_PAPEL || nivel.mapaAtual[x][y] == LIXEIRA_PLASTICO) {
        return 1; // Célula livre
    }
    return 0; // Célula ocupada
}

// Função para retornar o índice do bloco em nivel.blocos[] 
static int indiceBloco(int x, int y) {
    for(int i = 0; i < nivel.numBlocos; i++) {
        if(nivel.blocos[i].x == y && nivel.blocos[i].y == x) {
            return i; // Retorna o índice do bloco encontrado
        }
    }
    return -1; // Retorna -1 se não encontrar o bloco
}

// Função para retornar o índice da lixeira em nivel.lixeiras[]
static int indiceLixeira(int x, int y) {
    for(int i = 0; i < nivel.numLixeiras; i++) {
        if(nivel.lixeiras[i].x == y && nivel.lixeiras[i].y == x) {
            return i; // Retorna o índice da lixeira encontrada
        }
    }
    return -1; // Retorna -1 se não encontrar a lixeira
}

// Move o bloco b em (dx, dy) se possível
static int moveBloco(int b, int dx, int dy) {
    int novox = nivel.blocos[b].x += dx;
    int novoy = nivel.blocos[b].y += dy;

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

    // Se for lixeira, verifica tipo compatível
    int lixeiraIndex = indiceLixeira(novox, novoy);
    if(lixeiraIndex != -1) {
        if(nivel.blocos[b].tipo == BLOCO_PAPEL && nivel.lixeiras[lixeiraIndex].tipo != LIXEIRA_PAPEL) {
            return 0; // Não pode mover para lixeira de tipo diferente
        }
        if(nivel.blocos[b].tipo == BLOCO_PLASTICO && nivel.lixeiras[lixeiraIndex].tipo != LIXEIRA_PLASTICO) {
            return 0; // Não pode mover para lixeira de tipo diferente
        }
    }
    // Move o bloco para a nova posição
    nivel.blocos[b].x = novox;
    nivel.blocos[b].y = novoy;
    // Atualiza o mapaAtual para refletir a nova posição do bloco
    nivel.mapaAtual[nivel.blocos[b].x][nivel.blocos[b].y] = nivel.blocos[b].tipo;
    // Remove o bloco da posição antiga no mapaAtual
    nivel.mapaAtual[nivel.blocos[b].x - dx][nivel.blocos[b].y - dy] = PISO; // Marca a posição antiga como piso
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
    printf("Chamada: indice_bloco_em(novaY=%d, novaX=%d)\n", novoY, novoX);
    int blocoIndex = indiceBloco(novoX, novoY);
    if(blocoIndex >= 0){
        // Tenta empurrar: ver a celula adiante
        int frenteX = novoX + dx;
        int frenteY = novoY + dy;
        if(!celulaLivre(frenteX, frenteY)) return; // Não pode mover o jogador se não houver espaço na frente 

        // Certifica-se de que não há outro bloco na célula de frente
        if(indiceBloco(frenteX, frenteY) != -1) return;

        // Se a frente for lixeira, só empurre se o tipo combinar ou se for piso mesmo assim (permite mover)
        int lixeiraIndex = indiceLixeira(frenteX, frenteY);
        if(lixeiraIndex != -1) {
            // Se o bloco não combina com a lixeira, não pode empurrar
            if(nivel.blocos[blocoIndex].tipo != nivel.lixeiras[lixeiraIndex].tipo) return;
        }


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
    glutPostRedisplay(); // Redesenha a cena após mover o jogador
}


// Funcção para verificar se o jogador venceu o nível(a vitoria é quando todos os blocos no mapa estão nas suas respectivas lixeiras)
int checkVitoria() {
    // Verifica se todos os blocos estão em lixeiras
    for(int i = 0; i < nivel.numBlocos; i++) {
        int blocoX = nivel.blocos[i].x;
        int blocoY = nivel.blocos[i].y;
        int lixeiraIndex = indiceLixeira(blocoX, blocoY);
        
        // Se o bloco não está em uma lixeira, o jogador não venceu
        if(lixeiraIndex == -1 || 
           (nivel.blocos[i].tipo == BLOCO_PAPEL && nivel.lixeiras[lixeiraIndex].tipo != LIXEIRA_PAPEL) ||
           (nivel.blocos[i].tipo == BLOCO_PLASTICO && nivel.lixeiras[lixeiraIndex].tipo != LIXEIRA_PLASTICO)) {
            return 0; // Não venceu
        }
    }
    return 1; // Venceu
}