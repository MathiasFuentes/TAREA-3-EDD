#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXNODES 1000
#define MAXNAME 256

typedef struct List List;

typedef struct {
    char name[MAXITEMNAME]; 
    int weight;          
    int value;  
} Item;

typedef struct State {
    char    name[MAXNAME];
    char    description[MAXDESC];
    List*   availableItems;
    List*   playerInventory;
    int     tiempoRestante;
    bool    esFinal;
} State;

typedef struct Node {
    State state;
    struct Node** adjacents;
} Node;

typedef struct Graph {
    Node* nodes;
    int numberOfNodes;
    int capacidad;
    Node* start;
} Graph;

void leer_escenarios();
void mostrar_grafo();
extern Graph graph;

#endif
