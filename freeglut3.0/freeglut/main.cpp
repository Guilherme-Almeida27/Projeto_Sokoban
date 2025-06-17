// Criado por: Guilherme José Almeida Nunes e Anderson André Ferreira Gomes
// Disciplina: Computação Gráfica
// Descrição: Jogo Sokoban com temática de coleta seletiva em C++ usando OpenGL e FreeGLUT
// Compilação Makefile: makefile ou mingw32-make

#include <stdlib.h>
#include <GL/freeglut.h>
#include "jogo.h"
#include "render.h"
#include "util.h"
#include <stdio.h>

const int WINDOW_WIDTH = 800; // Largura da janela
const int WINDOW_HEIGHT = 600; // Altura da janela

// Estado do jogo
// int estadoJogo = 0; // 0 = menu inicial, 1 = jogando, 2 = vitoria // Comentário antigo
#define MENU_INICIAL 0
#define JOGANDO 1
#define VITORIA_JOGO 2
#define PAUSADO 3
int estadoJogo = MENU_INICIAL;
extern int nivelAtual; // Nível atual do jogo

// Menu GLUT
void menu_principal(int option) {
    switch(option) {
        case 1: // Iniciar jogo
            nivelAtual = 1; // Muda para estado de jogo
            MapaInicial(nivelAtual); // Cria o mapa inicial do nível
            initLevel(); // Inicializa o nível
            estadoJogo = JOGANDO; // Muda o estado do jogo para jogando
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
    if (estadoJogo == JOGANDO) {
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
        if(key == 27){ // ESC para pausar o jogo
            estadoJogo = PAUSADO; // Vai para o menu de pausa
            glutPostRedisplay(); // Redesenha a cena
        }
    } 
    else if (estadoJogo == MENU_INICIAL) {
            if (key == 13) { // Tecla Enter
                nivelAtual = 1; // Garante que começa do nível 1
                MapaInicial(nivelAtual);
                initLevel();
                estadoJogo = JOGANDO;
                glutPostRedisplay();
            } 
            else if (key == 27) { // Tecla ESC
                exit(0); // Sai do jogo
            }
    }
        else if (estadoJogo == PAUSADO) {
            switch (key) {
                case 'c':
                case 'C': // Continuar
                    estadoJogo = JOGANDO;
                    glutPostRedisplay();
                    break;
                case 'r':
                case 'R': // Reiniciar Nível
                    reiniciaLevel();
                    estadoJogo = JOGANDO; // Volta ao jogo após reiniciar
                    glutPostRedisplay();
                    break;
                case 'm':
                case 'M': // Voltar ao Menu Principal
                    estadoJogo = MENU_INICIAL;
                    glutPostRedisplay();
                    break;
                default:
                    break;
            }
        } 
        else if (estadoJogo == VITORIA_JOGO) {
            if (key == 27) { // Tecla ESC
                estadoJogo = MENU_INICIAL;
                glutPostRedisplay();
            }
        }
}

// Callback para teclas especiais (setas)
void tecladoEspecial(int tecla, int x, int y) {
    if(estadoJogo == JOGANDO){
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
}

// Função chamada pelo GLUT a cada frame para desenhar
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Limpa o buffer de cor
    if(estadoJogo == MENU_INICIAL) {
        // Menu inicial simples
        glColor3f(0.0f, 0.0f, 0.0f); // Cor preta para o texto
        desenhaTexto("Sokoban Coleta Seletiva", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 + 60);
        desenhaTexto("Pressione ENTER para Iniciar", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 + 20);
        desenhaTexto("Pressione ESC para Sair", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 - 20);
        desenhaTexto("Clique com o botao direito para mais opcoes.", WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 60);
    } 
    else if(estadoJogo == JOGANDO) {
        // Desenha a cena do jogo
        desenhaCena(); // Desenha o mapa, blocos e jogador
        int vitoriaStatus = checkVitoria(); // Verifica se o jogador venceu o nível
        if(vitoriaStatus == 2) {
            estadoJogo = VITORIA_JOGO; // Muda para estado de vitória do jogo
        }
        else if (vitoriaStatus == 1) { // checkVitoria retorna 1 se o nível atual foi concluído
            // O próximo nível já foi carregado por checkVitoria, apenas redesenha.
            // Poderia adicionar uma breve mensagem de "Nível Concluído!" aqui se desejado.
            glutPostRedisplay();
        }
    } 
    else if(estadoJogo == PAUSADO) {
        // Tela de Pausa
        // Desenha a cena do jogo por baixo para dar um efeito de pausa sobre o jogo
        desenhaCena(); 
        // Desenha um overlay semitransparente (opcional, para escurecer o fundo)
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.5f); // Preto semitransparente
        glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(WINDOW_WIDTH, 0);
            glVertex2f(WINDOW_WIDTH, WINDOW_HEIGHT);
            glVertex2f(0, WINDOW_HEIGHT);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para o texto do menu de pausa
        desenhaTexto("PAUSA", WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 60);
        desenhaTexto("Pressione C para Continuar", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 + 20);
        desenhaTexto("Pressione R para Reiniciar Nivel", WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 20);
        desenhaTexto("Pressione M para Voltar ao Menu Principal", WINDOW_WIDTH / 2 - 220, WINDOW_HEIGHT / 2 - 60);
    }
    else if(estadoJogo == VITORIA_JOGO) {
        // Tela de vitória
        glColor3f(0.0f, 0.5f, 0.0f); // Cor verde para o texto
        desenhaTexto("Voce Venceu Todos os Niveis!", WINDOW_WIDTH / 2 - 150, WINDOW_HEIGHT / 2 + 20);
        desenhaTexto("Pressione ESC para Voltar ao Menu Principal", WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 20);
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
    //MapaInicial(1); // Cria o mapa inicial do nível
    initRender(WINDOW_WIDTH, WINDOW_HEIGHT); // Inicializa a renderização
    carregaTexturaParede(); // Carrega a textura da parede
    carregaTexturaPlayer(); // Carrega as texturas do jogador
    carregaTexturaPiso(); // Carrega a textura do piso
    carregaTexturaItens(); // Carrega as texturas dos itens (blocos e lixeiras)
    //initLevel(); // Inicializa o nível

    // Callbacks do GLUT
    glutDisplayFunc(display); // Define a função de exibição
    glutKeyboardFunc(teclado); // Define a função de teclado
    glutSpecialFunc(tecladoEspecial); // Define a função de teclas especiais
    glutReshapeFunc(redimensionaJanela); // Define a função de redimensionamento da janela


    criaMenu(); // Cria o menu de contexto

    glutMainLoop(); // Entra no loop principal do GLUT
    return 0;
}