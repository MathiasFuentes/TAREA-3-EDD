#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extra.h"
#include "list.h"
#include "grafo.h"
#include "game.h"

#ifdef _MSC_VER
#define strdup _strdup
#endif

#ifndef HAVE_STRDUP
char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}
#endif

#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 100

char **leer_linea_csv(FILE *archivo, char separador) {
    static char linea[MAX_LINE_LENGTH];
    static char *campos[MAX_FIELDS];

    if (fgets(linea, MAX_LINE_LENGTH, archivo) == NULL) {
        return NULL;
    }

    int i = 0;
    char *ptr = linea;
    while (*ptr && i < MAX_FIELDS) {
        if (*ptr == '"') {
            ptr++;
            campos[i++] = ptr;
            while (*ptr && (*ptr != '"' || ((*(ptr + 1) != separador) && (*(ptr + 1) != '\n')))) ptr++;
            *ptr = '\0';
            ptr++;
            if (*ptr == separador) ptr++;
        } else {
            campos[i++] = ptr;
            while (*ptr && *ptr != separador && *ptr != '\n') ptr++;
            if (*ptr) *ptr++ = '\0';
        }
    }

    return campos;
}

List *split_string(const char *str, const char *delim) {
    List *lista = list_create();
    if (!lista || !str || !delim) return NULL;

    char *copia = strdup(str);
    if (!copia) return NULL;

    char *token = strtok(copia, delim);
    while (token != NULL) {
        char *dato = strdup(token);
        list_pushBack(lista, dato);
        token = strtok(NULL, delim);
    }

    free(copia);
    return lista;
}

void limpiarPantalla(void) {
    system(CLEAR_CMD);
}

void presioneTeclaParaContinuar() {
    printf("\nPresione ENTER para continuar...");
    getchar();
}


void liberarEscenarios(Graph* g) {
    if (g == NULL) return;

    for (int i = 0; i < g->numberOfNodes; i++) {
        Node* nodo = &g->nodes[i];
        if (nodo != NULL) {
            if (nodo->adjacents) free(nodo->adjacents);
            if (nodo->state.availableItems) liberarListaItems(nodo->state.availableItems);
        }
    }

    free(g->nodes);
}

void liberarListaItems(List* lista) {
    if (!lista) return;

    void* item = list_first(lista);
    while (item != NULL) {
        free(item);
        item = list_next(lista);
    }

    list_destroy(lista);
}

void liberarJuego(Graph* g) {
    liberarEscenarios(g);
}
