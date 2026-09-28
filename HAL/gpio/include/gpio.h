#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

// Ports available on the ATmega328P - 1 PORT = 8 PIN
typedef enum{
    GPIO_PORTB = 0,
    GPIO_PORTC,
    GPIO_PORTD
} gpio_port_t;

// Index PIN (0 -> 7)
typedef enum{
    GPIO_PIN0=0,
    GPIO_PIN1,
    GPIO_PIN2,
    GPIO_PIN3,
    GPIO_PIN4,
    GPIO_PIN5,
    GPIO_PIN6,
    GPIO_PIN7
} gpio_pin_t;

// Mode PIN
typedef enum{
    GPIO_MODE_INPUT=0,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_INPUT_PULLUP   
} gpio_mode_t;

// Logic state
typedef enum{
    GPIO_STATE_LOW = 0,
    GPIO_STATE_HIGH
}gpio_state_t;

// Pin initialization
void gpio_init(gpio_port_t port,gpio_pin_t pin,gpio_mode_t mode);

// Write , read
void gpio_write(gpio_port_t port,gpio_pin_t pin,gpio_state_t state);
void gpio_set_high(gpio_port_t port,gpio_pin_t pin);
void gpio_set_low(gpio_port_t port,gpio_pin_t pin);
gpio_state_t gpio_read(gpio_port_t port,gpio_pin_t pin);

void gpio_toggle(gpio_port_t port, gpio_pin_t pin);


#endif // GPIO_H