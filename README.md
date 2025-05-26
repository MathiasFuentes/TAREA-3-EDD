# GraphQuest

**GraphQuest** es un juego de consola escrito en C, donde los jugadores deben recorrer un mapa representado como un grafo, recolectar ítems estratégicamente y llegar al nodo final antes de quedarse sin tiempo. El juego cuenta con modo de un jugador y multijugador por turnos, e incorpora gestión de inventario, caminos ramificados y condiciones de victoria.

---

# Compilación y ejecución

Este programa ha sido desarrollado en C y puede ejecutarse fácilmente desde Visual Studio Code o Replit.

### Requisitos

- Sistema compatible con C (Linux, Windows o Mac).
- Compilador GCC instalado (`gcc`).
- Visual Studio Code (opcional) con extensión de C/C++.

## Cómo usar desde Visual Studio Code

Asegúrate de tener:
- Visual Studio Code instalado.
- La extensión de C/C++ instalada.
- Un compilador de C apropiado según tu sistema operativo.

Pasos:
1. Descomprime los archivos en una carpeta.
2. Abre esa carpeta desde Visual Studio Code: `File > Open Folder...`.
3. Abre una terminal integrada: `Terminal > New Terminal`.
4. Compila con el siguiente comando:

```bash
gcc main.c graphquest.c grafo.c game.c extra.c list.c -o graphquest
```

Ejecuta el programa: 
```bash
./graphquest
```

## Cómo usar desde Replit
1. Descomprime el archivo .zip en una carpeta local.

2. Crea una nueva app en Replit, seleccionando la plantilla de C.

3. Elimina el archivo main.c por defecto.

4. Usa la opción “Upload Folder” para subir la carpeta completa del proyecto.

5. Presiona el botón verde “Run” para compilar y ejecutar.

## Funcionalidades
1. Recorrido y muestreo de los escenarios incluidos.

2. Modo de un jugador y multijugador por turnos.

3. Sistema de inventario con recogida y descarte de ítems.

4. Tiempo límite para completar el recorrido.

5. Sincronización de ítems en modo multijugador.

6. Reinicio de partida manteniendo la lógica original del grafo.

## Problemas conocidos
1. En sistemas sin soporte de terminal ANSI, el limpiado de pantalla (limpiarPantalla()) puede no funcionar correctamente.

2. Si un jugador abandona abruptamente, no se guarda el progreso.

3. No existe aún una funcionalidad para guardar/cargar partidas desde archivo.

4. Los nodos del archivo deben con ID ordenados de menor a mayor, como se ve en el archivo de ejemplo.

## A mejorar
1. Interfaz gráfica o más amigable por texto (menús, color, etc).

2. Agregar eventos aleatorios o enemigos.

3. Manejo de errores de lectura csv.

## Ejemplo de uso

### Modo de Un Jugador
```bash
=======- Partida en Curso -=======
Escenario: Cocina

Descripción Restos de una batalla culinaria. Cuchillos por aqui, migajas por alla. Alguien cocino… o fue cocinado.
Tiempo restante: 8

Inventario:
 - Cuchillo (valor 3, peso 1)

Ítems disponibles aquí:
 - Pan (valor 2, peso 1)

------ Opciones Disponibles ------
(1)   Recoger Ítem(s)
(2)   Descartar Ítem(s)
(3)   Avanzar en una dirección
(4)   Reiniciar partida
(5)   Salir del juego
Ingrese una opción (1-5): 
```

### Modo Multijugador

```bash
====== TURNO DEL JUGADOR 1 ======
Escenario: Entrada principal

Descripción Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.
Tiempo restante: 10

Tu inventario está vacío.

No hay ítems disponibles aquí.

------ Opciones Disponibles ------
(1)   Recoger Ítem(s)
(2)   Descartar Ítem(s)
(3)   Avanzar en una dirección
(4)   Reiniciar partida
(5)   Salir del juego
Ingrese una opción (1-5): 
```

## Contribuciones

Implementación estructuras de grafo: Bastián Guerra y Matías Fuentes.

Lógica de juego e inventario: Bastián Guerra.

Modo multijugador y sincronización de ítems: Bastián Guerra y Matías Fuentes

Diseño y redacción de Readme.md: Matías Fuentes.

