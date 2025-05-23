#include "game.h"
#include "extra.h"
#include "grafo.h"
#include "list.h"

#define MAXOPTION 256

extern Graph graph;

void iniciar_partida(void) {
    if (graph.numberOfNodes == 0) {
        puts("Primero debes cargar el laberinto.");
        return;
    }

    GameState gs;
    gs.currentNode    = &graph.nodes[0];
    gs.tiempoRestante = 10;
    gs.inventory      = list_create();

    char option, buf[MAXOPTION];

    while (1) {
        mostrar_estado_actual(&gs);
        showGameOptions();
        option = readOption(buf, 5);

        if (gs.tiempoRestante <= 0) {
            puts("\n¡Se acabó el tiempo! Has perdido.");
            mostrar_puntaje_final(&gs);
            break;
        }

        switch (option) {
            case '1': recoger_items(&gs);      break;
            case '2': descartar_items(&gs);    break;
            case '3': moverse(&gs);            break;
            case '4':  // Reiniciar
                list_destroy(gs.inventory);
                gs.inventory = list_create();
                gs.currentNode = &graph.nodes[0];
                gs.tiempoRestante = 10;
                puts("Partida reiniciada.");
                break;
            case '5':  // Salir
                puts("Saliendo del juego...");
                list_destroy(gs.inventory);
                liberarJuego(&graph);
                exit(0);
            default:
                puts("Opción inválida.");
        }

        presioneTeclaParaContinuar();
    }

    // Al salir del bucle (derrota o victoria):
    list_destroy(gs.inventory);
    liberarJuego(&graph);
}
