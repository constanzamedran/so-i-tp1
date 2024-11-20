/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de archivos /proc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del buffer utilizado para leer las métricas.
 */
#define BUFFER_SIZE 256

/**
 * @brief Obtiene el porcentaje de uso de memoria desde /proc/meminfo.
 *
 * Lee los valores de memoria total y disponible desde /proc/meminfo y calcula
 * el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_usage();

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage();

/**
 * @brief Obtiene el porcentaje de uso de I/O del disco desde /proc/diskstats.
 *
 * Lee los porcentajes de uso del disco  desde /proc/diskstats y calcula el porcentaje de uso del disco.
 *
 * @return Uso de disco como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_IO_disco();

/**
 * @brief Obtiene el porcentaje de uso de la red desde /proc/net/dev.
 *
 * Lee los bytes transmitidos y recibidos y calcula el porcentaje de uso de la red.
 *
 * @return Uso de red como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_red_usage();

/**
 * @brief Obtiene el número de procesos en ejecución.
 *
 *
 * @return Numero con la cantidad de procesos en ejecución.
 */
double get_proc_number();

/**
 * @brief Obtiene la cantidad de cambios de contexto desde /proc/stat.
 *
 *
 * @return Número con la cantidad de cambios de contexto.
 */
double get_context_switches();
