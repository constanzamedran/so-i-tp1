#include "../include/metrics.h"

/**
 * @brief Obtiene el porcentaje de uso de memoria.
 * @return Porcentaje de uso de memoria.
 */
double get_memory_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de memoria
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    return mem_usage_percent;
}

/**
 * @brief Obtiene el porcentaje de uso del cpu.
 * @return Porcentaje de uso del cpu.
 */
double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

/**
 * @brief Obtiene el porcentaje de uso de red.
 * @return Porcentaje de uso de red.
 */

double get_IO_disco()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long read_sectors = 0, write_sectors = 0, total_read = 0, total_write = 0;
    static unsigned long long prev_total_read = 0, prev_total_write = 0;
    double io_usage_percent = 0.0;

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1.0;
    }

    // Leer los valores de lectura y escritura
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Leer los sectores leídos y escritos
        if (sscanf(buffer, "%*d %*d %*s %*u %*u %*u %llu %*u %*u %llu", &read_sectors, &write_sectors) == 2)
        {
            total_read += read_sectors;   // Agrega al total de lectura el valor de lectura de la linea actual
            total_write += write_sectors; // Agrega al total de escritua el valor de escritura de la linea actual
        }
    }

    fclose(fp);

    // Calcular el porcentaje de uso de I/O de disco
    unsigned long long total_sectors = total_read + total_write;
    unsigned long long totald = (total_read - prev_total_read) + (total_write - prev_total_write);

    if (total_sectors > 0)
    {
        io_usage_percent = ((double)totald / total_sectors) * 100.0;
    }

    // Actualizar los valores anteriores
    prev_total_read = total_read;
    prev_total_write = total_write;

    return io_usage_percent;
}

/**
 * @brief Obtiene el porcentaje de uso de red.
 * @return Porcentaje de uso de red.
 */
double get_red_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_rx_bytes = 0, total_tx_bytes = 0;
    static unsigned long long prev_total_rx_bytes = 0, prev_total_tx_bytes = 0;
    double net_usage_percent = 0.0;

    // Abrir el archivo /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1.0;
    }

    // Leer los valores de tráfico de red
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        unsigned long long rx_bytes = 0, tx_bytes = 0;
        if (sscanf(buffer, "%*s %llu %*u %*u %*u %*u %*u %*u %*u %llu", &rx_bytes, &tx_bytes) == 2)
        {
            total_rx_bytes += rx_bytes;
            total_tx_bytes += tx_bytes;
        }
    }

    fclose(fp);

    // Calcular el porcentaje de uso de red
    unsigned long long total_bytes = total_rx_bytes + total_tx_bytes;
    unsigned long long totald = (total_rx_bytes - prev_total_rx_bytes) + (total_tx_bytes - prev_total_tx_bytes);

    if (total_bytes > 0)
    {
        net_usage_percent = ((double)totald / total_bytes) * 100.0;
    }

    // Actualizar los valores anteriores
    prev_total_rx_bytes = total_rx_bytes;
    prev_total_tx_bytes = total_tx_bytes;

    return net_usage_percent;
}

/**
 * @brief Obtiene el número de procesos en ejecución.
 * @return Número de procesos en ejecución.
 */
double get_proc_number()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long proc_running = 0;

    // Abrir el archivo /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    // Leer el número de procesos en ejecución
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "procs_running %llu", &proc_running) == 1)
        {
            break; // Encontrado el número de procesos en ejecución
        }
    }

    fclose(fp);

    return (double)proc_running;
}

/**
 * @brief Obtiene el número de cambios de contexto.
 * @return Número de cambios de contexto.
 */
double get_context_switches()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long ctxt = 0;

    // Abrir el archivo /proc/stat
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    // Leer el número de cambios de contexto
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "ctxt %llu", &ctxt) == 1)
        {
            break; // Encontrado el número de cambios de contexto
        }
    }

    fclose(fp);

    return (double)ctxt;
}
