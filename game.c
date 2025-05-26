#include "game.h"
#include "extra.h"
#include "grafo.h"
#include "list.h"

#define MAXOPTION 256

void iniciar_partida(Graph* grafo) {
    GameState gs;
    gs.currentNode = grafo->start;
    gs.inventory = list_create();
    gs.tiempoRestante = 10; // Tiempo inicial

    while (gs.tiempoRestante > 0 && !gs.currentNode->state.esFinal) {
        mostrar_estado_actual(&gs, 1);
        showGameOptions();

        char buf[MAXOPTION];
        char opcion = readOption(buf, 5);

        bool partidaTerminada = false;
        
        switch (opcion) {
            case '1':
                recoger_items(&gs, false, NULL);
                break;
            case '2': descartar_items(&gs, 1); break;
            case '3': partidaTerminada = moverse(&gs); break;
            case '4': 
                // Reiniciar
                liberarJuego(grafo);
                iniciar_partida(grafo);
                return;
            case '5': 
                // Salir
                liberarJuego(grafo);
                return;
        }

        if (partidaTerminada) {
            printf("\nPresione cualquier tecla para volver al menú principal...");
            getchar(); // Esperar entrada del usuario
            liberarJuego(grafo);
            return;
        }
    }

    // Si salió del bucle por tiempo o final
    if (gs.tiempoRestante <= 0) {
        puts("\n¡Te quedaste sin tiempo!");
        mostrar_puntaje_final(&gs);
    } else if (gs.currentNode->state.esFinal) {
        puts("\n¡Llegaste al final!");
        mostrar_puntaje_final(&gs);
    }

    printf("\nPresione cualquier tecla para volver al menú principal...");
    getchar();
    liberarJuego(grafo);
}

/*
 * Función: recoger_items
 * ----------------------
 * Permite al jugador recoger un ítem del escenario actual y añadirlo a su inventario.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState con el estado actual del jugador.
 *
 * Funcionalidad:
 *  - Muestra los ítems disponibles en el nodo actual.
 *  - Solicita al usuario que seleccione un ítem por su índice.
 *  - Transfiere el ítem al inventario del jugador y lo elimina del escenario.
 *  - Resta 1 unidad al tiempo restante como penalización por recoger.
 *
 * Se maneja la entrada inválida en caso de índice fuera de rango.
 */

void recoger_items(GameState* gs, bool esMultijugador, Node* otroJugadorNode) {
    limpiarPantalla();
    puts("======- Recoger Ítem -======");
    Node* n = gs->currentNode;
    
    if (list_size(n->state.availableItems) == 0) {
        puts("No hay ítems para recoger.");
        presioneTeclaParaContinuar();
        return;
    }

    // Mostrar ítems disponibles
    printf("\nÍtems disponibles:\n");
    int itemCount = 0;
    Item* items[MAXNODES];
    
    for (Item* it = list_first(n->state.availableItems); it != NULL; it = list_next(n->state.availableItems)) {
        printf("(%d) %s (valor: %d, peso: %d)\n", itemCount, it->name, it->value, it->weight);
        items[itemCount] = it;
        itemCount++;
    }

    printf("\nIngresa el índice del ítem a recoger (0-%d): ", itemCount-1);
    char input[MAXOPTION];
    
    if (!fgets(input, MAXOPTION, stdin)) {
        puts("Error al leer la entrada.");
        presioneTeclaParaContinuar();
        return;
    }

    char* endptr;
    long choice = strtol(input, &endptr, 10);
    
    if (endptr == input || *endptr != '\n') {
        puts("Entrada inválida. Debe ser un número.");
        presioneTeclaParaContinuar();
        return;
    }

    if (choice < 0 || choice >= itemCount) {
        printf("Índice inválido. Debe estar entre 0 y %d.\n", itemCount-1);
        presioneTeclaParaContinuar();
        return;
    }

    Item* it = items[choice];
    
    // Verificar si el ítem todavía existe (para multijugador)
    bool itemExistente = false;
    for (Item* current = list_first(n->state.availableItems); current != NULL; current = list_next(n->state.availableItems)) {
        if (strcmp(current->name, it->name) == 0) {
            itemExistente = true;
            break;
        }
    }

    if (!itemExistente) {
        puts("Este ítem ya fue tomado por el otro jugador.");
        presioneTeclaParaContinuar();
        return;
    }

    Item* copia = malloc(sizeof(Item));
    *copia = *it;

    list_pushBack(gs->inventory, copia);

    // Eliminar el ítem de la lista actual
    for (Item* current = list_first(n->state.availableItems); current != NULL; current = list_next(n->state.availableItems)) {
        if (strcmp(current->name, it->name) == 0) {
            list_popCurrent(n->state.availableItems);
            break;
        }
    }

    // Si es multijugador, sincronizar con el otro jugador
    if (esMultijugador && otroJugadorNode != NULL) {
        for (Item* current = list_first(otroJugadorNode->state.availableItems); current != NULL; current = list_next(otroJugadorNode->state.availableItems)) {
            if (strcmp(current->name, it->name) == 0) {
                list_popCurrent(otroJugadorNode->state.availableItems);
                break;
            }
        }
    }

    printf("\nRecogiste: %s\n", copia->name);
    gs->tiempoRestante -= 1;
    presioneTeclaParaContinuar();
}



/*
 * Función: descartar_items
 * ------------------------
 * Permite al jugador eliminar un ítem de su inventario.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState del jugador.
 *
 * Funcionalidad:
 *  - Muestra los ítems actuales del inventario.
 *  - Solicita al usuario el número del ítem a descartar.
 *  - Elimina el ítem seleccionado y descuenta 1 unidad de tiempo restante.
 *
 * Si el jugador no tiene ítems o elige una opción inválida, se notifica.
 */

void descartar_items(GameState* gs, int indicador) {
    if (indicador == 1){
        limpiarPantalla();
        puts("======- Descartar Ítem -======");
    }
    if (list_size(gs->inventory) == 0) {
        puts("No tienes ítems para descartar.");
        presioneTeclaParaContinuar();  // AÑADE ESTA LÍNEA
        return;
    }
    else{
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
}
