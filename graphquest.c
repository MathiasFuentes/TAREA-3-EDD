#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "list.h"
#include "extra.h"

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4

/*
    Estado Actual

    - Descripción del escenario actual.
    - Lista de ítems disponibles en este escenario (con nombre, peso y valor).
    - Tiempo restante (puedes inicializarlo en 10 o el tiempo que estimes conveniente).
    - Inventario del jugador (ítems recogidos, peso total y puntaje acumulado).
    - Acciones posibles desde este escenario: direcciones disponibles (arriba, abajo, izquierda, derecha).
*/

typedef struct{
    char    name[MAXITEMNAME];
    int     weight;
    int     value;
} Item;

typedef struct State{
    char    description[MAXDESC];
    Item*   availableItems;
    List*   playerInventory;
    time_t  tiempoRestante;
    char    possibleDirections[MAXDIR];
} State;

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

void showOptions(){
    puts("---------- GraphQuest ----------\n");
    puts("------ Opciones Disponibles ------");
    puts("(1)   Recoger Ítem(s)");
    puts("(2)   Descartar Ítem(s)");
    puts("(3)   Avanzar en una dirección");
    puts("(4)   Reiniciar partida");
    puts("(5)   Modo Multijugador");
    puts("(6)   Salir del juego");
}