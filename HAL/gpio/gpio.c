#include "gpio.h"

// register struct (link port with hardware material)
typedef struct{
    volatile uint8_t *ddr;
    volatile uint8_t *port;
    volatile uint8_t *pin;
}gpio_register_t;

// Mapping registers ATmega328P
static const gpio_register_t GPIO_BUS[] = {
    [GPIO_PORTB] = { &DDRB, &PORTB, &PINB },
    [GPIO_PORTC] = { &DDRC, &PORTC, &PINC },
    [GPIO_PORTD] = { &DDRD, &PORTD, &PIND }
};

void gpio_init(gpio_port_t port,gpio_pin_t pin,gpio_mode_t mode){
    const gpio_register_t *bus = &GPIO_BUS[port];

    switch (mode)
    {
    case GPIO_MODE_INPUT:
        // Bit DDRx 0 => entry , bit PORT 0 => floating
        *(bus->ddr) &= ~(1<<pin);
        *(bus->port) &= ~(1<<pin);
        break;
    case GPIO_MODE_OUTPUT:
        // Bit DDRx 1 => output
        *(bus->ddr) |= (1<<pin);
        break;
    case GPIO_MODE_INPUT_PULLUP:
        // Bit DDRx 0 => entry , bit PORT 1 => Intern Pull Up
        *(bus->ddr) &= ~(1<<pin);
        *(bus->port) |= (1<<pin);
        break;
    }
}
void gpio_write(gpio_port_t port,gpio_pin_t pin,gpio_state_t state){
    if(state == GPIO_STATE_HIGH){
        gpio_set_high(port,pin);
    } else{
        gpio_set_low(port,pin);
    }
}
void gpio_set_high(gpio_port_t port,gpio_pin_t pin){
    // Write => Register PORT
    *(GPIO_BUS[port].port) |= (1<<pin); 
}

void gpio_set_low(gpio_port_t port,gpio_pin_t pin){
    *(GPIO_BUS[port].port) &= ~(1<<pin); 
}

gpio_state_t gpio_read(gpio_port_t port,gpio_pin_t pin){

    if(*(GPIO_BUS[port].pin) & (1<<pin)){
        return GPIO_STATE_HIGH;
    } else {
        return GPIO_STATE_LOW;
    }
}

void gpio_toggle(gpio_port_t port,gpio_pin_t pin){
    *(GPIO_BUS[port].pin) = (1<<pin);
    // Same :     *(GPIO_BUS[port].pin) = (1<<pin);

}
