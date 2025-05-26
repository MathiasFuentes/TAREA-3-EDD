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

void mostrar_estado_actual(GameState* gs, int indicador) {
    Node* n = gs->currentNode;
    if (indicador == 1){
        limpiarPantalla();
        puts("=======- Partida en Curso -=======");
    }
    printf("Escenario: %s\n", n->state.name);
    printf("\nDescripción %s\n", n->state.description);
    printf("Tiempo restante: %d\n", gs->tiempoRestante);

    if (list_size(gs->inventory) == 0) puts("\nTu inventario está vacío.");
    else    {
        puts("\nInventario:");
        for (Item* it = list_first(gs->inventory); it; it = list_next(gs->inventory)){
            printf(" - %s (valor %d, peso %d)\n", it->name, it->value, it->weight);
        }
        // printf("\n");
    }

    if (list_size(n->state.availableItems) == 0) puts("\nNo hay ítems disponibles aquí.\n");
    else    {
        printf("\nÍtems disponibles aquí:\n");
        for (Item* it = list_first(n->state.availableItems); it; it = list_next(n->state.availableItems)){
            printf(" - %s (valor %d, peso %d)\n", it->name, it->value, it->weight);
        }
        printf("\n");
        
    }
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

// Devuelve true si la partida terminó, false si continúa
bool moverse(GameState* gs) {
    Node* n = gs->currentNode;
    limpiarPantalla();
    puts("======- Moverse en una dirección -======");
    const char* dirNames[] = {"Norte", "Este", "Sur", "Oeste"};

    printf("\nDirecciones disponibles:\n");
    for (int i = 0; i < 4; i++) {
        if (n->adjacents[i]) {
            printf("(%d) %s -> %s\n", i + 1, dirNames[i], n->adjacents[i]->state.name);
        }
    }

    char buf[MAXOPTION];
    printf("\nElige una dirección (1-4): ");
    if (!fgets(buf, MAXOPTION, stdin)) {
        puts("Error de lectura.");
        return false;
    }

    int dir = atoi(buf) - 1;
    if (dir >= 0 && dir < 4 && n->adjacents[dir]) {
        int pesoTotal = 0;
        for (Item* it = list_first(gs->inventory); it; it = list_next(gs->inventory))
            pesoTotal += it->weight;

        int gasto = (pesoTotal + 1 + 9) / 10;
        gs->currentNode = n->adjacents[dir];
        gs->tiempoRestante -= gasto;

        printf("\nTe moviste. Gastaste %d de Tiempo. Tiempo restante: %d\n",
               gasto, gs->tiempoRestante);

        if (gs->tiempoRestante <= 0) {
            puts("\n¡Te quedaste sin tiempo! Has perdido.");
            mostrar_puntaje_final(gs);
            return true; // Partida terminada
        }

        if (gs->currentNode->state.esFinal) {
            puts("\n¡Llegaste al final!");
            mostrar_puntaje_final(gs);
            return true; // Partida terminada
        }
    } else {
        puts("Dirección inválida.");
    }
    return false;
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

/**
 * seleccionar_modo_y_comenzar_partida
 * -----------------------------------
 * Permite al usuario elegir entre modo de un jugador o multijugador.
 * En el modo de un jugador, se realiza una copia profunda del grafo original
 * (`graph`) a una copia temporal (`graphCpy`) que se usará en la partida.
 *
 * Esto permite reiniciar la partida con el estado original del laberinto intacto.
 *
 * Parámetros:
 *  - grafo: puntero al grafo original previamente cargado con escenarios.
 *
 * Consideraciones:
 *  - Si no se han cargado escenarios, no se podrá iniciar la partida.
 *  - Cada vez que se inicia o reinicia una partida, se vuelve a copiar el grafo.
 */
void seleccionar_modo_y_comenzar_partida(Graph** grafo) {
    limpiarPantalla();
    puts("======- Iniciar Partida -======");
    puts("Selecciona el número de jugadores:");
    puts("(1)   Un jugador");
    puts("(2)   Dos jugadores");

    char buf[MAXOPTION];
    char eleccion = readOption(buf, 2);

    if (graph.numberOfNodes == 0) {
        puts("Primero debes cargar el laberinto.");
        return;
    }

    if (eleccion == '1') {
        if (*grafo) {
            liberarJuego(*grafo);
            free(*grafo);
            *grafo = NULL;
        }

        *grafo = copiar_grafo(&graph);
        if (!*grafo) {
            puts("Error al crear la copia del grafo.");
            return;
        }
        iniciar_partida(*grafo);
    }
    else if (eleccion == '2') {
        iniciar_partida_multijugador(&graph);
    }
}


void sincronizar_items(Graph* grafo1, Graph* grafo2, GameState* jugador1, GameState* jugador2){
    // Eliminar del grafo2 los ítems que el jugador1 ya recogió
    for (int i = 0; i < grafo2->numberOfNodes; i++) {
        List* itemsNodo = grafo2->nodes[i].state.availableItems;
        if (!itemsNodo) continue;

        Item* itemNodo = (Item*) list_first(itemsNodo);
        while (itemNodo) {
            bool encontrado = false;

            // Buscar si este ítem está en el inventario del jugador1
            Item* itemInv = (Item*) list_first(jugador1->inventory);
            while (itemInv) {
                if (strcmp(itemNodo->name, itemInv->name) == 0) {
                    encontrado = true;
                    break;
                }
                itemInv = (Item*) list_next(jugador1->inventory);
            }

            if (encontrado) {
                list_popCurrent(itemsNodo);
                itemNodo = (Item*) list_current(itemsNodo);  // Ya está en la posición correcta
            } else {
                itemNodo = (Item*) list_next(itemsNodo);
            }
        }
    }

    // Eliminar del grafo1 los ítems que el jugador2 ya recogió
    for (int i = 0; i < grafo1->numberOfNodes; i++) {
        List* itemsNodo = grafo1->nodes[i].state.availableItems;
        if (!itemsNodo) continue;

        Item* itemNodo = (Item*) list_first(itemsNodo);
        while (itemNodo) {
            bool encontrado = false;

            // Buscar si este ítem está en el inventario del jugador2
            Item* itemInv = (Item*) list_first(jugador2->inventory);
            while (itemInv) {
                if (strcmp(itemNodo->name, itemInv->name) == 0) {
                    encontrado = true;
                    break;
                }
                itemInv = (Item*) list_next(jugador2->inventory);
            }

            if (encontrado) {
                list_popCurrent(itemsNodo);
                itemNodo = (Item*) list_current(itemsNodo);
            } else {
                itemNodo = (Item*) list_next(itemsNodo);
            }
        }
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
    // Crear una copia del grafo para cada jugador
    Graph* grafoJugador1 = copiar_grafo(grafo);
    Graph* grafoJugador2 = copiar_grafo(grafo);
    
    if (!grafoJugador1 || !grafoJugador2) {
        puts("Error al crear copias del grafo para multijugador");
        if (grafoJugador1) liberarJuego(grafoJugador1);
        if (grafoJugador2) liberarJuego(grafoJugador2);
        return;
    }

    GameState gs1, gs2;
    gs1.currentNode = grafoJugador1->start;
    gs1.inventory = list_create();
    gs1.tiempoRestante = 10;
    
    gs2.currentNode = grafoJugador2->start;
    gs2.inventory = list_create();
    gs2.tiempoRestante = 10;

    int jugadorActual = 0; // 0 para jugador 1, 1 para jugador 2
    bool partidaTerminada = false;

    while (!partidaTerminada) {
        GameState* gsActual = (jugadorActual == 0) ? &gs1 : &gs2;
        bool turnoTerminado = false;

        // Mostrar pantalla de turno
        limpiarPantalla();
        printf("====== TURNO DEL JUGADOR %d ======\n", jugadorActual + 1);
        mostrar_estado_actual(gsActual, 0); // 0 para multijugador
        
        while (!turnoTerminado && !partidaTerminada) {
            showGameOptions();
            char buf[MAXOPTION];
            char opcion = readOption(buf, 5);

            switch (opcion) {
                case '1': 
                    recoger_items(gsActual, true, (jugadorActual == 0) ? gs2.currentNode : gs1.currentNode);
                    sincronizar_items(grafoJugador1, grafoJugador2, &gs1, &gs2);
                    break;
                case '2': {
                    // Descartar ítem
                    limpiarPantalla();
                    printf("====== TURNO DEL JUGADOR %d ======\n", jugadorActual + 1);
                    descartar_items(gsActual, 0); // 0 para multijugador
                    presioneTeclaParaContinuar();
                    break;
                }
                case '3': {
                    // Moverse
                    bool movimientoTerminoPartida = moverse(gsActual);
                    if (movimientoTerminoPartida) {
                        partidaTerminada = true;
                    }
                    turnoTerminado = true;
                    break;
                }
                case '4': 
                    // Reiniciar partida
                    liberarJuego(grafoJugador1);
                    liberarJuego(grafoJugador2);
                    iniciar_partida_multijugador(grafo);
                    return;
                case '5': 
                    // Salir
                    partidaTerminada = true;
                    turnoTerminado = true;
                    break;
            }

            // Verificar si terminó el turno
            if (gsActual->tiempoRestante <= 0 || gsActual->currentNode->state.esFinal) {
                turnoTerminado = true;
                partidaTerminada = (gs1.tiempoRestante <= 0 || gs1.currentNode->state.esFinal) &&
                                  (gs2.tiempoRestante <= 0 || gs2.currentNode->state.esFinal);
            }

            // Mostrar estado actualizado
            if (!turnoTerminado && !partidaTerminada) {
                limpiarPantalla();
                printf("====== TURNO DEL JUGADOR %d ======\n", jugadorActual + 1);
                mostrar_estado_actual(gsActual, 0);
            }
        }

        // Cambiar jugador
        if (!partidaTerminada) {
            jugadorActual = 1 - jugadorActual;
        }
    }

    // Resultados finales
    limpiarPantalla();
    puts("====== RESULTADOS FINALES ======\n");
    
    puts("JUGADOR 1:");
    mostrar_puntaje_final(&gs1);
    printf("\n");
    
    puts("JUGADOR 2:");
    mostrar_puntaje_final(&gs2);
    
    // Liberar recursos
    liberarJuego(grafoJugador1);
    liberarJuego(grafoJugador2);
    free(grafoJugador1);
    free(grafoJugador2);
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
    graph.nodes = NULL;
    graph.numberOfNodes = 0;
    graph.capacidad = 0;
    graph.start = NULL;

    Graph* graphCpy = NULL;

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
                seleccionar_modo_y_comenzar_partida(&graphCpy);
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
