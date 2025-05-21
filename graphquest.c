#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "grafo.h"   // Contiene Item, State, Node, Graph, leer_escenarios()
#include "list.h"
#include "extra.h"

#define MAXOPTION 256

// Mostrar menú principal
void showPrincipalOptions() {
    limpiarPantalla();
    puts("---------- GraphQuest ----------\n");
    puts("-------- Menú Principal --------");
    puts("1)   Cargar Laberinto desde CSV");
    puts("2)   Iniciar Partida");
    puts("3)   Salir");
}

// Mostrar opciones de juego
void mostrarOpcionesJuego() {
    limpiarPantalla();
    puts("---------- GraphQuest ----------\n");
    puts("------ Opciones Disponibles ------");
    puts("1)   Recoger Ítem(s)");
    puts("2)   Descartar Ítem(s)");
    puts("3)   Avanzar en una dirección");
    puts("4)   Reiniciar partida");
    puts("5)   Salir del juego");
}

// Leer y validar opción del usuario
char readOption(char reading[MAXOPTION], int maxOpciones) {
    while (1) {
        printf("Ingrese una opción (1-%d): ", maxOpciones);
        fgets(reading, MAXOPTION, stdin);

        size_t len = strlen(reading);
        if (len > 0 && reading[len - 1] == '\n') {
            reading[len - 1] = '\0';
            len--;
        }

        if (len == 1 && isdigit(reading[0])) {
            int opcionNumerica = reading[0] - '0';
            if (opcionNumerica >= 1 && opcionNumerica <= maxOpciones) {
                return reading[0];
            }
        }

        return '0';
    }
}

// Mostrar el estado actual del jugador
void mostrar_estado_actual(Node* current) {
    printf("\nEscenario actual:\n%s\n", current->state.description);
    printf("Tiempo restante: %d\n", current->state.tiempoRestante);
    printf("Ítems disponibles:\n");

    if (list_size(current->state.availableItems) == 0) {
        puts("No hay ítems en este escenario.");
    } else {
        for (Item* it = list_first(current->state.availableItems); it != NULL; it = list_next(current->state.availableItems)) {
            printf("- %s (valor: %d, peso: %d)\n", it->name, it->value, it->weight);
        }
    }
}

// Moverse entre nodos del grafo
void moverse(Node** current) {
    Node* actual = *current;
    char* direcciones[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};

    puts("Direcciones disponibles:");
    for (int i = 0; i < 4; i++) {
        if (actual->adjacents[i] != NULL) {
            printf("(%d) %s\n", i, direcciones[i]);
        }
    }

    int dir;
    scanf("%d", &dir);
    getchar();

    if (dir >= 0 && dir < 4 && actual->adjacents[dir] != NULL) {
        int pesoTotal = 0;
        for (Item* it = list_first(actual->state.playerInventory); it != NULL; it = list_next(actual->state.playerInventory)) {
            pesoTotal += it->weight;
        }

        int tiempoGastado = (pesoTotal + 1) / 10;
        if (tiempoGastado == 0) tiempoGastado = 1;

        actual->state.tiempoRestante -= tiempoGastado;
        *current = actual->adjacents[dir];

    } else {
        puts("Dirección inválida o no disponible.");
    }
}

// Iniciar una partida del juego
void iniciar_partida(Graph* maze) {
    Node* actual = &maze->nodes[0];
    actual->state.tiempoRestante = 10;
    list_clean(actual->state.playerInventory);

    char option;
    char reading[MAXOPTION];

    while (1) {
        mostrar_estado_actual(actual);
        mostrarOpcionesJuego();
        option = readOption(reading, 5);

        switch (option) {
            case '1':
                puts("Recoger ítems aún no implementado.");
                break;
            case '2':
                puts("Descartar ítems aún no implementado.");
                break;
            case '3':
                moverse(&actual);
                if (actual->state.tiempoRestante <= 0) {
                    puts("¡Se acabó el tiempo! Has perdido.");
                    return;
                }
                if (actual->state.esFinal) {
                    puts("¡Has llegado al escenario final! ¡Felicidades!");
                    return;
                }
                break;
            case '4':
                actual = &maze->nodes[0];
                actual->state.tiempoRestante = 10;
                list_clean(actual->state.playerInventory);
                puts("Partida reiniciada.");
                break;
            case '5':
                puts("Saliendo del juego...");
                return;
            default:
                puts("Opción inválida.");
        }

        presioneTeclaParaContinuar();
    }
}


int main() {
    Graph* maze = NULL;
    char option;

    do {
        showPrincipalOptions();
        char reading[MAXOPTION];
        option = readOption(reading, 3);

        switch (option) {
            case '1':
                maze = leer_escenarios();
                if (maze) puts("Laberinto cargado correctamente.");
                else puts("Error al cargar el laberinto.");
                break;
            case '2':
                if (!maze) {
                    puts("Debes cargar el laberinto primero.");
                    break;
                }
                iniciar_partida(maze);
                break;
            case '3':
                puts("Gracias por jugar GraphQuest. ¡Hasta la próxima!");
                break;
            default:
                puts("Opción inválida.");
        }

        presioneTeclaParaContinuar();
    } while (option != '3');

    return 0;
}
