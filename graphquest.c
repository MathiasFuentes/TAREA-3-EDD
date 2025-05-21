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

/*
    Función readOption() HAY QUE COMENTAR AQUÍ
*/

char readOption(char reading[MAXOPTION], int maxOpciones) {
    while (1) {
        printf("Ingrese una opción (1-%d): ", maxOpciones);
        fgets(reading, MAXOPTION, stdin);

        // Eliminar salto de línea si está presente
        size_t len = strlen(reading);
        if (len > 0 && reading[len - 1] == '\n') {
            reading[len - 1] = '\0';
            len--;
        }

        // Validar que la entrada tenga exactamente un carácter numérico
        if (len == 1 && isdigit(reading[0])) {
            int opcionNumerica = reading[0] - '0';
            if (opcionNumerica >= 1 && opcionNumerica <= maxOpciones) {
                return reading[0]; // Retorna el carácter ('1', '2', etc.)
            }
        }

        // printf("Opción inválida. Intente nuevamente.\n");
        return '0';
    }
}

int main(){
    Graph* maze;
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
                mostrar_grafo();
                break;
            case '3':
                puts("OPCION INICIAR PARTIDA ");
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