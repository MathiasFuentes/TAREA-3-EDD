#ifndef GRAFO_H
#define GRAFO_H

#include "list.h"
#include <stdbool.h>

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXNODES 1000

typedef struct Node Node;

typedef struct {
    char name[MAXITEMNAME];
    int weight;
    int value;
} Item;

typedef struct State {
    char description[MAXDESC];
    char possibleDirections[MAXDIR];
    List* availableItems;
    List* playerInventory;
    int tiempoRestante;
    bool esFinal;
} State;

struct Node {
    State state;
    Node** adjacents;
};

typedef struct {
    int numberOfNodes;
    Node* nodes;
} Graph;

// Carga el grafo desde un CSV
Graph* leer_escenarios();

// (más adelante puedes agregar otras funciones como liberar_grafo, mostrar_nodo, etc.)

#endif
