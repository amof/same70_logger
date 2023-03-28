/** 
 *  @file
 *  @defgroup logger Logger library
 *  @code #include <logger.h> @endcode
 *  @brief Logger library
 *         Based on rxi library: https://github.com/rxi/log.c/
 *         Use macros (log_x) to use this logger.
 *         /!\ You have to initialize yourself the link you want to use.
 *  @date 2019-03-22
 *  @author Julien DELVAUX <delvaux.ju@gmail.com>
 *  @copyright MIT license
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "ip_addr.h"
#include "status_codes.h"
#include "uart_serial.h"

#if defined(TEST)
#  include <stdint.h>
#else
#  include "compiler.h"
#  include "conf_board.h"
#endif

/*
   ---------------------------------------
   ---------- Logger options ----------
   ---------------------------------------
*/

//! Log levels values possible
typedef enum {
   LOG_INTERFACE_SERIAL,
   LOG_INTERFACE_ETHERNET,
   LOG_INTERFACE_BOTH
} log_interface_t;

//! Log levels values possible
typedef enum {
   LOG_TRACE, 
   LOG_DEBUG, 
   LOG_INFO, 
   LOG_WARN, 
   LOG_ERROR, 
   LOG_FATAL
} log_level_t;

// Define if you want to use a more verbose option
#define ADVANCED_LOG
// Define the buffer length
#if defined(TEST)
#  define LOGGER_MESSAGE_MAX_LENGTH 255
#else
#  define LOGGER_MESSAGE_MAX_LENGTH 100
#endif

/*
   ---------------------------------------
   --------- Logger functions ---------
   ---------------------------------------
*/

#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)  log_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...)  log_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/**
 * @brief Initialize the logger 
 * @ingroup logger
 */
extern void logger_init(log_interface_t log_interface);
/**
 * @brief Set the UDP server destination of the logs
 * @ingroup logger
 * 
 * @param addr Address of the server
 * @param port Port of the server
 */
extern void logger_set_udp_server(ip_addr_t* addr, u16_t port);
/**
 * @brief Change the log level
 * @ingroup logger
 * @param[in] log_level_t
 */
extern void logger_set_log_level(log_level_t log_level);
/**
 * @brief Return the current log level
 * @ingroup logger
 * @param[out] log_level_t
 */
extern log_level_t logger_get_log_level(void);
/**
 * @brief Change the interface to output logs
 * @ingroup logger
 * @param[in] log_level_t 
 */
extern void logger_set_log_interface(log_interface_t log_interface);
/**
 * @brief Convert a uint8_t buffer into decimal character
 * @ingroup logger
 * @param[in] p_buff Pointer to the buffer of data
 * @param[in] buffer_length Length of the buffer
 * @return char* String of characters
 */
extern char * log_buffer(uint8_t *p_buff, uint8_t buffer_length);
/**
 * @brief Generate the log message and send it to the interface defined
 * 
 * @param level 
 * @param file 
 * @param line 
 * @param fmt 
 * @param ... 
 */
extern void log_log(log_level_t level, const char *file, uint32_t line, const char *fmt, ...) __attribute__ ((format (gnu_printf, 4, 5)));

#endif /* LOGGER_H_ */