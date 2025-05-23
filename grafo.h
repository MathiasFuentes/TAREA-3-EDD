#ifndef GRAFO_H
#define GRAFO_H

#include "list.h"
#include <stdbool.h>

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXNODES 1000
#define MAXNAME 256
/*
    Definiciones de structs a usar en grafo explícito.
*/

// Predefinición Nodo
typedef struct Node Node;

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
    int     remainingTime;
    bool    esFinal;
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
void leer_escenarios();

// Muestra el grafo global
void mostrar_grafo();

extern Graph graph;
// (más adelante puedes agregar otras funciones como liberar_grafo, mostrar_nodo, etc.)

#endif
