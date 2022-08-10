/** 
 *  @file
 *  @defgroup logger Logger library
 *  @code #include <logger.h> @endcode
 *  @brief Logger library
 *         Based on rxi library: https://github.com/rxi/log.c/
 *         Use macros (log_x) to use this logger.
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
#include "serial_mdw.h"

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

// Logger choice and options 
struct logger_state
{
	log_interface_t logger_log_interface;
	log_level_t logger_log_level;
	usart_serial_options_t serial_options;
	ip_addr_t dest_ipaddr;
	uint16_t dest_port;
};

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

extern void logger_init();
extern void serial_logger_init(log_level_t log_level);
extern void udp_logger_init(log_level_t log_level, ip_addr_t* addr, u16_t port);
extern void both_logger_init(log_level_t log_level, ip_addr_t* addr, u16_t port);
extern void logger_set_log_level(log_level_t log_level);
extern void logger_set_log_interface(log_interface_t log_interface);
extern char * log_buffer(uint8_t *p_buff, uint8_t buffer_length);
extern void log_log(log_level_t level, const char *file, uint32_t line, const char *fmt, ...) __attribute__ ((format (gnu_printf, 4, 5)));
extern uint8_t logger_get_log_level(void);

#endif /* LOGGER_H_ */