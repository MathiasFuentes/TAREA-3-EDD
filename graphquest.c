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
