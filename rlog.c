#include "rlog.h"
#include <stdarg.h>
#include <time.h>
#include "stdio.h"
#include "unistd.h"

#if defined(STM32G474xx) || defined(STM32F103xB) || defined(STM32F072xB) || defined(STM32F091xC) || defined(STM32F407xx) || defined(STM32F429xx) || \
    defined(STM32F765xx) || defined(STM32F103xE)
#    include "cmsis_os.h"
#endif

#if defined(RLOG_BUFFER_SUPPORT)
#    include "rbuffer.h"
#    include "stdlib.h"
#    include "pthread.h"

#    define RLOG_MAX_LINE_SIZE 128
#    define RLOG_BUFFER_SIZE 2048

rbuffer         rlog_buf;
void           *rlog_buf_mem;
bool            rlog_buf_inited = false;
char            rlog_line[RLOG_MAX_LINE_SIZE];
pthread_mutex_t mutex;

#endif  // defined(RLOG_BUFFER_SUPPORT)

char         data_time_string[30] = {0};
FILE        *stream;
static int   fd                 = STDOUT_FILENO;
static int   log_level          = RLOG_ERROR_PRINT | RLOG_WARNING_PRINT | RLOG_INFO_PRINT | RLOG_DEBUG_PRINT | RLOG_DATA_AND_TIME_PRINT;
static char *log_level_string[] = {"", " ERROR ", "WARNING", "", " INFO  ", "", "", "", " DEBUG "};
static int   log_level_color[]  = {0, 31, 33, 0, 32, 0, 0, 0, 36};

void rlog_print(int level, const char *format, ...)
{
    if (!(log_level & level))
        return;

    size_t current_size = 0;
    if (log_level & RLOG_TIME_PRINT)
    {
#if defined(RLOG_FOR_APPLE) || defined(RLOG_FOR_UNIX) || defined(RLOG_FOR_WINDOWS)
        time_t t     = time(NULL);
        current_size = strftime(data_time_string, 20, "%X", localtime(&t));
#else
        unsigned long time = osKernelSysTick();
        current_size       = sprintf(data_time_string, "%12lu", time);
#endif
    }
    else if (log_level & (RLOG_TIME_PRINT | RLOG_DATA_AND_TIME_PRINT))
    {
#if defined(RLOG_FOR_APPLE) || defined(RLOG_FOR_UNIX) || defined(RLOG_FOR_WINDOWS)
        time_t t     = time(NULL);
        current_size = strftime(data_time_string, 20, "%x-%X ", localtime(&t));
#else
        unsigned long time = osKernelSysTick();
        current_size       = sprintf(data_time_string, "%12lu", time);
#endif
    }

    if (fd != -1)
    {
        dprintf(fd, "%.*s \033[%d;1m [%s] \033[m", (int) current_size, data_time_string, log_level_color[level], log_level_string[level]);
        va_list ptr;
        va_start(ptr, format);
        vdprintf(fd, format, ptr);
        va_end(ptr);
#if defined(RLOG_FOR_APPLE) || defined(RLOG_FOR_UNIX) || defined(RLOG_FOR_WINDOWS)
        dprintf(fd, "\n");
#else
        printf("\n");
#endif
    }

#if defined(RLOG_BUFFER_SUPPORT)
    else
    {
        current_size = sprintf(rlog_line, "%.*s  [%s] ", (int) current_size, data_time_string, log_level_string[level]);
        va_list ptr;
        va_start(ptr, format);
        current_size += sprintf(&rlog_line[current_size], format, ptr);
        va_end(ptr);
        current_size += sprintf(&rlog_line[current_size], "\n");

        if (pthread_mutex_lock(&mutex) != 0)
            return;

        rbuffer_push(&rlog_buf, (uint8_t *) &rlog_line, current_size);

        if (pthread_mutex_unlock(&mutex) != 0)
            return;
    }
#endif  // defined(RLOG_BUFFER_SUPPORT)
}

void rlog_to_file(int level, const char *format, ...)
{
    if (!(log_level & level))
        return;

    size_t current_size = 0;
    if (log_level & RLOG_TIME_PRINT)
    {
#if defined(RLOG_FOR_APPLE) || defined(RLOG_FOR_UNIX) || defined(RLOG_FOR_WINDOWS)
        time_t t     = time(NULL);
        current_size = strftime(data_time_string, 20, "%X", localtime(&t));
#else
        unsigned long time = osKernelSysTick();
        current_size       = sprintf(data_time_string, "%12lu", time);
#endif
    }
    else if (log_level & (RLOG_TIME_PRINT | RLOG_DATA_AND_TIME_PRINT))
    {
#if defined(RLOG_FOR_APPLE) || defined(RLOG_FOR_UNIX) || defined(RLOG_FOR_WINDOWS)
        time_t t     = time(NULL);
        current_size = strftime(data_time_string, 20, "%x-%X ", localtime(&t));
#else
        unsigned long time = osKernelSysTick();
        current_size       = sprintf(data_time_string, "%12lu", time);
#endif
    }
    stream = fopen("logs.txt", "a+");
    if (!stream)
    {
        printf("Cannot open logs.txt\n");
        return;
    }
    if (stream != NULL)
    {
        fprintf(stream, "%.*s [%s] ", (int) current_size, data_time_string, log_level_string[level]);
        va_list ptr;
        va_start(ptr, format);
        vfprintf(stream, format, ptr);
        va_end(ptr);
#if defined(RLOG_FOR_APPLE) || defined(RLOG_FOR_UNIX) || defined(RLOG_FOR_WINDOWS)
        fprintf(stream, "\n");
#else
        printf("\n");
#endif
        fclose(stream);
    }
}

void rlog_set_level(int level)
{
    log_level = level;
}

rlog_out rlog_get_stream(void)
{
    switch (fd)
    {
    case STDOUT_FILENO:
        return log_to_stdout;
    case STDERR_FILENO:
        return log_to_stderr;
#if defined(RLOG_BUFFER_SUPPORT)
    case -1:
        return log_to_buffer;
#endif
    default:
        return log_to_stdout;
    }
}
void rlog_set_stream(rlog_out out)
{
    // TODO add mutex ?
    switch (out)
    {
    case log_to_stdout:
        fd = STDOUT_FILENO;
        break;
    case log_to_stderr:
        fd = STDERR_FILENO;
        break;
    case log_to_file:
        fd = STDOUT_FILENO;
        break;
#if defined(RLOG_BUFFER_SUPPORT)
    case log_to_buffer:
        if (!rlog_buf_inited)
        {
            rlog_buf_mem = calloc(RLOG_BUFFER_SIZE, sizeof(uint8_t));
            if (rlog_buf_mem == NULL)
                return;

            pthread_mutex_init(&mutex, NULL);
            rbuffer_create(&rlog_buf, rlog_buf_mem, RLOG_BUFFER_SIZE * sizeof(uint8_t));
            rlog_buf_inited = true;
        }
        fd = -1;
        break;
#endif  // defined(RLOG_BUFFER_SUPPORT)
    default:
        fd = STDOUT_FILENO;
        break;
    }
}

#if defined(RLOG_BUFFER_SUPPORT)

size_t rlog_data_available(void)
{
    return rbuffer_data_available(&rlog_buf);
}

size_t rlog_read_from_buffer(void *data, size_t count)
{
    size_t size;

    if (pthread_mutex_lock(&mutex) != 0)
        return 0;

    size = rbuffer_pop(&rlog_buf, data, count);

    pthread_mutex_unlock(&mutex);

    return size;
}

void rlog_destroy(void)
{
    free(rlog_buf_mem);
    pthread_mutex_destroy(&mutex);
}

#endif  // defined(RLOG_BUFFER_SUPPORT)
