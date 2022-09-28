#include "logger.h"
#include "lib/network/udp_client/udp_client.h"
#include "serial_mdw.h"

static const char *level_names[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
struct logger_config_t
{
	log_interface_t log_interface;
	log_level_t log_level;
	usart_if usart;
	ip_addr_t udp_dest_ipaddr;
	uint16_t udp_dest_port;
} *logger_config;

void logger_init(log_interface_t log_interface)
{
	logger_config = (struct logger_config_t*)mem_malloc(sizeof(struct logger_config_t));
	logger_config->log_interface = log_interface;
	logger_config->log_level = LOG_INFO;
	logger_config->usart = NULL;
	logger_config->udp_dest_ipaddr;
	logger_config->udp_dest_port = 0;
}

void logger_set_udp_server(ip_addr_t* addr, u16_t port)
{
	if(logger_config->log_interface == LOG_INTERFACE_ETHERNET || logger_config->log_interface == LOG_INTERFACE_BOTH)
	{
		logger_config->udp_dest_ipaddr = *addr;
		logger_config->udp_dest_port = port;
	}
}

void logger_set_serial_link(usart_if usart)
{
	if(logger_config->log_interface == LOG_INTERFACE_SERIAL || logger_config->log_interface == LOG_INTERFACE_BOTH)
		logger_config->usart = usart;
}

void logger_set_log_level(log_level_t log_level)
{
	logger_config->log_level = log_level;
}

log_level_t logger_get_log_level(void)
{
	return logger_config->log_level;
}

void logger_set_log_interface(log_interface_t log_interface)
{
	logger_config->log_interface = log_interface;
}

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

void log_log(log_level_t level, const char *file, uint32_t line, const char *fmt, ...)
{
	if (level >= logger_config->log_level)
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
            if (logger_config->log_interface == (log_interface_t)LOG_INTERFACE_SERIAL || logger_config->log_interface == (log_interface_t)LOG_INTERFACE_BOTH)
            {
                serial_mdw_send_bytes(LOGGER_SERIAL_INTERFACE, (uint8_t*)buffer, (uint32_t)length);
                // Send a return carriage + new line only when using serial link
                char returnToTheLine_buffer[2] = { '\r', '\n' };
                serial_mdw_send_bytes(LOGGER_SERIAL_INTERFACE, (uint8_t*)returnToTheLine_buffer, (uint32_t)2);
            }
            if (logger_config->log_interface == (log_interface_t)LOG_INTERFACE_ETHERNET || logger_config->log_interface == (log_interface_t)LOG_INTERFACE_BOTH)
            {
                udp_client_send_to((uint8_t*)buffer, (uint32_t)length, &(logger_config->udp_dest_ipaddr), logger_config->udp_dest_port);
            }
        }
		#elif defined(TEST)
			printf("%s\n",buffer, length);
        #endif
	}
	 
}