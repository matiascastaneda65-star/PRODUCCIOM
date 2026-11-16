/*
 * ISWZ1102 - PROGRAMACION 1 - Actividad evaluativa RC1
 * Sistema de optimizacion de produccion - Fabrica de componentes electronicos
 *
 * Restricciones del enunciado:
 *   - Sin estructuras (struct) ni archivos: los datos viven solo en memoria.
 *   - Solo arreglos paralelos y punteros.
 *   - Busqueda y ordenamiento con logica propia (sin qsort, bsearch, strcmp, etc.).
 *
 * Arreglos paralelos (misma posicion = mismo producto):
 *   nombres[i]  : nombre del producto
 *   demanda[i]  : unidades demandadas
 *   tiempoU[i] pr : horas de fabricacion por unidad
 *   recursoU[i] : unidades de recurso requeridas por unidad fabricada
 */

#include <stdio.h>

#define MAX_PRODUCTOS 5
#define LONG_NOMBRE   30
#define LONG_LINEA    100

/* ===================== UTILIDADES DE TEXTO ===================== */

int longitud(const char *s) {
    int n = 0;
    while (*(s + n) != '\0') n++;
    return n;
}

void copiarCadena(char *destino, const char *origen) {
    while ((*destino = *origen) != '\0') {
        destino++;
        origen++;
    }
}

char aMinuscula(char c) {
    if (c >= 'A' && c <= 'Z') return (char)(c + ('a' - 'A'));
    return c;
}

/* Comparacion propia, sin distinguir mayusculas. Retorna 1 si son iguales. */
int cadenasIguales(const char *a, const char *b) {
    while (*a != '\0' && *b != '\0') {
        if (aMinuscula(*a) != aMinuscula(*b)) return 0;
        a++;
        b++;
    }
    return (*a == '\0' && *b == '\0');
}

/* ===================== ENTRADA VALIDADA ===================== */

void leerLinea(const char *mensaje, char *buffer, int tam) {
    int i;
    printf("%s", mensaje);
    if (fgets(buffer, tam, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    for (i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
            break;
        }
    }
}

int leerEntero(const char *mensaje, int minimo, int maximo) {
    char linea[LONG_LINEA];
    int valor, ok;
    char extra;
    do {
        leerLinea(mensaje, linea, LONG_LINEA);
        ok = (sscanf(linea, "%d %c", &valor, &extra) == 1);
        if (!ok || valor < minimo || valor > maximo)
            printf("  [!] Valor invalido. Ingrese un entero entre %d y %d.\n", minimo, maximo);
        else
            return valor;
    } while (1);
}

double leerReal(const char *mensaje) {
    char linea[LONG_LINEA];
    double valor;
    char extra;
    do {
        leerLinea(mensaje, linea, LONG_LINEA);
        if (sscanf(linea, "%lf %c", &valor, &extra) == 1 && valor > 0.0)
            return valor;
        printf("  [!] Valor invalido. Ingrese un numero real mayor que 0.\n");
    } while (1);
}

void leerNombre(char *destino) {
    char linea[LONG_LINEA];
    do {
        leerLinea("  Nombre del producto: ", linea, LONG_LINEA);
        if (longitud(linea) == 0)
            printf("  [!] El nombre no puede estar vacio.\n");
        else if (longitud(linea) >= LONG_NOMBRE)
            printf("  [!] Maximo %d caracteres.\n", LONG_NOMBRE - 1);
        else
            break;
    } while (1);
    copiarCadena(destino, linea);
}

/* ===================== BUSQUEDA PROPIA (lineal) ===================== */

/* Retorna la posicion del producto o -1 si no existe. */
int buscarProducto(char (*nombres)[LONG_NOMBRE], int cantidad, const char *clave) {
    int i;
    for (i = 0; i < cantidad; i++) {
        if (cadenasIguales(*(nombres + i), clave)) return i;
    }
    return -1;
}

/* ===================== ORDENAMIENTO PROPIO (burbuja) ===================== */

/* Ordena el arreglo de indices 'orden' por tiempo total de fabricacion (mayor a menor).
 * No modifica los arreglos de datos. */
void ordenarPorTiempoTotal(int *orden, const int *demanda, const double *tiempoU, int cantidad) {
    int i, j, aux;
    for (i = 0; i < cantidad; i++) *(orden + i) = i;
    for (i = 0; i < cantidad - 1; i++) {
        for (j = 0; j < cantidad - 1 - i; j++) {
            double t1 = *(demanda + *(orden + j))     * *(tiempoU + *(orden + j));
            double t2 = *(demanda + *(orden + j + 1)) * *(tiempoU + *(orden + j + 1));
            if (t1 < t2) {
                aux = *(orden + j);
                *(orden + j) = *(orden + j + 1);
                *(orden + j + 1) = aux;
            }
        }
    }
}

/* ===================== OPERACIONES DEL MENU ===================== */

void registrarProducto(char (*nombres)[LONG_NOMBRE], int *demanda, double *tiempoU,
                       double *recursoU, int *cantidad) {
    char nuevo[LONG_NOMBRE];
    if (*cantidad >= MAX_PRODUCTOS) {
        printf("  [!] Capacidad maxima alcanzada (%d productos).\n", MAX_PRODUCTOS);
        return;
    }
    printf("\n--- Registrar producto %d de %d ---\n", *cantidad + 1, MAX_PRODUCTOS);
    leerNombre(nuevo);
    if (buscarProducto(nombres, *cantidad, nuevo) != -1) {
        printf("  [!] Ya existe un producto con ese nombre.\n");
        return;
    }
    copiarCadena(*(nombres + *cantidad), nuevo);
    *(demanda + *cantidad)  = leerEntero("  Demanda (unidades): ", 1, 1000000);
    *(tiempoU + *cantidad)  = leerReal("  Tiempo de fabricacion por unidad (horas): ");
    *(recursoU + *cantidad) = leerReal("  Recursos requeridos por unidad: ");
    (*cantidad)++;
    printf("  Producto registrado correctamente.\n");
}

void listarProductos(char (*nombres)[LONG_NOMBRE], const int *demanda, const double *tiempoU,
                     const double *recursoU, int cantidad) {
    int i;
    if (cantidad == 0) {
        printf("\n  No hay productos registrados.\n");
        return;
    }
    printf("\n%-3s %-20s %10s %12s %12s\n", "No", "Producto", "Demanda", "Horas/und", "Recurso/und");
    printf("------------------------------------------------------------\n");
    for (i = 0; i < cantidad; i++) {
        printf("%-3d %-20s %10d %12.2f %12.2f\n", i + 1, *(nombres + i),
               *(demanda + i), *(tiempoU + i), *(recursoU + i));
    }
}

void listarOrdenado(char (*nombres)[LONG_NOMBRE], const int *demanda, const double *tiempoU,
                    int cantidad) {
    int orden[MAX_PRODUCTOS];
    int k, i;
    if (cantidad == 0) {
        printf("\n  No hay productos registrados.\n");
        return;
    }
    ordenarPorTiempoTotal(orden, demanda, tiempoU, cantidad);
    printf("\nProductos ordenados por tiempo total de fabricacion (mayor a menor)\n");
    printf("%-3s %-20s %10s %16s\n", "No", "Producto", "Demanda", "Tiempo total (h)");
    printf("------------------------------------------------------\n");
    for (k = 0; k < cantidad; k++) {
        i = *(orden + k);
        printf("%-3d %-20s %10d %16.2f\n", k + 1, *(nombres + i), *(demanda + i),
               *(demanda + i) * *(tiempoU + i));
    }
}

void editarProducto(char (*nombres)[LONG_NOMBRE], int *demanda, double *tiempoU,
                    double *recursoU, int cantidad) {
    char clave[LONG_LINEA];
    char nuevo[LONG_NOMBRE];
    int pos, op;
    if (cantidad == 0) {
        printf("\n  No hay productos para editar.\n");
        return;
    }
    leerLinea("\nNombre del producto a editar: ", clave, LONG_LINEA);
    pos = buscarProducto(nombres, cantidad, clave);
    if (pos == -1) {
        printf("  [!] Producto no encontrado.\n");
        return;
    }
    printf("  Editando: %s\n", *(nombres + pos));
    printf("  1) Nombre  2) Demanda  3) Horas por unidad  4) Recurso por unidad\n");
    op = leerEntero("  Dato a modificar: ", 1, 4);
    switch (op) {
        case 1:
            leerNombre(nuevo);
            {
                int otro = buscarProducto(nombres, cantidad, nuevo);
                if (otro != -1 && otro != pos) {
                    printf("  [!] Ya existe otro producto con ese nombre.\n");
                    return;
                }
            }
            copiarCadena(*(nombres + pos), nuevo);
            break;
        case 2:
            *(demanda + pos) = leerEntero("  Nueva demanda (unidades): ", 1, 1000000);
            break;
        case 3:
            *(tiempoU + pos) = leerReal("  Nuevo tiempo por unidad (horas): ");
            break;
        case 4:
            *(recursoU + pos) = leerReal("  Nuevo recurso por unidad: ");
            break;
    }
    printf("  Producto actualizado.\n");
}

void eliminarProducto(char (*nombres)[LONG_NOMBRE], int *demanda, double *tiempoU,
                      double *recursoU, int *cantidad) {
    char clave[LONG_LINEA];
    char conf[LONG_LINEA];
    int pos, i;
    if (*cantidad == 0) {
        printf("\n  No hay productos para eliminar.\n");
        return;
    }
    leerLinea("\nNombre del producto a eliminar: ", clave, LONG_LINEA);
    pos = buscarProducto(nombres, *cantidad, clave);
    if (pos == -1) {
        printf("  [!] Producto no encontrado.\n");
        return;
    }
    printf("  Se eliminara: %s\n", *(nombres + pos));
    leerLinea("  Confirmar (s/n): ", conf, LONG_LINEA);
    if (!(conf[0] == 's' || conf[0] == 'S')) {
        printf("  Operacion cancelada.\n");
        return;
    }
    /* Desplazamiento a la izquierda para cerrar el hueco en todos los arreglos paralelos */
    for (i = pos; i < *cantidad - 1; i++) {
        copiarCadena(*(nombres + i), *(nombres + i + 1));
        *(demanda + i)  = *(demanda + i + 1);
        *(tiempoU + i)  = *(tiempoU + i + 1);
        *(recursoU + i) = *(recursoU + i + 1);
    }
    (*cantidad)--;
    printf("  Producto eliminado.\n");
}

void calcularProduccion(char (*nombres)[LONG_NOMBRE], const int *demanda, const double *tiempoU,
                        const double *recursoU, int cantidad) {
    double tiempoDisp, recursoDisp;
    double tiempoTotal = 0.0, recursoTotal = 0.0;
    double ratioT, ratioR, cobertura;
    int i;

    if (cantidad == 0) {
        printf("\n  No hay productos registrados para calcular.\n");
        return;
    }
    printf("\n--- Datos de la linea de produccion ---\n");
    tiempoDisp  = leerReal("  Tiempo de produccion disponible (horas): ");
    recursoDisp = leerReal("  Recursos disponibles (unidades): ");

    printf("\n%-3s %-20s %8s %14s %14s\n", "No", "Producto", "Demanda", "Tiempo (h)", "Recursos");
    printf("---------------------------------------------------------------\n");
    for (i = 0; i < cantidad; i++) {
        double t = *(demanda + i) * *(tiempoU + i);
        double r = *(demanda + i) * *(recursoU + i);
        tiempoTotal  += t;
        recursoTotal += r;
        printf("%-3d %-20s %8d %14.2f %14.2f\n", i + 1, *(nombres + i), *(demanda + i), t, r);
    }
    printf("---------------------------------------------------------------\n");
    printf("%-24s %8s %14.2f %14.2f\n", "TOTAL REQUERIDO", "", tiempoTotal, recursoTotal);
    printf("%-24s %8s %14.2f %14.2f\n", "DISPONIBLE", "", tiempoDisp, recursoDisp);

    printf("\nUso de tiempo:   %.1f %%\n", tiempoTotal / tiempoDisp * 100.0);
    printf("Uso de recursos: %.1f %%\n", recursoTotal / recursoDisp * 100.0);

    if (tiempoTotal <= tiempoDisp && recursoTotal <= recursoDisp) {
        printf("\nRESULTADO: LA FABRICA PUEDE CUMPLIR CON LA DEMANDA.\n");
        printf("  Holgura de tiempo:   %.2f h\n", tiempoDisp - tiempoTotal);
        printf("  Holgura de recursos: %.2f\n", recursoDisp - recursoTotal);
    } else {
        printf("\nRESULTADO: LA FABRICA NO PUEDE CUMPLIR CON LA DEMANDA.\n");
        if (tiempoTotal > tiempoDisp)
            printf("  Faltan %.2f horas de produccion.\n", tiempoTotal - tiempoDisp);
        if (recursoTotal > recursoDisp)
            printf("  Faltan %.2f unidades de recurso.\n", recursoTotal - recursoDisp);
        ratioT = tiempoDisp / tiempoTotal;
        ratioR = recursoDisp / recursoTotal;
        cobertura = (ratioT < ratioR ? ratioT : ratioR) * 100.0;
        printf("  Con los limites actuales solo se cubre aprox. el %.1f %% de la demanda.\n", cobertura);
    }
}

/* ===================== PROGRAMA PRINCIPAL ===================== */

void mostrarMenu(int cantidad) {
    printf("\n==================================================\n");
    printf("  OPTIMIZACION DE PRODUCCION - COMPONENTES ELECTRONICOS\n");
    printf("  Productos registrados: %d de %d\n", cantidad, MAX_PRODUCTOS);
    printf("==================================================\n");
    printf("  1. Registrar producto\n");
    printf("  2. Listar productos\n");
    printf("  3. Editar producto\n");
    printf("  4. Eliminar producto\n");
    printf("  5. Calcular produccion y factibilidad\n");
    printf("  6. Listar ordenado por tiempo total\n");
    printf("  0. Salir\n");
}

int main(void) {
    char   nombres[MAX_PRODUCTOS][LONG_NOMBRE];
    int    demanda[MAX_PRODUCTOS];
    double tiempoU[MAX_PRODUCTOS];
    double recursoU[MAX_PRODUCTOS];
    int    cantidad = 0;
    int    opcion;

    do {
        mostrarMenu(cantidad);
        opcion = leerEntero("Seleccione una opcion: ", 0, 6);
        switch (opcion) {
            case 1: registrarProducto(nombres, demanda, tiempoU, recursoU, &cantidad); break;
            case 2: listarProductos(nombres, demanda, tiempoU, recursoU, cantidad); break;
            case 3: editarProducto(nombres, demanda, tiempoU, recursoU, cantidad); break;
            case 4: eliminarProducto(nombres, demanda, tiempoU, recursoU, &cantidad); break;
            case 5: calcularProduccion(nombres, demanda, tiempoU, recursoU, cantidad); break;
            case 6: listarOrdenado(nombres, demanda, tiempoU, cantidad); break;
            case 0: printf("\nFin del programa.\n"); break;
        }
    } while (opcion != 0);

    return 0;
}
