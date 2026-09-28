#include "uart.h"
#include <avr/io.h>
#include <stdio.h>

static int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_transmission_byte('\r'); // clean synthax
    }
    uart_transmission_byte(c);
    return 0;
}

static FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

void uart_init(uint32_t baud_rate){
    // Speed config :
    // UBRR is CPU clock ticks per 1/16th of a bit (1 full UART bit takes 16 sub-ticks)
    // ATmega328P Datasheet, Section 19.3 "Clock Generation"

    uint16_t ubrr_value = (uint16_t)(F_CPU/ (16UL*baud_rate)-1);

    UBRR0H = (uint8_t)(ubrr_value >> 8);
    UBRR0L = (uint8_t)(ubrr_value);

    // Enable transmitter & receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // Format : Asynchronous , No parity, 1 bit stop, 8 data bits (8N1)
    UCSR0C = 0;
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // printf use : Standard output redirection
    stdout = &uart_output;
}

void uart_transmission_byte(uint8_t data){
    // Wait for transmission buffer empty (flag UDRE0 in register UCSR0A) (UDRE0 = 1 = empty)
    while (!(UCSR0A & (1 << UDRE0)));

    // Load data into buffer for transmission
    UDR0 = data;

}

void uart_transmission_string(const char *str){
    // *str = str[0]
    while(*str){
        uart_transmission_byte((uint8_t)*str);
        str++;
    }
}

// Receive function : use uart_data_available for non block use case
uint8_t uart_receive_byte(void){

    // Wait for data unread
    while (!(UCSR0A & (1<<RXC0)));

    return UDR0;
}

// check if there is unread data (bit UDR0 in UCSR0A)
bool uart_data_available(void){
    return((UCSR0A & (1<<RXC0)) != 0);

}

