// game.h
#ifndef GAME_H
#define GAME_H

#define MAXOPTION 256
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

char readOption(char reading[MAXOPTION], int maxOpciones);

void iniciar_partida(Graph* grafo);

void iniciar_partida_multijugador(Graph* grafo);

void recoger_items(GameState* gs, bool esMultijugador, Node* otroJugadorNode);

void descartar_items(GameState* gs, int indicador);

void mostrar_puntaje_final(GameState* gs);

void mostrar_estado_actual(GameState* gs, int indicador);

bool moverse(GameState* gs);

void iniciar_partida_multijugador(Graph* grafo);

void mostrar_resultados_finales(GameStateMultiplayer* gs);

void mostrar_estado_jugador_actual(GameStateMultiplayer* gs);

void sincronizar_items(Graph* grafo1, Graph* grafo2, GameState* jugador1, GameState* jugador2);

void showPrincipalOptions();

void showGameOptions();



#endif // GAME_H
