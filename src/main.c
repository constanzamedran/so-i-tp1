/**
 * @file main.c
 * @brief Entry point of the system
 */

#include "../include/expose_metrics.h"
#include "../include/metrics.h"
#include <stdbool.h>

/**
 * @brief Tiempo de espera entre actualizaciones de métricas en segundos.
 */
#define SLEEP_TIME 1
/**
 * @brief Ejecuta el programa principal.
 * @param argc Cantidad de argumentos.
 * @param argv Argumentos de la línea de comandos.
 * @return 0 si el programa termina correctamente, 1 en caso contrario.
 */

int main(int argc, char* argv[])
{
    init_metrics();
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Bucle principal para actualizar las métricas cada segundo
    while (true)
    {
        update_cpu_gauge();
        update_memory_gauge();
        update_disk_io_gauge();
        update_red_gauge();
        update_proc_number();
        update_context_switches();
        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}