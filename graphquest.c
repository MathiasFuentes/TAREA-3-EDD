#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "list.h"
#include "extra.h"

#define MAXDESC 1000
#define MAXITEMNAME 256
#define MAXDIR 4
#define MAXOPTION 256

/*
    Estado Actual

    - Descripción del escenario actual.
    - Lista de ítems disponibles en este escenario (con nombre, peso y valor).
    - Tiempo restante (Inicializado en 10).
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
    char    possibleDirections[MAXDIR];
    Item*   availableItems;
    List*   playerInventory;
    int     tiempoRestante;
    bool    esFinal;
} State;

typedef struct{
    State   state;
    List*   adjacents[MAXDIR];
} Node;

typedef struct{
    size_t numberOfNodes;
    Node* nodes;
} Graph;

/*
    Función leer_escenarios() HAY QUE COMENTAR AQUI
*/

// Variable global o pasada por referencia para guardar IDs de conexiones temporalmente
int** conexionesTemp; // conexionesTemp[i][0..3] = IDs vecinos de nodo i

Graph* leer_escenarios() {
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    // Leer y descartar encabezado
    char buffer[1024];
    fgets(buffer, sizeof(buffer), archivo);

    // Primero contar líneas para saber cuántos nodos hay
    size_t lineCount = 0;
    long pos = ftell(archivo);
    while (fgets(buffer, sizeof(buffer), archivo) != NULL) {
        lineCount++;
    }
    fseek(archivo, pos, SEEK_SET);

    // Crear grafo y nodos
    Graph* grafo = malloc(sizeof(Graph));
    grafo->numberOfNodes = lineCount;
    grafo->nodes = calloc(lineCount, sizeof(Node));

    // Reservar espacio para IDs de conexiones temporales
    conexionesTemp = malloc(lineCount * sizeof(int*));
    for (size_t i = 0; i < lineCount; i++) {
        conexionesTemp[i] = malloc(MAXDIR * sizeof(int));
        for (int d = 0; d < MAXDIR; d++) {
            conexionesTemp[i][d] = -1; // inicializar como no conexión
        }
    }

    size_t nodoIndex = 0;
    while ((fgets(buffer, sizeof(buffer), archivo)) != NULL) {
        // Parsear CSV línea (usa tu función split o sscanf según prefieras)
        // Aquí una forma simple con sscanf solo para ejemplo:
        // Recuerda que Items y Descripción con comas y espacios necesitan un parser robusto

        char idStr[10], nombre[MAXITEMNAME], descripcion[MAXDESC], itemsStr[256], arribaStr[10], abajoStr[10], izqStr[10], derStr[10], esFinalStr[10];
        
        // Ejemplo usando sscanf simplificado (ajustar según CSV real)
        sscanf(buffer, "%[^,],%[^,],\"%[^\"]\",%[^,],%[^,],%[^,],%[^,],%[^,],%s", 
            idStr, nombre, descripcion, itemsStr, arribaStr, abajoStr, izqStr, derStr, esFinalStr);

        int id = atoi(idStr);

        // Guardar datos básicos en el nodo
        Node* nodo = &grafo->nodes[nodoIndex];

        strcpy(nodo->state.description, descripcion);
        // Podrías guardar nombre en state si quieres, o en otro lado

        // Parsear Items (usa tu función split_string)
        List* itemsList = split_string(itemsStr, ";");
        // Aquí debes convertir la lista de strings a arreglo de Items (o List*)
        // Simplifico dejando en NULL por ahora
        nodo->state.availableItems = NULL;

        // Guardar IDs de conexiones
        conexionesTemp[nodoIndex][0] = atoi(arribaStr);
        conexionesTemp[nodoIndex][1] = atoi(abajoStr);
        conexionesTemp[nodoIndex][2] = atoi(izqStr);
        conexionesTemp[nodoIndex][3] = atoi(derStr);

        // Marcar si es final
        nodo->state.esFinal = (strcmp(esFinalStr, "Sí") == 0 || strcmp(esFinalStr, "Si") == 0);

        // Inicializar punteros de direcciones a NULL
        for (int d = 0; d < MAXDIR; d++) {
            nodo->adjacents[d] = NULL;
        }

        nodoIndex++;
    }

    fclose(archivo);

    // Segundo paso: asignar punteros de conexiones
    for (size_t i = 0; i < grafo->numberOfNodes; i++) {
        for (int d = 0; d < MAXDIR; d++) {
            int vecinoID = conexionesTemp[i][d];
            if (vecinoID != -1) {
                // El ID de nodo en el CSV es 1-based, pero el arreglo es 0-based
                size_t idxVecino = vecinoID - 1;
                if (idxVecino < grafo->numberOfNodes) {
                    grafo->nodes[i].adjacents[d] = &grafo->nodes[idxVecino];
                }
            }
        }
    }

    // Liberar conexiones temporales
    for (size_t i = 0; i < grafo->numberOfNodes; i++) {
        free(conexionesTemp[i]);
    }
    free(conexionesTemp);

    return grafo;
}



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
    puts("(1)   Cargar Laberinto desde CSV");
    puts("(2)   Iniciar Partida");
    puts("(3)   Salir");
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
        option = readOption(reading, 3);
        switch(option) {
            case '1':
                leer_escenarios();
                break;
            case '2':
                puts("OPCION INICIAR PARTIDA ");
                break;
            case '3':
                puts("OPCION SALIR");
                break;
            default:
                puts("\nOpción inválida, intente nuevamente");
                break;
        }
        presioneTeclaParaContinuar();
    } while (option != '3');

}