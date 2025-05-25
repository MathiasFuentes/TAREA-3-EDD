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
    char    name[MAXITEMNAME];  // Nombre del ítem (ej: "Espada mágica", "Poción curativa")
    int     weight;             // Peso del ítem en kilogramos (afecta capacidad del jugador)
    int     value;              // Valor del ítem en puntos (relevante para el puntaje del jugador)
} Item;


typedef struct State {
    char    name[MAXNAME];         // Nombre del escenario (ej: "Bosque encantado")
    char    description[MAXDESC];  // Descripción narrativa del escenario (se muestra al jugador)

    List*   availableItems;        // Lista de ítems disponibles en el escenario (List de Item*)
    List*   playerInventory;       // Inventario del jugador en este nodo (usualmente vacío inicialmente)
    
    int     tiempoRestante;        // Tiempo restante para completar acciones en este nodo
    bool    esFinal;               // Marca si este escenario es un nodo final del juego (true = sí, false = no)
} State;


typedef struct Node {
    State   state;              // Estado del nodo: incluye descripción, ítems, si es final, etc.
    struct  Node** adjacents;   // Puntero a arreglo de 4 punteros a nodos adyacentes (arriba, abajo, izquierda, derecha)
} Node;


typedef struct Graph {
    Node*   nodes;              // Arreglo dinámico de nodos (escenarios del juego)
    int     numberOfNodes;      // Número actual de nodos cargados desde el CSV
    int     capacidad;          // Capacidad del arreglo de nodos
    Node*   start;              // Nodo inicial donde comienza el jugador
} Graph;

// Función para leer los escenarios desde un archivo .csv
void leer_escenarios();

// Función para mostrar los escenarios del archivo leído
void mostrar_grafo();

extern Graph graph;

#endif
