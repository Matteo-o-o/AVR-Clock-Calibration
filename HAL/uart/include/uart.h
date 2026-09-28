#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

void uart_init(uint32_t baud_rate);

void uart_transmission_byte(uint8_t data);

void uart_transmission_string(const char *str);

uint8_t uart_receive_byte(void);

// check if the data is available in the receive buffer
bool uart_data_available(void);

#endif