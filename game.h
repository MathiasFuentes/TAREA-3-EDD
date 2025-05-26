// game.h
#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct List List;
typedef struct Node Node;
typedef struct Graph Graph;

typedef struct {
    List* inventory;
    int tiempoRestante;
    Node* currentNode;
    int puntaje;
} Player;

typedef struct {
    List* inventory;    // inventario único del jugador
    int tiempoRestante; // tiempo que le queda al jugador
    Node* currentNode;  // escenario actual
} GameState;

typedef struct {
    Player jugadores[2];
    Graph* grafo;
    int turnoActual;
} GameStateMultiplayer;

void iniciar_partida(Graph* grafo);
void mostrar_puntaje_final(GameState* gs);
void mostrar_estado_actual(GameState* gs, int indicador);
void showGameOptions(void);
char readOption(char* buf, int len);
void recoger_items(GameState* gs, bool esMultijugador, Node* otroJugadorNode);
void descartar_items(GameState* gs, int indicador);
bool moverse(GameState* gs);
void iniciar_partida_multijugador(Graph* grafo);


#endif // GAME_H
