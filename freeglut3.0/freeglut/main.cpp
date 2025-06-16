#include <stdlib.h>
#include <GL/freeglut.h>
#include "jogo.h"
#include "render.h"
#include "util.h"

const int WINDOW_WIDTH = 800; // Largura da janela
const int WINDOW_HEIGHT = 600; // Altura da janela

// Estado do jogo
int estadoJogo = 0; // 0 = menu inicial, 1 = jogando, 2 = vitoria
extern int nivelAtual; // Nível atual do jogo

// Menu GLUT
void menu_principal(int option) {
    switch(option) {
        case 1: // Iniciar jogo
            estadoJogo = 1; // Muda para estado de jogo
            initLevel(); // Inicializa o nível
            glutPostRedisplay(); // Redesenha a cena
            break;
        case 2: // Sair do jogo
            exit(0); // Encerra o programa
            break;
    }
}

// Cria menu de contexto (botão direito)
void criaMenu() {
    glutCreateMenu(menu_principal);
    glutAddMenuEntry("Iniciar Jogo", 1);
    glutAddMenuEntry("Sair", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON); // Anexa o menu ao botão direito do mouse
}

// Callback para o teclado
void teclado(unsigned char key, int x, int y) {
    if (estadoJogo == 1) {
        // Durante o jogo, usamos WASD para mover
        switch (key) {
            case 'w':
            case 'W': // Move para cima
                movePlayer(0, -1);
                break;
            case 's':
            case 'S': // Move para baixo
                movePlayer(0, 1);
                break;
            case 'a':
            case 'A': // Move para a esquerda
                movePlayer(-1, 0);
                break;
            case 'd':
            case 'D': // Move para a direita
                movePlayer(1, 0);
                break;
            case 'r':
            case 'R': // Reinicia o nível
                reiniciaLevel();
                glutPostRedisplay(); // Redesenha a cena
                break;
        }
        if(key == 27){
            estadoJogo = 0; // Volta para o menu inicial
            glutPostRedisplay(); // Redesenha a cena
        }
    }
}

// Callback para teclas especiais (setas)
void tecladoEspecial(int tecla, int x, int y) {
    switch(tecla) {
        case GLUT_KEY_LEFT:
            movePlayer(-1, 0);
            break;
        case GLUT_KEY_RIGHT:
            movePlayer(1, 0);
            break;
        case GLUT_KEY_UP:
            movePlayer(0, -1);
            break;
        case GLUT_KEY_DOWN:
            movePlayer(0, 1);
            break;
    }
}

// Função chamada pelo GLUT a cada frame para desenhar
void display(){
    glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor
    if(estadoJogo == 0) {
        // Menu inicial simples(precisa se melhorado posteriormentte)

    } 
    else if(estadoJogo == 1) {
        // Desenha a cena do jogo
        desenhaCena(); // Desenha o mapa, blocos e jogador
        if(checkVitoria() == 2) {
            estadoJogo = 2; // Muda para estado de vitória
        }
    }

    else if(estadoJogo == 2) {
        // Tela de vitória
        glColor3f(0.0f, 0.5f, 0.0f); // Cor verde para o texto
        desenhaTexto("Você venceu!", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 20);
        desenhaTexto("Pressione ESC para reiniciar", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2);
    }
    glutSwapBuffers(); // Troca os buffers para exibir o que foi desenhado
}


// Função principal
int main(int argc, char** argv){
    glutInit(&argc, argv); // Inicializa o GLUT
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); // Configura o modo de exibição
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); // Define o tamanho da janela
    glutInitWindowPosition(100, 100); // Define a posição da janela na tela
    glutCreateWindow("Sokobon Coleta Seletica(nome temporario)"); // Cria a janela com o título
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Define a cor de fundo como branca


    //  Inicializa Level e renderização
    MapaInicial(1); // Cria o mapa inicial do nível
    initRender(WINDOW_WIDTH, WINDOW_HEIGHT); // Inicializa a renderização
    carregaTexturaParede(); // Carrega a textura da parede
    carregaTexturaPlayer(); // Carrega as texturas do jogador
    carregaTexturaPiso(); // Carrega a textura do piso
    carregaTexturaItens(); // Carrega as texturas dos itens (blocos e lixeiras)
    initLevel(); // Inicializa o nível

    // Callbacks do GLUT
    glutDisplayFunc(display); // Define a função de exibição
    glutKeyboardFunc(teclado); // Define a função de teclado
    glutSpecialFunc(tecladoEspecial); // Define a função de teclas especiais
    glutReshapeFunc(redimensionaJanela); // Define a função de redimensionamento da janela


    criaMenu(); // Cria o menu de contexto

    glutMainLoop(); // Entra no loop principal do GLUT
    return 0;
}