// game.h
#ifndef GAME_H
#define GAME_H

#include "list.h"
#include "grafo.h"

typedef struct {
    List* inventory;    // inventario único del jugador
    int tiempoRestante; // tiempo que le queda al jugador
    Node* currentNode;  // escenario actual
} GameState;

void iniciar_partida(void);

void mostrar_puntaje_final(GameState* gs);
void mostrar_estado_actual(GameState* gs);
void showGameOptions(void);
char readOption(char* buf, int len);
void recoger_items(GameState* gs);
void descartar_items(GameState* gs);
void moverse(GameState* gs);


#endif // GAME_H
