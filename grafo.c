#include "grafo.h"
#include "extra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Graph* leer_escenarios() {
    FILE* archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    Node* nodos = (Node*) calloc(MAXNODES, sizeof(Node));
    int totalNodos = 0;

    char** campos;

    // Leer encabezado
    campos = leer_linea_csv(archivo, ',');
    if (campos) {
        for (int i = 0; campos[i] != NULL; i++) free(campos[i]);
        free(campos);
    }

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        if (!campos[0] || !campos[2] || !campos[3] || !campos[4] || !campos[5] || !campos[6] || !campos[7] || !campos[8]) {
            for (int i = 0; campos[i] != NULL; i++) free(campos[i]);
            free(campos);
            continue;
        }

        int id = atoi(campos[0]);
        if (id < 0 || id >= MAXNODES) {
            printf("ID inválido: %d\n", id);
            for (int i = 0; campos[i] != NULL; i++) free(campos[i]);
            free(campos);
            continue;
        }

        Node* nodo = &nodos[id];
        if (id >= totalNodos) totalNodos = id + 1;

        strncpy(nodo->state.description, campos[2], MAXDESC - 1);
        nodo->state.description[MAXDESC - 1] = '\0';

        nodo->state.availableItems = list_create();

        List* items_raw = split_string(campos[3], ";");
        for (char* raw_item = list_first(items_raw); raw_item != NULL; raw_item = list_next(items_raw)) {
            List* datos = split_string(raw_item, ",");

            if (!datos || list_size(datos) < 3) {
                if (datos) {
                    list_clean(datos);
                    free(datos);
                }
                continue;
            }

            char* name = list_first(datos);
            char* value_str = list_next(datos);
            char* weight_str = list_next(datos);

            if (!name || !value_str || !weight_str) {
                list_clean(datos);
                free(datos);
                continue;
            }

            Item* item = malloc(sizeof(Item));
            strncpy(item->name, name, MAXITEMNAME - 1);
            item->name[MAXITEMNAME - 1] = '\0';
            item->value = atoi(value_str);
            item->weight = atoi(weight_str);

            list_pushBack(nodo->state.availableItems, item);

            list_clean(datos);
            free(datos);
        }
        list_clean(items_raw);
        free(items_raw);

        for (int i = 0; i < MAXDIR; i++) {
            nodo->state.possibleDirections[i] = atoi(campos[4 + i]);
        }

        nodo->state.tiempoRestante = 10;
        nodo->state.playerInventory = list_create();
        nodo->state.esFinal = atoi(campos[8]);

        nodo->adjacents = calloc(MAXDIR, sizeof(Node*));

        for (int i = 0; campos[i] != NULL; i++) free(campos[i]);
        free(campos);
    }

    fclose(archivo);

    for (int i = 0; i < totalNodos; i++) {
        for (int dir = 0; dir < MAXDIR; dir++) {
            int vecinoID = nodos[i].state.possibleDirections[dir];
            if (vecinoID != -1 && vecinoID < totalNodos) {
                nodos[i].adjacents[dir] = &nodos[vecinoID];
            }
        }
    }

    Graph* grafo = malloc(sizeof(Graph));
    grafo->numberOfNodes = totalNodos;
    grafo->nodes = nodos;

    printf("Laberinto cargado: %d nodos\n", totalNodos);
    return grafo;
}
