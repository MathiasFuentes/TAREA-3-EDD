#ifndef GRAFO_H
#define GRAFO_H

#include "list.h"
#include <stdbool.h>

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXNODES 1000
#define MAXNAME 256

// Definición Item
typedef struct {
    char name[MAXITEMNAME]; // Nombre del item
    int weight;             // Peso del item en kg
    int value;              // Valor del item
} Item;

// Definición del estado
typedef struct State {
    char    name[MAXNAME];
    char    description[MAXDESC];          // Descripción del escenario actual 
    List*   availableItems;
    List*   playerInventory;
    int     tiempoRestante;
    bool    esFinal;
} State;

// ✅ Definición completa de Node (antes de usarla en Graph)
typedef struct Node {
    State state;
    struct Node** adjacents;
} Node;

// ✅ Definición completa de Graph
typedef struct {
    int numberOfNodes;
    Node* nodes;
} Graph;

void leer_escenarios();
void mostrar_grafo();
extern Graph graph;

#endif
