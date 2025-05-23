#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "list.h"
#include "extra.h"
#include "grafo.h"

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXOPTION 256
#define MAXNODES 1000

/*
### **Opciones del Jugador**

1. **Recoger Ítem(s)**
    - El jugador puede seleccionar uno o más ítems del escenario para agregarlos a su inventario. Se descuenta 1 de tiempo.

2. **Descartar Ítem(s)**
    - El jugador puede eliminar ítems de su inventario para reducir peso y move más rápido. Se descuenta 1 de tiempo.

3. **Avanzar en una Dirección**
    - El jugador elige una dirección válida.
    - Se actualiza el escenario actual, el inventario se conserva, y se descuenta el tiempo usado según el peso total transportado: T = (Peso total del inventario + 1) / 10
    - Si se alcanza el escenario final, se muestran los elementos del inventario y el **puntaje final.**
    - Si el **tiempo se agota**, se muestra un mensaje de derrota.

4. **Reiniciar Partida**
    - Se reinicia el juego desde el escenario inicial, con inventario vacío y tiempo completo.

5. **Salir del Juego**
    - Finaliza la partida y cierra la aplicación.
*/

void showPrincipalOptions(){
    limpiarPantalla();
    puts("---------- GraphQuest ----------\n");
    puts("-------- Menú Principal --------");
    puts("(1)   Cargar Laberinto desde CSV.");
    puts("(2)   Mostrar Escenarios Cargados.");
    puts("(3)   Iniciar Partida.");
    puts("(4)   Salir");
}

void showGameOptions(){
    puts("-------- Partida en Curso --------\n");
    puts("------ Opciones Disponibles ------");
    puts("(1)   Recoger Ítem(s)");
    puts("(2)   Descartar Ítem(s)");
    puts("(3)   Avanzar en una dirección");
    puts("(4)   Reiniciar partida");
    puts("(5)   Salir del juego");
}

char readOption(char reading[MAXOPTION], int maxOptions) {
    while (1) {
        printf("Ingrese una opción (1-%d): ", maxOptions);
        fgets(reading, MAXOPTION, stdin);

        // Eliminar salto de línea si está presente
        size_t len = strlen(reading);
        if (len > 0 && reading[len - 1] == '\n') {
            reading[len - 1] = '\0';
            len--;
        }

        // Validar que la entrada tenga exactamente un carácter numérico
        if (len == 1 && isdigit(reading[0])) {
            int numericalOption = reading[0] - '0';
            if (numericalOption >= 1 && numericalOption <= maxOptions) {
                return reading[0]; // Retorna el carácter ('1', '2', etc.)
            }
        }

        // printf("Opción inválida. Intente nuevamente.\n");
        return '0';
    }
}

/*
    Función readOption() HAY QUE COMENTAR AQUÍ
*/

void show_actual_state(Node* current) {
    
    if (current == NULL) {
        puts("Error: el nodo actual es NULL.");
        return;
    }

    if (current->state.availableItems == NULL) {
        puts("Advertencia: availableItems es NULL.");
        current->state.availableItems = list_create(); // Solución de emergencia
    }

    if (current->state.playerInventory == NULL) {
        puts("Advertencia: playerInventory es NULL.");
        current->state.playerInventory = list_create(); // Solución de emergencia
    }

    printf("\nEscenario actual:\n%s\n", current->state.description);
    printf("Tiempo restante: %d\n", current->state.remainingTime);
    printf("Ítems disponibles:\n");

    if (list_size(current->state.availableItems) == 0) {
        puts("No hay ítems en este escenario.");
    }
    else {
        for (Item* it = list_first(current->state.availableItems); it != NULL; it = list_next(current->state.availableItems)) {
            printf("- %s (valor: %d, peso: %d)\n", it->name, it->value, it->weight);
        }
    }
}

// Move entre nodos del grafo
void move(Node** current) {
    Node* actual = *current;
    const char* directions[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};

    puts("\nDirecciones disponibles:");
    for (int i = 0; i < 4; i++) {
        if (actual->adjacents[i] != NULL) {
            printf("(%d) %s\n", i, directions[i]);
        }
    }

    printf("Ingrese una dirección (0-3): ");
    char input[16];
    fgets(input, sizeof(input), stdin);
    int dir = atoi(input);

    if (dir >= 0 && dir < 4 && actual->adjacents[dir] != NULL) {
        if (actual->state.playerInventory == NULL)
            actual->state.playerInventory = list_create();

        int pesoTotal = 0;
        for (Item* it = list_first(actual->state.playerInventory); it != NULL; it = list_next(actual->state.playerInventory)) {
            pesoTotal += it->weight;
        }

        int tiempoGastado = (pesoTotal + 1) / 10;
        if (tiempoGastado == 0) tiempoGastado = 1;

        *current = actual->adjacents[dir];

        if ((*current)->state.availableItems == NULL)
            (*current)->state.availableItems = list_create();

        if ((*current)->state.playerInventory == NULL)
            (*current)->state.playerInventory = list_create();

        (*current)->state.remainingTime -= tiempoGastado;

        printf("\nTe has movido a un nuevo escenario.\n");
        printf("Tiempo gastado: %d\n", tiempoGastado);
        printf("Tiempo restante: %d\n", (*current)->state.remainingTime);
    } else {
        puts("Dirección inválida o no disponible.");
    }
}



// Iniciar una partida del juego
void start_game() {
    limpiarPantalla();
    if (graph.numberOfNodes == 0) {
        puts("Primero debes cargar el laberinto desde el CSV.");
        return;
    }

    Node* actual = &graph.nodes[0]; // Comienza desde el primer nodo
    actual->state.remainingTime = 10;

    if (actual->state.playerInventory == NULL)
        actual->state.playerInventory = list_create();
    else
        list_clean(actual->state.playerInventory);

    char option;
    char reading[MAXOPTION];
    
    printf("current: %p\n", actual);
    printf("inv: %p, items: %p\n", actual->state.playerInventory, actual->state.availableItems);

    while (1) {
        show_actual_state(actual);
        showGameOptions();
        option = readOption(reading, 5);

        switch (option) {
            case '1':
                puts("Recoger ítems aún no implementado.");
                break;

            case '2':
                puts("Descartar ítems aún no implementado.");
                break;

            case '3':
                move(&actual);
                if (actual->state.remainingTime <= 0) {
                    puts("\n¡Se acabó el tiempo! Has perdido.");
                    return;
                }
                if (actual->state.esFinal) {
                    puts("\n¡Has llegado al escenario final! ¡Felicidades!");
                    // Puedes imprimir aquí los ítems del inventario y puntaje
                    return;
                }
                break;

            case '4': // Reiniciar partida
                actual = &graph.nodes[0];
                actual->state.remainingTime = 10;
                list_clean(actual->state.playerInventory);
                puts("Partida reiniciada.");
                break;

            case '5': // Salir
                puts("Saliendo del juego...");
                return;

            default:
                puts("Opción inválida.");
        }

        presioneTeclaParaContinuar();
    }
}

void mostrar_escenarios(){
    limpiarPantalla();
    if (graph.numberOfNodes == 0) {
        puts("Primero debes importar escenarios desde un archivo csv! :(");
        return;
    }
    mostrar_grafo(graph);
}

int main(){
    bool Flag = true;
    char option;
    do {
        showPrincipalOptions();
        char reading[MAXOPTION];
        option = readOption(reading, 4);
        switch(option) {
            case '1':
                leer_escenarios();
                break;
            case '2':
                mostrar_escenarios();
                break;
            case '3':
                start_game();
                break;
            case '4':
                puts("OPCION SALIR");
                break;
            default:
                puts("\nOpción inválida, intente nuevamente");
                break;
        }
        presioneTeclaParaContinuar();
    } while (option != '4');

}