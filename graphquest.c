#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "list.h"
#include "extra.h"
#include "grafo.h"
#include "game.h"

#define MAXOPTION 256
#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXNODES 1000

/*
 * Función: showPrincipalOptions
 * -----------------------------
 * Muestra el menú principal del juego "GraphQuest" en la consola.
 * 
 * Objetivo:
 *  - Limpiar la pantalla y desplegar las opciones iniciales que el usuario puede seleccionar
 *    para comenzar a interactuar con el programa.
 * 
 * Esta función no recibe parámetros ni retorna valores. Su única finalidad es la presentación
 * visual del menú principal mediante impresión por consola.
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

/*
 * Función: showGameOptions
 * ------------------------
 * Muestra el menú de opciones disponibles durante una partida activa del juego "GraphQuest".
 * 
 * Objetivo:
 *  - Limpiar la pantalla y presentar al jugador las acciones que puede realizar en su turno
 *    dentro del juego.
 * 
 * Esta función no recibe parámetros ni retorna valores. Está diseñada para guiar al jugador
 * mostrando el conjunto de comandos que puede ejecutar.
 */

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

/*
 * Función: readOption
 * -------------------
 * Solicita al usuario una opción numérica dentro de un rango válido, validando la entrada 
 * para asegurar que sea un solo dígito numérico entre 1 y maxOpciones.
 * 
 * Parámetros:
 *  - reading: Arreglo de caracteres donde se almacena la entrada del usuario.
 *  - maxOpciones: Valor entero que indica el número máximo de opciones válidas (inclusive).
 * 
 * Retorno:
 *  - Retorna el carácter correspondiente a la opción elegida por el usuario, si es válida.
 * 
 * Funcionamiento:
 *  - Muestra un mensaje solicitando una opción dentro del rango [1-maxOpciones].
 *  - Usa fgets para leer la entrada como cadena, verificando que no haya errores.
 *  - Elimina el salto de línea final si está presente.
 *  - Verifica que la entrada sea de un solo carácter y que sea un dígito.
 *  - Convierte el carácter a un número entero y verifica si está dentro del rango permitido.
 *  - Si la entrada es válida, retorna el carácter.
 *  - Si no es válida, muestra un mensaje de error y repite el ciclo.
 */

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

/*
 * Función: mostrar_estado_actual
 * ------------------------------
 * Muestra en pantalla el estado actual del jugador durante la partida.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState que contiene la información del juego actual.
 *
 * Funcionalidad:
 *  - Limpia la pantalla.
 *  - Muestra la descripción del escenario actual y el tiempo restante.
 *  - Imprime los ítems en el inventario del jugador, si los hay.
 *  - Lista los ítems disponibles en el nodo actual, si existen.
 *
 * Esta función permite al jugador evaluar su situación actual antes de tomar decisiones.
 */

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

/*
 * Función: moverse
 * ----------------
 * Permite al jugador desplazarse a un nodo adyacente en el grafo del juego.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState con el estado actual del jugador.
 *
 * Funcionalidad:
 *  - Muestra las direcciones disponibles desde el nodo actual.
 *  - Lee la opción del usuario y valida la dirección elegida.
 *  - Calcula el costo de movimiento en base al peso del inventario.
 *  - Actualiza el nodo actual y el tiempo restante del jugador.
 *  - Verifica si el jugador se quedó sin tiempo o llegó al nodo final.
 *
 * Si el jugador llega al nodo final o agota su tiempo, se muestra el puntaje final.
 */

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

/*
 * Función: mostrar_puntaje_final
 * ------------------------------
 * Muestra los ítems finales del inventario del jugador y calcula su puntaje total.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState del jugador.
 *
 * Funcionalidad:
 *  - Recorre el inventario sumando el valor de cada ítem.
 *  - Imprime los ítems y el puntaje total acumulado.
 */

void mostrar_puntaje_final(GameState* gs) {
    int puntaje_total = 0;
    printf("\n--- Inventario final ---\n");
    for (Item* item = list_first(gs->inventory); item != NULL; item = list_next(gs->inventory)) {
        printf(" - %s (valor: %d)\n", item->name, item->value);
        puntaje_total += item->value;
    }
    printf("Puntaje total: %d\n", puntaje_total);
}

/*
 * Función: mostrar_resultados_finales
 * -----------------------------------
 * Muestra los puntajes finales de ambos jugadores en modo multijugador.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameStateMultiplayer con los estados de ambos jugadores.
 *
 * Funcionalidad:
 *  - Imprime el puntaje de cada jugador.
 *  - Calcula y muestra el puntaje total colaborativo.
 */

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

/*
 * Función: mostrar_estado_jugador_actual
 * --------------------------------------
 * Muestra información básica del turno actual en modo multijugador.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameStateMultiplayer.
 *
 * Funcionalidad:
 *  - Imprime el turno actual, escenario y tiempo restante del jugador activo.
 */

void mostrar_estado_jugador_actual(GameStateMultiplayer* gs) {
    Player* actual = &gs->jugadores[gs->turnoActual];
    printf("\n== TURNO DEL JUGADOR %d ==\n", gs->turnoActual + 1);
    printf("Escenario actual: %s\n", actual->currentNode->state.name);
    printf("Tiempo restante: %d\n", actual->tiempoRestante);
    // Puedes extender esto con el inventario, ítems disponibles, etc.
}

/*
 * Función: seleccionar_modo_y_comenzar_partida
 * --------------------------------------------
 * Solicita al usuario que seleccione el modo de juego: un jugador o dos jugadores.
 *
 * Parámetros:
 *  - grafo: Puntero a la estructura Graph que representa el laberinto del juego.
 *
 * Funcionalidad:
 *  - Muestra el menú de selección de modo de juego.
 *  - Llama a la función de inicio correspondiente según la elección del usuario.
 *  - Si la opción ingresada es inválida, se muestra un mensaje de error.
 */

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

/*
 * Función: iniciar_partida_multijugador
 * -------------------------------------
 * Inicia una partida para dos jugadores en modo multijugador.
 *
 * Parámetros:
 *  - grafo: Puntero a la estructura Graph que contiene el laberinto.
 *
 * Funcionalidad:
 *  - Inicializa los estados de los dos jugadores.
 *  - Ejecuta turnos alternados donde cada jugador puede realizar hasta dos acciones por turno.
 *  - Las acciones posibles son: recoger ítems, descartar ítems, moverse o salir.
 *  - Verifica condiciones de término (sin tiempo o llegada al nodo final).
 *  - Al final, muestra los puntajes individuales y el total colaborativo.
 */

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

/*
 * Función: main
 * -------------
 * Función principal del programa. Controla el ciclo general del juego y el menú principal.
 *
 * Funcionalidad:
 *  - Muestra un menú con las opciones principales del juego.
 *  - Lee la opción seleccionada por el usuario.
 *  - Ejecuta una acción según la opción:
 *      - '1': Carga los escenarios desde archivo (función leer_escenarios).
 *      - '2': Muestra el grafo del juego (función mostrar_grafo).
 *      - '3': Inicia la partida en modo uno o dos jugadores (función seleccionar_modo_y_comenzar_partida).
 *      - '4': Libera la memoria y termina el juego.
 *      - Otra: Informa que la opción es inválida.
 *  - El ciclo continúa hasta que el usuario elija salir ('4').
 *
 * Detalles adicionales:
 *  - Utiliza `showPrincipalOptions` para mostrar el menú.
 *  - Usa `readOption` para capturar la opción con validación de entrada.
 *  - Llama a `presioneTeclaParaContinuar` al final de cada iteración para dar tiempo al usuario.
 */

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
