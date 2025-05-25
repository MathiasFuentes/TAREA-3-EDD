#ifndef EXTRA_H
#define EXTRA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct List List;
typedef struct Graph Graph;

#ifdef _WIN32
#define CLEAR_CMD "cls"
#else
#define CLEAR_CMD "clear"
#endif

char **leer_linea_csv(FILE *archivo, char separador);
List *split_string(const char *str, const char *delim);
void limpiarPantalla(void);
void presioneTeclaParaContinuar(void);
void liberarEscenarios(Graph* g);
void liberarListaItems(List* lista);
void liberarJuego(Graph* g);

#endif // EXTRA_H
