#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "list.h"
#include "extra.h"
#include "grafo.h" // Asegúrate de que grafo.h incluya la definición completa de struct Graph y struct Node
#include "game.h"

#define MAXOPTION 256
#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXNODES 1000

/*
### **Opciones del Jugador**

1. **Recoger Ítem(s)**
    - El jugador puede seleccionar uno o más ítems del escenario para agregarlos a su inventario. Se descuenta 1 de tiempo.

2. **Descartar Ítem(s)**
    - El jugador puede eliminar ítems de su inventario para reducir peso y moverse más rápido. Se descuenta 1 de tiempo.

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
    limpiarPantalla();
    puts("---------- GraphQuest ----------\n");
    puts("------ Opciones Disponibles ------");
    puts("(1)   Recoger Ítem(s)");
    puts("(2)   Descartar Ítem(s)");
    puts("(3)   Avanzar en una dirección");
    puts("(4)   Reiniciar partida");
    puts("(5)   Salir del juego");
}

char readOption(char reading[MAXOPTION], int maxOpciones) {
    while (1) {
        printf("Ingrese una opción (1-%d): ", maxOpciones);
        if (!fgets(reading, MAXOPTION, stdin)) {
            clearerr(stdin);
            continue;
        }

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

        puts("Opción inválida. Intente nuevamente.");
    }
}

void mostrar_estado_actual(GameState* gs) {
    Node* n = gs->currentNode;
    limpiarPantalla();
    printf("\nEscenario: %s\n", n->state.description);
    printf("Tiempo restante: %d\n", gs->tiempoRestante);

    printf("Inventario del jugador:\n");
    if (list_size(gs->inventory) == 0)
        puts("  (vacío)");
    else
        for (Item* it = list_first(gs->inventory); it; it = list_next(gs->inventory))
            printf(" - %s (valor %d, peso %d)\n", it->name, it->value, it->weight);

    printf("\nÍtems disponibles aquí:\n");
    if (list_size(n->state.availableItems) == 0)
        puts("  Ninguno");
    else
        for (Item* it = list_first(n->state.availableItems); it; it = list_next(n->state.availableItems))
            printf(" - %s (valor %d, peso %d)\n", it->name, it->value, it->weight);
}

// Moverse entre nodos del grafo
void moverse(GameState* gs) {
    Node* n = gs->currentNode;

    const char* dirNames[] = {"Norte", "Este", "Sur", "Oeste"};
    printf("Direcciones disponibles:\n");
    for (int i = 0; i < 4; i++) {
        if (n->adjacents[i]) {
            printf("(%d) %s -> %s\n", i + 1, dirNames[i], n->adjacents[i]->state.name);
        }
    }

    char buf[MAXOPTION];
    printf("Elige una dirección (1-4): ");
    if (!fgets(buf, MAXOPTION, stdin)) {
        puts("Error de lectura.");
        return;
    }

    int dir = atoi(buf) - 1; // Convertimos 1-4 a índice 0-3
    if (dir >= 0 && dir < 4 && n->adjacents[dir]) {
        int pesoTotal = 0;
        for (Item* it = list_first(gs->inventory); it; it = list_next(gs->inventory))
            pesoTotal += it->weight;

        int gasto = (pesoTotal + 1 + 9) / 10; // equivalente a ceil((peso+1)/10)
        gs->currentNode    = n->adjacents[dir];
        gs->tiempoRestante -= gasto;

        printf("\nTe moviste. Gastaste %d. Tiempo restante: %d\n",
               gasto, gs->tiempoRestante);

        if (gs->tiempoRestante <= 0) {
            puts("\n¡Te quedaste sin tiempo! Has perdido.");
            mostrar_puntaje_final(gs);
            return;
        }

        if (gs->currentNode->state.esFinal) {
            puts("\n¡Llegaste al final!");
            mostrar_puntaje_final(gs);
        }
    } else {
        puts("Dirección inválida.");
    }
}


void recoger_items(GameState* gs) {
    Node* n = gs->currentNode;
    if (list_size(n->state.availableItems) == 0) {
        puts("No hay ítems para recoger.");
        return;
    }

    int index = 0;
    printf("Ítems disponibles:\n");
    for (Item* it = list_first(n->state.availableItems); it; it = list_next(n->state.availableItems))
        printf("(%d) %s (valor: %d, peso: %d)\n", index++, it->name, it->value, it->weight);

    printf("Ingresa el índice del ítem a recoger: ");
    char input[MAXOPTION];
    fgets(input, MAXOPTION, stdin);
    int choice = atoi(input);

    list_first(n->state.availableItems);
    for (int i = 0; i < choice; i++)
        list_next(n->state.availableItems);

    Item* it = list_current(n->state.availableItems);
    if (it) {
        list_pushBack(gs->inventory, it);
        list_popCurrent(n->state.availableItems);
        printf("Recogiste: %s\n", it->name);
        gs->tiempoRestante -= 1;
    } else {
        puts("Índice inválido.");
    }
}

void descartar_items(GameState* gs) {
    if (list_size(gs->inventory) == 0) {
        puts("No tienes ítems para descartar.");
        return;
    }

    printf("Ítems en tu inventario:\n");
    int index = 1;
    for (Item* it = list_first(gs->inventory); it; it = list_next(gs->inventory))
        printf("(%d) %s (valor %d, peso %d)\n", index++, it->name, it->value, it->weight);

    printf("Ingrese el número del ítem a descartar (0 para cancelar): ");
    char buf[MAXOPTION];
    if (!fgets(buf, MAXOPTION, stdin)) {
        puts("Error de lectura.");
        return;
    }

    int seleccion = atoi(buf);
    if (seleccion == 0) return;

    int actual = 1;
    for (Item* it = list_first(gs->inventory); it; it = list_next(gs->inventory), actual++) {
        if (actual == seleccion) {
            printf("Descartaste: %s\n", it->name);
            list_popCurrent(gs->inventory); // Elimina el ítem actual
            gs->tiempoRestante -= 1;
            return;
        }
    }

    puts("Selección inválida.");
}

void mostrar_puntaje_final(GameState* gs) {
    int puntaje_total = 0;
    printf("\n--- Inventario final ---\n");
    for (Item* item = list_first(gs->inventory); item != NULL; item = list_next(gs->inventory)) {
        printf(" - %s (valor: %d)\n", item->name, item->value);
        puntaje_total += item->value;
    }
    printf("Puntaje total: %d\n", puntaje_total);
}

void mostrar_resultados_finales(GameStateMultiplayer* gs) {
    int totalPuntaje = 0;
    for (int i = 0; i < 2; i++) {
        Player* p = &gs->jugadores[i];
        printf("\nJugador %d:\n", i + 1);
        printf("Puntaje: %d\n", p->puntaje);
        // Mostrar ítems
        totalPuntaje += p->puntaje;
    }
    printf("\nPuntaje total colaborativo: %d\n", totalPuntaje);
}

void mostrar_estado_jugador_actual(GameStateMultiplayer* gs) {
    Player* actual = &gs->jugadores[gs->turnoActual];
    printf("\n== TURNO DEL JUGADOR %d ==\n", gs->turnoActual + 1);
    printf("Escenario actual: %s\n", actual->currentNode->state.name);
    printf("Tiempo restante: %d\n", actual->tiempoRestante);
    // Puedes extender esto con el inventario, ítems disponibles, etc.
}

void seleccionar_modo_y_comenzar_partida(Graph* grafo) {
    limpiarPantalla();
    puts("¿Cuántos jugadores jugarán?");
    puts("(1) Un jugador");
    puts("(2) Dos jugadores");

    char buf[MAXOPTION];
    fgets(buf, MAXOPTION, stdin);
    int eleccion = atoi(buf);

    if (eleccion == 1) {
        iniciar_partida();  // Tu función actual de 1 jugador
    }
    else if (eleccion == 2) {
        iniciar_partida_multijugador(grafo);
    }
    else {
        puts("Opción inválida.");
    }
}

void iniciar_partida_multijugador(Graph* grafo) {
    GameStateMultiplayer gs;
    gs.grafo = grafo;
    gs.turnoActual = 0;
    for (int i = 0; i < 2; i++) {
        gs.jugadores[i].currentNode = grafo->start;
        gs.jugadores[i].inventory = list_create();
        gs.jugadores[i].tiempoRestante = 10;
        gs.jugadores[i].puntaje = 0;
    }
    while (true) {
        Player* p = &gs.jugadores[gs.turnoActual];
        if (p->tiempoRestante <= 0 || p->currentNode->state.esFinal) {
            bool todosTerminaron = true;
            for (int i = 0; i < 2; i++) {
                if (gs.jugadores[i].tiempoRestante > 0 && !gs.jugadores[i].currentNode->state.esFinal) {
                    todosTerminaron = false;
                    break;
                }
            }
            if (todosTerminaron) break;
            else {
                gs.turnoActual = 1 - gs.turnoActual;
                continue;
            }
        }
        mostrar_estado_jugador_actual(&gs);
        showGameOptions();
        int acciones = 0;
        while (acciones < 2) {
            char buf[10];
            printf("Elige una acción: ");
            if (!fgets(buf, sizeof(buf), stdin)) {
                puts("Error de lectura.");
                continue;
            }
            char opcion = buf[0];

            GameState temp;
            temp.inventory = p->inventory;
            temp.tiempoRestante = p->tiempoRestante;
            temp.currentNode = p->currentNode;

            switch (opcion) {
                case '1': recoger_items(&temp); break;
                case '2': descartar_items(&temp); break;
                case '3': moverse(&temp); break;
                case '4': return; // salir
                default: puts("Opción inválida."); continue;
            }

            p->inventory = temp.inventory;
            p->tiempoRestante = temp.tiempoRestante;
            p->currentNode = temp.currentNode;

            acciones++;

            if (acciones < 2) {
                printf("¿Deseas hacer otra acción? (s/n): ");
                if (!fgets(buf, sizeof(buf), stdin)) break;
                if (tolower(buf[0]) != 's') break;
            }
        }

        gs.turnoActual = 1 - gs.turnoActual;
    }

    for (int i = 0; i < 2; i++) {
        Player* p = &gs.jugadores[i];
        p->puntaje = 0;
        for (Item* it = list_first(p->inventory); it; it = list_next(p->inventory))
            p->puntaje += it->value;
    }

    mostrar_resultados_finales(&gs);
}

int main(){
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
                mostrar_grafo();
                break;
            case '3':
                seleccionar_modo_y_comenzar_partida(&graph);
                break;
            case '4':
                puts("Saliendo del juego...");
                liberarJuego(&graph);
                break;
            default:
                puts("\nOpción inválida, intente nuevamente");
                break;
        }
        presioneTeclaParaContinuar();
    } while (option != '4');

}
