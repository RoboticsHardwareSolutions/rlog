#ifndef __RLOG_H__
#define __RLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define RLOG_ERROR_PRINT 0x1
#define RLOG_WARNING_PRINT 0x2
#define RLOG_INFO_PRINT 0x4
#define RLOG_DEBUG_PRINT 0x8
#define RLOG_TIME_PRINT 0x10
#define RLOG_DATA_AND_TIME_PRINT 0x20

#define RLOG_ERROR(...) rlog_print(RLOG_ERROR_PRINT, __VA_ARGS__)
#define RLOG_WARNING(...) rlog_print(RLOG_WARNING_PRINT, __VA_ARGS__)
#define RLOG_INFO(...) rlog_print(RLOG_INFO_PRINT, __VA_ARGS__)
#define RLOG_DEBUG(...) rlog_print(RLOG_DEBUG_PRINT, __VA_ARGS__)

#define RLOGF_ERROR(...) rlog_to_file(RLOG_ERROR_PRINT, __VA_ARGS__)
#define RLOGF_WARNING(...) rlog_to_file(RLOG_WARNING_PRINT, __VA_ARGS__)
#define RLOGF_INFO(...) rlog_to_file(RLOG_INFO_PRINT, __VA_ARGS__)
#define RLOGF_DEBUG(...) rlog_to_file(RLOG_DEBUG_PRINT, __VA_ARGS__)

typedef enum
{
    log_to_stdout,
    log_to_stderr,
    log_to_file,
#if defined(RLOG_BUFFER_SUPPORT)
    log_to_buffer,
#endif
} rlog_out;

/**
 * @param out
 */
void rlog_set_stream(rlog_out out);

/**
 * @return current stream
 */
rlog_out rlog_get_stream(void);

/**
 * @param level use mask for enable or disable ERROR / WARNING / INFO / DEBUG - message
 * Example
 *  rlog_set_level(RLOG_DEBUG_PRINT | RLOG_ERROR_PRINT ); // enabled ERROR and DEBUG message
 */
void rlog_set_level(int level);

/**
 * @param level
 * @param format
 * @param ...
 */
void rlog_print(int level, const char *format, ...);

/**
 * @brief Logs to file. Call rlog_set_stream(logs_to_file) first.
 *
 * @param level
 * @param format
 * @param ...
 */
void rlog_to_file(int level, const char *format, ...);

#if defined(RLOG_BUFFER_SUPPORT)

/**
 * @return amount data in buffer
 */
size_t rlog_data_available(void);

/**
 * @param data  - readed data
 * @param count - amount of data to be read
 * @return - readed amount data
 */
size_t rlog_read_from_buffer(void *data, size_t count);

/*
 * use for delete memory and mutex rlog
 */
void rlog_destroy(void);

#endif  // defined(RLOG_BUFFER_SUPPORT)

#ifdef __cplusplus
}
#endif

#endif
