#include "logger.h"

// Serial includes
#include "serial_mdw.h"
#include "delay.h"

// Ethernet includes
#include "lib/network/udp_client/udp_client.h"

static const uint8_t	LOGGER_UDP_DELAY = 100; // This parameter will influence greatly the behavior of the system because of the delay introduced
static const uint32_t	LOGGER_SERIAL_SPEED = 115200ul;
static const char *level_names[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

struct logger_state* logger;

/**
 * @brief Initialize the logger 
 * @ingroup logger
 */
void logger_init()
{
	logger = (struct logger_state*)mem_malloc(sizeof(struct logger_state));

	logger->logger_log_interface = LOG_INTERFACE_BOTH;
	logger->logger_log_level = LOG_DEBUG;
	logger->serial_options = (usart_serial_options_t){
		.baudrate = LOGGER_SERIAL_SPEED,
		.charlength = US_MR_CHRL_8_BIT,
		.paritytype = US_MR_PAR_NO,
		.stopbits = US_MR_NBSTOP_1_BIT
	};
	logger->dest_port = 10000;
	IP4_ADDR(&(logger->dest_ipaddr), RPI_IP_ADDR_A, RPI_IP_ADDR_B, RPI_IP_ADDR_C, RPI_IP_ADDR_D);

	#if !defined(TEST)
	// Initialize serial interface
	if (logger->logger_log_interface == LOG_INTERFACE_SERIAL || logger->logger_log_interface == LOG_INTERFACE_BOTH) {
		serial_mdw_init_interface(LOGGER_SERIAL_INTERFACE, &(logger->serial_options), TIMESTAMP_USED);
	}
	// Initialize ethernet interface
	if (logger->logger_log_interface == LOG_INTERFACE_ETHERNET || logger->logger_log_interface == LOG_INTERFACE_BOTH) {
		udp_client_init();
	}
	#endif
}

/**
 * @brief Change the log level
 * @ingroup logger
 * @param[in] log_level_t
 */
void logger_set_log_level(log_level_t log_level)
{
	logger->logger_log_level = log_level;
}

/**
 * @brief Read the log level
 * @ingroup logger
 * @param[out] log_level_t
 */
uint8_t logger_get_log_level(void)
{
	return logger->logger_log_level;
}

/**
 * @brief Change the interface to output logs
 * @ingroup logger
 * @param[in] log_interface_t 
 */
void logger_set_log_interface(log_interface_t log_interface)
{
	logger->logger_log_interface = log_interface;
}

/**
 * @brief Convert a uint8_t buffer into decimal character
 * @ingroup logger
 * @param[in] p_buff Pointer to the buffer of data
 * @param[in] buffer_length Length of the buffer
 * @return char* String of characters
 */
char * log_buffer(uint8_t *p_buff, uint8_t buffer_length)
{
	uint8_t length = 0;
    char *buffer = (char*) malloc(LOGGER_MESSAGE_MAX_LENGTH * sizeof(char));

    length += snprintf(buffer + length, LOGGER_MESSAGE_MAX_LENGTH, "<");

    for(uint8_t i=0; i<buffer_length; i++){
        length += snprintf(buffer + length, LOGGER_MESSAGE_MAX_LENGTH, "%u:", *p_buff);
		p_buff++;
	}
	if(buffer_length > 0)
	{
		length += snprintf(buffer + length, LOGGER_MESSAGE_MAX_LENGTH, "\b>");
	}else
	{
		length += snprintf(buffer + length, LOGGER_MESSAGE_MAX_LENGTH, ">");
	}
	
	return buffer;
}

/**
 * @brief Generate the log message and send it to the interface defined (SERIAL_LOG, CONSOLE_LOG)
 * 
 */
void log_log(log_level_t level, const char *file, uint32_t line, const char *fmt, ...)
{
	if (level >= logger->logger_log_level)
	{
		int length = 0;
		char buffer [LOGGER_MESSAGE_MAX_LENGTH] ={0};
		va_list args;

		va_start(args, fmt);
		length = vsnprintf(buffer, LOGGER_MESSAGE_MAX_LENGTH, fmt, args);
		va_end (args);
		
		#if defined(ADVANCED_LOG)
			char output_message[LOGGER_MESSAGE_MAX_LENGTH]={0};
			int length_advanced_log = 0;
			length_advanced_log = snprintf(output_message, LOGGER_MESSAGE_MAX_LENGTH, "%-5s %s:%lu: ", level_names[level], file, line,fmt, args);

			// Protection against length being > LOGGER_MESSAGE_MAX_LENGTH
			if(	length_advanced_log >= 0 && 
				(length + length_advanced_log <= LOGGER_MESSAGE_MAX_LENGTH))
			{
				length += length_advanced_log;
				strncat(output_message, buffer, length_advanced_log + length);
			}
			else if	(length_advanced_log >= 0 && 
					(length + length_advanced_log > LOGGER_MESSAGE_MAX_LENGTH))
			{
				// Restrain the size to LOGGER_MESSAGE_MAX_LENGTH - length preamble + length of data - 3 (indicate that there is more data but can't be displayed)  
				strncat(output_message, buffer, LOGGER_MESSAGE_MAX_LENGTH - length + length_advanced_log - 3);
				strncat(output_message, "...", LOGGER_MESSAGE_MAX_LENGTH);
				length = LOGGER_MESSAGE_MAX_LENGTH;
			}
			memcpy(buffer, output_message, length);
		#endif

		#if !defined(TEST)
			if (length >= 0) {
				if (logger->logger_log_interface == (log_interface_t)LOG_INTERFACE_SERIAL || logger->logger_log_interface == (log_interface_t)LOG_INTERFACE_BOTH)
				{
					serial_mdw_send_bytes(LOGGER_SERIAL_INTERFACE, (uint8_t*)buffer, (uint32_t)length);
					// Send a return carriage + new line only when using serial link
					char returnToTheLine_buffer[2] = { '\r', '\n' };
					serial_mdw_send_bytes(LOGGER_SERIAL_INTERFACE, (uint8_t*)returnToTheLine_buffer, (uint32_t)2);
				}
				if (logger->logger_log_interface == (log_interface_t)LOG_INTERFACE_ETHERNET || logger->logger_log_interface == (log_interface_t)LOG_INTERFACE_BOTH)
				{
					udp_client_send_to((uint8_t*)buffer, (uint32_t)length, &(logger->dest_ipaddr), logger->dest_port);

				}
			}
		#elif defined(TEST)
			printf("%s\n",buffer, length);
        #endif
	}
	 
}