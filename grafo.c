#include "grafo.h"
#include "extra.h"
#include "list.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <ctype.h>

Graph graph;

/**
 * leer_escenarios
 * ----------------
 * Carga los datos de los escenarios desde un archivo CSV indicado por el usuario
 * y los almacena en la estructura global 'graph'. Cada fila del archivo representa
 * un nodo del grafo con sus propiedades: nombre, descripción, ítems disponibles,
 * adyacencias y si es un nodo final.
 *
 * Funcionamiento:
 *  - Solicita al usuario el nombre del archivo CSV a cargar.
 *  - Abre el archivo y lee los encabezados.
 *  - Realiza una primera pasada para crear los nodos con sus atributos.
 *  - En una segunda pasada, establece las adyacencias entre nodos (arriba, abajo, etc).
 *  - Informa cuántos escenarios fueron cargados correctamente.
 *
 * Consideraciones:
 *  - Si el archivo no existe o no puede abrirse, muestra un error y no altera el grafo.
 *  - Expande dinámicamente la memoria si hay más nodos que la capacidad inicial.
 *  - Usa listas para representar los ítems en cada nodo.
 */

void leer_escenarios() {
    limpiarPantalla();
    if (graph.nodes != NULL) liberarEscenarios(&graph);
    puts("======== Leer Escenarios ========");
    char nombreArchivo[100];
    printf("Ingrese el nombre del archivo CSV (ej: graphquest.csv): ");
    fgets(nombreArchivo, sizeof(nombreArchivo), stdin);
    nombreArchivo[strcspn(nombreArchivo, "\n")] = 0;

    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo \"%s\".\n", nombreArchivo);
        perror("Error");
        return;
    }

    size_t contadorEscenarios = 0;
    char **campos;
    int capacidad = 100; 
    graph.nodes = malloc(sizeof(Node) * capacidad);
    graph.numberOfNodes = 0;

    campos = leer_linea_csv(archivo, ','); 

    // Primera pasada: leer nodos
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        if (graph.numberOfNodes >= capacidad) {
            capacidad *= 2;
            graph.nodes = realloc(graph.nodes, sizeof(Node) * capacidad);
        }

        int id = atoi(campos[0]);
        Node *node = &graph.nodes[id - 1];
        graph.numberOfNodes++;
        contadorEscenarios++;

        
        strncpy(node->state.name, campos[1], MAXNAME);
        strncpy(node->state.description, campos[2], MAXDESC);
        node->state.availableItems = list_create();
        node->state.playerInventory = list_create();
        node->state.tiempoRestante = 0;

        
        List* items = split_string(campos[3], ";");
        for (char *item = list_first(items); item != NULL; item = list_next(items)) {
            List *values = split_string(item, ",");
            if (values == NULL || list_size(values) < 3) continue;

            Item *newItem = malloc(sizeof(Item));
            strncpy(newItem->name, list_first(values), MAXITEMNAME - 1);
            newItem->name[MAXITEMNAME - 1] = '\0';
            newItem->value = atoi(list_next(values));
            newItem->weight = atoi(list_next(values));

            list_pushBack(node->state.availableItems, newItem);
            list_clean(values);
            free(values);
        }
        list_clean(items);
        free(items);

        
        char *es_final = campos[8];
        for (int i = 0; es_final[i]; i++) es_final[i] = tolower(es_final[i]);
        node->state.esFinal = (strncmp(es_final, "sí", 2) == 0 || strncmp(es_final, "si", 2) == 0);

        
        node->adjacents = calloc(4, sizeof(Node*));
    }

    // Segunda pasada: establecer adyacencias
    rewind(archivo);
    leer_linea_csv(archivo, ',');
    int idx = 0;
    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Node *node = &graph.nodes[idx++];

        int arriba    = atoi(campos[4]);
        int abajo     = atoi(campos[5]);
        int izquierda = atoi(campos[6]);
        int derecha   = atoi(campos[7]);

        if (arriba != -1)    node->adjacents[0] = &graph.nodes[arriba - 1];
        if (abajo != -1)     node->adjacents[1] = &graph.nodes[abajo - 1];
        if (izquierda != -1) node->adjacents[2] = &graph.nodes[izquierda - 1];
        if (derecha != -1)   node->adjacents[3] = &graph.nodes[derecha - 1];
    }

    // Después de cargar todos los nodos y sus adyacencias:
    if (graph.numberOfNodes > 0) {
        graph.start = &graph.nodes[0];
    } else {
        graph.start = NULL;
    }

    fclose(archivo);
    printf("\nSe cargaron %zu escenarios desde \"%s\" correctamente.\n", contadorEscenarios, nombreArchivo);
}

/**
 * mostrar_grafo
 * --------------
 * Muestra por consola todos los nodos cargados en el grafo, junto con sus atributos.
 * Esta función es útil para depurar, verificar que el archivo se cargó correctamente,
 * o simplemente para mostrar al jugador los escenarios disponibles.
 *
 * Para cada nodo, se imprime:
 *  - ID y nombre del nodo.
 *  - Descripción del escenario.
 *  - Lista de ítems disponibles (nombre, valor en puntos, peso en kg).
 *  - Las adyacencias del nodo en cada dirección cardinal.
 *  - Si el nodo representa un escenario final.
 *
 * Consideraciones:
 *  - Si no se han cargado nodos previamente, muestra una advertencia.
 *  - Usa nombres legibles y un formato limpio para facilitar la lectura.
 */

void mostrar_grafo() {
    limpiarPantalla();

    if (graph.numberOfNodes == 0){
        puts("====- Advertencia -====");
        puts("Debes incluir el archivo .csv con los escenarios!");
        return;
    }

    const char* direcciones[] = {"Arriba", "Abajo", "Izquierda", "Derecha"};
    
    for (int i = 0; i < graph.numberOfNodes; i++) {
        if (i == 0) { puts("================ Mostrar Escenarios del Grafo ================");  }
        Node* node = &graph.nodes[i];
        printf("Nodo        : %d\n", i + 1);
        printf("Nombre      : '%s'\n", node->state.name);
        printf("\nDescripción:\n'%s'\n", node->state.description);

        if (list_size(node->state.availableItems) == 0){
            puts("\nNo hay ítems en este escenario.");
        } else {
            printf("\nLista de Ítems disponibles:\n");
            for (Item* item = list_first(node->state.availableItems); item != NULL; item = list_next(node->state.availableItems)) {
                printf("    - %s (%d pts, %d kg)\n", item->name, item->value, item->weight);
            }
        }

        printf("\nLista de Nodos Adyacentes:\n");
        for (int j = 0; j < 4; j++) {
            if (node->adjacents[j] != NULL) {
                int id_adyacente = (int)(node->adjacents[j] - graph.nodes) + 1; // calcular ID relativo
                printf("    %s → Nodo %d\n", direcciones[j], id_adyacente);
            } else {
                printf("    %s → Ninguno\n", direcciones[j]);
            }
        }

        printf("\n¿Es final?: %s\n", node->state.esFinal ? "Sí" : "No");
        puts("===============================================================");
    }
}

/**
 * copiar_grafo
 * -------------
 * Crea una copia profunda del grafo actual, replicando todos los nodos y sus atributos.
 * Esta función es útil para reiniciar la partida desde cero sin alterar el grafo original,
 * permitiendo mantener intactos los escenarios iniciales y sus ítems.
 *
 * Para cada nodo copiado, se duplica:
 *  - Su ID, nombre, descripción, y estado final.
 *  - La lista de ítems disponibles, con copias independientes de cada ítem.
 *  - Se inicializa un inventario vacío.
 *  - Las conexiones (adyacencias) se actualizan tras copiar todos los nodos.
 *
 * Consideraciones:
 *  - El puntero `start` del nuevo grafo apunta al nodo equivalente al original.
 *  - No se copian los jugadores ni estados de partida; solo la estructura base.
 *  - Es responsabilidad del usuario liberar la copia con `liberarEscenarios` al terminar.
 */

Graph* copiar_grafo(const Graph* original) {
    if (!original) return NULL;

    Graph* copia = malloc(sizeof(Graph));
    if (!copia) return NULL;

    copia->numberOfNodes = original->numberOfNodes;
    copia->capacidad = original->capacidad;

    // Reserva el arreglo de nodos
    if (copia->capacidad <= 0) {
        copia->capacidad = copia->numberOfNodes > 0 ? copia->numberOfNodes : 1;
    }
    copia->nodes = malloc(sizeof(Node) * copia->capacidad);
    if (!copia->nodes) {
        free(copia);
        return NULL;
    }

    // Copiar cada nodo uno a uno
    for (int i = 0; i < copia->numberOfNodes; i++) {
        // Copiar estructura Node (incluye State)
        copia->nodes[i] = original->nodes[i];

        // 1) Copiar cadenas (opcional, si no están garantizadas independientes)
        // Normalmente no hace falta porque memcpy copia los arrays estáticos

        // 2) Crear nuevas listas vacías para availableItems y playerInventory
        copia->nodes[i].state.availableItems = list_create();
        copia->nodes[i].state.playerInventory = list_create();

        // 3) Copiar items de availableItems (lista de Item*)
        for (Item* it = list_first(original->nodes[i].state.availableItems);
             it != NULL;
             it = list_next(original->nodes[i].state.availableItems)) {
            Item* nuevoItem = malloc(sizeof(Item));
            if (nuevoItem) {
                *nuevoItem = *it;  // copia el contenido de Item
                list_pushBack(copia->nodes[i].state.availableItems, nuevoItem);
            }
        }

        // No copiamos playerInventory (usualmente vacío al inicio)
        // Si quieres copiar inventarios, hazlo igual que arriba

        // 4) Reservar espacio para punteros de adyacentes, iniciar en NULL
        copia->nodes[i].adjacents = calloc(4, sizeof(Node*));
    }

    // 5) Calcular el nodo start dentro de la copia
    if (original->start) {
        ptrdiff_t offset = original->start - original->nodes; // índice relativo del nodo start
        copia->start = &copia->nodes[offset];
    } else {
        copia->start = NULL;
    }

    // 6) Asignar punteros adjacentes en copia, apuntando a nodos de la copia
    for (int i = 0; i < copia->numberOfNodes; i++) {
        for (int j = 0; j < 4; j++) {
            if (original->nodes[i].adjacents[j]) {
                ptrdiff_t adj_offset = original->nodes[i].adjacents[j] - original->nodes;
                copia->nodes[i].adjacents[j] = &copia->nodes[adj_offset];
            } else {
                copia->nodes[i].adjacents[j] = NULL;
            }
        }
    }

    return copia;
}



