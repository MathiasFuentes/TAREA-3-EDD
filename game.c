#include "game.h"
#include "extra.h"
#include "grafo.h"
#include "list.h"

#include <ctype.h>
#define MAXOPTION 256

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
 * Función: iniciar_partida
 * ------------------------
 * Inicia y controla el ciclo principal de una partida para un solo jugador
 * en el grafo proporcionado.
 *
 * Parámetros:
 *  - grafo: Puntero al grafo del juego que contiene los nodos y el estado inicial.
 *
 * Funcionalidad:
 *  - Inicializa un estado de juego (GameState) con el nodo de inicio, inventario vacío
 *    y tiempo restante predeterminado.
 *  - Ejecuta un bucle principal mientras el jugador tenga tiempo y no haya llegado al nodo final.
 *  - En cada iteración:
 *      - Muestra el estado actual del jugador y opciones disponibles.
 *      - Lee y valida la opción del usuario.
 *      - Según la opción, permite recoger ítems, descartar ítems, moverse, reiniciar o salir.
 *      - Si la acción termina la partida (por ejemplo, movimiento a nodo final), se sale del bucle.
 *  - Soporta reinicio recursivo al llamar a sí misma cuando el jugador elige reiniciar.
 *  - Al finalizar, muestra un mensaje según se haya terminado por tiempo o por alcanzar el nodo final,
 *    y muestra el puntaje final.
 *  - Libera los recursos asociados al juego antes de terminar.
 *  - Espera la entrada del usuario antes de volver al menú principal.
 *
 * Detalles adicionales:
 *  - Usa funciones auxiliares como mostrar_estado_actual, showGameOptions, readOption,
 *    recoger_items, descartar_items, moverse y liberarJuego.
 */

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
                // Reiniciar sin liberar el grafo, para mantener ítems recogidos
                puts("\n--- Reiniciando la partida... ---");
                presioneTeclaParaContinuar();
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
 * Función: iniciar_partida_multijugador
 * -------------------------------------
 * Inicia y controla una partida en modo multijugador para dos jugadores.
 *
 * Parámetros:
 *  - grafo: Puntero a la estructura Graph que contiene el laberinto original.
 *
 * Funcionalidad:
 *  - Crea copias independientes del grafo para cada jugador para evitar interferencias.
 *  - Inicializa los estados de ambos jugadores con nodo inicial, inventario vacío y tiempo inicial.
 *  - Ejecuta turnos alternados donde cada jugador puede realizar múltiples acciones 
 *    (recoger ítems, descartar ítems, moverse, reiniciar o salir) hasta terminar el turno.
 *  - Sincroniza los ítems disponibles entre ambos grafos según las acciones de los jugadores.
 *  - Verifica condiciones de término: tiempo agotado o llegada al nodo final para ambos jugadores.
 *  - Al finalizar la partida, muestra puntajes individuales y libera los recursos asignados.
 *
 * Detalles adicionales:
 *  - Soporta reinicio recursivo de la partida.
 *  - Utiliza funciones auxiliares para mostrar estado, leer opciones, sincronizar ítems, etc.
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
                    // presioneTeclaParaContinuar();
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
                    puts("\n--- Reiniciando la partida multijugador... ---");
                    presioneTeclaParaContinuar();
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
 * Función: recoger_items
 * ----------------------
 * Permite al jugador recoger un ítem disponible en el nodo actual y añadirlo a su inventario.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState con el estado actual del jugador.
 *  - esMultijugador: Indica si la partida está en modo multijugador (true o false).
 *  - otroJugadorNode: Puntero al nodo actual del otro jugador (solo usado en multijugador para sincronizar ítems).
 *
 * Funcionalidad:
 *  - Muestra los ítems disponibles en el nodo actual.
 *  - Solicita al usuario que seleccione un ítem por su índice.
 *  - Valida la entrada del usuario y el índice seleccionado.
 *  - Verifica que el ítem seleccionado aún esté disponible (considerando posibles cambios en multijugador).
 *  - Copia el ítem al inventario del jugador y lo elimina del nodo actual.
 *  - En modo multijugador, elimina el ítem también del nodo del otro jugador para mantener sincronización.
 *  - Deduce 1 unidad de tiempo restante como penalización por recoger un ítem.
 *  - Muestra mensajes de error o confirmación según corresponda.
 *
 * Manejo de errores:
 *  - Detecta entrada inválida o índice fuera de rango.
 *  - Notifica si el ítem ya fue tomado por el otro jugador.
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
 *  - indicador: Entero que controla si se limpia la pantalla y se muestra título (1 para mostrar, otro valor para no mostrar).
 *
 * Funcionalidad:
 *  - Si indicador es 1, limpia pantalla y muestra encabezado de la sección.
 *  - Muestra la lista de ítems actuales en el inventario con un índice numérico.
 *  - Solicita al usuario el número correspondiente al ítem a descartar.
 *  - Si el usuario ingresa 0, cancela la acción sin cambios.
 *  - Valida la selección y, si es válida, elimina el ítem del inventario y descuenta 1 unidad del tiempo restante.
 *  - En caso de no tener ítems o de selección inválida, muestra mensajes informativos.
 *  - Usa `presioneTeclaParaContinuar()` para esperar cuando no hay ítems.
 *
 * Manejo de errores:
 *  - Controla entrada inválida y selección fuera de rango.
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
 * Función: mostrar_estado_actual
 * ------------------------------
 * Muestra en pantalla el estado actual del jugador durante la partida.
 *
 * Parámetros:
 *  - gs: Puntero a la estructura GameState que contiene la información del juego actual.
 *  - indicador: Entero que indica si se debe limpiar la pantalla y mostrar encabezado (1 para sí, otro valor para no).
 *
 * Funcionalidad:
 *  - Si indicador es 1, limpia la pantalla y muestra un encabezado informativo.
 *  - Muestra la descripción y nombre del escenario actual (nodo en el grafo).
 *  - Muestra el tiempo restante disponible para el jugador.
 *  - Lista los ítems que el jugador tiene en su inventario, o informa si está vacío.
 *  - Lista los ítems disponibles para recoger en el nodo actual, o informa si no hay ninguno.
 *
 * Propósito:
 *  - Permitir al jugador evaluar su situación actual y recursos antes de decidir su siguiente acción.
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
 * Permite al jugador moverse desde el nodo actual hacia uno de sus nodos adyacentes.
 *
 * Parámetros:
 *  - gs: Puntero al estado actual del juego (GameState), que contiene la posición del jugador,
 *        el inventario y el tiempo restante.
 *
 * Funcionalidad:
 *  - Muestra las direcciones disponibles (Norte, Este, Sur, Oeste) hacia nodos adyacentes.
 *  - Solicita al jugador elegir una dirección válida para moverse.
 *  - Calcula el costo de tiempo para moverse, que depende del peso total de los ítems en el inventario.
 *  - Actualiza la posición actual del jugador y decrementa el tiempo restante según el gasto calculado.
 *  - Informa al jugador sobre el movimiento y el tiempo gastado.
 *  - Verifica si el jugador se quedó sin tiempo o llegó al nodo final:
 *      - En ambos casos, muestra el puntaje final y retorna `true` indicando que la partida terminó.
 *  - Si la dirección elegida es inválida, muestra un mensaje de error y retorna `false`.
 *
 * Retorna:
 *  - `true` si la partida terminó (por tiempo o porque se llegó al final).
 *  - `false` si la partida continúa.
 *
 * Detalles adicionales:
 *  - Utiliza funciones auxiliares como limpiarPantalla, mostrar_puntaje_final, list_first y list_next.
 *  - El gasto de tiempo se calcula como (pesoTotalInventario + 10) / 10, asegurando un gasto mínimo.
 */

bool moverse(GameState* gs) {
    Node* n = gs->currentNode;
    limpiarPantalla();
    puts("======- Moverse en una dirección -======");
    const char* dirNames[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};

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
 * Función: sincronizar_items
 * ---------------------------
 * Sincroniza los ítems disponibles entre dos grafos de juego utilizados por dos jugadores
 * diferentes en una partida multijugador. Garantiza que un ítem recogido por un jugador
 * ya no esté disponible en el grafo del otro jugador.
 *
 * Parámetros:
 *  - grafo1: Puntero al grafo del jugador 1.
 *  - grafo2: Puntero al grafo del jugador 2.
 *  - jugador1: Estado del jugador 1 (incluye su inventario).
 *  - jugador2: Estado del jugador 2 (incluye su inventario).
 *
 * Funcionalidad:
 *  - Recorre todos los nodos del grafo2.
 *  - En cada nodo, elimina los ítems que ya están en el inventario del jugador1.
 *  - Luego, recorre todos los nodos del grafo1.
 *  - En cada nodo, elimina los ítems que ya están en el inventario del jugador2.
 *
 * Detalles adicionales:
 *  - Utiliza `list_first`, `list_next`, `list_popCurrent` y `list_current` para recorrer
 *    y modificar las listas de ítems disponibles en cada nodo.
 *  - La comparación de ítems se realiza por nombre (`strcmp`).
 *  - Se evita modificar la lista mientras se recorre con índices, usando punteros
 *    para mantener correctamente la posición tras eliminar un elemento.
 */

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