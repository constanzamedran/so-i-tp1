#include "../include/expose_metrics.h"

/**
 * @file expose_metrics.c
 * @brief Implementación de las funciones para exponer métricas a Prometheus.
 */

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/**
 * @brief Métrica de Prometheus para el uso de la CPU
 */
static prom_gauge_t* cpu_usage_metric;

/**
 * @brief Métrica de Prometheus para el uso de memoria
 */
static prom_gauge_t* memory_usage_metric;

/**
 * @brief Métrica de Prometheus para el uso de I/O del disco
 */
static prom_gauge_t* io_disk_usage_metric;

/**
 * @brief Métrica de Prometheus para el uso de la red
 */
static prom_gauge_t* red_usage_metric;

/**
 * @brief Métrica de Prometheus para el número de procesos
 */
static prom_gauge_t* proc_number_metric;

/**
 * @brief Métrica de Prometheus para los cambios de contexto
 */
static prom_gauge_t* context_switches_metric;

/**
 * @brief Actualiza la métrica de uso de CPU.
 *
 * Obtiene el uso de CPU y actualiza la métrica correspondiente en Prometheus.
 * Si no se puede obtener el uso de CPU, se imprime un mensaje de error.
 */
void update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
}

/**
 * @brief Actualiza la métrica de uso de memoria.
 *
 * Obtiene el uso de memoria y actualiza la métrica correspondiente en Prometheus.
 * Si no se puede obtener el uso de memoria, se imprime un mensaje de error.
 */
void update_memory_gauge()
{
    double usage = get_memory_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

/**
 * @brief Actualiza la métrica de uso de I/O del disco.
 *
 * Obtiene el uso de I/O del disco y actualiza la métrica correspondiente en Prometheus.
 * Si no se puede obtener el uso de I/O del disco, se imprime un mensaje de error.
 */

void update_disk_io_gauge()
{
    double usage = get_IO_disco();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(io_disk_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

void update_red_gauge()
{
    double usage = get_red_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(red_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de red\n");
    }
}

/**
 * @brief Actualiza la métrica de procesos en ejecución.
 *
 * Obtiene la cantidad de procesos ejecutandose y actualiza la métrica correspondiente en Prometheus.
 * Si no se puede obtener la cantidad de procesos, se imprime un mensaje de error.
 */

void update_proc_number()
{
    double number = get_proc_number();
    if (number >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(proc_number_metric, number, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de procesos\n");
    }
}
/**
 * @brief Actualiza la métrica de cambios de contexto.
 *
 * Obtiene los cambios de contexto que se están dando y actualiza la métrica correspondiente en Prometheus.
 * Si no se puede obtener la cantidad de cambios de contexto, se imprime un mensaje de error.
 */

void update_context_switches()
{
    double number = get_context_switches();
    if (number >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(context_switches_metric, number, NULL);
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener la cantidad de cambios de contexto\n");
    }
}

/**
 * @brief Expone las métricas vía HTTP en el puerto 8000.
 *
 * Obtiene todas las metricas y las actualiza en Prometheus.
 * Si no se puede iniciar el servidor, se imprime un mensaje de error.
 */
void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

/**
 * @brief Inicializa el mutex y las métricas de Prometheus.
 *
 * Inicializa el mutex y las métricas de Prometheus para el uso de CPU y memoria.
 * Si no se pueden inicializar, se imprime un mensaje de error.
 */
void init_metrics()
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
    }

    // Creamos la métrica para el uso de CPU
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
    }

    // Creamos la métrica para el uso de memoria
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
    }

    // Creamos la métrica para el uso de I/O de disco
    io_disk_usage_metric = prom_gauge_new("io_disk_usage_percentage", "Porcentaje de uso de I/O de disco", 0, NULL);
    if (io_disk_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de I/O de disco\n");
    }

    // Creamos la métrica para el uso de red
    red_usage_metric = prom_gauge_new("red_usage_percentage", "Porcentaje de uso de red", 0, NULL);
    if (red_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de Red\n");
    }

    // Creamos la métrica para la cantidad de procesos en ejecución
    proc_number_metric = prom_gauge_new("execution_process_number", "Cantidad de procesos en ejecución", 0, NULL);
    if (proc_number_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de cantidad de procesos en ejecución\n");
    }

    // Creamos la métrica para la cantidad de procesos en ejecución
    context_switches_metric = prom_gauge_new("context_switches", "Cantidad de cambios de contexto", 0, NULL);
    if (context_switches_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de cambios de contexto\n");
    }

    // Registramos las métricas en el registro por defecto
    if (prom_collector_registry_must_register_metric(memory_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - memoria\n");
    }
    if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - cpu\n");
    }
    if (prom_collector_registry_must_register_metric(io_disk_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - IO\n");
    }
    if (prom_collector_registry_must_register_metric(red_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas de uso de red\n");
    }
    if (prom_collector_registry_must_register_metric(proc_number_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas de cantidad de procesos en ejecución\n");
    }
    if (prom_collector_registry_must_register_metric(context_switches_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas de cambio de contexto\n");
    }
}

/**
 * @brief Destruye el mutex.
 */
void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
