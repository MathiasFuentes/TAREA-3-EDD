#include "grafo.h"
#include "extra.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Graph graph;

void leer_escenarios() {
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    char **campos;
    int capacidad = 100; // Asumimos un máximo inicial
    graph.nodes = malloc(sizeof(Node) * capacidad);
    graph.numberOfNodes = 0;

    campos = leer_linea_csv(archivo, ','); // Leer encabezados

    // Primero: leer y crear nodos
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        if (graph.numberOfNodes >= capacidad) {
            capacidad *= 2;
            graph.nodes = realloc(graph.nodes, sizeof(Node) * capacidad);
        }

        int id = atoi(campos[0]);
        Node *node = &graph.nodes[id - 1]; // ID empieza en 1
        graph.numberOfNodes++;

        // Asignar nombre y descripción
        strncpy(node->state.description, campos[2], MAXDESC);

        // Crear lista de ítems
        node->state.availableItems = list_create();
        node->state.playerInventory = list_create(); // vacía por defecto
        node->state.tiempoRestante = 0;

        // Parsear items
        List* items = split_string(campos[3], ";");
        for (char *item = list_first(items); item != NULL; item = list_next(items)) {
            List *values = split_string(item, ",");
            if (values == NULL || list_size(values) < 3) continue;

            Item *newItem = malloc(sizeof(Item));
            strncpy(newItem->name, list_first(values), MAXITEMNAME);
            newItem->value = atoi(list_next(values));
            newItem->weight = atoi(list_next(values));

            list_pushBack(node->state.availableItems, newItem);

            list_clean(values);
            free(values);
        }
        list_clean(items);
        free(items);

        // Marcar si es final
        char *es_final = campos[8];
        for (int i = 0; es_final[i]; i++) es_final[i] = tolower(es_final[i]);
        node->state.esFinal = (strncmp(es_final, "sí", 2) == 0 || strncmp(es_final, "si", 2) == 0);

        // Inicializar adyacencias
        node->adjacents = calloc(4, sizeof(Node*));
    }

    // Segundo: abrir de nuevo el archivo para setear adyacencias por ID
    rewind(archivo);
    leer_linea_csv(archivo, ','); // saltar encabezado otra vez

    int idx = 0;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Node *node = &graph.nodes[idx++];

        int arriba    = atoi(campos[4]);
        int abajo     = atoi(campos[5]);
        int izquierda = atoi(campos[6]);
        int derecha   = atoi(campos[7]);

        if (arriba != -1)    node->adjacents[0] = &graph.nodes[arriba - 1];
        if (abajo != -1)     node->adjacents[1] = &graph.nodes[abajo - 1];
        if (izquierda != -1) node->adjacents[2] = &graph.nodes[izquierda - 1];
        if (derecha != -1)   node->adjacents[3] = &graph.nodes[derecha - 1];
    }

    fclose(archivo);
}
